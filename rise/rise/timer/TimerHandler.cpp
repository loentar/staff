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

#include <iostream>
#include <list>
#include <signal.h>
#ifndef WIN32
#include <sys/time.h>
#include <rise/signal/SignalHandler.h>
#include <limits.h>
#endif
#include <rise/common/Log.h>
#include <rise/common/Exception.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include "TimerHandler.h"

namespace rise
{
  //////////////////////////////////////////////////////////////////////////////
  //    CLASS:          CTimeCatcher
  //    DESCRIPTION:    диспетчер сигналов от таймера
  //////////////////////////////////////////////////////////////////////////////
  static class CTimeCatcher
#ifndef WIN32
    : public CSignalHandler
#endif
  {
    protected:
      // тип - список таймеров
      struct STimer // данные о таймере
      {
        CTimerHandler::TTimerID  m_wID;      // идентификатор таймера
        dword                    m_dwMSec;   // задержка между повторениями в милисекундах
        dword                    m_dwRepeat; // количество срабатываний таймера
        CTimerHandler*           m_pTimer;   // указатель на класс обработчика событий от таймера
        
        STimer(CTimerHandler::TTimerID wID, dword dwMSec, dword dwRepeat, CTimerHandler* pTimer):
          m_wID(wID), m_dwMSec(dwMSec), m_dwRepeat(dwRepeat), m_pTimer(pTimer)
        {
        }

        STimer():
          m_wID(0), m_dwMSec(0), m_dwRepeat(0), m_pTimer(NULL)
        {}
      };
  
      typedef std::list<STimer> CTimerList;  
  
      CTimerList                m_lsTimer;      // список таймеров
      CTimerHandler::TTimerID   m_wLastId;
#ifndef WIN32
      dword      m_dwTime;       // общее прошедшее время
      dword      m_dwLastDelTimerta;  // последнее время срабатывания таймера
#endif

    public:
      //////////////////////////////////////////////////////////////////////////////
      //    CONSTRUCTOR:    CTimeCatcher
      //    DESCRIPTION:    default constructor
      //    PARAMETRS:      none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      CTimeCatcher():
        m_wLastId(0)
#ifndef WIN32
        , m_dwTime(0), m_dwLastDelTimerta(0)
#endif
      {
      }
  
      //////////////////////////////////////////////////////////////////////////////
      //    DESTRUCTOR:     CTimeCatcher
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      virtual ~CTimeCatcher()
      {
        if ( !m_lsTimer.empty() )
        {
          LogDebug2() << "timers list is not empty!";
  
          CTimerList::iterator iter = m_lsTimer.begin();
          // поиск соответствующего(их) таймера(ов)
          while ( iter != m_lsTimer.end() )
          {
            LogDebug2() << "Erasing timer: ID=" << iter->m_wID <<
                     " MSec=" << iter->m_dwMSec <<
                     " Repeat=" << iter->m_dwRepeat;
            iter = m_lsTimer.erase(iter);
          }
#ifndef WIN32
          LogDebug2() << "TODO: UnRegisterSignal(SIGALRM)";
          UnRegisterSignal(SIGALRM);
#endif
        }
      }
  
#ifdef WIN32
      friend void CALLBACK TimerProc( HWND, UINT, UINT nIDEvent, DWORD);
#endif

      //////////////////////////////////////////////////////////////////////////////
      //    FUNCTION:       AddTimer
      //    DESCRIPTION:    добавление таймера в список
      //    PARAMETRS:      none
      //    RETURN:         none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      CTimerHandler::TTimerID AddTimer(dword dwMSec, dword dwRepeat, CTimerHandler* pTimer)
      {
        RISE_ASSERTP(pTimer);
        RISE_ASSERTP(dwMSec > 0);
  
#ifdef WIN32
        CTimerList::iterator iter = m_lsTimer.begin();

        m_wLastId = (CTimerHandler::TTimerID)::SetTimer( 0, 0, dwMSec, &TimerProc);
        RISE_ASSERTES(m_wLastId, CInternalInternalException, "Failed to Create Timer!")
        m_lsTimer.push_back(STimer(m_wLastId, dwMSec, dwRepeat, pTimer));
#else
        ++m_wLastId;
        bool bEmpty = m_lsTimer.empty();
        if ( bEmpty )
          RegisterSignal(SIGALRM);
  
        m_lsTimer.push_back(STimer(m_wLastId, dwMSec, dwRepeat, pTimer));
    
        if ( bEmpty )
        {
          m_dwLastDelTimerta = dwMSec;
          SetTimer( m_dwLastDelTimerta );
        }
#endif
        LogDebug2() << "Added Timer: wID=" << m_wLastId << " dwRepeat=" << dwRepeat;
        return m_wLastId;
      }
  
      //////////////////////////////////////////////////////////////////////////////
      //    FUNCTION:       DelTimer
      //    DESCRIPTION:    удаление таймера из списка
      //    PARAMETRS:      none
      //    RETURN:         none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      void DelTimer(CTimerHandler::TTimerID wID, CTimerHandler* pTimer)
      {
        RISE_ASSERTP(pTimer);
  
        // поиск соответствующего таймера
        CTimerList::iterator iter = m_lsTimer.begin();
        for( ; iter != m_lsTimer.end(); ++iter )
          if ( iter->m_pTimer == pTimer && iter->m_wID == wID )
          {
            m_lsTimer.erase(iter);
#ifdef WIN32
            ::KillTimer( 0, wID );
#else
            if ( m_lsTimer.empty() )
              UnRegisterSignal(SIGALRM);
#endif
            return;
          }
  
        RISE_THROW(CLogicNoItemException);
      }
  
      //////////////////////////////////////////////////////////////////////////////
      //    FUNCTION:       DelAllTimers
      //    DESCRIPTION:    очистка списка таймеров
      //    PARAMETRS:      none
      //    RETURN:         none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      void DelAllTimers(CTimerHandler* pTimer)
      {
#ifndef WIN32
        bool bWasEmpty = m_lsTimer.empty();
#endif
        
        CTimerList::iterator iter = m_lsTimer.begin();
        // поиск соответствующего(их) таймера(ов)
        while( iter != m_lsTimer.end() )
          if ( iter->m_pTimer == pTimer )
          {
#ifdef WIN32
            ::KillTimer( 0, iter->m_wID );
#endif
            iter = m_lsTimer.erase(iter);
          }
          else
            ++iter;
#ifndef WIN32
        if ( m_lsTimer.empty() && !bWasEmpty )
          UnRegisterSignal(SIGALRM);
#endif
      }
  
#ifndef WIN32
      //////////////////////////////////////////////////////////////////////////////
      //    FUNCTION:       SetTimer
      //    DESCRIPTION:    установить время срабатывания
      //    PARAMETRS:      none
      //    RETURN:         none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      void SetTimer( dword dwTime )
      {
        RISE_ASSERTP(dwTime);
  
        itimerval value = { { LONG_MAX, LONG_MAX }, {  (long)(dwTime / 1000), (long)((dwTime % 1000) * 1000) } };
  
        int iRet = setitimer( ITIMER_REAL, &value, NULL );
        
        RISE_ASSERTE(iRet != -1, CFileIOException);
      }
  
      //////////////////////////////////////////////////////////////////////////////
      //    FUNCTION:       KillTimer
      //    DESCRIPTION:    Удалить таймер
      //    PARAMETRS:      none
      //    RETURN:         none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      void KillTimer()
      {
        itimerval value = { { 0, 0 }, { 0, 0 } };
  
        int iRet = setitimer( ITIMER_REAL, &value, NULL );
        
        RISE_ASSERTE(iRet != -1, CFileIOException);
      }
#endif  

    protected:
#ifdef WIN32
      //////////////////////////////////////////////////////////////////////////////
      //    FUNCTION:       OnTimer
      //    DESCRIPTION:    функция обработки сигналов от таймера
      //    PARAMETRS:      nIdEvent - идентификатор таймера
      //    RETURN:         none
      //    EXCEPTIONS:     none
      //    COMMENT:        Win32
      //////////////////////////////////////////////////////////////////////////////      
      void OnTimer( CTimerHandler::TTimerID nIDEvent )
      {      
        for(CTimerList::iterator iter = m_lsTimer.begin(); iter != m_lsTimer.end(); )
        {
          if ( iter->m_wID == nIDEvent )  // текущий таймер должен сработать
          {
            iter->m_pTimer->OnTimer( iter->m_wID ); // вызов функции таймера
            // если таймер не должен бесконечно повторяется
            if ( iter->m_dwRepeat != CTimerHandler::EINFINITE )
            {
              --(iter->m_dwRepeat);          // уменьшение числа оставшихся срабатываний
              if ( iter->m_dwRepeat == 0 )   // последнее срабатывание
              {
                ::KillTimer( 0, iter->m_wID );
                iter = m_lsTimer.erase(iter);  // удаление таймера
                continue;
              }
            }
          }
          ++iter;
        }
      }
#else
      //////////////////////////////////////////////////////////////////////////////
      //    FUNCTION:       SignalHandler(...)
      //    DESCRIPTION:    обработчик сигнала
      //    PARAMETRS:      (in) nSignal - номер сигнала
      //    RETURN:         none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      void SignalHandler(int nSignal)
      {
        RISE_ASSERTP(nSignal == SIGALRM);
        
        CTimerList::iterator iter = m_lsTimer.begin();
  
        m_dwTime += m_dwLastDelTimerta;  // общее прошедшее время
        m_dwLastDelTimerta = 0;  // начальное значение
        
        while ( iter != m_lsTimer.end() ) // поиск минимального времени срабатывания
        {
          dword dwRest = m_dwTime % iter->m_dwMSec;  // время
  
          if ( dwRest == 0 )  // текущий таймер должен сработать
          {
            iter->m_pTimer->OnTimer( iter->m_wID ); // вызов функции таймера
             // если таймер не должен бесконечно повторяется
            if ( iter->m_dwRepeat != CTimerHandler::EINFINITE )
            {
              --(iter->m_dwRepeat);          // уменьшение числа оставшихся срабатываний
              if ( iter->m_dwRepeat == 0 )   // последнее срабатывание
              {
                iter = m_lsTimer.erase(iter);  // удаление таймера
  
                if ( m_lsTimer.empty() )      // если таймер был последним
                  UnRegisterSignal(SIGALRM);  // снимаем обработчик сигнала
  
                continue;
              }
            }
            // таймер сработал, но его значение тоже нужно учесть при поиске минимального
            if ( m_dwLastDelTimerta > iter->m_dwMSec || !m_dwLastDelTimerta )
              m_dwLastDelTimerta = iter->m_dwMSec;
  
          } else  // время срабатывания таймера - остаток от деления = время до сл. срабатывания
          if ( m_dwLastDelTimerta > (iter->m_dwMSec - dwRest) || !m_dwLastDelTimerta )
            m_dwLastDelTimerta = iter->m_dwMSec - dwRest;
  
          ++iter;
        }
  
        if ( m_dwLastDelTimerta != 0 )
          SetTimer( m_dwLastDelTimerta );
        else
          KillTimer();
      }
#endif
  } timeCatcher;

#ifdef WIN32
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       TimerProc
  //    DESCRIPTION:    функция обработки сигналов от таймера
  //    PARAMETRS:      nIdEvent - идентификатор таймера
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        Win32
  //////////////////////////////////////////////////////////////////////////////      
  void CALLBACK TimerProc( HWND, UINT, UINT nIDEvent, DWORD)
  {
    timeCatcher.OnTimer(static_cast<CTimerHandler::TTimerID>(nIDEvent));
  }
#endif

  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CTimerHandler
  //    PARAMETRS:      none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CTimerHandler::CTimerHandler()
  {
  }

  //////////////////////////////////////////////////////////////////////////////
  //    DESTRUCTOR:     CTimerHandler
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////
  CTimerHandler::~CTimerHandler()
  {
    timeCatcher.DelAllTimers(this);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       AddTimerTimer
  //    DESCRIPTION:    добавление таймера
  //    PARAMETERS:     wID - идентификатор таймера
  //                    dwMSec - время милисекундах до срабатывания таймера
  //                    dwRepeat  - количество повторений событий от таймера
  //    RETURN:         none
  //    EXCEPTIONS:     CAssertException - неверные параметры вызова
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CTimerHandler::TTimerID CTimerHandler::AddTimer( dword dwMSec, dword dwRepeat /*= EINFINITE*/ )
  {
    return timeCatcher.AddTimer(dwMSec, dwRepeat, this);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       DelTimerTimer
  //    DESCRIPTION:    удаление таймера
  //    PARAMETRS:      wID - идентификатор таймера
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CTimerHandler::DelTimer( TTimerID wID )
  {
    timeCatcher.DelTimer(wID, this);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       DelTimereteAllTimers
  //    DESCRIPTION:    удаление всех таймеров
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CTimerHandler::DeleteAllTimers()
  {
    timeCatcher.DelAllTimers(this);
  }

} // namespace rise
