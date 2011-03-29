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

#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
#include "ServiceWrapper.h"
#include "CompositeComponent.h"

namespace staff
{
  CCompositeComponent::CCompositeComponent()
  {
  }

  CCompositeComponent::~CCompositeComponent()
  {
  }

  const std::string& CCompositeComponent::GetName() const
  {
    return m_sName;
  }

  void CCompositeComponent::Compose( CComponent* pComponent )
  {
    RISE_ASSERTP(pComponent);

    if (m_sName == "")
      m_sName = pComponent->GetName();
    else
      RISE_ASSERTP(m_sName == pComponent->GetName());

    const TServiceWrapperMap& rmComponentServices = pComponent->GetServices();
    for (TServiceWrapperMap::const_iterator itService = rmComponentServices.begin();
      itService != rmComponentServices.end(); ++itService)
    {
      const std::pair<TServiceWrapperMap::iterator, bool>& tInsertResult = 
        m_mServices.insert(*itService);

      if (!tInsertResult.second)
        rise::LogWarning() << "Duplicate service: \"" << itService->first 
        << "\": in component: \"" << m_sName << "\".";
    }
  }

  const CServiceWrapper* CCompositeComponent::GetService( const std::string& sName ) const
  {
    TServiceWrapperMap::const_iterator itService = m_mServices.find(sName);
    if (itService == m_mServices.end())
      return NULL;
    return itService->second;
  }

  CServiceWrapper* CCompositeComponent::GetService( const std::string& sName )
  {
    TServiceWrapperMap::iterator itService = m_mServices.find(sName);
    if (itService == m_mServices.end())
      return NULL;
    return itService->second;
  }

  const TServiceWrapperMap& CCompositeComponent::GetServices() const
  {
    return m_mServices;
  }

  void CCompositeComponent::AddService( CServiceWrapper* pService )
  {
    if (pService == NULL)
      return;

    m_mServices[pService->GetName()] = pService;
  }

  void CCompositeComponent::RemoveService( const std::string& sName )
  {
    m_mServices.erase(sName);
  }
}
