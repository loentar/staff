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

#include <rise/os/oscommon.h>
#include <errno.h>
#include <iostream>
#include <rise/common/exmacros.h>
#include <rise/common/Exception.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include "Thread.h"

namespace rise
{
  namespace threading
  {
    CThread::CThread():
      m_hThread(0), m_bStopping(false)
    {
    }

    CThread::~CThread()
    {
      try
      {
        if ( IsWorking() )
          Stop(5000);
      }
      catch(const CLogicNoInitException&)
      {
      }
      catch(const CLogicTimeoutException&)
      {
        if (m_hThread != 0)
        {
          rise::LogWarning() << "Terminating thread #" << m_hThread << " by timeout!";
          Cancel();
        }
      }
      RISE_CATCH_ALL
    }

    bool CThread::Start(void* pParam /*= NULL*/, bool bDetached /* = true*/)
    {
      CScopedCriticalSection cs(m_cs);
//      m_hThread = reinterpret_cast<HThread>(0xFFFFFFFFul);   // WARN!! isWorking = true;
      m_pParam = pParam;
      return osCreateThread(InternalRun, &m_hThread, this, bDetached);
    }
    
    bool CThread::IsWorking() const
    {
      return m_hThread != 0 && osIsThreadExists(m_hThread);
    }

    HThread CThread::GetCurrentId()
    {
      return osGetCurrentThread();
    }

    HThread CThread::GetId() const
    {
      return m_hThread;
    }

    void CThread::Exit()
    {
      osExitThread();
    }

    bool CThread::Cancel()
    {
      CScopedCriticalSection cs(m_cs);
      
      LogWarning() << "Terminating thread!";
    
      if ( m_hThread == 0 )
      {
        LogError() << "thread is not running";
        return false;
      }

      if ( !osTerminateThread(m_hThread) )
      { 
        LogError() << "osTerminateThread(" << m_hThread << ") error#" << errno <<
               " (" << strerror(errno) << ")";
        return false;
      }

      m_hThread = 0;
      return true;
    }

    void CThread::JoinThread()
    {
      osJoinThread(m_hThread);
    }

    bool CThread::Wait()
    {
      return osWaitForThread(m_hThread);
    }

    bool CThread::Wait(unsigned long ulTimeout)
    {
      return osWaitForThreadExit(m_hThread, ulTimeout);
    }

    void CThread::Stop( ulong ulTimeout /*= 0ul*/ )
    {
      CScopedCriticalSection cs(m_cs);
      RISE_ASSERTES(IsWorking(), CLogicNoInitException, "Thread does not exists");
      m_bStopping = true;
      OnStop();
      if(!osIsCurrentThread(GetId()))
      {
        RISE_ASSERTE(Wait(ulTimeout), CLogicTimeoutException);
        m_bStopping = false;
      }
    }

    bool CThread::IsStopping()
    {
      return m_bStopping;
    }

    void CThread::Sleep(unsigned long ulMSec)
    {
      osSleep(ulMSec);
    }

    void* CThread::InternalRun(void* pParam)
    {
      CThread* pThread = static_cast<CThread*>(pParam);
  
      if ( pThread == NULL )
        LogError() << "invalid param: pParam == NULL!";
      else
      {
        try
        {
          pThread->Run(pThread->m_pParam);
        }
        RISE_CATCH_ALL
      }

      pThread->m_hThread = 0;
      pThread->m_bStopping = false;
      Exit();
      return NULL;
    }

    void CThread::OnStop()
    {
    }

    CCriticalSection& CThread::GetCS()
    {
      return m_cs;
    }
  } // namespace threading

}  // namespace rise
