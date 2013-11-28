/*
 *  Copyright 2010 Utkin Dmitry
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

#include <staff/utils/Log.h>
#include <staff/utils/SharedPtr.h>
#include <staff/utils/File.h>
#include <staff/utils/DynamicLibrary.h>
#include <staff/utils/PluginManager.h>
#include <staff/common/Runtime.h>
#include "ProviderFactory.h"

namespace staff
{
namespace das
{
  class ProviderFactory::ProviderFactoryImpl
  {
  public:
    typedef std::map<std::string, IProviderAllocator*> ProviderAllocatorMap;

  public:
    void Init()
    {
      const std::string sProvidersDir = Runtime::Inst().GetComponentHome("staff.das") 
        + STAFF_PATH_SEPARATOR "providers";
      StringList lsProviderDirs;

      // find directories with providers
      File(sProvidersDir).List(lsProviderDirs, "*", File::AttributeDirectory);
      if (lsProviderDirs.size() == 0)
      {
        LogDebug() << "providers is not found";
      }

      for (StringList::const_iterator itDir = lsProviderDirs.begin();
                itDir != lsProviderDirs.end(); ++itDir)
      {
        // finding libraries with providers
        StringList lsProvidersLibs;
        StringList lsProvidersNames;
        const std::string& sProviderDir =
            sProvidersDir + STAFF_PATH_SEPARATOR + *itDir + STAFF_PATH_SEPARATOR;

        File(sProviderDir).List(lsProvidersLibs, "*" STAFF_LIBRARY_VEREXT, File::AttributeRegularFile);
        for (StringList::const_iterator itProvider = lsProvidersLibs.begin();
             itProvider != lsProvidersLibs.end(); ++itProvider)
        {
          const std::string& sProviderPluginPath = sProviderDir + *itProvider;
          try
          {
            // loading provider
            LogDebug() << "Loading DAS provider: " << sProviderPluginPath;
            IProviderAllocator* pAllocator = m_tPluginManager.Load(sProviderPluginPath, true);
            STAFF_ASSERT(pAllocator, "Can't get allocator for provider: " + *itProvider);

            pAllocator->GetProvidersList(lsProvidersNames);
            for (StringList::const_iterator itProviderName = lsProvidersNames.begin();
                    itProviderName != lsProvidersNames.end(); ++itProviderName)
            {
              LogDebug1() << "Setting DAS provider: " << *itProviderName;
              m_mAllocators[*itProviderName] = pAllocator;
            }
          }
          catch (const Exception& rEx)
          {
            LogWarning() << "Can't load provider: " << sProviderPluginPath << ": " << rEx.what();
            continue;
          }
        }
      }
    }

  public:
    ProviderAllocatorMap m_mAllocators;
    PluginManager<IProviderAllocator> m_tPluginManager;
  };


  ProviderFactory::ProviderFactory()
  {
    m_pImpl = new ProviderFactoryImpl;
    try
    {
      m_pImpl->Init();
    }
    STAFF_CATCH_ALL
  }

  ProviderFactory::~ProviderFactory()
  {
    delete m_pImpl;
  }

  ProviderFactory& ProviderFactory::Inst()
  {
    static ProviderFactory tInst;
    return tInst;
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
    STAFF_ASSERT(itProvider != m_pImpl->m_mAllocators.end(), "Can't get allocator for " + sProvider);
    return itProvider->second->Allocate(sProvider);
  }

}
}
