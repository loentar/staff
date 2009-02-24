#include "QName.h"
#include "Value.h"
#include "Operation.h"

namespace staff
{
  COperation::COperation(const rise::CString& sName):
    m_tdoRequest(sName), m_tdoResult(sName + "Result")
  {
  }

  COperation::~COperation()
  {
  }

  void COperation::SetName( const rise::CString& sName )
  {
    m_tdoRequest.SetLocalName(sName);
    m_tdoResult.SetLocalName(sName + "Result");
  }

  const rise::CString COperation::GetName() const
  {
    return m_tdoRequest.GetLocalName();
  }

  void COperation::AddParameter( const rise::CString& sName, const CValue& tValue )
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

  void COperation::SetFault( const rise::CString& sReason, const rise::CString& sFaultDetail /*= ""*/, const rise::CString& sFaultCode /*= ""*/ )
  {
    CDataObject& rFault = Result();

    rFault.SetLocalName("Fault");

//     rFault.CreateChildOnce("faultstring").Value() = sReason;
//     rFault.CreateChildOnce("faultcode").Value() = sFaultCode;
//     rFault.CreateChildOnce("detail").CreateChildOnce("Exception").Value() = sFaultDetail;
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

  rise::CString COperation::GetFaultString() const
  {
    rise::CString sResult;
    const rise::CString& sReason = GetFaultReason();
    const rise::CString& sCode = GetFaultCode();
    const rise::CString& sDetail = GetFaultDetail();
    
    if (sReason != "")
      sResult += "Reason: " + sReason + "\n";
    if (sCode != "")
      sResult += "Code: " + sCode + "\n";
    if (sDetail != "")
      sResult += "Detail: " + sDetail + "\n";

    return sResult;
  }

  rise::CString COperation::GetFaultReason() const
  {
    static const rise::CString sEmpty = "";

    CDataObject::ConstIterator itFind = Result().FindChildByLocalName("Reason");
    if (itFind == Result().End())
    {
      itFind = Result().FindChildByLocalName("faultstring");
      if (itFind == Result().End())
        return sEmpty;

      return itFind->GetText();
    }

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Text");
    if (itFindValue == itFind->End())
      return sEmpty;

    return itFindValue->GetText();
  }

  rise::CString COperation::GetFaultCode() const
  {
    static const rise::CString sEmpty = "";

    CDataObject::ConstIterator itFind = Result().FindChildByLocalName("Code");
    if (itFind == Result().End())
    {
      itFind = Result().FindChildByLocalName("faultcode");
      if (itFind == Result().End())
        return sEmpty;

      return itFind->GetText();
    } 

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Value");
    if (itFindValue == itFind->End())
      return sEmpty;

    return itFindValue->GetText();
  }

  rise::CString COperation::GetFaultDetail() const
  {
    static const rise::CString sEmpty = "";

    CDataObject::ConstIterator itFind = Result().FindChildByLocalName("Detail");
    if (itFind == Result().End())
    {
      itFind = Result().FindChildByLocalName("detail");
      if (itFind == Result().End())
        return sEmpty;
    }

    CDataObject::ConstIterator itFindValue = itFind->FindChildByLocalName("Exception");
    if (itFindValue == itFind->End())
      return sEmpty;

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
      if(m_tdoResult.GetLocalName() == "Result" || m_tdoResult.GetLocalName() == "")
        m_tdoResult.SetLocalName(m_tdoRequest.GetLocalName() + "Result");
    }

    if (m_tdoResult.GetPrefix() == "" || m_tdoResult.GetNamespaceUri() == "")
    {
      CQName tqnResult(m_tdoResult.GetLocalName(), m_tdoRequest.GetNamespaceUri(), m_tdoRequest.GetPrefix());
      m_tdoResult.SetQName(tqnResult);
    }
  }
}
