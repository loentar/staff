#include <map>
#include <signal.h>
#include <rise/common/Log.h>
#include <rise/common/Exception.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include "SignalHandler.h"

namespace rise
{
  class CSignalCatcher  // диспетчер сигналов
  {
  public:
    typedef std::multimap<int, CSignalHandler*>  TSignalHandlerMap;  // тип - список обработчиков сигнала
    typedef std::map<int, CString>               TStrSignalMap;
#ifndef WIN32
    typedef std::map<int, struct sigaction>      TSignalActionMap;
#endif

  public:
    CSignalCatcher()      
    {
#ifndef WIN32
      sigfillset(&(m_stAction.sa_mask));
#endif

#define RISE_REGISTER_SIGNAL_DESCR(SIGNAL_NAME)\
        m_tStrSignalMap[CSignalHandler::SIGNAL_NAME] = #SIGNAL_NAME;

#ifdef WIN32
      RISE_REGISTER_SIGNAL_DESCR(ESIGNONE)
      RISE_REGISTER_SIGNAL_DESCR(ESIGABRT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGABRT_COMPAT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGFPE)
      RISE_REGISTER_SIGNAL_DESCR(ESIGILL)
      RISE_REGISTER_SIGNAL_DESCR(ESIGINT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGSEGV)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTERM)
      RISE_REGISTER_SIGNAL_DESCR(ESIGHUP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGQUIT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTRAP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGBUS)
      RISE_REGISTER_SIGNAL_DESCR(ESIGKILL)
      RISE_REGISTER_SIGNAL_DESCR(ESIGUSR1)
      RISE_REGISTER_SIGNAL_DESCR(ESIGUSR2)
      RISE_REGISTER_SIGNAL_DESCR(ESIGPIPE)
      RISE_REGISTER_SIGNAL_DESCR(ESIGALRM)
      RISE_REGISTER_SIGNAL_DESCR(ESIGCHLD)
      RISE_REGISTER_SIGNAL_DESCR(ESIGCONT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGSTOP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTSTP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTTIN)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTTOU)
      RISE_REGISTER_SIGNAL_DESCR(ESIGURG)
      RISE_REGISTER_SIGNAL_DESCR(ESIGXCPU)
      RISE_REGISTER_SIGNAL_DESCR(ESIGXFSZ)
      RISE_REGISTER_SIGNAL_DESCR(ESIGVTALRM)
      RISE_REGISTER_SIGNAL_DESCR(ESIGPROF)
      RISE_REGISTER_SIGNAL_DESCR(ESIGWINCH)
      RISE_REGISTER_SIGNAL_DESCR(ESIGIO)
      RISE_REGISTER_SIGNAL_DESCR(ESIGPWR)
      RISE_REGISTER_SIGNAL_DESCR(ESIGSYS)
      RISE_REGISTER_SIGNAL_DESCR(ESIGRTMIN)
#else
      RISE_REGISTER_SIGNAL_DESCR(ESIGNONE)
      RISE_REGISTER_SIGNAL_DESCR(ESIGHUP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGINT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGQUIT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGILL)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTRAP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGABRT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGBUS)
      RISE_REGISTER_SIGNAL_DESCR(ESIGFPE)
      RISE_REGISTER_SIGNAL_DESCR(ESIGKILL)
      RISE_REGISTER_SIGNAL_DESCR(ESIGUSR1)
      RISE_REGISTER_SIGNAL_DESCR(ESIGSEGV)
      RISE_REGISTER_SIGNAL_DESCR(ESIGUSR2)
      RISE_REGISTER_SIGNAL_DESCR(ESIGPIPE)
      RISE_REGISTER_SIGNAL_DESCR(ESIGALRM)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTERM)
      RISE_REGISTER_SIGNAL_DESCR(ESIGCHLD)
      RISE_REGISTER_SIGNAL_DESCR(ESIGCONT)
      RISE_REGISTER_SIGNAL_DESCR(ESIGSTOP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTSTP)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTTIN)
      RISE_REGISTER_SIGNAL_DESCR(ESIGTTOU)
      RISE_REGISTER_SIGNAL_DESCR(ESIGURG)
      RISE_REGISTER_SIGNAL_DESCR(ESIGXCPU)
      RISE_REGISTER_SIGNAL_DESCR(ESIGXFSZ)
      RISE_REGISTER_SIGNAL_DESCR(ESIGVTALRM)
      RISE_REGISTER_SIGNAL_DESCR(ESIGPROF)
      RISE_REGISTER_SIGNAL_DESCR(ESIGWINCH)
      RISE_REGISTER_SIGNAL_DESCR(ESIGIO)
      RISE_REGISTER_SIGNAL_DESCR(ESIGPWR)
      RISE_REGISTER_SIGNAL_DESCR(ESIGSYS)
      RISE_REGISTER_SIGNAL_DESCR(ESIGRTMIN)
#endif

#undef RISE_REGISTER_SIGNAL_DESCR
    }

    ~CSignalCatcher()
    {
    }
    
    void MapSignal(int nSignal)
    {
#ifndef WIN32
      UnMapSignal(nSignal); // чтобы при второй регистрации не затерся старый обработчик
      m_stAction.sa_handler = SignalCatcherInternal;
      struct sigaction& rSigaction = m_mSignalAction[nSignal];
      sigaction(nSignal, &m_stAction, &rSigaction);
      rise::LogDebug() << "added signal " << GetSignalStr(nSignal) <<  " , previous action: " 
                        << rSigaction.sa_sigaction << ": handler" << rSigaction.sa_handler;
#else
      if (nSignal <= CSignalHandler::ESIGTERM)
        signal(nSignal, &SignalCatcherInternal);
#endif
    }
    
    void UnMapSignal(int nSignal)
    {
#ifndef WIN32
      TSignalActionMap::iterator itSignalAct = m_mSignalAction.find(nSignal);
      if(itSignalAct != m_mSignalAction.end())
      {
        sigaction(nSignal, &(itSignalAct->second), NULL);
        m_mSignalAction.erase(itSignalAct);
      }
#else
      signal(nSignal, NULL);
#endif
    }
  
    void RegisterSignal(int nSignal, CSignalHandler* pSignalHandler)
    {
      RISE_ASSERTP(pSignalHandler);  // проверка параметров
    
      LogDebug2() << "registering signal " << CSignalHandler::GetSignalStr(nSignal) << 
                          " on handler: " << pSignalHandler;

      if ( m_mHandlers.find(nSignal) == m_mHandlers.end() )  // если еще нет обработчика сигнала
        MapSignal(nSignal);                                  // установка обработчика
      
      // вставка в список обработчиков сигнала
      m_mHandlers.insert(std::make_pair(nSignal, pSignalHandler));
    }

    void UnRegisterSignal(int nSignal, CSignalHandler* pSignalHandler)
    {
      RISE_ASSERTP(pSignalHandler);  // проверка параметров

      // поиск обработчика по сигналу
      TSignalHandlerMap::iterator iter = m_mHandlers.find(nSignal);
    
      // поиск подходящего обработчика
      for(; iter != m_mHandlers.end() && iter->first == nSignal; ++iter)
        if ( iter->second == pSignalHandler ) // если обработчик тот
        {
          m_mHandlers.erase(iter);  // удаление его из массива
          // если это был последний обработчик этого сигнала
          if ( m_mHandlers.find(nSignal) == m_mHandlers.end() )
            UnMapSignal(nSignal);  // снятие обработки этого сигнала
          return;
        }

      // сигнал не найден
      RISE_THROWS(CLogicNoItemException, "Signal #" + CSignalHandler::GetSignalStr(nSignal) + 
          " does not registered on handler " + ToStr(pSignalHandler));
    }
    
    void UnRegisterAll(CSignalHandler* pSignalHandler)
    {
      RISE_ASSERTP(pSignalHandler);  // проверка параметров

      for(TSignalHandlerMap::iterator iter = m_mHandlers.begin(); iter != m_mHandlers.end(); )
      {
        if ( iter->second == pSignalHandler )
        {
          int nSignal = iter->first;
          m_mHandlers.erase(iter);

          if ( m_mHandlers.find(nSignal) == m_mHandlers.end() )
            UnMapSignal(nSignal);

          iter = m_mHandlers.begin(); // MCBC bug
        } else
          ++iter;
      }
    }
    
    const CString& GetSignalStr( int nSignal ) const
    {
      TStrSignalMap::const_iterator itFnd = m_tStrSignalMap.find(nSignal);
      if (itFnd == m_tStrSignalMap.end())
      {
        if(nSignal > CSignalHandler::ESIGRTMIN)
        {
          static std::string sSig;
          sSig = "ESIGRTMIN+" + ToStr(nSignal - CSignalHandler::ESIGRTMIN);
          return sSig;
        }
        return m_sSigUnknown;
      }

      return itFnd->second;    
    }
    
  public:
    static CSignalCatcher& Inst()
    {
      if(m_pInst == NULL)
        m_pInst = new CSignalCatcher;
      
      return *m_pInst;
    }

  private:
    static void SignalCatcherInternal(int nSignal)
    {
      Inst().SignalCatcher(nSignal);
    }
    
    
    void SignalCatcher(int nSignal)
    {
      rise::LogDebug2() << "sending signal: " << GetSignalStr(nSignal) << " to handlers";
      TSignalHandlerMap::iterator itHandler = m_mHandlers.find(nSignal);
      for(; itHandler != m_mHandlers.end() && itHandler->first == nSignal; ++itHandler)
        itHandler->second->SignalHandler(nSignal);

#ifndef WIN32
      rise::LogDebug2() << "sending signal: " << GetSignalStr(nSignal) << " to previous handlers";
      TSignalActionMap::iterator itAction = m_mSignalAction.find(nSignal);
      if(itAction != m_mSignalAction.end() && itAction->second.sa_handler != NULL)
      {
        rise::LogDebug2() << "PREV HANDLER";
        itAction->second.sa_handler(nSignal);
      }
#endif
    }

  public:
    static const CString     m_sSigUnknown;
    TStrSignalMap            m_tStrSignalMap;
    TSignalHandlerMap        m_mHandlers;
#ifndef WIN32
    TSignalActionMap         m_mSignalAction;
    struct sigaction         m_stAction;
#endif

  private:
    static CSignalCatcher* m_pInst;
  };
  
  const CString CSignalCatcher::m_sSigUnknown = "EUNKNOWN";
  CSignalCatcher* CSignalCatcher::m_pInst = NULL;

/////////////////////////////////////////////////////////////////////////////////////////
  
  CSignalHandler::~CSignalHandler()
  {
    UnRegisterAll();
  }

  void CSignalHandler::RegisterSignal(int nSignal)
  {
    CSignalCatcher::Inst().RegisterSignal(nSignal, this);
  }

  void CSignalHandler::UnRegisterSignal(int nSignal)
  {
    CSignalCatcher::Inst().UnRegisterSignal(nSignal, this);
  }

  void CSignalHandler::UnRegisterAll()
  {
    CSignalCatcher::Inst().UnRegisterAll(this);
  }  

  const CString& CSignalHandler::GetSignalStr( int nSignal )
  {
    return CSignalCatcher::Inst().GetSignalStr(nSignal);
  }

} // namespace rise
