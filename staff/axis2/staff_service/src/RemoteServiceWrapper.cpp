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
#include <rise/common/StreamBuffer.h>
#include <rise/os/ossocket.h>
#include <rise/threading/CriticalSection.h>
#include <staff/common/Value.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObjectHelper.h>
#include "ServiceDispatcher.h"
#include "RemoteService.h"
#include "RemoteServiceWrapper.h"

namespace staff
{
  CRemoteServiceWrapper::CRemoteServiceWrapper(CComponent* pComponent):
    m_pComponent(pComponent)
  {
  }

  CRemoteServiceWrapper::~CRemoteServiceWrapper()
  {
  }

  const std::string& CRemoteServiceWrapper::GetName() const
  {
    return m_mServices.begin()->second->GetName();
  }

  const std::string& CRemoteServiceWrapper::GetDescr() const
  {
    return m_mServices.begin()->second->GetDescr();
  }

  CDataObject CRemoteServiceWrapper::GetOperations() const
  {
    return m_mServices.begin()->second->GetOperations();
  }

  void CRemoteServiceWrapper::Invoke( COperation& rOperation, const std::string& sID )
  {
    TRemoteServiceMap::iterator it = m_mServices.find(sID);
    if (it == m_mServices.end())
    {
      PRemoteService pService;
      CServiceDispatcher::Inst().GetRemoteService(GetName(), sID, pService);

      RISE_ASSERTP(pService);

      pService->Invoke(rOperation);
      return;
    }

    it->second->Invoke(rOperation);
  }

  const CComponent* CRemoteServiceWrapper::GetComponent() const
  {
    return m_pComponent;
  }

  CComponent* CRemoteServiceWrapper::GetComponent()
  {
    return m_pComponent;
  }

  void* CRemoteServiceWrapper::GetImpl( const std::string& sID )
  {
    return NULL;
  }

  TRemoteServiceMap& CRemoteServiceWrapper::GetServices()
  {
    return m_mServices;
  }

  rise::CStringList CRemoteServiceWrapper::GetServiceIds() const
  {
    rise::CStringList lsServiceIds;
    for (TRemoteServiceMap::const_iterator itService = m_mServices.begin();
          itService != m_mServices.end(); ++itService)
    {
      lsServiceIds.push_back(itService->first);
    }

    return lsServiceIds;
  }

}
