#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
#include "Service.h"
#include "CompositeComponent.h"

namespace staff
{
  CCompositeComponent::CCompositeComponent()
  {
  }

  CCompositeComponent::~CCompositeComponent()
  {
  }

  const rise::CString& CCompositeComponent::GetName() const
  {
    return m_sName;
  }

  void CCompositeComponent::Compose( CComponent* pComponent )
  {
    RISE_ASSERTP(pComponent);
  
    if (m_sName == "") // инициализация имени компонента
      m_sName = pComponent->GetName();
    else
      RISE_ASSERTP(m_sName == pComponent->GetName());

    const TServiceMap& rmComponentServices = pComponent->GetServices();
    for (TServiceMap::const_iterator itService = rmComponentServices.begin();
      itService != rmComponentServices.end(); ++itService)
    {
      const std::pair<TServiceMap::iterator, bool>& tInsertResult = 
        m_mServices.insert(*itService);

      if (!tInsertResult.second)
        rise::LogWarning() << "Duplicate service: \"" << itService->first 
        << "\": in component: \"" << m_sName << "\".";
    }
  }

  const CService* CCompositeComponent::GetService( const rise::CString& sName ) const
  {
#ifdef DEBUG
    for (TServiceMap::const_iterator itService = m_mServices.begin();
      itService != m_mServices.end(); ++itService)
    {
      rise::LogNotice() << "service: \"" << itService->first << "\" / \"" << sName << "\": component: \"" << m_sName << "\"";
    }
#endif
  
    TServiceMap::const_iterator itService = m_mServices.find(sName);
    if (itService == m_mServices.end())
      return NULL;
    return itService->second;
  }

  CService* CCompositeComponent::GetService( const rise::CString& sName )
  {
#ifdef DEBUG
    for (TServiceMap::const_iterator itService = m_mServices.begin();
      itService != m_mServices.end(); ++itService)
    {
      rise::LogNotice() << "service: \"" << itService->first << "\" / \"" << sName << "\": component: \"" << m_sName << "\"";
    }
#endif

    TServiceMap::iterator itService = m_mServices.find(sName);
    if (itService == m_mServices.end())
      return NULL;
    return itService->second;
  }

  const TServiceMap& CCompositeComponent::GetServices() const
  {
    return m_mServices;
  }

  CDataObject& CCompositeComponent::GetProperties()
  {
    return m_doProperties;
  }

  void CCompositeComponent::AddService( CService* pService )
  {
    if (pService == NULL)
      return;

    m_mServices[pService->GetName()] = pService;
  }

  void CCompositeComponent::RemoveService( const rise::CString& sName )
  {
    m_mServices.erase(sName);
  }
}
