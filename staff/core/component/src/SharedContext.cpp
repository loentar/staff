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
#include <rise/common/MutablePtr.h>
#include "ServiceWrapper.h"
#include "SharedContext.h"

namespace staff
{
  SharedContext::SharedContext()
  {
  }

  SharedContext::~SharedContext()
  {
  }

  SharedContext& SharedContext::Inst()
  {
    static SharedContext tInst;
    return tInst;
  }

  void SharedContext::AddComponent( Component* pComponent )
  {
    CompositeComponentMap::iterator itFind = m_mComponents.find(pComponent->GetName());
    if (itFind == m_mComponents.end())
    {
      const std::pair<CompositeComponentMap::iterator, bool>& tInsertResult = 
        m_mComponents.insert(CompositeComponentMap::value_type(pComponent->GetName(), PCompositeComponent(new CompositeComponent)));
      tInsertResult.first->second->Compose(pComponent);
    } else
    {
      itFind->second->Compose(pComponent);
    }


    ServiceWrapperMap::const_iterator itExistingWrapper;
    const ServiceWrapperMap& rmWrappers = pComponent->GetServices();
    for (ServiceWrapperMap::const_iterator itWrapper = rmWrappers.begin();
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

  CompositeComponent* SharedContext::GetComponent( const std::string& sName )
  {
    CompositeComponentMap::iterator itFind = m_mComponents.find(sName);
    if (itFind == m_mComponents.end())
      return NULL;
    return itFind->second;
  }

  const CompositeComponent* SharedContext::GetComponent( const std::string& sName ) const
  {
    CompositeComponentMap::const_iterator itFind = m_mComponents.find(sName);
    if (itFind == m_mComponents.end())
      return NULL;
    return itFind->second;
  }

  const CompositeComponentMap& SharedContext::GetComponents() const
  {
    return m_mComponents;
  }

  const ServiceWrapper* SharedContext::GetService( const std::string& sName ) const
  {
    ServiceWrapperMap::const_iterator itExistingWrapper =
        m_mServiceWrappers.find(sName);

    if (itExistingWrapper != m_mServiceWrappers.end())
    {
      return itExistingWrapper->second;
    }

    return NULL;
  }

  ServiceWrapper* SharedContext::GetService( const std::string& sName )
  {
    ServiceWrapperMap::const_iterator itExistingWrapper =
        m_mServiceWrappers.find(sName);

    if (itExistingWrapper != m_mServiceWrappers.end())
    {
      return itExistingWrapper->second;
    }

    return NULL;
  }

  const ServiceWrapperMap& SharedContext::GetServices() const
  {
    return m_mServiceWrappers;
  }

  void SharedContext::Clear()
  {
    m_mComponents.clear();
    m_mServiceWrappers.clear();
  }

}

