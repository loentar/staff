#include <rise/common/MutablePtr.h>
#include <staff/component/ServiceWrapperFactory.h>
#include <staff/component/ServiceWrapper.h>
#include "ProviderComponent.h"

namespace staff
{
namespace das
{
  ProviderComponent::ProviderComponent(const std::string& sName):
    m_sName(sName)
  {
  }

  void ProviderComponent::AddServiceWrapper(const std::string& sServiceName, PServiceWrapper& rpServiceWrapper)
  {
    m_mServices[sServiceName] = rpServiceWrapper;
    CServiceWrapperFactory::Inst().RegisterServiceWrapper(sServiceName, rpServiceWrapper);
  }

  const std::string& ProviderComponent::GetName() const
  {
    return m_sName;
  }

  const CServiceWrapper* ProviderComponent::GetService(const std::string& sService) const
  {
    staff::TServiceWrapperMap::const_iterator itService = m_mServices.find(sService);
    if (itService == m_mServices.end())
    {
      return NULL;
    }

    return itService->second;
  }

  CServiceWrapper* ProviderComponent::GetService(const std::string& sService)
  {
    staff::TServiceWrapperMap::iterator itService = m_mServices.find(sService);
    if (itService == m_mServices.end())
    {
      return NULL;
    }

    return itService->second;
  }

  const TServiceWrapperMap& ProviderComponent::GetServices() const
  {
    return m_mServices;
  }

}
}
