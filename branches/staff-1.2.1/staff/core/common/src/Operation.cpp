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

#include "QName.h"
#include "Value.h"
#include "Operation.h"

namespace staff
{
  COperation::COperation(const std::string& sName /*= ""*/, const std::string& sResponseName /*= ""*/, const std::string& sResultName /*= ""*/):
    m_tdoRequest(sName), m_tdoResponse(sResponseName), m_sResultName(sResultName)
  {
  }

  COperation::~COperation()
  {
  }

  void COperation::SetName( const std::string& sName )
  {
    m_tdoRequest.SetLocalName(sName);
  }

  const std::string COperation::GetName() const
  {
    return m_tdoRequest.GetLocalName();
  }

  void COperation::SetResponseName( const std::string& sResponseName )
  {
    m_tdoResponse.SetLocalName(sResponseName);
  }

  const std::string COperation::GetResponseName() const
  {
    return m_tdoRequest.GetLocalName();
  }

  void COperation::SetResultName( const std::string& sResultName )
  {
    m_sResultName = sResultName;
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

  void COperation::AddParameter( const std::string& sName, const CValue& tValue )
  {
    CDataObject tParam(sName);
    tParam.Value() = tValue;
    m_tdoRequest.AppendChild(tParam);
  }

  void COperation::AddParameter( CDataObject& rDataObject )
  {
    m_tdoRequest.AppendChild(rDataObject);
  }

  const CDataObject& COperation::Request() const
  {
    return m_tdoRequest;
  }

  CDataObject& COperation::Request()
  {
    return m_tdoRequest;
  }

  const CDataObject& COperation::Result() const
  {
    m_tdoResult = (m_sResultName.size() == 0 ? m_tdoResponse : m_tdoResponse.GetChildByLocalName(m_sResultName));
    m_tdoResult.SetOwner(false);
    return m_tdoResult;
  }

  CDataObject& COperation::Result()
  {
    m_tdoResult = (m_sResultName.size() == 0 ? m_tdoResponse : m_tdoResponse.CreateChildOnce(m_sResultName));
    m_tdoResult.SetOwner(false);
    return m_tdoResult;
  }

  CValue COperation::ResultValue()
  {
    return Result().Value();
  }

  const CValue COperation::ResultValue() const
  {
    return Result().Value();
  }

  void COperation::SetResult( CDataObject& rDataObject )
  {
    Result().ReplaceNode(rDataObject);
  }

  CDataObject& COperation::GetResponse()
  {
    return m_tdoResponse;
  }
  
  const CDataObject& COperation::GetResponse() const
  {
    return m_tdoResponse;
  }
  
  void COperation::SetResponse(staff::CDataObject& rdoResponse)
  {
    m_tdoResponse = rdoResponse;
  }

  void COperation::SetResultValue( const CValue& rValue )
  {
    Result().Value() = rValue;
  }

  bool COperation::IsFault() const
  {
    return GetResponse().GetLocalName() == "Fault";
  }

  void COperation::SetFault( const std::string& sReason, const std::string& sFaultDetail /*= ""*/, const std::string& sFaultCode /*= ""*/ )
  {
    CDataObject& rFault = GetResponse();

    rFault.SetLocalName("Fault");
    rFault.CreateChildOnce("Reason").CreateChildOnce("Text").Value() = sReason;
    rFault.CreateChildOnce("Code").CreateChildOnce("Value").Value() = sFaultCode;
    rFault.CreateChildOnce("Detail").CreateChildOnce("Exception").Value() = sFaultDetail;
  }

  void COperation::ResetFault()
  {
    CDataObject& rFault = GetResponse();

    rFault.SetLocalName(Request().GetLocalName() + "Result");
    rFault.RemoveChildByLocalName("Reason");
    rFault.RemoveChildByLocalName("Code");
    rFault.RemoveChildByLocalName("Detail");
  }

  void COperation::SetUserFault( CDataObject& rDataObjectFault )
  {
    SetResponse(rDataObjectFault);
    GetResponse().SetLocalName("Fault");
  }

  std::string COperation::GetFaultString() const
  {
    std::string sResult;
    const std::string& sReason = GetFaultReason();
    const std::string& sCode = GetFaultCode();
    const std::string& sDetail = GetFaultDetail();
    
    if (sReason != "")
    {
      sResult += "Reason: " + sReason + "\n";
    }
    if (sCode != "")
    {
      sResult += "Code: " + sCode + "\n";
    }
    if (sDetail != "")
    {
      sResult += "Detail: " + sDetail + "\n";
    }

    return sResult;
  }

  std::string COperation::GetFaultReason() const
  {
    static const std::string sEmpty = "";

    CDataObject::ConstIterator itFind = Result().FindChildByLocalName("Reason");
    if (itFind == Result().End())
    {
      itFind = Result().FindChildByLocalName("faultstring");
      if (itFind == Result().End())
      {
        return sEmpty;
      }

      return itFind->GetText();
    }

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Text");
    if (itFindValue == itFind->End())
    {
      return sEmpty;
    }

    return itFindValue->GetText();
  }

  std::string COperation::GetFaultCode() const
  {
    static const std::string sEmpty = "";

    CDataObject::ConstIterator itFind = Result().FindChildByLocalName("Code");
    if (itFind == Result().End())
    {
      itFind = Result().FindChildByLocalName("faultcode");
      if (itFind == Result().End())
      {
        return sEmpty;
      }

      return itFind->GetText();
    } 

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Value");
    if (itFindValue == itFind->End())
    {
      return sEmpty;
    }

    return itFindValue->GetText();
  }

  std::string COperation::GetFaultDetail() const
  {
    static const std::string sEmpty = "";

    CDataObject::ConstIterator itFind = Result().FindChildByLocalName("Detail");
    if (itFind == Result().End())
    {
      itFind = Result().FindChildByLocalName("detail");
      if (itFind == Result().End())
      {
        return sEmpty;
      }
    }

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Exception");
    if (itFindValue == itFind->End())
    {
      return sEmpty;
    }

    return itFindValue->GetText();
  }

  const CDataObject COperation::GetFault() const
  {
    return m_tdoResponse.GetChildByLocalName("Fault");
  }

  void COperation::PrepareResult()
  {
//    Result().NamespaceList() = Request().NamespaceList();

    if (!IsFault())
    {
      if(m_tdoResponse.GetLocalName() == "")
      {
        m_tdoResponse.SetLocalName(m_tdoRequest.GetLocalName() + "Result");
      }
    }

    if (m_tdoResponse.GetPrefix() == "" || m_tdoResponse.GetNamespaceUri() == "")
    {
      std::string sPrefix = m_tdoRequest.GetPrefix();
      CQName tqnResult(m_tdoResponse.GetLocalName(), m_tdoRequest.GetNamespaceUri(), sPrefix.size() != 0 ? sPrefix : "ns");
      m_tdoResponse.SetQName(tqnResult);
    }
  }
}
