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

#include <staff/utils/SharedPtr.h>
#include <staff/utils/Log.h>
#include <staff/utils/Process.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/tostring.h>
#include <staff/xml/Document.h>
#include <staff/common/Exception.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Operation.h>
#include <staff/component/SharedContext.h>
#include <staff/component/ServiceWrapper.h>
#include <staff/das/common/DataSource.h>
#include <staff/das/common/Executor.h>
#include "Shell.h"

namespace staff
{
namespace das
{

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
      STAFF_ASSERT(m_pProvider, "Not Initialized");

      Process tProc;
      tProc.Start(m_pProvider->m_sScriptsDir + sExecute, true);

      const std::string& sResult = tProc.ReadAllStdandardOutput();
      tProc.Wait();

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
        unsigned nResultSize = 0;
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
        STAFF_ASSERT(lsResult.size() == rReturnType.lsChilds.size(), "Fields count does not match: " +
            ToString(lsResult.size()) + " expected: " + ToString(rReturnType.lsChilds.size()));

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
            STAFF_ASSERT(lsResult.size() == 1, "Fields count does not match: " +
                         ToString(lsResult.size()) + " expected: 1");
            rdoResult.CreateChild("Item").SetText(lsResult.front());
        }
        else
        if (rItemType.eType == DataType::Struct) // list of structs
        {
          STAFF_ASSERT(lsResult.size() == rItemType.lsChilds.size(), "Fields count does not match: " +
                       ToString(lsResult.size()) + " expected: " + ToString(rItemType.lsChilds.size()));
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
          STAFF_THROW_ASSERT("Unsupported list item type: " + rReturnType.sType);
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

  void ShellProvider::Init(const xml::Element& rConfig)
  {
    // initialize connection
    const xml::Element& rConnection = rConfig.GetChildElementByName("connection");

    const xml::Element* pElem = rConnection.FindChildElementByName("scriptsdir");
    if (pElem)
    {
      m_sScriptsDir = pElem->GetTextValue();
    }

    pElem = rConnection.FindChildElementByName("colheaders");
    if (pElem)
    {
      m_bColHeaders = pElem->GetValue();
    }

    pElem = rConnection.FindChildElementByName("firstcolcount");
    if (pElem)
    {
      m_bFirstColCount = pElem->GetValue();
    }

    pElem = rConnection.FindChildElementByName("coldelims");
    if (pElem)
    {
      m_sColDelims = pElem->GetTextValue();
    }

    pElem = rConnection.FindChildElementByName("rowdelims");
    if (pElem)
    {
      m_sRowDelims = pElem->GetTextValue();
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

