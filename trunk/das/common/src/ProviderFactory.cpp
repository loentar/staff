#include <rise/common/MutablePtr.h>
#include <rise/tools/FileFind.h>
#include <rise/plugin/PluginManager.h>
#include <staff/common/Runtime.h>
#include "ProviderFactory.h"

namespace staff
{
namespace das
{
  class ProviderFactory::ProviderFactoryImpl
  {
  public:
    typedef std::map<std::string, ProviderAllocator*> ProviderAllocatorMap;

  public:
    void Init()
    {
      const std::string sProvidersDir = CRuntime::Inst().GetComponentHome("staff.das") + "/providers";
      StringList lsProviderDirs;

      // find directories with providers
      rise::CFileFind::Find(sProvidersDir, lsProviderDirs, "*", rise::CFileFind::EFA_DIR);
      if (lsProviderDirs.size() == 0)
      {
        rise::LogDebug() << "providers is not found";
      }

      for (StringList::const_iterator itDir = lsProviderDirs.begin();
                itDir != lsProviderDirs.end(); ++itDir )
      {
        // finding libraries with providers
        StringList lsProvidersLibs;
        StringList lsProvidersNames;
        std::string sProviderDir = sProvidersDir + RISE_PATH_SEPARATOR + *itDir + RISE_PATH_SEPARATOR;
  #ifdef __linux__
        rise::CFileFind::Find(sProviderDir, lsProvidersLibs, "*.so", rise::CFileFind::EFA_FILE);
  #endif
  #ifdef WIN32
        rise::CFileFind::Find(sProviderDir, lsProvidersLibs, "*.dll", rise::CFileFind::EFA_FILE);
  #endif
        for (StringList::const_iterator itProvider = lsProvidersLibs.begin();
                itProvider != lsProvidersLibs.end(); ++itProvider )
        {
          try
          {
            // loading provider
            rise::LogDebug() << "Loading DAS provider: " << (sProviderDir + *itProvider);
            ProviderAllocator* pAllocator = m_tPluginManager.LoadPlugin(sProviderDir + *itProvider, true);
            RISE_ASSERTS(pAllocator, "Can't get allocator for provider: " + *itProvider);

            pAllocator->GetProvidersList(lsProvidersNames);
            for (StringList::const_iterator itProviderName = lsProvidersNames.begin();
                    itProviderName != lsProvidersNames.end(); ++itProviderName )
            {
              rise::LogDebug1() << "Setting DAS provider: " << *itProviderName;
              m_mAllocators[*itProviderName] = pAllocator;
            }
          }
          catch(const rise::CException& rEx)
          {
            rise::LogWarning() << "Can't load provider: " << (sProviderDir + *itProvider) << ": " << rEx.GetString();
            continue;
          }
        }
      }
    }

  public:
    ProviderAllocatorMap m_mAllocators;
    rise::plugin::CPluginManager<ProviderAllocator> m_tPluginManager;
  };


  ProviderFactory::ProviderFactory()
  {
    m_pImpl = new ProviderFactoryImpl;
  }

  ProviderFactory::~ProviderFactory()
  {
    delete m_pImpl;
  }

  ProviderFactory& ProviderFactory::Inst()
  {
    if (!m_pInst)
    {
      m_pInst = new ProviderFactory;
      m_pInst->m_pImpl->Init();
    }

    return *m_pInst;
  }

  void ProviderFactory::FreeInst()
  {
    if (m_pInst)
    {
      delete m_pInst;
      m_pInst = NULL;
    }
  }

  void ProviderFactory::GetProviders(StringList& rlsProviders)
  {
    rlsProviders.clear();
    for (ProviderFactoryImpl::ProviderAllocatorMap::const_iterator itProvider = m_pImpl->m_mAllocators.begin();
          itProvider != m_pImpl->m_mAllocators.end(); ++itProvider)
    {
      rlsProviders.push_back(itProvider->first);
    }
  }

  PProvider ProviderFactory::Allocate(const std::string& sProvider)
  {
    ProviderFactoryImpl::ProviderAllocatorMap::iterator itProvider = m_pImpl->m_mAllocators.find(sProvider);
    RISE_ASSERTS(itProvider != m_pImpl->m_mAllocators.end(), "Can't get allocator for " + sProvider);
    return itProvider->second->Allocate(sProvider);
  }


  ProviderFactory* ProviderFactory::m_pInst = NULL;
}
}
