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

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/Log.h>
#include "Service.h"
#include "SharedContext.h"

namespace staff
{
  CSharedContext::CSharedContext()
  {
  }

  CSharedContext::~CSharedContext()
  {
  }

  CSharedContext& CSharedContext::Inst()
  {
    if (m_pInst == NULL)
      m_pInst = new CSharedContext;

    return *m_pInst;
  }

  void CSharedContext::AddComponent( CComponent* pComponent )
  {
    TCompositeComponentMap::iterator itFind = m_mComponents.find(pComponent->GetName());
    if (itFind == m_mComponents.end())
    {
      const std::pair<TCompositeComponentMap::iterator, bool>& tInsertResult = 
        m_mComponents.insert(TCompositeComponentMap::value_type(pComponent->GetName(), PCompositeComponent(new CCompositeComponent)));
      tInsertResult.first->second->Compose(pComponent);
    } else
      itFind->second->Compose(pComponent);
  }

  CCompositeComponent* CSharedContext::GetComponent( const std::string& sName )
  {
    TCompositeComponentMap::iterator itFind = m_mComponents.find(sName);
    if (itFind == m_mComponents.end())
      return NULL;
    return itFind->second;
  }

  const CCompositeComponent* CSharedContext::GetComponent( const std::string& sName ) const
  {
    TCompositeComponentMap::const_iterator itFind = m_mComponents.find(sName);
    if (itFind == m_mComponents.end())
      return NULL;
    return itFind->second;
  }

  const TCompositeComponentMap& CSharedContext::GetComponents() const
  {
    return m_mComponents;
  }

  const CService* CSharedContext::GetService( const std::string& sName ) const
  {
    std::string sComponentName;
    std::string sServiceName;
    std::string::size_type nPos = sName.find_last_of('.');
    if (nPos != std::string::npos)
    {
      sComponentName.assign(sName, 0, nPos - 1);
      sServiceName.assign(sName, nPos + 1, std::string::npos);
    } else
      sServiceName = sName;

    const CCompositeComponent* pComponent = GetComponent(sComponentName);
    if (pComponent == NULL)
    {
      pComponent = GetComponent("");
      if (pComponent == NULL)
      {
        return NULL;
      }
    }

    return pComponent->GetService(sName);
  }

  CService* CSharedContext::GetService( const std::string& sName )
  {
    std::string sComponentName;
    std::string sServiceName;
    std::string::size_type nPos = sName.find_last_of('.');
    if (nPos != std::string::npos)
    {
      sComponentName.assign(sName, 0, nPos);
      sServiceName.assign(sName, nPos + 1, std::string::npos);
    } else
      sServiceName = sName;

    CCompositeComponent* pComponent = GetComponent(sComponentName);
    if (pComponent == NULL)
    {
      pComponent = GetComponent("");
      if (pComponent == NULL)
      {
        return NULL;
      }
    }

    return pComponent->GetService(sName);
  }

  TServiceMap CSharedContext::GetServices() const
  {
    TServiceMap mServices;
    for(TCompositeComponentMap::const_iterator itComponent = m_mComponents.begin();
          itComponent != m_mComponents.end(); ++itComponent)
    {
      const TServiceMap& rmNewComponent = itComponent->second->GetServices();
      for (TServiceMap::const_iterator itService = rmNewComponent.begin();
        itService != rmNewComponent.end(); ++itService)
      {
        const std::pair<TServiceMap::iterator, bool>& tInsertResult = 
          mServices.insert(TServiceMap::value_type(itService->first, itService->second));
        if (!tInsertResult.second)
          rise::LogWarning() << "Duplicate service: \"" << itService->first 
          << "\": in component: \"" << itComponent->first << "\".";
      }
    }

    return mServices;
  }

  void CSharedContext::Clear()
  {
    m_mComponents.clear();
  }

  CSharedContext* CSharedContext::m_pInst = NULL;
}

