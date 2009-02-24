#include <rise/common/Log.h>
#include <rise/common/StreamBuffer.h>
#include <rise/os/ossocket.h>
#include <rise/threading/CriticalSection.h>
#include <staff/common/Value.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObjectHelper.h>
#include <staff/component/Service.h>
#include "RemoteService.h"

namespace staff
{
  class CRemoteService::CRemoteServiceImpl
  {
  public:
    void GetServiceInfo()
    {
      if (m_sName == "")
      {
        COperation tOperation("GetServiceInfo");
        CDataObject& rResult = tOperation.Result();
        // request info from service;
        m_pSelf->Invoke(tOperation);

        m_sName = const_cast<const CDataObject&>(rResult)["Name"].AsString();
        if(m_sName == "")
        {
          rise::LogError() << "Failed to get Service Name";
          m_pSelf->Close();
          return;
        }

        m_sSessionId = const_cast<const CDataObject&>(rResult)["SessionId"].AsString();

        m_sDescr = const_cast<const CDataObject&>(rResult)["Description"].AsString();

        m_tOperations = rResult.GetChildByLocalName("Operations").DetachNode();
        if(m_tOperations.GetLocalName() == "")
          rise::LogError() << "Failed to get Operations";
      }
    }

    CRemoteServiceImpl():
      m_pSelf(NULL),
      m_pRemoteServiceWrapper(NULL)
    {
    }

  public:
    CRemoteService* m_pSelf;
    rise::threading::CCriticalSection m_tCs;
    rise::CStreamBuffer m_tRecvBuffer;
    rise::CStreamBuffer m_tSendBuffer;
    rise::CString m_sName;
    rise::CString m_sSessionId;
    rise::CString m_sDescr;
    CDataObject m_tOperations;
    CRemoteServiceWrapper* m_pRemoteServiceWrapper;
  };

  CRemoteService::CRemoteService():
    m_pImpl(new CRemoteServiceImpl)
  {
    m_pImpl->m_pSelf = this;
  }

  CRemoteService::~CRemoteService()
  {
    if (m_pImpl != NULL)
      delete m_pImpl;
  }

  const rise::CString& CRemoteService::GetName() const
  {
    if (m_pImpl->m_sName == "")
      m_pImpl->GetServiceInfo();

    return m_pImpl->m_sName;
  }

  const rise::CString& CRemoteService::GetID() const
  {
    if (m_pImpl->m_sSessionId == "")
      m_pImpl->GetServiceInfo();

    return m_pImpl->m_sSessionId;
  }

  const rise::CString& CRemoteService::GetDescr() const
  {
    if (m_pImpl->m_sDescr == "")
      m_pImpl->GetServiceInfo();

    return m_pImpl->m_sDescr;
  }

  const CDataObject& CRemoteService::GetOperations() const
  {
    if (m_pImpl->m_tOperations.GetLocalName() == "")
      m_pImpl->GetServiceInfo();

    return m_pImpl->m_tOperations;
  }

  void CRemoteService::Invoke( COperation& rOperation )
  {
    rise::threading::CScopedCriticalSection tScs(m_pImpl->m_tCs);
    m_pImpl->m_tSendBuffer.Reset();
    m_pImpl->m_tSendBuffer << rOperation.Request();

#if defined DEBUG || defined _DEBUG
    rise::LogDebug2() << "invoking operation";
#endif

    // отправка запроса
    {
      rise::ulong ulSent = 0;
      rise::ulong ulBuffPos = 0;
      rise::ulong ulDataSize = m_pImpl->m_tSendBuffer.GetSize();

#if defined DEBUG || defined _DEBUG
      rise::LogDebug2() << "sending request";
#endif
      while (ulDataSize > 0)
      {
#if defined DEBUG || defined _DEBUG
        rise::LogDebug2() << "sending part";
#endif
        Send(m_pImpl->m_tSendBuffer.GetData() + ulBuffPos, ulDataSize, &ulSent);
        ulBuffPos += ulSent;
        ulDataSize -= ulSent;
      }
    }

#if defined DEBUG || defined _DEBUG
    rise::LogDebug2() << "receiving result";
#endif
    // принятие ответа
    {
      bool bRcvd = true;
      rise::ulong ulCurrentSize = 0;
      rise::ulong ulReceived = m_pImpl->m_tRecvBuffer.GetSize();
      m_pImpl->m_tRecvBuffer.Normalize();
      while (bRcvd)
      {
        if(memchr(m_pImpl->m_tRecvBuffer.GetData() + ulCurrentSize, '\0', ulReceived) != NULL)
          break;

        ulCurrentSize += ulReceived;

#if defined DEBUG || defined _DEBUG
        rise::LogDebug2() << "waiting for recv";
#endif

        WaitForData();

#if defined DEBUG || defined _DEBUG
        rise::LogDebug2() << "data arrived";
#endif
        bRcvd = Recv(m_pImpl->m_tRecvBuffer, true, &ulReceived);

#if defined DEBUG || defined _DEBUG
        rise::LogDebug3() << "received part: " << ulReceived << " bytes/(" << (ulCurrentSize + ulReceived) << "):\n\n" << 
          std::string(reinterpret_cast<const char*>(m_pImpl->m_tRecvBuffer.GetData()), ulReceived);
#endif
      } // while (bRcvd)

      if (ulReceived == 0)
      {
        Close();
        rise::LogDebug1() << "Connection Closed";
        return;
      }
    }

    m_pImpl->m_tRecvBuffer >> rOperation.Result();
  }

  bool CRemoteService::CheckConnection()
  {
    static char tBuf[32];
    int nRcvd = recv(GetHandle(), tBuf, sizeof(tBuf), MSG_PEEK);
    int nErr = GetError();
    
    rise::LogDebug2() << "rcvd: " << nRcvd << "; nErr = " << nErr << "(" << strerror(nErr) << "):";
#if defined DEBUG || defined _DEBUG
    if(nRcvd > 0 && nErr == 0)
      rise::LogDebug2() << std::string(tBuf, nRcvd);
#endif

    // ECONNRESET
    return !((nRcvd == SOCKET_ERROR) && (nErr != EWOULDBLOCK) && (nErr != EMSGSIZE) || (nRcvd == 0));
  }

  void CRemoteService::CreateServiceID( const rise::CString& sID )
  {
    COperation tOperation("CreateServiceID");
    tOperation.AddParameter("sSessionId", sID);
    Invoke(tOperation);
  }

  CRemoteServiceWrapper* CRemoteService::GetRemoteServiceWrapper()
  {
    return m_pImpl->m_pRemoteServiceWrapper;
  }

  void CRemoteService::SetRemoteServiceWrapper( CRemoteServiceWrapper* pRemoteServiceWrapper )
  {
    m_pImpl->m_pRemoteServiceWrapper = pRemoteServiceWrapper;
  }

  void CRemoteService::Init()
  {
    SetNonBlockingMode();
  }

}
