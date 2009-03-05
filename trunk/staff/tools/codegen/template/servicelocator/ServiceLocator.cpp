#include <map>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
#include <staff/component/Service.h>
#include <staff/component/SharedContext.h>
#foreach $(Project.Interfaces)
#foreach $(Interface.Classes)
#include "$(Class.ServiceName)Proxy.h"
#end
#end
#include "ServiceLocator.h"

namespace staff
{
  class CServiceLocator::CServiceLocatorImpl
  {
  public:
    typedef std::map<rise::CString, void*> TRemoteServicesMap;
    TRemoteServicesMap m_mRemoteServices;

    void* CreateRemoteServiceProxy(staff::CService* pService, const rise::CString& sID)
    {
      const rise::CString& sName = pService->GetName();
#foreach $(Project.Interfaces)
#foreach $(Interface.Classes)
      if (sName == "$(Class.ServiceNsName)")
        return new $(Class.NsName)Proxy(pService, sID);
#end
#end
      RISE_THROWS(rise::CLogicNoItemException, "Cannot create C" + sName + "Proxy");
    }
    
    void UpdateRemoteServiceProxy(void* pProxy, staff::CService* pService)
    {
      const rise::CString& sName = pService->GetName();
#foreach $(Project.Interfaces)
#foreach $(Interface.Classes)
      if (sName == "$(Class.ServiceNsName)")
      {
        reinterpret_cast< $(Class.NsName)Proxy* >(pProxy)->SetService(pService);
        return;
      }
#end
#end
      RISE_THROWS(rise::CLogicNoItemException, "Cannot Update C" + sName + "Proxy");
    }
    
    ~CServiceLocatorImpl()
    {
      for(TRemoteServicesMap::iterator it = m_mRemoteServices.begin();
            it != m_mRemoteServices.end(); ++it)
      {
        if(it->second != NULL)
        {
          std::string::size_type nPos = it->first.find(':');
          std::string sName = (nPos != std::string::npos) ? it->first.substr(0, nPos) : it->first;
#foreach $(Project.Interfaces)
#foreach $(Interface.Classes)
          if (sName == "$(Class.ServiceNsName)")
          {
            delete reinterpret_cast< $(Class.NsName)Proxy* >(it->second);
            continue;
          }
#end
#end
        }
      }
      
      m_mRemoteServices.clear();
    }
  };

  CServiceLocator::CServiceLocator():
    m_pImpl(new CServiceLocatorImpl)
  {
  }

  CServiceLocator::~CServiceLocator()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  void* CServiceLocator::LocateService(const rise::CString& sServiceName, const rise::CString& sID)
  {
    staff::CService* pService = staff::CSharedContext::Inst().GetService(sServiceName);
    RISE_ASSERTES(pService != NULL, rise::CLogicNoItemException, "Service [" + sServiceName + "] with id [" + sID + "] not found");

    void* pServiceImpl = pService->GetImpl(sID);
    if (pServiceImpl == NULL) // remote service trying to find in map
    {
      CServiceLocatorImpl::TRemoteServicesMap::iterator itFind = m_pImpl->m_mRemoteServices.find(sServiceName + ":" + sID);
      if (itFind == m_pImpl->m_mRemoteServices.end())
      {
        std::pair<CServiceLocatorImpl::TRemoteServicesMap::iterator, bool> tInsertResult = 
          m_pImpl->m_mRemoteServices.insert(CServiceLocatorImpl::TRemoteServicesMap::value_type(sServiceName + ":" + sID, m_pImpl->CreateRemoteServiceProxy(pService, sID)));
        return tInsertResult.first->second;
      }

      m_pImpl->UpdateRemoteServiceProxy(itFind->second, pService);
      return itFind->second;
    }
    
    return pServiceImpl;
  }
  
  CServiceLocator& CServiceLocator::Inst()
  {
    if(m_pInst == NULL)
      m_pInst = new CServiceLocator;
    
    return *m_pInst;
  }

  CServiceLocator* CServiceLocator::m_pInst;
}
