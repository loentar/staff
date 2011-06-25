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

#include <list>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/SharedPtr.h>
#include <rise/common/MutablePtr.h>
#include <rise/tools/FileFind.h>
#include <rise/plugin/PluginManager.h>
#include <staff/common/Runtime.h>
#include <staff/common/Operation.h>
#include <staff/common/Exception.h>
#include <staff/component/ServiceWrapper.h>
#include <staff/component/SharedContext.h>
#include <staff/component/SessionManager.h>
#include <staff/component/ServiceInstanceManager.h>
#include <staff/security/tools.h>
#include "ServiceDispatcher.h"


namespace staff
{
//////////////////////////////////////////////////////////////////////////
// Local Impl

  class ServiceDispatcher::ServiceDispatcherImpl
  {
  public:
    typedef std::list<std::string> StringList;
    typedef std::map<std::string, StringList> DepsMap;

  public:
    void Init()
    {
rise::LogEntry();
      const std::string sComponentsDir = Runtime::Inst().GetComponentsHome();
      SharedContext& rSharedContext = SharedContext::Inst();
      StringList lsComponentDirs;
      
      // find directories with components
      rise::CFileFind::Find(sComponentsDir, lsComponentDirs, "*", rise::CFileFind::EFA_DIR);
      if (lsComponentDirs.size() == 0)
      {
        rise::LogDebug() << "components is not found";
      }

      for (StringList::const_iterator itDir = lsComponentDirs.begin();
                itDir != lsComponentDirs.end(); ++itDir)
      {
        // finding libraries with components
        StringList lsComponents;
        std::string sComponentDir = sComponentsDir + RISE_PATH_SEPARATOR + *itDir + RISE_PATH_SEPARATOR;
        rise::CFileFind::Find(sComponentDir, lsComponents, "*" RISE_LIBRARY_EXT, rise::CFileFind::EFA_FILE);
        for (StringList::const_iterator itComponent = lsComponents.begin();
                itComponent != lsComponents.end(); ++itComponent)
        {
          try
          {
            // loading component
            rise::LogDebug() << "Loading component: " << (sComponentDir + *itComponent);
            Component* pComponent = m_lsComponents.LoadPlugin(sComponentDir + *itComponent, true);
            rSharedContext.AddComponent(pComponent);
          }
          catch(const rise::CException& rEx)
          {
            rise::LogWarning() << "Can't load component: " << (sComponentDir + *itComponent) << ": " << rEx.GetString();
            continue;
          }
          catch(...)
          {
            rise::LogWarning() << "Can't load component: " << (sComponentDir + *itComponent) << ": unknown error";
            continue;
          }
        }
      }

      staff::SessionManager::Inst().Start();

      LoadServices();

      const ServiceWrapperMap& rServices = rSharedContext.GetServices();
      if (m_stEvents.pOnConnect != NULL)
      {
        for (ServiceWrapperMap::const_iterator itService = rServices.begin();
                 itService != rServices.end(); ++itService)
        {
          m_stEvents.pOnConnect(itService->second);
        }
      }
    }

    void Deinit()
    {
      staff::SessionManager::Inst().Stop();

      SharedContext& rSharedContext = SharedContext::Inst();
      if (m_stEvents.pOnDisconnect != NULL)
      {
        const ServiceWrapperMap& rServices = rSharedContext.GetServices();
        for (ServiceWrapperMap::const_iterator itService = rServices.begin();
                itService != rServices.end(); ++itService)
        {
          m_stEvents.pOnDisconnect(itService->first);
        }
      }

      rSharedContext.Clear();
      ServiceInstanceManager::Inst().FreeAllSessions();
      // do not unload component libraries here
    }


    void ResolveDepsOrder(StringList& rlsLoadOrder, const DepsMap& rmDeps)
    {
      bool bWasChanged = false;
      for (StringList::size_type nRetry = rlsLoadOrder.size(); nRetry; --nRetry)
      {
        bWasChanged = false;
        for (StringList::iterator itThisDep = rlsLoadOrder.begin();
            itThisDep != rlsLoadOrder.end();)
        {
          DepsMap::const_iterator itThisDeps = rmDeps.find(*itThisDep);
          RISE_ASSERT(itThisDeps != rmDeps.end()); // should not happen

          const StringList& rlsThisDeps = itThisDeps->second;
          if (!rlsThisDeps.empty())
          {
            StringList::iterator itOtherDep = itThisDep;
            ++itOtherDep;
            for (; itOtherDep != rlsLoadOrder.end(); ++itOtherDep)
            {
              bool bHasDep = false;
              for (StringList::const_iterator itFind = rlsThisDeps.begin();
                   itFind != rlsThisDeps.end(); ++itFind)
              {
                if (*itFind == *itOtherDep)
                {
                  bHasDep = true;
                  break;
                }
              }

              if (bHasDep)
              {
                rise::LogDebug() << *itThisDep << " => " << *itOtherDep;
                rlsLoadOrder.splice(itThisDep++, rlsLoadOrder, itOtherDep);
                // now itThisDep points to new pos of the itThisDep
                bWasChanged = true;
                break;
              }
            }
          }

          ++itThisDep;
        }

        if (!bWasChanged)
        {
          break;
        }
      }

      if (bWasChanged)
      {
        rise::LogWarning() << "Failed to reorder services.";
      }
    }

    // load services that marked as loadAtStartup
    void LoadServices()
    {
      // build dependencies

      const ServiceWrapperMap& rmServices = SharedContext::Inst().GetServices();

      StringList lsServicesLoadOrder;
      DepsMap mDependencies;

      for (ServiceWrapperMap::const_iterator itService = rmServices.begin();
           itService != rmServices.end(); ++itService)
      {
        if (itService->second->IsLoadAtStartup())
        {
          lsServicesLoadOrder.push_back(itService->first);

          // fill in dependencies
          StringList& rlsDeps = mDependencies[itService->first];
          const std::string& sDeps = itService->second->GetDependencies();
          if (!sDeps.empty())
          {
            std::string::size_type nPosBegin = 0;
            std::string::size_type nPosEnd = 0;
            while (nPosEnd != std::string::npos)
            {
              nPosEnd = sDeps.find(',', nPosBegin);
              std::string sDep = sDeps.substr(nPosBegin,
                (nPosEnd == std::string::npos) ? std::string::npos : (nPosEnd - nPosBegin));

              rise::StrTrim(sDep);
              rlsDeps.push_back(sDep);

              nPosBegin = nPosEnd + 1;
            }
          }
        }
      }

      // check dependencies
      for (DepsMap::const_iterator itService = mDependencies.begin();
           itService != mDependencies.end(); ++itService)
      {
        for (StringList::const_iterator itDependOn = itService->second.begin();
            itDependOn != itService->second.end(); ++itDependOn)
        {
          if (!mDependencies.count(*itDependOn))
          {
            rise::LogWarning() << "Service [" << itService->first
                               << "] has a dependency on unknown service [" << *itDependOn << "]";
          }
        }
      }

      // resolve dependencies
      ResolveDepsOrder(lsServicesLoadOrder, mDependencies);

      // load services in order
      staff::ServiceInstanceManager& rInstanceManager = staff::ServiceInstanceManager::Inst();
      for (StringList::const_iterator itService = lsServicesLoadOrder.begin();
          itService != lsServicesLoadOrder.end(); ++itService)
      {
        rise::LogDebug() << "loading service [" << *itService << "] marked as loadAtStartup";
        rInstanceManager.CreateServiceInstance(STAFF_SECURITY_NOBODY_SESSION_ID, *itService, "");
      }
    }

    ServiceDispatcher::Events m_stEvents;
    rise::plugin::CPluginManager<Component> m_lsComponents;
  };

//////////////////////////////////////////////////////////////////////////
// Events
  ServiceDispatcher::Events::Events(void (*pOnConnectInit)(const ServiceWrapper*), void (*pOnDisconnectInit)(const std::string&)) :
    pOnConnect(pOnConnectInit), pOnDisconnect(pOnDisconnectInit)
  {
  }

  ServiceDispatcher::Events::Events():
    pOnConnect(NULL), pOnDisconnect(NULL)
  {
  }

  ServiceDispatcher::Events& ServiceDispatcher::Events::operator=(const Events& rEvents)
  {
    pOnConnect = rEvents.pOnConnect;
    pOnDisconnect = rEvents.pOnDisconnect;
    return *this;
  }


  ServiceDispatcher& ServiceDispatcher::Inst()
  {
    static ServiceDispatcher tInst;
    return tInst;
  }

  void ServiceDispatcher::Init(const Events& stEvents)
  {
    m_pImpl->m_stEvents = stEvents;
    m_pImpl->Init();
  }

  void ServiceDispatcher::Deinit()
  {
    m_pImpl->Deinit();
  }

  void ServiceDispatcher::InvokeSelf(Operation& rOperation)
  {
    staff::DataObject& rResult = rOperation.Result();

    SharedContext& rSharedContext = SharedContext::Inst();
    const std::string& sOpName = rOperation.GetName();
    if (sOpName == "GetServices")
    {
      const ServiceWrapperMap& rServices = rSharedContext.GetServices();
      for (ServiceWrapperMap::const_iterator itService = rServices.begin();
            itService != rServices.end(); ++itService)
      {
        rResult.CreateChild("Service", itService->second->GetName());
      }
    }
    else
    if (sOpName == "GetOperations")
    {
      const staff::DataObject& rRequest = rOperation.Request();
      const std::string& sServiceName = rRequest.GetChildByLocalName("sServiceName").GetText();
      const ServiceWrapper* pService = rSharedContext.GetService(sServiceName);
      RISE_ASSERTES(pService, RemoteInternalException, "Service [" + sServiceName + "] is not registered");

      rResult = pService->GetOperations();
    }
    else
    {
      RISE_THROWS(RemoteInternalException, "Operation not found: " + sOpName);
    }
  }

  ServiceDispatcher::ServiceDispatcher()
  {
    m_pImpl = new ServiceDispatcherImpl;
  }

  ServiceDispatcher::~ServiceDispatcher()
  {
    if (m_pImpl)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

}
