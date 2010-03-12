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
#include <staff/component/Service.h>
#include <staff/component/SharedContext.h>
#include "RemoteService.h"
#include "RemoteServiceWrapper.h"
#include "ServiceDispatcher.h"


namespace staff
{
//////////////////////////////////////////////////////////////////////////
// Remote Impl
  class CServiceDispatcher::CRemoteServiceDispatcherImpl: private rise::threading::CThread
  {
  public:
    class CRemoteComponent: public CComponent
    {
    public:
      const std::string& GetName() const
      {
        return m_sName;
      }

      const CService* GetService(const std::string& sService) const
      {
        return NULL;
      }

      CService* GetService(const std::string& sService)
      {
        return NULL;
      }

      const TServiceMap& GetServices() const
      {
        return m_tMap;
      }
      
    private:
      static std::string m_sName;
      static TServiceMap m_tMap;
    };
  public:

    CRemoteServiceDispatcherImpl()
    {
rise::LogEntry();
#ifdef __linux__
      m_pSelf = this;
      m_pSigHandler = signal(SIGINT, SigCatcher);
      rise::LogDebug() << "Setting new SIGINT signal handler, pervious handler = " << (void*)m_pSigHandler;
#endif
      CRemoteComponent tComponent;
      CSharedContext::Inst().AddComponent(&tComponent);
      m_pComponent = CSharedContext::Inst().GetComponent(tComponent.GetName());
    }
    
#ifdef __linux__
    static void SigCatcher(int nSignal)
    {
      try
      {
        rise::LogDebug() << "Signal: " << nSignal;
        if(nSignal == SIGINT)
        {
          if(m_pSelf->GetId() == CThread::GetCurrentId()) // dispatcher's thread
          {
            rise::LogDebug1() << "dispatcher\'s thread: stop";
            m_pSelf->Stop();
          }
          else // axis2c thread
          {
#ifndef OS_MCBC
            pthread_kill(m_pSelf->GetId(), SIGINT);
#endif
            rise::LogDebug() << "waiting for dispatcher stops";
            for(int i = 0; i < 200 && m_pSelf->IsWorking(); ++i)
              Sleep(10);
              
            if(m_pSelf->IsWorking())
            {
              rise::LogError() << "dispatcher is still running, terminating!!!";
              m_pSelf->Cancel();
            } else
            {
              rise::LogDebug() << "dispatcher stopped";
            }

            rise::LogDebug() << "Sending signal to Axis2/C";

            // restore signal mapping
            signal(SIGINT, m_pSigHandler);
            if(kill(0, SIGINT) != 0)
            {
              rise::LogError() << "Can\'t send signal to Axis2/c";
            }
          }
        }
      }
      RISE_CATCH_ALL
    }
#endif

    using CThread::Start;
    using CThread::Stop;
    using CThread::IsWorking;

    bool WaitService( const std::string& sServiceName, const std::string& sSessionId, PRemoteService& rpService )
    {
      for(int nRetry = 0; nRetry < 20; ++nRetry)
      {
        rise::threading::CThread::Sleep(500);
        for (TRemoteServiceMap::iterator itFind = m_mRemoteServiceMap.begin();
          itFind != m_mRemoteServiceMap.end(); ++itFind)
        {
          if (sServiceName == itFind->second->GetName() && sSessionId == itFind->second->GetID())
          {
            rpService = itFind->second;
            return true;
          }
        }
      }

      return false;
    }

    void GetService( const std::string& sServiceName, const std::string& sSessionId, PRemoteService& rpRemoteService )
    {
      rise::LogDebug2() << "GetService [" << sServiceName << "] with SSessionId=[" << sSessionId << "]...";

      TRemoteServiceMap::iterator itLastFind = m_mRemoteServiceMap.end();

      rise::LogDebug2() << "services Count= " << m_mRemoteServiceMap.size();

      const std::string& sServiceNameID = sServiceName + ":" + sSessionId;


      TRemoteServiceMap::iterator itFind = m_mRemoteServiceMap.find(sServiceNameID);
      if (itFind != m_mRemoteServiceMap.end())
      {
        rpRemoteService = itFind->second;
        return;
      }

      itLastFind = m_mRemoteServiceMap.find(sServiceName + ":" + staff::security::CSessions::sNobodySessionId);

      if (itLastFind != m_mRemoteServiceMap.end())
      {
        SPendingService stService(sServiceName, sSessionId);
        std::pair<TPendingServiceSet::iterator, bool> tpInsPendingService = m_tsPendingServiceSet.insert(stService);
        if (!tpInsPendingService.second)
        {  // service already in queue
          if (WaitService(sServiceName, sSessionId, rpRemoteService))
            return;
        } else
        { // service is not found in queue
          rise::LogDebug2() << ">>> Service [" << sServiceName << "] with SSessionId=[" << sSessionId << "] is not found, trying to create...";
          itLastFind->second->CreateServiceID(sSessionId);

          try
          {
            bool bServiceReady = WaitService(sServiceName, sSessionId, rpRemoteService);
            rise::LogDebug2() << ">>> Removing Service [" << sServiceName << "] with SSessionId=[" << sSessionId << "] from queue: " << (bServiceReady ? "Service Ready" : "Timeout");
            m_tsPendingServiceSet.erase(tpInsPendingService.first);
            if (bServiceReady)
              return;
          }
          catch(...)
          {
            rise::LogDebug2() << ">>> Removing Service [" << sServiceName << "] with SSessionId=[" << sSessionId << "] from queue: Exception";
            m_tsPendingServiceSet.erase(tpInsPendingService.first);
          }
        }
      }

      rpRemoteService = NULL;
    }

  protected:
    virtual void Run(void*)
    {
rise::LogEntry();
      bool bReadyToAccept = false;
      const rise::xml::CXMLNode& rXmlConfig = CConfig::Inst().GetModuleConfig("ServiceDispatcher");
      rise::ushort ushPort = rXmlConfig["DispatcherPort"];
      int nMaxServices = rXmlConfig["nMaxServices"];

      rise::LogNotice() << " _____ STARTING DISPATCHER _____ ";

      while (!CThread::IsStopping())
      {
        try
        {
          m_tServerSocket.Create(ushPort, inet_addr("127.0.0.1"), rise::sockets::CSocket::ST_STREAM, nMaxServices);
          break;
        }
        catch(const rise::CException& rEx)
        {
          int nError = m_tServerSocket.GetError();
          if(nError == EADDRINUSE)
          {
            rise::LogError() << "Can't Create ServiceDispatcher socket: " << rEx.GetDescr();
            return;
          }
          rise::LogWarning() << "Can't Create ServiceDispatcher socket: " << rEx.GetDescr() << ".. waiting for 10 sec...";
          Sleep(10000);
        }
      }
      
      while (!CThread::IsStopping())
      {
        try
        {
          PRemoteService pNewService(new CRemoteService);
          for (;;)
          {
            bReadyToAccept = SelectAndClean();
            if (IsStopping())
              break;
            
            if (bReadyToAccept)
              if (m_tServerSocket.Accept(*pNewService))
              {
                pNewService->Init();
                break;
              }
          }

          if (IsStopping())
            break;
          
          OnServiceConnect(pNewService);

        }
        RISE_CATCH_ALL
      }
      
      rise::LogNotice() << " _____ STOPPING DISPATCHER _____ ";
      Cleanup();
    }

    void OnServiceConnect(PRemoteService& pNewService)
    {
      const std::string& sServiceName = pNewService->GetName();
      const std::string& sSessionId = pNewService->GetID();
      const std::string& sServiceNameID = sServiceName + ":" + sSessionId;
      if (sServiceName != "")
      {
        std::string sError;

        if(staff::security::CSessions::Inst().Validate(sSessionId))
        {
          CService* pExistingService = m_pComponent->GetService(sServiceName);

          // find local service with same name
          if (pExistingService == NULL || (pExistingService->GetImpl(sSessionId) == NULL &&
            m_mRemoteServiceMap.find(sServiceNameID) == m_mRemoteServiceMap.end()))
          {
            CRemoteServiceWrapper* pRemoteServiceWrapper = reinterpret_cast<CRemoteServiceWrapper*>(pExistingService);

            if (pRemoteServiceWrapper == NULL) // no remote service found
            {
              pRemoteServiceWrapper = new CRemoteServiceWrapper(m_pComponent);
              pRemoteServiceWrapper->GetServices()[sSessionId] = pNewService;
              m_pComponent->AddService(pRemoteServiceWrapper);
            } else
              pRemoteServiceWrapper->GetServices()[sSessionId] = pNewService;

            rise::LogInfo() << "RemoteService \"" << sServiceName << "(" << sSessionId << ")\" connected...";
            m_mRemoteServiceMap[sServiceNameID] = pNewService;
            if(m_stEvents.pOnConnect != NULL && sSessionId == staff::security::CSessions::sNobodySessionId)
            {
              m_stEvents.pOnConnect(sServiceName, pRemoteServiceWrapper);
            }

            return;
          }
          
          sError = "already connected";
        }
        else
        {
          sError = "session unknown or expired";
        }

        {
          rise::LogWarning() << "Service \"" + sServiceName + "(" + sSessionId +  ")\" " + sError;
          COperation tOperation("Fault");
          tOperation.Request().Value() = "Service \"" + sServiceName + "(" + sSessionId + ")\": " + sError;
          pNewService->Invoke(tOperation);
        }
      } else
        rise::LogError() << "Error while connecting Service. Service disconnected.";
    }

    void OnServiceDisconnect(PRemoteService& pService)
    {
      const std::string& sServiceName = pService->GetName();
      const std::string& sSessionId = pService->GetID();
      if(m_stEvents.pOnDisconnect != NULL && sSessionId == staff::security::CSessions::sNobodySessionId)
      {
        m_stEvents.pOnDisconnect(sServiceName);
      }

      CService* pExistingService = m_pComponent->GetService(sServiceName);

      // service is remote
      if (pExistingService != NULL && pExistingService->GetImpl(sSessionId) == NULL)
      {
        TRemoteServiceMap& rmServices = dynamic_cast<CRemoteServiceWrapper*>(pExistingService)->GetServices();
        rmServices.erase(sSessionId);
        
        if (rmServices.size() == 0)
        {
          m_pComponent->RemoveService(sServiceName);
        }
      }
    }

    bool SelectAndClean()
    {
      bool bReadyToAccept = false;
      rise::SOCKET hSockMax = m_tServerSocket.GetHandle();
      fd_set stRead;

      FD_ZERO(&stRead);

      FD_SET(m_tServerSocket.GetHandle(), &stRead);
      for (TRemoteServiceMap::iterator itService = m_mRemoteServiceMap.begin(); 
                    itService != m_mRemoteServiceMap.end(); ++itService )
      {
        rise::SOCKET hSocket = itService->second->GetHandle();
        FD_SET(hSocket, &stRead);

        if(hSockMax < hSocket)
          hSockMax = hSocket;
      }

      int nRes = select(hSockMax + 1, &stRead, NULL, NULL, NULL);
      
      if (nRes > 0) // some sockets are signalled
      {
        if (FD_ISSET(m_tServerSocket.GetHandle(), &stRead))
        {
          rise::LogDebug2() << "Server is Ready to read (accept, closed/reset/terminated)";
          bReadyToAccept = true;
        }

        for (TRemoteServiceMap::iterator itService = m_mRemoteServiceMap.begin(); 
              itService != m_mRemoteServiceMap.end(); )
        {
          if (FD_ISSET(itService->second->GetHandle(), &stRead))
          {
            const std::string& rServiceName = itService->second->GetName();
            rise::LogDebug2() << "ServiceClient: \"" << rServiceName << "\"(" << itService->second->GetID() 
                << ") Ready to read (closed/reset/terminated)";
            if(!itService->second->CheckConnection())
            {
              rise::LogInfo() << "ServiceClient: \"" << rServiceName << "\" disconnected\n";
              OnServiceDisconnect(itService->second);
              itService->second->Close();


              TRemoteServiceMap::iterator itRemoveService = itService;
              ++itService;
              m_mRemoteServiceMap.erase(itRemoveService);

//              itService = m_mRemoteServiceMap.erase(itService);
              continue;
            } else
              Sleep(1);
          }
          ++itService;
        }
      }
      
      return bReadyToAccept;
    }

    void Cleanup()
    {
      rise::LogDebug() << "Cleanup";
      rise::LogDebug() << "Closing server...";
      m_tServerSocket.Close();

      for (TRemoteServiceMap::iterator itService = m_mRemoteServiceMap.begin(); 
          itService != m_mRemoteServiceMap.end(); ++itService)
      {
        rise::LogDebug() << "closing connection for service: " << itService->second->GetName();
        itService->second->Close();
      }
      
      m_mRemoteServiceMap.clear();
    }

  public:
    struct SPendingService
    {
      std::string sName;
      std::string sID;
      
      SPendingService(const std::string& sNameInit, const std::string& sIDInit):
        sName(sNameInit), sID(sIDInit)
      {
      }
      
      bool operator<(const SPendingService& rOther) const
      {
        return sName < rOther.sName && sID < rOther.sID;
      }
    };
    
    typedef std::map<std::string, PRemoteService> TRemoteServiceMap;
    typedef std::set<SPendingService> TPendingServiceSet;

  private:
#ifdef __linux__
    static __sighandler_t m_pSigHandler;
    static CRemoteServiceDispatcherImpl* m_pSelf;
#endif

  public:
    rise::sockets::CServerSocket        m_tServerSocket;
    TRemoteServiceMap                   m_mRemoteServiceMap;
    CServiceDispatcher::SEvents         m_stEvents;
    TPendingServiceSet                  m_tsPendingServiceSet;
    CCompositeComponent*                m_pComponent;
  };

#ifdef __linux__
  CServiceDispatcher::CRemoteServiceDispatcherImpl* CServiceDispatcher::CRemoteServiceDispatcherImpl::m_pSelf = NULL;
  __sighandler_t CServiceDispatcher::CRemoteServiceDispatcherImpl::m_pSigHandler = NULL;
#endif


//////////////////////////////////////////////////////////////////////////
// Local Impl

  class CServiceDispatcher::CLocalServiceDispatcherImpl
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
      if(lsComponentDirs.size() == 0)
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
        }
      }

      if (m_stEvents.pOnConnect != NULL)
      {
        const TServiceMap& rServices = rSharedContext.GetServices();
        for (TServiceMap::const_iterator itService = rServices.begin(); 
          itService != rServices.end(); ++itService)
          m_stEvents.pOnConnect(itService->first, itService->second);
      }
    }

    void Deinit()
    {
      CSharedContext& rSharedContext = CSharedContext::Inst();
      if (m_stEvents.pOnDisconnect != NULL)
      {
        const TServiceMap& rServices = rSharedContext.GetServices();
        for (TServiceMap::const_iterator itService = rServices.begin(); 
          itService != rServices.end(); ++itService)
          m_stEvents.pOnDisconnect(itService->first);
      }

      rSharedContext.Clear();
      m_lsComponents.UnloadAll();
    }

    CServiceDispatcher::SEvents m_stEvents;
    rise::plugin::CPluginManager<CComponent> m_lsComponents;
  };

//////////////////////////////////////////////////////////////////////////
// SEvents
  CServiceDispatcher::SEvents::SEvents( void (*pOnConnectInit)(const std::string&, const CService*), void (*pOnDisconnectInit)(const std::string&) ) :
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


//////////////////////////////////////////////////////////////////////////
// facade  

  CServiceDispatcher& CServiceDispatcher::Inst()
  {
    if(m_pInst == NULL)
      m_pInst = new CServiceDispatcher;
    return *m_pInst;
  }

  void CServiceDispatcher::FreeInst()
  {
    if(m_pInst != NULL)
    {
      delete m_pInst;
      m_pInst = NULL;
    }
  }

  void CServiceDispatcher::Init( const SEvents& stEvents )
  {
rise::LogEntry();
    if(m_pLocalImpl != NULL)
    {
      m_pLocalImpl->m_stEvents = stEvents;
      m_pLocalImpl->Init();
    }
    
    if(m_pRemoteImpl != NULL)
    {
      m_pRemoteImpl->m_stEvents = stEvents;
      m_pRemoteImpl->Start();
    }
  }

  void CServiceDispatcher::Deinit()
  {
rise::LogEntry();
    if(m_pRemoteImpl != NULL && m_pRemoteImpl->IsWorking())
    {
      rise::LogError() << "Remote dispatcher is still runnung!!";
    }

    if(m_pLocalImpl != NULL)
    {
      m_pLocalImpl->Deinit();
    }
  }

  void CServiceDispatcher::InvokeSelf( COperation& rOperation )
  {
rise::LogEntry();
    CSharedContext& rSharedContext = CSharedContext::Inst();
    const std::string& sOpName = rOperation.GetName();
    if (sOpName == "GetServices")
    {
      const TServiceMap& rServices = rSharedContext.GetServices();
      for (TServiceMap::const_iterator itService = rServices.begin(); 
            itService != rServices.end(); ++itService)
      {
        std::string sServiceName;
        const CComponent* pServiceComponent = itService->second->GetComponent();

        if(pServiceComponent != NULL)
        {
          sServiceName = pServiceComponent->GetName();
          if(sServiceName.size() != 0)
            sServiceName += ".";
        }

        sServiceName += itService->second->GetName();
        rOperation.Result().CreateChild("Service", sServiceName);
      }
    } else
    if (sOpName == "GetOperations")
    {
      const staff::CDataObject& rRequest = rOperation.Request();
      const CService* pService = rSharedContext.GetService(rRequest["sServiceName"]);
      RISE_ASSERTES(pService != NULL, CRemoteInternalException, 
        "Service is not registered: " + rOperation.Request()["ServiceName"].AsString());

      rOperation.Result() = pService->GetOperations();
    } else
      RISE_THROWS(CRemoteInternalException, "Operation not found: " + sOpName);
  }

  CServiceDispatcher::CServiceDispatcher():
    m_pLocalImpl(new CLocalServiceDispatcherImpl),
    m_pRemoteImpl(new CRemoteServiceDispatcherImpl)
  {
  }

  CServiceDispatcher::~CServiceDispatcher()
  {
rise::LogEntry();
    if (m_pRemoteImpl != NULL)
    {
      delete m_pRemoteImpl;
      m_pRemoteImpl = NULL;
    }
    
    if (m_pLocalImpl != NULL)
    {
      delete m_pLocalImpl;
      m_pLocalImpl = NULL;
    }
  }

  void CServiceDispatcher::GetRemoteService( const std::string& sServiceName, const std::string& sSessionId, PRemoteService& rpService )
  {
    m_pRemoteImpl->GetService(sServiceName, sSessionId, rpService);
  }

  CServiceDispatcher* CServiceDispatcher::m_pInst = NULL;


  std::string CServiceDispatcher::CRemoteServiceDispatcherImpl::CRemoteComponent::m_sName = "";
  TServiceMap CServiceDispatcher::CRemoteServiceDispatcherImpl::CRemoteComponent::m_tMap;
}
