#include <rise/common/Log.h>
#include <rise/common/StreamBuffer.h>
#include <rise/os/ossocket.h>
#include <rise/threading/CriticalSection.h>
#include <staff/common/Value.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObjectHelper.h>
#include "ServiceDispatcher.h"
#include "RemoteService.h"
#include "RemoteServiceWrapper.h"

namespace staff
{
  CRemoteServiceWrapper::CRemoteServiceWrapper(CComponent* pComponent):
    m_pComponent(pComponent)
  {
  }

  CRemoteServiceWrapper::~CRemoteServiceWrapper()
  {
  }

  const rise::CString& CRemoteServiceWrapper::GetName() const
  {
    return m_mServices.begin()->second->GetName();
  }

  const rise::CString& CRemoteServiceWrapper::GetDescr() const
  {
    return m_mServices.begin()->second->GetDescr();
  }

  CDataObject CRemoteServiceWrapper::GetOperations() const
  {
    return m_mServices.begin()->second->GetOperations();
  }

  void CRemoteServiceWrapper::Invoke( COperation& rOperation, const rise::CString& sID )
  {
    TRemoteServiceMap::iterator it = m_mServices.find(sID);
    if (it == m_mServices.end())
    {
      PRemoteService pService;
      CServiceDispatcher::Inst().GetRemoteService(GetName(), sID, pService);

      RISE_ASSERTP(pService);

      pService->Invoke(rOperation);
      return;
    }

    it->second->Invoke(rOperation);
  }

  const CComponent* CRemoteServiceWrapper::GetComponent() const
  {
    return m_pComponent;
  }

  CComponent* CRemoteServiceWrapper::GetComponent()
  {
    return m_pComponent;
  }

  void* CRemoteServiceWrapper::GetImpl( const rise::CString& sID )
  {
    return NULL;
  }

  TRemoteServiceMap& CRemoteServiceWrapper::GetServices()
  {
    return m_mServices;
  }

}
