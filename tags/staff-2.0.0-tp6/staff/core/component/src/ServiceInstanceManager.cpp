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

#ifdef _MSC_VER
#pragma warning (disable: 4503) //  decorated name length exceeded, name was truncated
#endif

#include <map>
#include <rise/common/MutablePtr.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/Log.h>
#include <rise/common/SharedPtr.h>
#include <staff/common/IService.h>
#include "SharedContext.h"
#include "ServiceWrapper.h"
#include "ServiceInstanceManager.h"

namespace staff
{
  class ServiceInstanceManager::ServiceInstanceManagerImpl
  {
  public:
    typedef std::map<std::string, PIService> InstanceMap;
    typedef std::map<std::string, InstanceMap> ServiceMap;
    typedef std::map<std::string, ServiceMap> SessionMap;

    ~ServiceInstanceManagerImpl()
    {
      try
      {
        Destroy();
      }
      RISE_CATCH_ALL
    }

    PIService& CreateServiceInstance(ServiceMap& rmServiceMap, const std::string& sSessionId,
                                     const std::string& sServiceName, const std::string& sInstanceId)
    {
      ServiceWrapper* pServiceWrapper = staff::SharedContext::Inst().GetService(sServiceName);
      RISE_ASSERTS(pServiceWrapper, "Service [" + sServiceName + "] is not found: ");
      PIService tpService = pServiceWrapper->NewImpl();

      tpService->Init(sServiceName, sSessionId, sInstanceId);

      try
      {
        tpService->OnCreate();
      }
      RISE_CATCH_ALL_DESCR_ACTION("Exception while creating service instance... ["
                                  + sServiceName + ":" + sInstanceId + "]: service is not created",
                                  throw);

      return (rmServiceMap[sServiceName][sInstanceId] = tpService);
    }

    void Destroy()
    {
      // generate OnDestroy event for all services
      for (ServiceInstanceManagerImpl::SessionMap::iterator itSession = m_mSessions.begin();
           itSession != m_mSessions.end(); ++itSession)
      {
        for (ServiceInstanceManagerImpl::ServiceMap::iterator itService = itSession->second.begin();
          itService != itSession->second.end(); ++itService)
        {
          for (ServiceInstanceManagerImpl::InstanceMap::iterator itInstance = itService->second.begin();
            itInstance != itService->second.end(); ++itInstance)
          {
            try
            {
              itInstance->second->OnDestroy();
            }
            RISE_CATCH_ALL_DESCR("Exception while destroying service instance... [" + itService->first + ":" +
                                 itInstance->first + "]: ignored");
          }
        }
      }

      m_mSessions.clear();
    }

    SessionMap m_mSessions;
  };

  ServiceInstanceManager& ServiceInstanceManager::Inst()
  {
    static ServiceInstanceManager tInst;
    return tInst;
  }

  ServiceInstanceManager::ServiceInstanceManager()
  {
    m_pImpl = new ServiceInstanceManagerImpl;
  }

  ServiceInstanceManager::~ServiceInstanceManager()
  {
    if (m_pImpl)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  void ServiceInstanceManager::CreateSession(const std::string& sSessionId)
  {
    m_pImpl->m_mSessions[sSessionId];
  }

  void ServiceInstanceManager::FreeSession(const std::string& sSessionId)
  {
    ServiceInstanceManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    // generate on destroy event for all services in session
    for (ServiceInstanceManagerImpl::ServiceMap::iterator itService = itSession->second.begin();
      itService != itSession->second.end(); ++itService)
    {
      for (ServiceInstanceManagerImpl::InstanceMap::iterator itInstance = itService->second.begin();
        itInstance != itService->second.end(); ++itInstance)
      {
        try
        {
          itInstance->second->OnDestroy();
        }
        RISE_CATCH_ALL_DESCR("Exception while destroying service instance... [" + itService->first + ":" +
                             itInstance->first + "]: ignored");
      }
    }

    m_pImpl->m_mSessions.erase(sSessionId);
  }

  void ServiceInstanceManager::FreeAllSessions()
  {
    m_pImpl->Destroy();
  }

  PIService& ServiceInstanceManager::ServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    ServiceInstanceManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    ServiceInstanceManagerImpl::ServiceMap::iterator itService = itSession->second.find(sServiceName);
    if (itService == itSession->second.end())
    {
      return m_pImpl->CreateServiceInstance(itSession->second, sSessionId, sServiceName, sInstanceId);
    }

    ServiceInstanceManagerImpl::InstanceMap::iterator itInstance = itService->second.find(sInstanceId);
    if (itInstance == itService->second.end())
    {
      return m_pImpl->CreateServiceInstance(itSession->second, sSessionId, sServiceName, sInstanceId);
    }

    return itInstance->second;
  }

  PIService& ServiceInstanceManager::CreateServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    ServiceInstanceManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    return m_pImpl->CreateServiceInstance(itSession->second, sSessionId, sServiceName, sInstanceId);
  }

  void ServiceInstanceManager::FreeServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    ServiceInstanceManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    ServiceInstanceManagerImpl::ServiceMap::iterator itService = itSession->second.find(sServiceName);
    RISE_ASSERTES(itService != itSession->second.end(), rise::CLogicNoItemException, "Service does not exists: " + sInstanceId);

    ServiceInstanceManagerImpl::InstanceMap::iterator itInstance = itService->second.find(sInstanceId);
    RISE_ASSERTES(itInstance != itService->second.end(), rise::CLogicNoItemException, "Instance does not exists: " + sInstanceId);

    try
    {
      itInstance->second->OnDestroy();
    }
    RISE_CATCH_ALL_DESCR("Exception while destroying service instance... [" + sServiceName + ":" + sInstanceId + "]: ignored");

    itService->second.erase(itInstance);
  }

  PIService& ServiceInstanceManager::GetServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId)
  {
    ServiceInstanceManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERTES(itSession != m_pImpl->m_mSessions.end(), rise::CLogicNoItemException, "Session does not exists: " + sSessionId);

    ServiceInstanceManagerImpl::ServiceMap::iterator itService = itSession->second.find(sServiceName);
    if (itService == itSession->second.end())
    {
      // lazy creation of default instance
      RISE_ASSERTES(sInstanceId.empty(), rise::CLogicNoItemException, "Service does not exists: " + sServiceName);
      return CreateServiceInstance(sSessionId, sServiceName, sInstanceId);
    }

    ServiceInstanceManagerImpl::InstanceMap::iterator itInstance = itService->second.find(sInstanceId);
    if (itInstance == itService->second.end())
    {
      // lazy creation of default instance if non default is already created
      RISE_ASSERTES(sInstanceId.empty(), rise::CLogicNoItemException, "Instance does not exists: " + sInstanceId);
      return CreateServiceInstance(sSessionId, sServiceName, sInstanceId);
    }

    return itInstance->second;
  }

  PIService& ServiceInstanceManager::ServiceInstance(const IService* pService, const std::string& sServiceName)
  {
    RISE_ASSERTS(pService, "pService is NULL!");
    return ServiceInstance(pService->GetSessionId(), sServiceName, pService->GetInstanceId());
  }

}
