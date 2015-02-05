/*
 *  Copyright 2009 Utkin Dmitry
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

#include "Exception.h"
#include "QName.h"
#include "Value.h"
#include "Operation.h"

namespace staff
{
  Operation::Operation(const std::string& sName /*= ""*/, const std::string& sResponseName /*= ""*/, const std::string& sResultName /*= ""*/):
    m_sRequestName(sName), m_sResponseName(sResponseName), m_sResultName(sResultName), m_pMessageContext(NULL)
  {
  }

  Operation::~Operation()
  {
  }

  void Operation::SetName(const std::string& sName)
  {
    if (m_tdoRequest.IsInit())
    {
      m_tdoRequest.SetLocalName(sName);
    }
    else
    {
      m_sRequestName = sName;
    }
  }

  const std::string Operation::GetName() const
  {
    return m_tdoRequest.IsInit() ? m_tdoRequest.GetLocalName() : m_sRequestName;
  }

  void Operation::SetResponseName(const std::string& sResponseName)
  {
    m_sResponseName = sResponseName;
    if (m_tdoResponse.IsInit())
    {
      m_tdoResponse.SetLocalName(sResponseName);
    }
  }

  const std::string Operation::GetResponseName() const
  {
    return m_tdoResponse.IsInit() ? m_tdoResponse.GetLocalName() : m_sResponseName;
  }

  void Operation::SetResultName(const std::string& sResultName)
  {
    m_sResultName = sResultName;
    if (m_tdoResult.IsInit() && m_tdoResult != m_tdoRequest)
    {
      m_tdoResult.SetLocalName(sResultName);
    }
  }

  const std::string& Operation::GetResultName() const
  {
    return m_sResultName;
  }

  void Operation::SetSoapAction(const std::string& sSoapAction)
  {
    m_sSoapAction = sSoapAction;
  }

  const std::string& Operation::GetSoapAction() const
  {
    return m_sSoapAction;
  }

  void Operation::AddParameter(const std::string& sName, const Value& tValue)
  {
    Request().CreateChild(sName, tValue);
  }

  void Operation::AddParameter(DataObject& rDataObject)
  {
    Request().AppendChild(rDataObject);
  }

  const DataObject& Operation::Request() const
  {
    if (!m_tdoRequest.IsInit())
    {
      m_tdoRequest.Create(m_sRequestName);
    }
    return m_tdoRequest;
  }

  DataObject& Operation::Request()
  {
    if (!m_tdoRequest.IsInit())
    {
      m_tdoRequest.Create(m_sRequestName);
    }
    return m_tdoRequest;
  }

  void Operation::SetRequest(DataObject& rdoRequest)
  {
    m_tdoRequest = rdoRequest;
  }

  void Operation::SetRequest(axiom_node_t* pRequest)
  {
    m_tdoRequest = pRequest;
  }

  const DataObject& Operation::ResultOpt() const
  {
    if (m_sResultName.empty())
    {
      return m_tdoResponse;
    }

    if (m_tdoResult.IsNull() && m_tdoResponse.IsInit())
    {
      m_tdoResult = m_tdoResponse.GetChildByLocalNameOpt(m_sResultName);
    }
    return m_tdoResult;
  }

  DataObject& Operation::ResultOpt()
  {
    if (m_sResultName.empty())
    {
      return m_tdoResponse;
    }

    if (m_tdoResult.IsNull() && m_tdoResponse.IsInit())
    {
      m_tdoResult = m_tdoResponse.GetChildByLocalNameOpt(m_sResultName);
    }
    return m_tdoResult;
  }

  const DataObject& Operation::Result() const
  {
    if (m_tdoResult.IsNull())
    {
      if (m_sResultName.empty())
      {
        return GetResponse();
      }

      m_tdoResult = GetResponse().GetChildByLocalName(m_sResultName);
    }
    return m_tdoResult;
  }

  DataObject& Operation::Result()
  {
    if (m_tdoResult.IsNull())
    {
      if (m_sResultName.empty())
      {
        return GetResponse();
      }

      m_tdoResult = GetResponse().CreateChildOnce(m_sResultName);
    }
    return m_tdoResult;
  }

  void Operation::SetResult(DataObject& rDataObject)
  {
    Result().ReplaceNode(rDataObject);
  }

  DataObject& Operation::GetResponse()
  {
    if (!m_tdoResponse.IsInit())
    {
      m_tdoResponse.Create(m_sResponseName);
    }
    return m_tdoResponse;
  }
  
  const DataObject& Operation::GetResponse() const
  {
    if (!m_tdoResponse.IsInit())
    {
      m_tdoResponse.Create(m_sResponseName);
    }
    return m_tdoResponse;
  }
  
  void Operation::SetResponse(staff::DataObject& rdoResponse)
  {
    m_tdoResponse = rdoResponse;
  }

  void Operation::SetResponse(axiom_node_t* pResponse)
  {
    m_tdoResponse = pResponse;
  }

  void Operation::SetResultValue(const Value& rValue)
  {
    Result().SetValue(rValue);
  }

  bool Operation::IsFault() const
  {
    return GetResponse().GetLocalName() == "Fault";
  }

  void Operation::SetFault(const std::string& sFaultCode, const std::string& sFaultString, const std::string& sDetail /*= ""*/)
  {
    DataObject& rFault = GetResponse();

    rFault.SetLocalName("Fault");
    rFault.SetNamespaceUri("http://schemas.xmlsoap.org/soap/envelope/");
    rFault.CreateChildOnce("faultcode").SetText(sFaultCode);
    rFault.CreateChildOnce("faultstring").SetText(sFaultString);
    rFault.CreateChildOnce("detail").SetText(sDetail);
    m_bIsUserFault = false;
  }

  void Operation::ResetFault()
  {
    DataObject& rFault = GetResponse();

    rFault.SetLocalName(Request().GetLocalName() + "Result");
    rFault.SetNamespaceUri("");
    rFault.RemoveChildByLocalName("faultcode");
    rFault.RemoveChildByLocalName("faultstring");
    rFault.RemoveChildByLocalName("detail");
    m_bIsUserFault = false;
  }

  void Operation::SetUserFault(DataObject& rDataObjectFault)
  {
    SetResponse(rDataObjectFault);
    GetResponse().SetLocalName("Fault");
    m_bIsUserFault = true;
  }

  bool Operation::IsUserFault() const
  {
    return m_bIsUserFault;
  }

  std::string Operation::GetFaultDescr() const
  {
    if (m_bIsUserFault)
    {
      return GetResponse().ToString();
    }
    else
    {
      std::string sResult;
      const std::string& sFaultCode = GetFaultCode();
      const std::string& sFaultString = GetFaultString();
      const std::string& sFaultDetail = GetFaultDetail();

      if (!sFaultCode.empty())
      {
        sResult += "faultcode: " + sFaultCode + "\n";
      }
      if (!sFaultString.empty())
      {
        sResult += "faultstring: " + sFaultString + "\n";
      }
      if (!sFaultDetail.empty())
      {
        sResult += "detail: " + sFaultDetail + "\n";
      }

      return sResult;
    }
  }

  std::string Operation::GetFaultString() const
  {
    const DataObject& rResponse = GetResponse();
    DataObject::ConstIterator itFind = rResponse.FindChildByLocalName("faultstring");
    if (itFind != rResponse.End())
    {
      return itFind->GetText();
    }

    itFind = rResponse.FindChildByLocalName("Reason");
    if (itFind == rResponse.End())
    {
      return "";
    }

    DataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Text");
    if (itFindValue == itFind->End())
    {
      return itFind->GetText();
    }

    return itFindValue->GetText();
  }

  std::string Operation::GetFaultCode() const
  {
    const DataObject& rResponse = GetResponse();
    DataObject::ConstIterator itFind = rResponse.FindChildByLocalName("faultcode");
    if (itFind != rResponse.End())
    {
      return itFind->GetText();
    }

    itFind = rResponse.FindChildByLocalName("Code");
    if (itFind == rResponse.End())
    {
      return "";
    }

    DataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Value");
    if (itFindValue == itFind->End())
    {
      return itFind->GetText();
    }

    return itFindValue->GetText();
  }

  std::string Operation::GetFaultDetail() const
  {
    const DataObject& rResponse = GetResponse();
    DataObject::ConstIterator itFind = rResponse.FindChildByLocalName("detail");
    if (itFind != rResponse.End())
    {
      return itFind->GetText();
    }

    itFind = rResponse.FindChildByLocalName("Detail");
    if (itFind == rResponse.End())
    {
      return "";
    }

    DataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Exception");
    if (itFindValue == itFind->End())
    {
      return itFind->GetText();
    }

    return itFindValue->GetText();
  }

  const DataObject Operation::GetFault() const
  {
    return GetResponse().Copy();
  }

  void Operation::PrepareResult()
  {
    const DataObject& rResponse = GetResponse();
    if (GetResponse().GetLocalName().empty())
    {
      rResponse.SetLocalName(m_tdoRequest.GetLocalName() + "Result");
    }
  }

  void Operation::SetMessageContext(MessageContext& rMessageContext)
  {
    m_pMessageContext = &rMessageContext;
  }

  MessageContext& Operation::GetMessageContext()
  {
    STAFF_ASSERT(m_pMessageContext, "Message context is NULL");
    return *m_pMessageContext;
  }

}
