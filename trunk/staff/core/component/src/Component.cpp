/*
 *  Copyright 2011 Utkin Dmitry
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

#include <staff/utils/SharedPtr.h>
#include "ServiceWrapper.h"
#include "Component.h"

namespace staff
{

  Component::~Component()
  {
  }

  const ServiceWrapper* Component::GetService(const std::string& sService) const
  {
    staff::ServiceWrapperMap::const_iterator itService = m_mServices.find(sService);
    if (itService == m_mServices.end())
    {
      return NULL;
    }

    return itService->second;
  }

  ServiceWrapper* Component::GetService(const std::string& sService)
  {
    staff::ServiceWrapperMap::iterator itService = m_mServices.find(sService);
    if (itService == m_mServices.end())
    {
      return NULL;
    }

    return itService->second;
  }

  const ServiceWrapperMap& Component::GetServices() const
  {
    return m_mServices;
  }

  void Component::RegisterService(const std::string& sService, ServiceWrapper* pWrapper)
  {
    m_mServices[sService] = pWrapper;
  }

}
