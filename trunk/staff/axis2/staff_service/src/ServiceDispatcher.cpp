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

#include <signal.h>
#ifdef __linux__
#include <pthread.h>
#endif
#include <list>
#include <set>
#include <stdio.h>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
#include <rise/socket/ServerSocket.h>
#include <rise/threading/Thread.h>
#include <rise/xml/XMLNode.h>
#include <rise/tools/FileFind.h>
#include <rise/plugin/PluginManager.h>
#include <staff/security/Sessions.h>
#include <staff/common/Runtime.h>
#include <staff/common/Config.h>
#include <staff/common/Operation.h>
#include <staff/common/Exception.h>
#include <staff/common/Value.h>
#include <staff/component/ServiceWrapper.h>
#include <staff/component/SharedContext.h>
#include "ServiceDispatcher.h"


namespace staff
{
//////////////////////////////////////////////////////////////////////////
// Local Impl

  class CServiceDispatcher::CServiceDispatcherImpl
  {
  public:
    void Init()
    {
rise::LogEntry();
      const std::string sComponentsDir = CRuntime::Inst().GetComponentsHome();
      CSharedContext& rSharedContext = CSharedContext::Inst();
      rise::CStringList lsComponentDirs;
      
      // find directories with components
      rise::CFileFind::Find(sComponentsDir, lsComponentDirs, "*", rise::CFileFind::EFA_DIR);
      if (lsComponentDirs.size() == 0)
      {
        rise::LogDebug() << "components is not found";
      }

      for (rise::CStringList::const_iterator itDir = lsComponentDirs.begin(); 
                itDir != lsComponentDirs.end(); ++itDir )
      {
        // finding libraries with components
        rise::CStringList lsComponents;
        std::string sComponentDir = sComponentsDir + RISE_PATH_SEPARATOR + *itDir + RISE_PATH_SEPARATOR;
#ifdef __linux__
        rise::CFileFind::Find(sComponentDir, lsComponents, "*.so", rise::CFileFind::EFA_FILE);
#endif
#ifdef WIN32
        rise::CFileFind::Find(sComponentDir, lsComponents, "*.dll", rise::CFileFind::EFA_FILE);
#endif
        for (rise::CStringList::const_iterator itComponent = lsComponents.begin(); 
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

      if (m_stEvents.pOnConnect != NULL)
      {
        const TServiceWrapperMap& rServices = rSharedContext.GetServices();
        for (TServiceWrapperMap::const_iterator itService = rServices.begin();
                 itService != rServices.end(); ++itService)
        {
          m_stEvents.pOnConnect(itService->first, itService->second);
        }
      }
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
    CSharedContext& rSharedContext = CSharedContext::Inst();
    const std::string& sOpName = rOperation.GetName();
    if (sOpName == "GetServices")
    {
      const TServiceWrapperMap& rServices = rSharedContext.GetServices();
      for (TServiceWrapperMap::const_iterator itService = rServices.begin();
            itService != rServices.end(); ++itService)
      {
        std::string sServiceName;
        const CComponent* pServiceComponent = itService->second->GetComponent();

        if (pServiceComponent != NULL)
        {
          sServiceName = pServiceComponent->GetName();
          if (sServiceName.size() != 0)
          {
            sServiceName += ".";
          }
        }

        sServiceName += itService->second->GetName();
        rOperation.Result().CreateChild("Service", sServiceName);
      }
    }
    else
    if (sOpName == "GetOperations")
    {
      const staff::CDataObject& rRequest = rOperation.Request();
      const CServiceWrapper* pService = rSharedContext.GetService(rRequest["sServiceName"]);
      RISE_ASSERTES(pService != NULL, CRemoteInternalException, 
        "Service is not registered: " + rOperation.Request()["ServiceName"].AsString());

      rOperation.Result() = pService->GetOperations();
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
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  CServiceDispatcher* CServiceDispatcher::m_pInst = NULL;
}
