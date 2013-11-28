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

#include <staff/utils/Log.h>
#include <staff/common/Exception.h>
#include <staff/utils/SharedPtr.h>
#include "ServiceWrapper.h"
#include "CompositeComponent.h"

namespace staff
{
  CompositeComponent::CompositeComponent()
  {
  }

  CompositeComponent::~CompositeComponent()
  {
  }

  const std::string& CompositeComponent::GetName() const
  {
    return m_sName;
  }

  void CompositeComponent::Compose(Component* pComponent)
  {
    STAFF_ASSERT_PARAM(pComponent);

    if (m_sName == "")
      m_sName = pComponent->GetName();
    else
      STAFF_ASSERT_PARAM(m_sName == pComponent->GetName());

    const ServiceWrapperMap& rmComponentServices = pComponent->GetServices();
    for (ServiceWrapperMap::const_iterator itService = rmComponentServices.begin();
      itService != rmComponentServices.end(); ++itService)
    {
      const std::pair<ServiceWrapperMap::iterator, bool>& tInsertResult = 
        m_mServices.insert(*itService);

      if (!tInsertResult.second)
        LogWarning() << "Duplicate service: \"" << itService->first 
        << "\": in component: \"" << m_sName << "\".";
    }
  }

  const ServiceWrapper* CompositeComponent::GetService(const std::string& sName) const
  {
    ServiceWrapperMap::const_iterator itService = m_mServices.find(sName);
    if (itService == m_mServices.end())
      return NULL;
    return itService->second;
  }

  ServiceWrapper* CompositeComponent::GetService(const std::string& sName)
  {
    ServiceWrapperMap::iterator itService = m_mServices.find(sName);
    if (itService == m_mServices.end())
      return NULL;
    return itService->second;
  }

  const ServiceWrapperMap& CompositeComponent::GetServices() const
  {
    return m_mServices;
  }

  void CompositeComponent::AddService(ServiceWrapper* pService)
  {
    if (pService == NULL)
      return;

    m_mServices[pService->GetName()] = pService;
  }

  void CompositeComponent::RemoveService(const std::string& sName)
  {
    m_mServices.erase(sName);
  }
}
