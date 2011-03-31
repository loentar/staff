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
#include <staff/component/ServiceInstanceManager.h>
#include <staff/security/tools.h>
#include "ServiceDispatcher.h"


namespace staff
{
//////////////////////////////////////////////////////////////////////////
// Local Impl

  class CServiceDispatcher::CServiceDispatcherImpl
  {
  public:
    typedef std::list<std::string> TStringList;
    typedef std::map<std::string, TStringList> TDepsMap;

  public:
    void Init()
    {
rise::LogEntry();
      const std::string sComponentsDir = CRuntime::Inst().GetComponentsHome();
      CSharedContext& rSharedContext = CSharedContext::Inst();
      TStringList lsComponentDirs;
      
      // find directories with components
      rise::CFileFind::Find(sComponentsDir, lsComponentDirs, "*", rise::CFileFind::EFA_DIR);
      if (lsComponentDirs.size() == 0)
      {
        rise::LogDebug() << "components is not found";
      }

      for (TStringList::const_iterator itDir = lsComponentDirs.begin();
                itDir != lsComponentDirs.end(); ++itDir )
      {
        // finding libraries with components
        TStringList lsComponents;
        std::string sComponentDir = sComponentsDir + RISE_PATH_SEPARATOR + *itDir + RISE_PATH_SEPARATOR;
        rise::CFileFind::Find(sComponentDir, lsComponents, "*" RISE_LIBRARY_EXT, rise::CFileFind::EFA_FILE);
        for (TStringList::const_iterator itComponent = lsComponents.begin();
                itComponent != lsComponents.end(); ++itComponent )
        {
          try
          {
            // loading component
            rise::LogDebug() << "Loading component: " << (sComponentDir + *itComponent);
            CComponent* pComponent = m_lsComponents.LoadPlugin(sComponentDir + *itComponent, true);
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

      const TServiceWrapperMap& rServices = rSharedContext.GetServices();
      if (m_stEvents.pOnConnect != NULL)
      {
        for (TServiceWrapperMap::const_iterator itService = rServices.begin();
                 itService != rServices.end(); ++itService)
        {
          m_stEvents.pOnConnect(itService->first, itService->second);
        }
      }

      LoadServices();
    }

    void Deinit()
    {
      CSharedContext& rSharedContext = CSharedContext::Inst();
      if (m_stEvents.pOnDisconnect != NULL)
      {
        const TServiceWrapperMap& rServices = rSharedContext.GetServices();
        for (TServiceWrapperMap::const_iterator itService = rServices.begin();
                itService != rServices.end(); ++itService)
        {
          m_stEvents.pOnDisconnect(itService->first);
        }
      }

      rSharedContext.Clear();
      m_lsComponents.UnloadAll();
    }


    void ResolveDepsOrder(TStringList& rlsLoadOrder, const TDepsMap& rmDeps)
    {
      bool bWasChanged = false;
      for (TStringList::size_type nRetry = rlsLoadOrder.size(); nRetry; --nRetry)
      {
        bWasChanged = false;
        for (TStringList::iterator itThisDep = rlsLoadOrder.begin();
            itThisDep != rlsLoadOrder.end();)
        {
          TDepsMap::const_iterator itThisDeps = rmDeps.find(*itThisDep);
          RISE_ASSERT(itThisDeps != rmDeps.end()); // should not happen

          const TStringList& rlsThisDeps = itThisDeps->second;
          if (!rlsThisDeps.empty())
          {
            TStringList::iterator itOtherDep = itThisDep;
            ++itOtherDep;
            for (; itOtherDep != rlsLoadOrder.end(); ++itOtherDep)
            {
              bool bHasDep = false;
              for (TStringList::const_iterator itFind = rlsThisDeps.begin();
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

      const TServiceWrapperMap& rmServices = CSharedContext::Inst().GetServices();

      TStringList lsServicesLoadOrder;
      TDepsMap mDependencies;

      for (TServiceWrapperMap::const_iterator itService = rmServices.begin();
           itService != rmServices.end(); ++itService)
      {
        if (itService->second->IsLoadAtStartup())
        {
          lsServicesLoadOrder.push_back(itService->first);

          // fill in dependencies
          TStringList& rlsDeps = mDependencies[itService->first];
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
      for (TDepsMap::const_iterator itService = mDependencies.begin();
           itService != mDependencies.end(); ++itService)
      {
        for (TStringList::const_iterator itDependOn = itService->second.begin();
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
      staff::CServiceInstanceManager& rInstanceManager = staff::CServiceInstanceManager::Inst();
      for (TStringList::const_iterator itService = lsServicesLoadOrder.begin();
          itService != lsServicesLoadOrder.end(); ++itService)
      {
        rise::LogDebug() << "loading service [" << *itService << "]";
        rInstanceManager.CreateServiceInstance(STAFF_SECURITY_NOBODY_SESSION_ID, *itService, "");
      }
    }

    CServiceDispatcher::SEvents m_stEvents;
    rise::plugin::CPluginManager<CComponent> m_lsComponents;
  };

//////////////////////////////////////////////////////////////////////////
// SEvents
  CServiceDispatcher::SEvents::SEvents( void (*pOnConnectInit)(const std::string&, const CServiceWrapper*), void (*pOnDisconnectInit)(const std::string&) ) :
    pOnConnect(pOnConnectInit), pOnDisconnect(pOnDisconnectInit)
  {
  }

  CServiceDispatcher::SEvents::SEvents():
    pOnConnect(NULL), pOnDisconnect(NULL)
  {
  }

  CServiceDispatcher::SEvents& CServiceDispatcher::SEvents::operator=( const SEvents& rEvents )
  {
    pOnConnect = rEvents.pOnConnect;
    pOnDisconnect = rEvents.pOnDisconnect;
    return *this;
  }


  CServiceDispatcher& CServiceDispatcher::Inst()
  {
    if (m_pInst == NULL)
    {
      m_pInst = new CServiceDispatcher;
    }

    return *m_pInst;
  }

  void CServiceDispatcher::FreeInst()
  {
    if (m_pInst != NULL)
    {
      delete m_pInst;
      m_pInst = NULL;
    }
  }

  void CServiceDispatcher::Init( const SEvents& stEvents )
  {
    m_pImpl->m_stEvents = stEvents;
    m_pImpl->Init();
  }

  void CServiceDispatcher::Deinit()
  {
    m_pImpl->Deinit();
  }

  void CServiceDispatcher::InvokeSelf( COperation& rOperation )
  {
    staff::CDataObject& rResult = rOperation.Result();

    CSharedContext& rSharedContext = CSharedContext::Inst();
    const std::string& sOpName = rOperation.GetName();
    if (sOpName == "GetServices")
    {
      const TServiceWrapperMap& rServices = rSharedContext.GetServices();
      for (TServiceWrapperMap::const_iterator itService = rServices.begin();
            itService != rServices.end(); ++itService)
      {
        rResult.CreateChild("Service", itService->second->GetName());
      }
    }
    else
    if (sOpName == "GetOperations")
    {
      const staff::CDataObject& rRequest = rOperation.Request();
      const std::string& sServiceName = rRequest.GetChildByLocalName("sServiceName").GetText();
      const CServiceWrapper* pService = rSharedContext.GetService(sServiceName);
      RISE_ASSERTES(pService, CRemoteInternalException, "Service [" + sServiceName + "] is not registered");

      rResult = pService->GetOperations();
    }
    else
    {
      RISE_THROWS(CRemoteInternalException, "Operation not found: " + sOpName);
    }
  }

  CServiceDispatcher::CServiceDispatcher()
  {
    m_pImpl = new CServiceDispatcherImpl;
  }

  CServiceDispatcher::~CServiceDispatcher()
  {
    if (m_pImpl)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  CServiceDispatcher* CServiceDispatcher::m_pInst = NULL;
}
