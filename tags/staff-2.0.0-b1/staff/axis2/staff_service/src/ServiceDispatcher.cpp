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
#include <staff/utils/Log.h>
#include <staff/utils/File.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/SharedPtr.h>
#include <staff/utils/File.h>
#include <staff/utils/PluginManager.h>
#include <staff/common/Exception.h>
#include <staff/common/Runtime.h>
#include <staff/common/Operation.h>
#include <staff/common/Exception.h>
#include <staff/component/ServiceWrapper.h>
#include <staff/component/SharedContext.h>
#include <staff/component/ServiceInstanceManager.h>
#ifndef WITHOUT_SECURITY
#include <staff/component/SessionManager.h>
#include <staff/security/tools.h>
#endif
#include "ServiceDispatcher.h"

#ifdef WITHOUT_SECURITY
#define STAFF_SECURITY_NOBODY_SESSION_ID ""
#endif


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
      LogEntry();
      const std::string sComponentsDir = Runtime::Inst().GetComponentsHome();
      SharedContext& rSharedContext = SharedContext::Inst();
      StringList lsComponentDirs;

      // find directories with components
      File(sComponentsDir).List(lsComponentDirs, "*", File::AttributeDirectory);
      if (lsComponentDirs.empty())
      {
        LogDebug() << "no components found";
      }

      for (StringList::const_iterator itDir = lsComponentDirs.begin();
                itDir != lsComponentDirs.end(); ++itDir)
      {
        // finding libraries with components
        StringList lsComponents;
        std::string sComponentDir = sComponentsDir + STAFF_PATH_SEPARATOR + *itDir + STAFF_PATH_SEPARATOR;
#if defined __APPLE__
        File(sComponentDir).List(lsComponents, "*" STAFF_LIBRARY_EXT, File::AttributeRegularFile);
#else
        File(sComponentDir).List(lsComponents, "*" STAFF_LIBRARY_EXT, File::AttributeAnyFile);
#endif
        for (StringList::const_iterator itComponent = lsComponents.begin();
                itComponent != lsComponents.end(); ++itComponent)
        {
          const std::string& sComponentPath = sComponentDir + *itComponent;
          try
          {
            // loading component
            LogDebug() << "Loading component: " << sComponentPath;
            Component* pComponent = m_lsComponents.Load(sComponentPath, true);
            rSharedContext.AddComponent(pComponent);
          }
          catch (const Exception& rEx)
          {
            LogWarning() << "Can't load component: " << sComponentPath << ": " << rEx.what();
          }
          catch (...)
          {
            LogWarning() << "Can't load component: " << sComponentPath << ": unknown error";
          }
        }
      }

#ifndef WITHOUT_SECURITY
      SessionManager::Inst().Start();
#endif

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
#ifndef WITHOUT_SECURITY
      SessionManager::Inst().Stop();
#endif

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
          STAFF_ASSERT_PARAM(itThisDeps != rmDeps.end()); // should not happen

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
                LogDebug() << *itThisDep << " => " << *itOtherDep;
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
        LogWarning() << "Failed to reorder services.";
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

              StringTrim(sDep);
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
            LogWarning() << "Service [" << itService->first
                               << "] has a dependency on unknown service [" << *itDependOn << "]";
          }
        }
      }

      // resolve dependencies
      ResolveDepsOrder(lsServicesLoadOrder, mDependencies);

      // load services in order
      ServiceInstanceManager& rInstanceManager = ServiceInstanceManager::Inst();
      for (StringList::const_iterator itService = lsServicesLoadOrder.begin();
          itService != lsServicesLoadOrder.end(); ++itService)
      {
        LogDebug() << "loading service [" << *itService << "] marked as loadAtStartup";
        rInstanceManager.CreateServiceInstance(STAFF_SECURITY_NOBODY_SESSION_ID, *itService, "");
      }
    }

    ServiceDispatcher::Events m_stEvents;
    staff::PluginManager<Component> m_lsComponents;
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
    DataObject& rResult = rOperation.Result();

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
      const DataObject& rRequest = rOperation.Request();
      const std::string& sServiceName = rRequest.GetChildByLocalName("sServiceName").GetText();
      const ServiceWrapper* pService = rSharedContext.GetService(sServiceName);
      STAFF_ASSERT(pService, "Service [" + sServiceName + "] is not registered");

      rResult = pService->GetOperations();
    }
    else
    {
      STAFF_THROW(RemoteException, "Operation not found: " + sOpName);
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
