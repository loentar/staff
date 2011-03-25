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
#include "ServiceWrapper.h"
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
    {
      itFind->second->Compose(pComponent);
    }


    TServiceWrapperMap::const_iterator itExistingWrapper;
    const TServiceWrapperMap& rmWrappers = pComponent->GetServices();
    for (TServiceWrapperMap::const_iterator itWrapper = rmWrappers.begin();
         itWrapper != rmWrappers.end(); ++itWrapper)
    {
      itExistingWrapper = m_mServiceWrappers.find(itWrapper->first);
      if (itExistingWrapper != m_mServiceWrappers.end())
      {
        rise::LogError() << "Service [" << itWrapper->second->GetName()
          << "] from component [" << pComponent->GetName()
          << "] already registered in component ["
          << itWrapper->second->GetComponent()->GetName() << "].";
      }
      else
      {
        m_mServiceWrappers.insert(*itWrapper);
      }
    }
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

  const CServiceWrapper* CSharedContext::GetService( const std::string& sName ) const
  {
    TServiceWrapperMap::const_iterator itExistingWrapper =
        m_mServiceWrappers.find(sName);

    if (itExistingWrapper != m_mServiceWrappers.end())
    {
      return itExistingWrapper->second;
    }

    return NULL;
  }

  CServiceWrapper* CSharedContext::GetService( const std::string& sName )
  {
    TServiceWrapperMap::const_iterator itExistingWrapper =
        m_mServiceWrappers.find(sName);

    if (itExistingWrapper != m_mServiceWrappers.end())
    {
      return itExistingWrapper->second;
    }

    return NULL;
  }

  const TServiceWrapperMap& CSharedContext::GetServices() const
  {
    return m_mServiceWrappers;
  }

  void CSharedContext::Clear()
  {
    m_mComponents.clear();
  }

  CSharedContext* CSharedContext::m_pInst = NULL;
}

