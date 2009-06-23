#include "QName.h"
#include "Value.h"
#include "Operation.h"

namespace staff
{
  COperation::COperation(const std::string& sName /*= ""*/, const std::string& sResponseName /*= ""*/):
    m_tdoRequest(sName), m_tdoResult(sResponseName)
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
    m_tdoResult.SetLocalName(sResponseName);
  }

  const std::string COperation::GetResponseName() const
  {
    return m_tdoRequest.GetLocalName();
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
    return m_tdoResult;
  }

  CDataObject& COperation::Result()
  {
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
    Result() = rDataObject;
  }

  void COperation::SetResultValue( const CValue& rValue )
  {
    Result().Value() = rValue;
  }

  bool COperation::IsFault() const
  {
    return Result().GetLocalName() == "Fault";
  }

  void COperation::SetFault( const std::string& sReason, const std::string& sFaultDetail /*= ""*/, const std::string& sFaultCode /*= ""*/ )
  {
    CDataObject& rFault = Result();

    rFault.SetLocalName("Fault");
    rFault.CreateChildOnce("Reason").CreateChildOnce("Text").Value() = sReason;
    rFault.CreateChildOnce("Code").CreateChildOnce("Value").Value() = sFaultCode;
    rFault.CreateChildOnce("Detail").CreateChildOnce("Exception").Value() = sFaultDetail;
  }

  void COperation::ResetFault()
  {
    CDataObject& rFault = Result();

    rFault.SetLocalName(Request().GetLocalName() + "Result");
    rFault.RemoveChildByLocalName("Reason");
    rFault.RemoveChildByLocalName("Code");
    rFault.RemoveChildByLocalName("Detail");
  }

  void COperation::SetUserFault( CDataObject& rDataObjectFault )
  {
    Result() = rDataObjectFault;
    Result().SetLocalName("Fault");
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
    return m_tdoResult.GetChildByLocalName("Fault");
  }

  void COperation::PrepareResult()
  {
//    Result().NamespaceList() = Request().NamespaceList();

    if (!IsFault())
    {
      if(m_tdoResult.GetLocalName() == "")
      {
        m_tdoResult.SetLocalName(m_tdoRequest.GetLocalName() + "Result");
      }
    }

    if (m_tdoResult.GetPrefix() == "" || m_tdoResult.GetNamespaceUri() == "")
    {
      CQName tqnResult(m_tdoResult.GetLocalName(), m_tdoRequest.GetNamespaceUri(), m_tdoRequest.GetPrefix());
      m_tdoResult.SetQName(tqnResult);
    }
  }
}
