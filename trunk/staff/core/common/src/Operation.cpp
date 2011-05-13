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

#include <rise/common/ExceptionTemplate.h>
#include "QName.h"
#include "Value.h"
#include "Operation.h"

namespace staff
{
  COperation::COperation(const std::string& sName /*= ""*/, const std::string& sResponseName /*= ""*/, const std::string& sResultName /*= ""*/):
    m_sRequestName(sName), m_sResponseName(sResponseName), m_sResultName(sResultName), m_pMessageContext(NULL)
  {
  }

  COperation::~COperation()
  {
  }

  void COperation::SetName(const std::string& sName)
  {
    m_sRequestName = sName;
  }

  const std::string COperation::GetName() const
  {
    return m_tdoRequest.IsInit() ? m_tdoRequest.GetLocalName() : m_sRequestName;
  }

  void COperation::SetResponseName(const std::string& sResponseName)
  {
    m_sResponseName = sResponseName;
    if (m_tdoResponse.IsInit())
    {
      m_tdoResponse.SetLocalName(sResponseName);
    }
  }

  const std::string COperation::GetResponseName() const
  {
    return m_tdoResponse.IsInit() ? m_tdoResponse.GetLocalName() : m_sResponseName;
  }

  void COperation::SetResultName(const std::string& sResultName)
  {
    m_sResultName = sResultName;
    if (m_tdoResult.IsInit() && m_tdoResult != m_tdoRequest)
    {
      m_tdoResult.SetLocalName(sResultName);
    }
  }

  const std::string& COperation::GetResultName() const
  {
    return m_sResultName;
  }

  void COperation::SetSoapAction(const std::string& sSoapAction)
  {
    m_sSoapAction = sSoapAction;
  }

  const std::string& COperation::GetSoapAction() const
  {
    return m_sSoapAction;
  }

  void COperation::AddParameter(const std::string& sName, const CValue& tValue)
  {
    CDataObject tParam(sName);
    tParam.Value() = tValue;
    Request().AppendChild(tParam);
  }

  void COperation::AddParameter(CDataObject& rDataObject)
  {
    Request().AppendChild(rDataObject);
  }

  const CDataObject& COperation::Request() const
  {
    if (!m_tdoRequest.IsInit())
    {
      m_tdoRequest.Create(m_sRequestName);
    }
    return m_tdoRequest;
  }

  CDataObject& COperation::Request()
  {
    if (!m_tdoRequest.IsInit())
    {
      m_tdoRequest.Create(m_sRequestName);
    }
    return m_tdoRequest;
  }

  void COperation::SetRequest(CDataObject& rdoRequest)
  {
    m_tdoRequest = rdoRequest;
  }

  void COperation::SetRequest(axiom_node_t* pRequest)
  {
    m_tdoRequest = pRequest;
  }

  const CDataObject& COperation::Result() const
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

  CDataObject& COperation::Result()
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

  CValue COperation::ResultValue()
  {
    return Result().Value();
  }

  CValue COperation::ResultValue() const
  {
    return Result().GetValue();
  }

  void COperation::SetResult(CDataObject& rDataObject)
  {
    Result().ReplaceNode(rDataObject);
  }

  CDataObject& COperation::GetResponse()
  {
    if (!m_tdoResponse.IsInit())
    {
      m_tdoResponse.Create(m_sResponseName);
    }
    return m_tdoResponse;
  }
  
  const CDataObject& COperation::GetResponse() const
  {
    if (!m_tdoResponse.IsInit())
    {
      m_tdoResponse.Create(m_sResponseName);
    }
    return m_tdoResponse;
  }
  
  void COperation::SetResponse(staff::CDataObject& rdoResponse)
  {
    m_tdoResponse = rdoResponse;
  }

  void COperation::SetResponse(axiom_node_t* pResponse)
  {
    m_tdoResponse = pResponse;
  }

  void COperation::SetResultValue(const CValue& rValue)
  {
    Result().Value() = rValue;
  }

  bool COperation::IsFault() const
  {
    return GetResponse().GetLocalName() == "Fault";
  }

  void COperation::SetFault(const std::string& sFaultCode, const std::string& sFaultString, const std::string& sDetail /*= ""*/)
  {
    CDataObject& rFault = GetResponse();

    rFault.SetLocalName("Fault");
    rFault.SetNamespaceUri("http://schemas.xmlsoap.org/soap/envelope/");
    rFault.CreateChildOnce("faultcode").SetText(sFaultCode);
    rFault.CreateChildOnce("faultstring").SetText(sFaultString);
    rFault.CreateChildOnce("detail").SetText(sDetail);
  }

  void COperation::ResetFault()
  {
    CDataObject& rFault = GetResponse();

    rFault.SetLocalName(Request().GetLocalName() + "Result");
    rFault.SetNamespaceUri("");
    rFault.RemoveChildByLocalName("faultcode");
    rFault.RemoveChildByLocalName("faultstring");
    rFault.RemoveChildByLocalName("detail");
  }

  void COperation::SetUserFault(CDataObject& rDataObjectFault)
  {
    SetResponse(rDataObjectFault);
    GetResponse().SetLocalName("Fault");
  }

  std::string COperation::GetFaultDescr() const
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

  std::string COperation::GetFaultString() const
  {
    const CDataObject& rResponse = GetResponse();
    CDataObject::ConstIterator itFind = rResponse.FindChildByLocalName("faultstring");
    if (itFind != rResponse.End())
    {
      return itFind->GetText();
    }

    itFind = rResponse.FindChildByLocalName("Reason");
    if (itFind == rResponse.End())
    {
      return "";
    }

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Text");
    if (itFindValue == itFind->End())
    {
      return itFind->GetText();
    }

    return itFindValue->GetText();
  }

  std::string COperation::GetFaultCode() const
  {
    const CDataObject& rResponse = GetResponse();
    CDataObject::ConstIterator itFind = rResponse.FindChildByLocalName("faultcode");
    if (itFind != rResponse.End())
    {
      return itFind->GetText();
    }

    itFind = rResponse.FindChildByLocalName("Code");
    if (itFind == rResponse.End())
    {
      return "";
    }

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Value");
    if (itFindValue == itFind->End())
    {
      return itFind->GetText();
    }

    return itFindValue->GetText();
  }

  std::string COperation::GetFaultDetail() const
  {
    const CDataObject& rResponse = GetResponse();
    CDataObject::ConstIterator itFind = rResponse.FindChildByLocalName("detail");
    if (itFind != rResponse.End())
    {
      return itFind->GetText();
    }

    itFind = rResponse.FindChildByLocalName("Detail");
    if (itFind == rResponse.End())
    {
      return "";
    }

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Exception");
    if (itFindValue == itFind->End())
    {
      return itFind->GetText();
    }

    return itFindValue->GetText();
  }

  const CDataObject COperation::GetFault() const
  {
    return GetResponse();
  }

  void COperation::PrepareResult()
  {
//    Result().NamespaceList() = Request().NamespaceList();

    const CDataObject& rResponse = GetResponse();
    if (GetResponse().GetLocalName().empty())
    {
      rResponse.SetLocalName(m_tdoRequest.GetLocalName() + "Result");
    }

    if (rResponse.GetPrefix().empty() || rResponse.GetNamespaceUri().empty())
    {
      const std::string& sNamespaceUri = m_tdoRequest.GetNamespaceUri();
      if (!sNamespaceUri.empty())
      {
        const std::string& sPrefix = m_tdoRequest.GetPrefix();
        CQName tqnResult(rResponse.GetLocalName(), sNamespaceUri,
                         sPrefix.empty() ? "ns" : sPrefix);
        rResponse.SetQName(tqnResult);
      }
    }
  }

  void COperation::SetMessageContext(CMessageContext& rMessageContext)
  {
    m_pMessageContext = &rMessageContext;
  }

  CMessageContext& COperation::GetMessageContext()
  {
    RISE_ASSERTS(m_pMessageContext, "Message context is NULL");
    return *m_pMessageContext;
  }

}
