/*
 *  Copyright 2012 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
#include <rise/xml/XMLDocument.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
#include <rise/string/String.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Operation.h>
#include <staff/security/tools.h>
#include <staff/component/SharedContext.h>
#include <staff/component/ServiceWrapper.h>
#include <staff/das/common/DataSource.h>
#include <staff/das/common/Executor.h>
#include "Shell.h"

namespace staff
{
namespace das
{
  static std::string StartProcessAndReadOutput(const std::string& sCommand)
  {
    std::string sOutput;
#ifndef WIN32
    static const int nMaxArgs = 256;
    char* aszArgs[nMaxArgs];
    char chFirst = '\0';
    std::string::size_type nSize = sCommand.size();

    char* szArgs = reinterpret_cast<char*>(malloc(nSize + 1));
    RISE_ASSERTS(szArgs, "Memory allocation failed");
    char* szArgsEnd = szArgs + nSize;
    char* szPosBegin = szArgs;
    char* szPosEnd = szArgs;
    int nArg = 0;

    memcpy(szArgs, sCommand.c_str(), nSize + 1);
    try
    {
      while (szPosBegin < szArgsEnd)
      {
        chFirst = *szPosBegin;
        if (chFirst == '\'' || chFirst == '\"')
        {
          ++szPosBegin;
          RISE_ASSERTS(szPosBegin <= szArgsEnd, "Unexpected end of command in [" + sCommand + "]");

          // find matched quote
          for (szPosEnd = szPosBegin;; ++szPosEnd)
          {
            szPosEnd = strchr(szPosEnd, chFirst);
            RISE_ASSERTS(szPosEnd, "Unexpected end of command in [" + sCommand + "]");
            if (*(szPosEnd - 1) != '\\')
            {
              break;
            }
          }
        }
        else
        {
          szPosEnd = szPosBegin;
          while (*szPosEnd && *szPosEnd != ' ' && *szPosEnd != '\t')
          {
            ++szPosEnd;
          }

          if (szPosEnd == szPosBegin)
          {
            ++szPosBegin;
            continue;
          }
        }

        *szPosEnd = '\0';
        aszArgs[nArg++] = szPosBegin;
        szPosBegin = szPosEnd + 1;

        if (nArg == (nMaxArgs - 1))
        {
          break;
        }
      }

      aszArgs[nArg++] = NULL;


      int anFds[2];

      RISE_ASSERTS(!pipe(anFds), std::string("error while creating pipe: ") + strerror(errno));

      pid_t tPid = fork();
      switch (tPid)
      {
        case -1:
        {
          RISE_THROWS(rise::CInternalAssertException,
                     std::string("error while forking process: ") + strerror(errno));
        }

        case 0: // child
        {
          close(anFds[0]);
          dup2(anFds[1], 1);
          close(anFds[1]);

          execvp(aszArgs[0], aszArgs);

          rise::LogError() << "Failed to start [" + sCommand + "]: " + strerror(errno);
          exit(-1);
        }

        default: // parent
        {
          static const int nBuffSize = 1024;
          char achBuffer[nBuffSize];
          ssize_t nReaded = 0;

          close(anFds[1]);
          while ((nReaded = read(anFds[0], achBuffer, nBuffSize)))
          {
            RISE_ASSERTS(nReaded != -1, std::string("Read from child process failed: ") + strerror(errno));
            sOutput.append(achBuffer, nReaded);
          }

          int nStatus = 0;
          RISE_ASSERTS(waitpid(tPid, &nStatus, 0) != -1, std::string("Failed to wait child: ")
                       + strerror(errno));
        }
      }

    }
    catch(...)
    {
      free(szArgs);
      throw;
    }
    free(szArgs);
#else
    // WINDOWS
    static const int nBuffSize = 1024;
    char szBuffer[1024];
    SECURITY_ATTRIBUTES tSecurityAttributes;
    STARTUPINFO tStartupInfo;
    PROCESS_INFORMATION tProcessInformation;
    HANDLE hStdOutPipeRead, hStdOutPipeWrite;
    BOOL bProcessCreated = FALSE;
    DWORD dwBytesRead = 0;

    tSecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    tSecurityAttributes.bInheritHandle = TRUE;
    tSecurityAttributes.lpSecurityDescriptor = NULL;

    CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &tSecurityAttributes, 0);

    memset(&tStartupInfo, 0, sizeof(tStartupInfo));
    tStartupInfo.cb = sizeof(tStartupInfo);
    tStartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    tStartupInfo.wShowWindow = SW_HIDE;
    tStartupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    tStartupInfo.hStdOutput = hStdOutPipeWrite;
    tStartupInfo.hStdError = hStdOutPipeWrite;

    bProcessCreated = CreateProcess(sCommand.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL,
                                    &tStartupInfo, &tProcessInformation);
    CloseHandle(hStdOutPipeWrite);
    RISE_ASSERTS(bProcessCreated, "Failed to create process [" + sCommand + "]");

    // if there was a problem, or the async. operation is still pending.
    for (;;)
    {
      BOOL bResult = ReadFile(hStdOutPipeRead, szBuffer, sizeof(szBuffer) - 1, &dwBytesRead, NULL);
      if (!bResult)
      {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_HANDLE_EOF || dwError == ERROR_BROKEN_PIPE)
        {
          break;
        }

        Sleep(0);
      }
      else
      {
        sOutput.append(szBuffer, dwBytesRead);
      }
    }

    DWORD dwExitCode = 0;
    WaitForSingleObject(tProcessInformation.hProcess, INFINITE);
    BOOL bExitCode = GetExitCodeProcess(tProcessInformation.hProcess, &dwExitCode);

    CloseHandle(tProcessInformation.hThread);
    CloseHandle(tProcessInformation.hProcess);
    CloseHandle(hStdOutPipeRead);
#endif

    return sOutput;
  }


  class ShellRawExecutor: public IRawExecutor
  {
  public:
    ShellRawExecutor(ShellProvider* pProvider):
      m_pProvider(pProvider), m_nFirstColCount(0)
    {
    }

    virtual void Execute(const std::string& sExecute, const DataObject& /*rdoContext*/,
                         const DataType& rReturnType, DataObject& rdoResult)
    {
      RISE_ASSERTS(m_pProvider, "Not Initialized");

      const std::string& sResult = StartProcessAndReadOutput(m_pProvider->m_sScriptsDir + sExecute);
      if (rReturnType.eType == DataType::Generic)
      {
        rdoResult.SetText(sResult);
      }
      else
      if (rReturnType.eType != DataType::Void)
      {
        StringList lsResult;

        std::string::size_type nRowBegin = 0;
        std::string::size_type nRowEnd = 0;
        std::string::size_type nColBegin = 0;
        std::string::size_type nColEnd = 0;
        int nResultSize = 0;
        do
        {
          lsResult.clear();
          nResultSize = 0;
          nRowEnd = sResult.find_first_of(m_pProvider->m_sRowDelims, nRowBegin);
          const std::string& sRow = (nRowEnd != std::string::npos) ?
                                        sResult.substr(nRowBegin, nRowEnd - nRowBegin):
                                        sResult.substr(nRowBegin);

          if (!sRow.empty())
          {
            nColBegin = 0;
            do
            {
              nColBegin = sRow.find_first_not_of(m_pProvider->m_sColDelims, nColBegin);
              // limit fields count
              if (m_pProvider->m_bFirstColCount && (nResultSize + 1) == m_nFirstColCount)
              {
                lsResult.push_back(sRow.substr(nColBegin));
                break;
              }
              else
              {
                nColEnd = sRow.find_first_of(m_pProvider->m_sColDelims, nColBegin);
                const std::string& sCell = (nColEnd != std::string::npos) ?
                      sRow.substr(nColBegin, nColEnd - nColBegin):
                      sRow.substr(nColBegin);

                if (!sCell.empty())
                {
                  lsResult.push_back(sCell);
                  ++nResultSize;
                }
              }
              nColBegin = nColEnd + 1;
            }
            while (nColEnd != std::string::npos);

            // first line
            if (!nRowBegin)
            {
              m_nFirstColCount = nResultSize;
            }

            // remember fields names
            if (!nRowBegin && m_pProvider->m_bColHeaders)
            {
              m_lsFieldsNames = lsResult;
            }
            else
            {
              ProcessComplexResult(lsResult, rReturnType, rdoResult);
            }
          }

          nRowBegin = nRowEnd + 1;
        }
        while (nRowEnd != std::string::npos);

      }
    }

    void ProcessComplexResult(const StringList& lsResult, const DataType& rReturnType, DataObject& rdoResult)
    {
      if (rReturnType.eType == DataType::Struct)
      {
        RISE_ASSERTS(lsResult.size() == rReturnType.lsChilds.size(), "Fields count does not match: " +
            rise::ToStr(lsResult.size()) + " expected: " + rise::ToStr(rReturnType.lsChilds.size()));

        StringList::const_iterator itResult = lsResult.begin();
        for (DataTypesList::const_iterator itType = rReturnType.lsChilds.begin();
            itType != rReturnType.lsChilds.end(); ++itType, ++itResult)
        {
          rdoResult.CreateChild(itType->sName).SetText(*itResult);
        }
      }
      else
      if (rReturnType.eType == DataType::DataObject ||
          (rReturnType.eType == DataType::List && rReturnType.lsChilds.front().eType == DataType::DataObject))
      {
        staff::DataObject tdoItem = rdoResult.CreateChild("Item");

        if (m_lsFieldsNames.empty())
        {
          for (StringList::const_iterator itResult = lsResult.begin();
               itResult != lsResult.end(); ++itResult)
          {
            tdoItem.CreateChild("Cell", *itResult);
          }
        }
        else
        {
          for (StringList::const_iterator itResult = lsResult.begin(), itName = m_lsFieldsNames.begin();
               itResult != lsResult.end(); ++itResult, ++itName)
          {
            tdoItem.CreateChild(*itName).SetText(*itResult);
          }
        }
      }
      else
      if (rReturnType.eType == DataType::List) // ---------------- list ----------------------------
      {
        const DataType& rItemType = rReturnType.lsChilds.front();
        if (rItemType.eType == DataType::Generic) // list of generics
        {
            RISE_ASSERTS(lsResult.size() == 1, "Fields count does not match: " +
                         rise::ToStr(lsResult.size()) + " expected: 1");
            rdoResult.CreateChild("Item").SetText(lsResult.front());
        }
        else
        if (rItemType.eType == DataType::Struct) // list of structs
        {
          RISE_ASSERTS(lsResult.size() == rItemType.lsChilds.size(), "Fields count does not match: " +
                       rise::ToStr(lsResult.size()) + " expected: " + rise::ToStr(rItemType.lsChilds.size()));
          staff::DataObject tdoItem = rdoResult.CreateChild("Item");

          StringList::const_iterator itResult = lsResult.begin();
          for (DataTypesList::const_iterator itType = rItemType.lsChilds.begin();
              itType != rItemType.lsChilds.end(); ++itType, ++itResult)
          {
            tdoItem.CreateChild(itType->sName).SetText(*itResult);
          }
        }
        else
        {
          RISE_THROWS(rise::CLogicNoItemException, "Unsupported list item type: " + rReturnType.sType);
        }
      }
    }

  private:
    ShellProvider* m_pProvider;
    StringList m_lsFieldsNames;
    unsigned m_nFirstColCount;
  };


  ShellProvider::ShellProvider():
    m_bColHeaders(false), m_bFirstColCount(true), m_sColDelims(" \t"), m_sRowDelims("\n\r")
  {
  }

  ShellProvider::~ShellProvider()
  {
    Deinit();
  }

  void ShellProvider::Init(const rise::xml::CXMLNode& rConfig)
  {
    // initialize connection
    const rise::xml::CXMLNode& rConnection = rConfig.Subnode("connection");

    rise::xml::CXMLNode::TXMLNodeConstIterator itNode = rConnection.FindSubnode("scriptsdir");
    if (itNode != rConnection.NodeEnd())
    {
      m_sScriptsDir = itNode->NodeContent().AsString();
    }

    itNode = rConnection.FindSubnode("colheaders");
    if (itNode != rConnection.NodeEnd())
    {
      m_bColHeaders = itNode->NodeContent().AsString() == "1"
          || itNode->NodeContent().AsString() == "true";
    }

    itNode = rConnection.FindSubnode("firstcolcount");
    if (itNode != rConnection.NodeEnd())
    {
      m_bFirstColCount = itNode->NodeContent().AsString() == "1"
          || itNode->NodeContent().AsString() == "true";
    }

    itNode = rConnection.FindSubnode("coldelims");
    if (itNode != rConnection.NodeEnd())
    {
      m_sColDelims = itNode->NodeContent().AsString();
    }

    itNode = rConnection.FindSubnode("rowdelims");
    if (itNode != rConnection.NodeEnd())
    {
      m_sRowDelims = itNode->NodeContent().AsString();
    }
  }

  void ShellProvider::Deinit()
  {
  }

  const std::string& ShellProvider::GetName() const
  {
    static const std::string sName = "staff.das.Shell";
    return sName;
  }

  const std::string& ShellProvider::GetDescr() const
  {
    static const std::string sDescr = "Shell data access provider";
    return sDescr;
  }

  PExecutor ShellProvider::GetExecutor()
  {
    return new ShellRawExecutor(this);
  }

}
}

