#include <signal.h>
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
      const rise::CString& GetName() const
      {
        return m_sName;
      }

      const CService* GetService(const rise::CString& sService) const
      {
        return NULL;
      }

      CService* GetService(const rise::CString& sService)
      {
        return NULL;
      }

      const TServiceMap& GetServices() const
      {
        return m_tMap;
      }
      
    private:
      static rise::CString m_sName;
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
        rise::LogDebug() << "Сигнал: " << nSignal;
        if(nSignal == SIGINT)
        {
          if(m_pSelf->GetId() == CThread::GetCurrentId()) // поток диспетчера
          {
            rise::LogDebug1() << "поток диспетчера: Останов";
            m_pSelf->Stop();
          }
          else // поток axis2c
          {
            rise::LogDebug() << "Ожидание завершения диспетчера";
            for(int i = 0; i < 200 && m_pSelf->IsWorking(); ++i)
              Sleep(10);
              
            if(m_pSelf->IsWorking())
            {
              rise::LogError() << "Поток диспетчера всё еще запущен: уничтожение!!!";
              m_pSelf->Cancel();
            } else
            {
              rise::LogDebug() << "Диспетчер завершил работу";
            }

            rise::LogDebug() << "Отправка сигнала Axis2/C";

            // восстанавливаем обработчик сигнала и посылаем сигнал axis2c
            // явный вызов разрушает стек
            signal(SIGINT, m_pSigHandler);
            if(kill(0, SIGINT) != 0)
            {
              rise::LogError() << "Ошибка отправки сигнала Axis2/c";
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

    bool WaitService( const rise::CString& sServiceName, const rise::CString& sSessionId, PRemoteService& rpService )
    {
      // ожидание готовности сервиса
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

    void GetService( const rise::CString& sServiceName, const rise::CString& sServiceID, PRemoteService& rpRemoteService )
    {
      rise::LogDebug2() << "GetService [" << sServiceName << "] with SSessionId=[" << sServiceID << "]...";

      TRemoteServiceMap::iterator itLastFind = m_mRemoteServiceMap.end();

      rise::LogDebug2() << "services Count= " << m_mRemoteServiceMap.size();

      const rise::CString& sServiceNameID = sServiceName + ":" + sServiceID;


      TRemoteServiceMap::iterator itFind = m_mRemoteServiceMap.find(sServiceNameID);
      if (itFind != m_mRemoteServiceMap.end())
      {
        rpRemoteService = itFind->second;
        return;
      }

      itLastFind = m_mRemoteServiceMap.find(sServiceName + ":");

      if (itLastFind != m_mRemoteServiceMap.end())
      {
        SPendingService stService(sServiceName, sServiceID);
        std::pair<TPendingServiceSet::iterator, bool> tpInsPendingService = m_tsPendingServiceSet.insert(stService);
        if (!tpInsPendingService.second)
        {  // если не была вставка, значит сервис уже в очереди, ожидаем его
          if (WaitService(sServiceName, sServiceID, rpRemoteService))
            return;
        } else
        { // сервиса еще нет в очереди
          rise::LogDebug2() << ">>> Service [" << sServiceName << "] with SSessionId=[" << sServiceID << "] is not found, trying to create...";
          itLastFind->second->CreateServiceID(sServiceID);

          try
          {
            bool bServiceReady = WaitService(sServiceName, sServiceID, rpRemoteService);
            rise::LogDebug2() << ">>> Removing Service [" << sServiceName << "] with SSessionId=[" << sServiceID << "] from queue: " << (bServiceReady ? "Service Ready" : "Timeout");
            m_tsPendingServiceSet.erase(tpInsPendingService.first);
            if (bServiceReady)
              return;
          }
          catch(...)
          {
            rise::LogDebug2() << ">>> Removing Service [" << sServiceName << "] with SSessionId=[" << sServiceID << "] from queue: Exception";
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
          if(nError == EALREADY)
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
      const rise::CString& sServiceName = pNewService->GetName();
      const rise::CString& sServiceID = pNewService->GetID();
      const rise::CString& sServiceNameID = sServiceName + ":" + sServiceID;
      if (sServiceName != "")
      {
        CService* pExistingService = m_pComponent->GetService(sServiceName);

        // локального сервиса с таким именем нет?
        if (pExistingService == NULL || pExistingService->GetImpl(sServiceID) == NULL && 
          m_mRemoteServiceMap.find(sServiceNameID) == m_mRemoteServiceMap.end())
        {
          CRemoteServiceWrapper* pRemoteServiceWrapper = reinterpret_cast<CRemoteServiceWrapper*>(pExistingService);

          if (pRemoteServiceWrapper == NULL) // нет еще сервисов в этой группе
          {
            pRemoteServiceWrapper = new CRemoteServiceWrapper(m_pComponent); // создаем новый менеджер удаленыых сервисв группы
            pRemoteServiceWrapper->GetServices()[sServiceID] = pNewService; // добавляем сервим в менеджер группы
            m_pComponent->AddService(pRemoteServiceWrapper); // добавляем сервис в компонент
          } else
            pRemoteServiceWrapper->GetServices()[sServiceID] = pNewService; // добавляем сервим в менеджер группы

          rise::LogInfo() << "RemoteService \"" << sServiceName << "(" << sServiceID << ")\" connected...";
          m_mRemoteServiceMap[sServiceNameID] = pNewService;
          if(m_stEvents.pOnConnect != NULL && sServiceID == "")
            m_stEvents.pOnConnect(sServiceName, pRemoteServiceWrapper);
          
          return;
        }

        {
          rise::LogWarning() << "Service \"" + sServiceName + "(" + sServiceID + ")\" already connected";
          COperation tOperation("Fault");
          tOperation.Request().Value() = "Service \"" + sServiceName + "\" already connected";
          pNewService->Invoke(tOperation);
        }
      } else
        rise::LogError() << "Error while connecting Service. Service disconnected.";
    }

    void OnServiceDisconnect(PRemoteService& pService)
    {
      const rise::CString& sServiceName = pService->GetName();
      const rise::CString& sServiceID = pService->GetID();
      if(m_stEvents.pOnDisconnect != NULL && sServiceID == "")
        m_stEvents.pOnDisconnect(sServiceName);

      CService* pExistingService = m_pComponent->GetService(sServiceName);

      // сервис не локальный
      if (pExistingService != NULL && pExistingService->GetImpl(sServiceID) == NULL)
      {
        TRemoteServiceMap& rmServices = dynamic_cast<CRemoteServiceWrapper*>(pExistingService)->GetServices();
        rmServices.erase(sServiceID);
        
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
      
      if (nRes > 0) // если есть среагировавшие сокеты 
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
      rise::CString sName;
      rise::CString sID;
      
      SPendingService(const rise::CString& sNameInit, const rise::CString& sIDInit):
        sName(sNameInit), sID(sIDInit)
      {
      }
      
      bool operator<(const SPendingService& rOther) const
      {
        return sName < rOther.sName && sID < rOther.sID;
      }
    };
    
    typedef std::map<rise::CString, PRemoteService> TRemoteServiceMap;
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
      const rise::CString sComponentsDir = CRuntime::Inst().GetComponentsHome();
      CSharedContext& rSharedContext = CSharedContext::Inst();
      rise::CStringList lsComponentDirs;
      
      // поиск каталогов с компонентами
      rise::CFileFind::Find(sComponentsDir, lsComponentDirs, "*", rise::CFileFind::EFA_DIR);
      if(lsComponentDirs.size() == 0)
      {
        rise::LogDebug() << "не найдено ни одного компонента";
      }

      for (rise::CStringList::const_iterator itDir = lsComponentDirs.begin(); 
                itDir != lsComponentDirs.end(); ++itDir )
      {
        // поиск библиотек с компонентами
        rise::CStringList lsComponents;
        rise::CString sComponentDir = sComponentsDir + RISE_PATH_SEPARATOR + *itDir + RISE_PATH_SEPARATOR;
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
            // загрузка компонента
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
      rise::LogError() << "удаленный диспетчер всё еще запущен!!";
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
    const rise::CString& sOpName = rOperation.GetName();
    if (sOpName == "GetServices")
    {
      const TServiceMap& rServices = rSharedContext.GetServices();
      for (TServiceMap::const_iterator itService = rServices.begin(); 
            itService != rServices.end(); ++itService)
      {
        rise::CString sServiceName;
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

      rOperation.Result().Clone(pService->GetOperations());
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

  void CServiceDispatcher::GetRemoteService( const rise::CString& sServiceName, const rise::CString& sSessionId, PRemoteService& rpService )
  {
    m_pRemoteImpl->GetService(sServiceName, sSessionId, rpService);
  }

  CServiceDispatcher* CServiceDispatcher::m_pInst = NULL;


  rise::CString CServiceDispatcher::CRemoteServiceDispatcherImpl::CRemoteComponent::m_sName = "";
  TServiceMap CServiceDispatcher::CRemoteServiceDispatcherImpl::CRemoteComponent::m_tMap;
}
