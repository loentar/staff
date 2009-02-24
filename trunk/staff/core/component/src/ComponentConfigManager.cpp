#include <map>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/MutablePtr.h>
#include "ComponentConfig.h"
#include "ComponentConfigManager.h"

namespace staff
{
  class CComponentConfigManager::CComponentConfigManagerImpl
  {
  public:
    typedef rise::CMutablePtr<CComponentConfig> PComponentConfig;
    typedef std::map<rise::CString, PComponentConfig> TComponentConfigMap;
    TComponentConfigMap m_mConfigs;
  };

  CComponentConfigManager& CComponentConfigManager::Inst()
  {
    if (m_pInst == NULL)
    {
      m_pInst = new CComponentConfigManager;
    }

    return *m_pInst;
  }

  CComponentConfigManager::CComponentConfigManager()
  {
    m_pImpl = new CComponentConfigManagerImpl;
  }

  CComponentConfigManager::~CComponentConfigManager()
  {
    if(m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  CComponentConfig& CComponentConfigManager::GetComponentConfig( const rise::CString& sComponent, const rise::CString& sConfig /*= "config.xml"*/, bool bCreate /*= false*/ )
  {
    CComponentConfigManagerImpl::PComponentConfig& rpComponentConfig = m_pImpl->m_mConfigs[sComponent];

    if (rpComponentConfig.Get() == NULL)
    {
      rpComponentConfig = new CComponentConfig;
      rpComponentConfig->Init(sComponent, sConfig, bCreate);
    }
    return *rpComponentConfig;  }

  CComponentConfigManager* CComponentConfigManager::m_pInst = NULL;
}
