/*
 *  Copyright 2009 Utkin Dmitry
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

#include <map>
#include <staff/utils/SharedPtr.h>
#include <staff/common/Exception.h>
#include <staff/common/IService.h>
#include "ComponentConfig.h"
#include "ComponentConfigManager.h"

namespace staff
{
  class ComponentConfigManager::ComponentConfigManagerImpl
  {
  public:
    typedef SharedPtr<ComponentConfig> PComponentConfig;
    typedef std::map<std::string, PComponentConfig> ComponentConfigMap;
    ComponentConfigMap m_mConfigs;
  };

  ComponentConfigManager& ComponentConfigManager::Inst()
  {
    static ComponentConfigManager tInst;
    return tInst;
  }

  ComponentConfigManager::ComponentConfigManager()
  {
    m_pImpl = new ComponentConfigManagerImpl;
  }

  ComponentConfigManager::~ComponentConfigManager()
  {
    if(m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  ComponentConfig& ComponentConfigManager::GetComponentConfig(const std::string& sComponent,
                                                              const std::string& sConfig /*= "config.xml"*/,
                                                              bool bCreate /*= true*/)
  {
    ComponentConfigManagerImpl::PComponentConfig& rpComponentConfig = m_pImpl->m_mConfigs[sComponent];

    if (!rpComponentConfig)
    {
      rpComponentConfig = new ComponentConfig;
      rpComponentConfig->Init(sComponent, sConfig, bCreate);
    }
    return *rpComponentConfig;
  }

  ComponentConfig& ComponentConfigManager::GetComponentConfig(const IService* pService,
                                                              const std::string& sSuffix /*= ".xml"*/,
                                                              bool bCreate /*= true*/)
  {
    STAFF_ASSERT_PARAM(pService);

    const std::string& sServiceName = pService->GetServiceName();
    std::string sComponent;
    std::string sConfig;
    std::string::size_type nPos = sServiceName.find_last_of('.');
    if (nPos != std::string::npos)
    {
      sComponent = sServiceName.substr(0, nPos);
      sConfig = sServiceName.substr(nPos + 1) + sSuffix;
    }
    else
    {
      sComponent = "default";
      sConfig = sServiceName + sSuffix;
    }

    return GetComponentConfig(sComponent, sConfig, bCreate);
  }

}
