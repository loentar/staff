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
  class CComponentConfigManager::CComponentConfigManagerImpl
  {
  public:
    typedef rise::CMutablePtr<CComponentConfig> PComponentConfig;
    typedef std::map<std::string, PComponentConfig> TComponentConfigMap;
    TComponentConfigMap m_mConfigs;
  };

  CComponentConfigManager& CComponentConfigManager::Inst()
  {
    static CComponentConfigManager tInst;
    return tInst;
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

  CComponentConfig& CComponentConfigManager::GetComponentConfig( const std::string& sComponent, const std::string& sConfig /*= "config.xml"*/, bool bCreate /*= false*/ )
  {
    CComponentConfigManagerImpl::PComponentConfig& rpComponentConfig = m_pImpl->m_mConfigs[sComponent];

    if (rpComponentConfig.Get() == NULL)
    {
      rpComponentConfig = new CComponentConfig;
      rpComponentConfig->Init(sComponent, sConfig, bCreate);
    }
    return *rpComponentConfig;
  }

}
