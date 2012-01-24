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
#include "Services.h"

namespace staff
{
namespace das
{

  class ServicesRawExecutor: public IRawExecutor
  {
  public:
    ServicesRawExecutor(ServicesProvider* pProvider):
      m_pProvider(pProvider)
    {
    }

    virtual void Execute(const std::string& sExecute, const DataObject& rdoContext,
                         const DataType& /*rReturnType*/, DataObject& rdoResult)
    {
      RISE_ASSERTS(m_pProvider != NULL && m_pProvider->m_pServiceWrapper != NULL, "Not Initialized");

      std::string::size_type nPosBegin = 0;
      std::string::size_type nPosEnd = 0;

      nPosBegin = sExecute.find('(');
      nPosEnd = sExecute.find_last_of(')');

      RISE_ASSERTS(nPosBegin != std::string::npos && nPosEnd != std::string::npos &&
                   nPosEnd > nPosBegin, "Invalid execute statement.\nIt should be in form "
                   "\"<execute>ElementName([par0=val, par1=$var1, par2=$[DO_val], ...])</execute>\"");

      std::string sParam;
      std::string sValue;
      std::string sElement;
      std::string sArguments = sExecute.substr(nPosBegin + 1, nPosEnd - nPosBegin - 1);

      rise::StrTrim(sArguments);

      // check for junk
      sElement = sExecute.substr(nPosEnd + 1);
      rise::StrTrim(sElement);
      RISE_ASSERTS(sElement.empty(), "Junk at end of operation: [" + sElement + "]");

      // request element name
      sElement = sExecute.substr(0, nPosBegin);
      rise::StrTrim(sElement);

      // operation name
      staff::Operation tOperation(sElement);
      staff::DataObject& rdoRequest = tOperation.Request();
      tOperation.SetResponse(rdoResult);
      tOperation.GetResponse().SetOwner(false);

      if (!sArguments.empty())
      {
        nPosBegin = 0;
        nPosEnd = 0;

        while (nPosEnd != std::string::npos)
        {
          nPosEnd = sArguments.find('=', nPosBegin);
          RISE_ASSERTS(nPosEnd != std::string::npos, "Unexpected end of parameter after: ["
                       + sArguments.substr(nPosBegin) + "]");

          // parameter's name
          sParam = sArguments.substr(nPosBegin, nPosEnd - nPosBegin);
          rise::StrTrim(sParam);
          nPosBegin = nPosEnd + 1;

          // parameter's value
          nPosEnd = sArguments.find(',', nPosBegin);

          if (nPosEnd != std::string::npos)
          {
            sValue = sArguments.substr(nPosBegin, nPosEnd - nPosBegin);
          }
          else
          {
            sValue = sArguments.substr(nPosBegin);
          }
          rise::StrTrim(sValue);

          if (!sValue.empty() && sValue[0] == '$')
          {
            // pass dataobject
            if (sValue[1] == '[' && sValue[sValue.size() - 1] == ']')
            {
              const std::string& sChildName = sValue.substr(2, sValue.size() - 3);
              DataObject doChild;
              GetChild(rdoContext, sChildName, doChild);
              doChild.SetLocalName(sParam);
              rdoRequest.AppendChild(doChild);
            }
          }
          else
          {
            rdoRequest.CreateChild(sParam, sValue);
          }

          nPosBegin = nPosEnd + 1;
        }
      }

      // TODO: get SessionId and InstanceId
      // security module can't filter this request 'cause we're inside
      m_pProvider->m_pServiceWrapper->Invoke(tOperation, STAFF_SECURITY_NOBODY_SESSION_ID, "");

      /* example of response:
        <Response>
          <RowItem>
            <Field_1>data 1</Field_1><Field_2>2</Field_2>...
          </RowItem>
          ...
        </Response>
      */
    }

    void GetChild(const DataObject& rdoContext, const std::string& sPath, DataObject& rdoResult)
    {
      rdoResult = rdoContext;

      if (!sPath.empty())
      {
        std::string sName;
        std::string::size_type nSize = sPath.size();
        RISE_ASSERTS(nSize > 1, "invalid node name: [" + sPath + "]");

        std::string::size_type nBegin = 0;
        std::string::size_type nEnd = 0;
        do
        {
          nEnd = sPath.find('.', nBegin);
          if (nEnd == std::string::npos)
          {
            sName = sPath.substr(nBegin);
          }
          else
          {
            sName = sPath.substr(nBegin, nEnd - nBegin);
          }

          if (sName.empty()) // parent node
          {
            rdoResult = rdoResult.Parent();
            RISE_ASSERTS(!rdoResult.IsNull(), "parent of root element reached while GetChild[" + sPath + "]");
          }
          else
          {
            rdoResult = rdoResult.GetChildByLocalName(sName);
          }

          nBegin = nEnd + 1;
        }
        while(nEnd != std::string::npos);
      }
    }

  private:
    ServicesProvider* m_pProvider;
  };


  ServicesProvider::ServicesProvider():
    m_pServiceWrapper(NULL)
  {
  }

  ServicesProvider::~ServicesProvider()
  {
    Deinit();
  }

  void ServicesProvider::Init(const DataSource& rDataSource)
  {
    const rise::xml::CXMLNode& rConfig = rDataSource.GetProviderConfig();

    // initialize connection
    const rise::xml::CXMLNode& rConnection = rConfig.Subnode("connection");

    const std::string& sService = rConnection["service"].AsString();
    m_pServiceWrapper = SharedContext::Inst().GetService(sService);
    RISE_ASSERTS(m_pServiceWrapper, "Can't find service: [" + sService + "]");
  }

  void ServicesProvider::Deinit()
  {
    if (m_pServiceWrapper)
    {
      m_pServiceWrapper = NULL;
    }
  }

  const std::string& ServicesProvider::GetName() const
  {
    static const std::string sName = "staff.das.Services";
    return sName;
  }

  const std::string& ServicesProvider::GetDescr() const
  {
    static const std::string sDescr = "Services data access provider";
    return sDescr;
  }

  PExecutor ServicesProvider::GetExecutor()
  {
    return new ServicesRawExecutor(this);
  }

}
}

