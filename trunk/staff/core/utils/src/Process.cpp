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
#include <signal.h>
#else
#include <windows.h>
#endif
#include <limits.h>
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined __APPLE__
#include <mach-o/dyld.h>
#endif
#include "Log.h"
#include "Error.h"
#include "Exception.h"
#include "Process.h"

namespace staff
{

  struct Process::ProcessImpl
  {
#ifndef WIN32
    pid_t tPid;
    int anFds[2];
#else
    PROCESS_INFORMATION tProcessInformation;
    HANDLE hStdOutPipeRead;
#endif

    ProcessImpl()
    {
#ifndef WIN32
      tPid = 0;
#else
      tProcessInformation.hProcess = 0;
#endif
    }
  };

  Process::Process():
    m_pImpl(new ProcessImpl)
  {
  }

  Process::~Process()
  {
    if (IsRunning())
    {
      LogWarning() << "Process object destroyed while process is still running";
    }

    delete m_pImpl;
  }

  void Process::Start(const std::string& sCommandLine, bool bReadOutput /*= false*/)
  {
    std::string sOutput;
#ifndef WIN32
    static const int nMaxArgs = 256;
    char* aszArgs[nMaxArgs];
    char chFirst = '\0';
    std::string::size_type nSize = sCommandLine.size();

    char* szArgs = reinterpret_cast<char*>(malloc(nSize + 1));
    STAFF_ASSERT(szArgs, "Memory allocation failed");
    char* szArgsEnd = szArgs + nSize;
    char* szPosBegin = szArgs;
    char* szPosEnd = szArgs;
    int nArg = 0;

    memcpy(szArgs, sCommandLine.c_str(), nSize + 1);
    try
    {
      while (szPosBegin < szArgsEnd)
      {
        chFirst = *szPosBegin;
        if (chFirst == '\'' || chFirst == '\"')
        {
          ++szPosBegin;
          STAFF_ASSERT(szPosBegin <= szArgsEnd, "Unexpected end of command in [" + sCommandLine + "]");

          // find matched quote
          for (szPosEnd = szPosBegin;; ++szPosEnd)
          {
            szPosEnd = strchr(szPosEnd, chFirst);
            STAFF_ASSERT(szPosEnd, "Unexpected end of command in [" + sCommandLine + "]");
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


      if (bReadOutput)
      {
        STAFF_ASSERT(!pipe(m_pImpl->anFds), std::string("error while creating pipe: ") + strerror(errno));
      }

      m_pImpl->tPid = fork();
      switch (m_pImpl->tPid)
      {
        case -1:
        {
          STAFF_THROW_ASSERT(std::string("error while forking process: ") + strerror(errno));
        }

        case 0: // child
        {
          if (bReadOutput)
          {
            close(m_pImpl->anFds[0]);
            dup2(m_pImpl->anFds[1], 1);
            close(m_pImpl->anFds[1]);
          }

          execvp(aszArgs[0], aszArgs);

          LogError() << "Failed to start [" + sCommandLine + "]: " + strerror(errno);
          exit(-1);
        }

        default:
        {
          close(m_pImpl->anFds[1]);
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
    STARTUPINFOA tStartupInfo;
    HANDLE hStdOutPipeWrite;
    BOOL bProcessCreated = FALSE;

    if (bReadOutput)
    {
      SECURITY_ATTRIBUTES tSecurityAttributes;
      tSecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
      tSecurityAttributes.bInheritHandle = TRUE;
      tSecurityAttributes.lpSecurityDescriptor = NULL;

      CreatePipe(&m_pImpl->hStdOutPipeRead, &hStdOutPipeWrite, &tSecurityAttributes, 0);
    }

    memset(&tStartupInfo, 0, sizeof(tStartupInfo));
    tStartupInfo.cb = sizeof(tStartupInfo);
    tStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    tStartupInfo.wShowWindow = SW_HIDE;
    if (bReadOutput)
    {
      tStartupInfo.dwFlags |= STARTF_USESTDHANDLES;
      tStartupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
      tStartupInfo.hStdOutput = hStdOutPipeWrite;
      tStartupInfo.hStdError = hStdOutPipeWrite;
    }

    bProcessCreated = CreateProcessA(sCommandLine.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL,
                                     &tStartupInfo, &m_pImpl->tProcessInformation);
    STAFF_ASSERT(bProcessCreated, "Failed to create process [" + sCommandLine + "]");
    if (bReadOutput)
    {
      CloseHandle(hStdOutPipeWrite);
    }
#endif
  }

  bool Process::IsRunning()
  {
#ifndef WIN32
    return !kill(m_pImpl->tPid, 0);
#else
    DWORD dwRetCode = 0;
    GetExitCodeProcess(m_pImpl->tProcessInformation.hProcess, &dwRetCode);
    return dwRetCode == STILL_ACTIVE;
#endif
  }

  void Process::Terminate()
  {
#ifndef WIN32
    kill(m_pImpl->tPid, 9);
#else
    TerminateProcess(m_pImpl->tProcessInformation.hProcess, 1); 
#endif
  }

  unsigned long Process::GetCurrentId()
  {
#ifndef WIN32
    return static_cast<unsigned long>(getpid());
#else
    return static_cast<unsigned long>(GetCurrentProcessId());
#endif
  }

  unsigned long Process::GetId()
  {
#ifndef WIN32
    return static_cast<unsigned long>(m_pImpl->tPid);
#else
    return static_cast<unsigned long>(reinterpret_cast<unsigned long long>(m_pImpl->tProcessInformation.hProcess));
#endif
  }


  std::string Process::ReadAllStdandardOutput()
  {
    std::string sOutput;
    static const int nBuffSize = 1024;
    char achBuffer[nBuffSize];
#ifndef WIN32
    ssize_t nReaded = 0;

    while ((nReaded = read(m_pImpl->anFds[0], achBuffer, nBuffSize)))
    {
      STAFF_ASSERT(nReaded != -1, std::string("Read from child process failed: ") + strerror(errno));
      sOutput.append(achBuffer, nReaded);
    }
#else
    DWORD dwBytesRead = 0;
    for (;;)
    {
      BOOL bResult = ReadFile(m_pImpl->hStdOutPipeRead, achBuffer, sizeof(achBuffer) - 1, &dwBytesRead, NULL);
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
        sOutput.append(achBuffer, dwBytesRead);
      }
    }
#endif
    return sOutput;
  }

  int Process::Wait()
  {
#ifndef WIN32
    int nStatus = 0;
    STAFF_ASSERT(waitpid(m_pImpl->tPid, &nStatus, 0) != -1,
                 "Failed to wait child: " + Error::GetLastErrorStr());
    return WEXITSTATUS(nStatus);
#else
    DWORD dwExitCode = 0;
    WaitForSingleObject(m_pImpl->tProcessInformation.hProcess, INFINITE);
    GetExitCodeProcess(m_pImpl->tProcessInformation.hProcess, &dwExitCode);

    CloseHandle(m_pImpl->tProcessInformation.hThread);
    CloseHandle(m_pImpl->tProcessInformation.hProcess);
    CloseHandle(m_pImpl->hStdOutPipeRead);

    return static_cast<int>(dwExitCode);
#endif
  }

  std::string Process::GetCurrentExecPath()
  {
#ifdef WIN32
    char szPath[MAX_PATH];

    if (!GetModuleFileNameA(NULL, szPath, MAX_PATH))
#else
    char szPath[PATH_MAX];
#ifdef __FreeBSD__
    size_t nPathSize = PATH_MAX;
    int anMib[4];
    anMib[0] = CTL_KERN;
    anMib[1] = KERN_PROC;
    anMib[2] = KERN_PROC_PATHNAME;
    anMib[3] = -1;
    sysctl(anMib, 4, szPath, &nPathSize, NULL, 0);

    if(nPathSize > 0)
    {
      szPath[nPathSize] = '\0';
    }
    else
#else
#ifdef __APPLE__
    uint32_t nPathSize = PATH_MAX;
    _NSGetExecutablePath(szPath, &nPathSize);
    if(nPathSize > 0)
    {
      szPath[nPathSize] = '\0';
    }
    else
#else
    int nReaded = readlink("/proc/self/exe", szPath, PATH_MAX);

    if(nReaded > 0)
    {
      szPath[nReaded] = '\0';
    }
    else
#endif
#endif
#endif
    {
      return "";
    }

    return szPath;
  }


}
