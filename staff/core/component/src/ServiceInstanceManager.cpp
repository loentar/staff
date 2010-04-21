/*
 *  Copyright 2010 Utkin Dmitry
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
#include <rise/common/MutablePtr.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/Log.h>
#include <staff/common/IService.h>
#include "ServiceWrapper.h"
#include "ServiceWrapperFactory.h"
#include "ServiceInstanceManager.h"

namespace staff
{
  class CServiceInstanceManager::CServiceInstanceManagerImpl
  {
  public:
    typedef std::map<std::string, PIService> TInstanceMap;
    typedef std::map<std::string, TInstanceMap> TServiceMap;
    typedef std::map<std::string, TServiceMap> TSessionMap;

    PIService& CreateServiceInstance(TServiceMap& rmServiceMap, const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
    {
      PIService tpService(CServiceWrapperFactory::Inst().GetServiceWrapper(sServiceName)->NewImpl());
      tpService->m_sServiceName = sServiceName;
      tpService->m_sSessionId = sSessionId;
      tpService->m_sInstanceId = sInstanceId;

      try
      {
        tpService->OnCreate();
      }
      RISE_CATCH_ALL_DESCR_ACTION("Exception while creating service instance... [" + sServiceName + ":" + sInstanceId + "]: service is not created", throw);

      return (rmServiceMap[sServiceName][sInstanceId] = tpService);
    }

    TSessionMap m_mSessions;
  };

  CServiceInstanceManager& CServiceInstanceManager::Inst()
  {
    if (!m_pInst)
    {
      m_pInst = new CServiceInstanceManager;
    }

    return *m_pInst;
  }

  void CServiceInstanceManager::FreeInst()
  {
    if (m_pInst)
    {
      delete m_pInst;
      m_pInst = NULL;
    }
  }

  CServiceInstanceManager::CServiceInstanceManager()
  {
    m_pImpl = new CServiceInstanceManagerImpl;
  }

  CServiceInstanceManager::~CServiceInstanceManager()
  {
    if (m_pImpl)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  void CServiceInstanceManager::CreateSession(const std::string& sSessionId)
  {
    m_pImpl->m_mSessions[sSessionId];
  }

  void CServiceInstanceManager::FreeSession(const std::string& sSessionId)
  {
    m_pImpl->m_mSessions.erase(sSessionId);
  }

  PIService& CServiceInstanceManager::ServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    CServiceInstanceManagerImpl::TSessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    CServiceInstanceManagerImpl::TServiceMap::iterator itService = itSession->second.find(sServiceName);
    if (itService == itSession->second.end())
    {
      return m_pImpl->CreateServiceInstance(itSession->second, sSessionId, sServiceName, sInstanceId);
    }

    CServiceInstanceManagerImpl::TInstanceMap::iterator itInstance = itService->second.find(sInstanceId);
    if (itInstance == itService->second.end())
    {
      return m_pImpl->CreateServiceInstance(itSession->second, sSessionId, sServiceName, sInstanceId);
    }

    return itInstance->second;
  }

  PIService& CServiceInstanceManager::CreateServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    CServiceInstanceManagerImpl::TSessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    return m_pImpl->CreateServiceInstance(itSession->second, sSessionId, sServiceName, sInstanceId);
  }

  void CServiceInstanceManager::FreeServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    CServiceInstanceManagerImpl::TSessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    CServiceInstanceManagerImpl::TServiceMap::iterator itService = itSession->second.find(sServiceName);
    RISE_ASSERTES(itService != itSession->second.end(), rise::CLogicNoItemException, "Service does not exists: " + sInstanceId);

    CServiceInstanceManagerImpl::TInstanceMap::iterator itInstance = itService->second.find(sInstanceId);
    RISE_ASSERTES(itInstance != itService->second.end(), rise::CLogicNoItemException, "Instance does not exists: " + sInstanceId);

    try
    {
      itInstance->second->OnDestroy();
    }
    RISE_CATCH_ALL_DESCR("Exception while destroying service instance... [" + sServiceName + ":" + sInstanceId + "]: ignored");

    itService->second.erase(itInstance);
  }

  PIService& CServiceInstanceManager::GetServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    CServiceInstanceManagerImpl::TSessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    CServiceInstanceManagerImpl::TServiceMap::iterator itService = itSession->second.find(sServiceName);
    if (itService == itSession->second.end())
    {
      // lazy creation of default instance
      RISE_ASSERTES(!sInstanceId.size(), rise::CLogicNoItemException, "Service does not exists: " + sServiceName);
      return CreateServiceInstance(sSessionId, sServiceName, sInstanceId);
    }

    CServiceInstanceManagerImpl::TInstanceMap::iterator itInstance = itService->second.find(sInstanceId);
    RISE_ASSERTES(itInstance != itService->second.end(), rise::CLogicNoItemException, "Instance does not exists: " + sInstanceId);

    return itInstance->second;
  }

  CServiceInstanceManager* CServiceInstanceManager::m_pInst = NULL;
}
