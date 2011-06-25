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
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/MutablePtr.h>
#include "ComponentConfig.h"
#include "ComponentConfigManager.h"

namespace staff
{
  class ComponentConfigManager::ComponentConfigManagerImpl
  {
  public:
    typedef rise::CMutablePtr<ComponentConfig> PComponentConfig;
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

  ComponentConfig& ComponentConfigManager::GetComponentConfig( const std::string& sComponent, const std::string& sConfig /*= "config.xml"*/, bool bCreate /*= false*/ )
  {
    ComponentConfigManagerImpl::PComponentConfig& rpComponentConfig = m_pImpl->m_mConfigs[sComponent];

    if (rpComponentConfig.Get() == NULL)
    {
      rpComponentConfig = new ComponentConfig;
      rpComponentConfig->Init(sComponent, sConfig, bCreate);
    }
    return *rpComponentConfig;
  }

}
