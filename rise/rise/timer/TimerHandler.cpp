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
#include <errno.h>
#endif
#include <rise/common/Log.h>
#include <rise/common/Exception.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include "TimerHandler.h"

namespace rise
{
  static class CTimeCatcher
#ifndef WIN32
    : public CSignalHandler
#endif
  {
    protected:
      struct STimer
      {
        CTimerHandler::TTimerID  m_wID;
        dword                    m_dwMSec;
        dword                    m_dwRepeat;
        CTimerHandler*           m_pTimer;
        
        STimer(CTimerHandler::TTimerID wID, dword dwMSec, dword dwRepeat, CTimerHandler* pTimer):
          m_wID(wID), m_dwMSec(dwMSec), m_dwRepeat(dwRepeat), m_pTimer(pTimer)
        {
        }

        STimer():
          m_wID(0), m_dwMSec(0), m_dwRepeat(0), m_pTimer(NULL)
        {}
      };
  
      typedef std::list<STimer> CTimerList;  
  
      CTimerList                m_lsTimer;
      CTimerHandler::TTimerID   m_wLastId;
#ifndef WIN32
      dword      m_dwTime;
      dword      m_dwLastDelTimerta;
#endif

    public:
      CTimeCatcher():
        m_wLastId(0)
#ifndef WIN32
        , m_dwTime(0), m_dwLastDelTimerta(0)
#endif
      {
      }
  
      virtual ~CTimeCatcher()
      {
        if ( !m_lsTimer.empty() )
        {
          LogDebug2() << "timers list is not empty!";
  
          CTimerList::iterator iter = m_lsTimer.begin();

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
  
      void DelTimer(CTimerHandler::TTimerID wID, CTimerHandler* pTimer)
      {
        RISE_ASSERTP(pTimer);
  
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
  
      void DelAllTimers(CTimerHandler* pTimer)
      {
#ifndef WIN32
        bool bWasEmpty = m_lsTimer.empty();
#endif
        
        CTimerList::iterator iter = m_lsTimer.begin();
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
      void SetTimer( dword dwTime )
      {
        RISE_ASSERTP(dwTime);
  
        itimerval value = { { 0, 0 }, {  (long)(dwTime / 1000), (long)((dwTime % 1000) * 1000) } };
  
        int iRet = setitimer( ITIMER_REAL, &value, NULL );
        
        RISE_ASSERTES(iRet != -1, CFileIOException, strerror(errno));
      }
  
      void KillTimer()
      {
        itimerval value = { { 0, 0 }, { 0, 0 } };
  
        int iRet = setitimer( ITIMER_REAL, &value, NULL );
        
        RISE_ASSERTE(iRet != -1, CFileIOException);
      }
#endif  

    protected:
#ifdef WIN32
      void OnTimer( CTimerHandler::TTimerID nIDEvent )
      {      
        for(CTimerList::iterator iter = m_lsTimer.begin(); iter != m_lsTimer.end(); )
        {
          if ( iter->m_wID == nIDEvent )
          {
            iter->m_pTimer->OnTimer( iter->m_wID );

            if ( iter->m_dwRepeat != CTimerHandler::EINFINITE )
            {
              --(iter->m_dwRepeat);
              if ( iter->m_dwRepeat == 0 )
              {
                ::KillTimer( 0, iter->m_wID );
                iter = m_lsTimer.erase(iter);
                continue;
              }
            }
          }
          ++iter;
        }
      }
#else
      void SignalHandler(int nSignal)
      {
        RISE_ASSERTP(nSignal == SIGALRM);
        
        CTimerList::iterator iter = m_lsTimer.begin();
  
        m_dwTime += m_dwLastDelTimerta;
        m_dwLastDelTimerta = 0;
        
        while ( iter != m_lsTimer.end() )
        {
          dword dwRest = m_dwTime % iter->m_dwMSec;
  
          if ( dwRest == 0 )
          {
            iter->m_pTimer->OnTimer( iter->m_wID );

            if ( iter->m_dwRepeat != CTimerHandler::EINFINITE )
            {
              --(iter->m_dwRepeat);
              if ( iter->m_dwRepeat == 0 )
              {
                iter = m_lsTimer.erase(iter);
  
                if ( m_lsTimer.empty() )
                  UnRegisterSignal(SIGALRM);
  
                continue;
              }
            }

            if ( m_dwLastDelTimerta > iter->m_dwMSec || !m_dwLastDelTimerta )
              m_dwLastDelTimerta = iter->m_dwMSec;
  
          } else
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
  void CALLBACK TimerProc( HWND, UINT, UINT nIDEvent, DWORD)
  {
    timeCatcher.OnTimer(static_cast<CTimerHandler::TTimerID>(nIDEvent));
  }
#endif

  CTimerHandler::CTimerHandler()
  {
  }

  CTimerHandler::~CTimerHandler()
  {
    timeCatcher.DelAllTimers(this);
  }

  CTimerHandler::TTimerID CTimerHandler::AddTimer( dword dwMSec, dword dwRepeat /*= EINFINITE*/ )
  {
    return timeCatcher.AddTimer(dwMSec, dwRepeat, this);
  }

  void CTimerHandler::DelTimer( TTimerID wID )
  {
    timeCatcher.DelTimer(wID, this);
  }

  void CTimerHandler::DeleteAllTimers()
  {
    timeCatcher.DelAllTimers(this);
  }

} // namespace rise
