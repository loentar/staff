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

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CThread
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CThread::CThread():
      m_hThread(0), m_bStopping(false)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CThread
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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
          rise::LogWarning() << "уничтожение потока #" << m_hThread << " по таймауту!";
          Cancel();
        }
      }
      RISE_CATCH_ALL
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Start
    //    DESCRIPTION:    создание и запуск потока
    //    PARAMETRS:      (in) pparam - указатель на параметр передаваемый потоку
    //    RETURN:         0, если создание потока произошло успешно
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CThread::Start(void* pParam /*= NULL*/)
    {
      CScopedCriticalSection cs(m_cs);
//      m_hThread = reinterpret_cast<HThread>(0xFFFFFFFFul);   // WARN!! isWorking = true;
      m_pParam = pParam;
      return osCreateThread(InternalRun, &m_hThread, this);
    }
    
    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       IsWorking
    //    DESCRIPTION:    запущен ли поток
    //    PARAMETRS:      none
    //    RETURN:         true если работает
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CThread::IsWorking() const
    {
      return m_hThread != 0 && osIsThreadExists(m_hThread);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetCurrentId
    //    DESCRIPTION:    получение идентификатора вызывающего потока
    //    PARAMETRS:      none
    //    RETURN:         идентификатор
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    HThread CThread::GetCurrentId()
    {
      return osGetCurrentThread();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetId
    //    DESCRIPTION:    получение идентификатора потока, связаного с обьектом класса
    //    PARAMETRS:      none
    //    RETURN:         идентификатор
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    HThread CThread::GetId() const
    {
      return m_hThread;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Exit
    //    DESCRIPTION:    выход из потока, в котором была вызвана эта функция
    //    PARAMETRS:      none
    //    RETURN:         none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CThread::Exit()
    {
      osExitThread();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Cancel
    //    DESCRIPTION:    завершает работу потока, связанным с обьектом класса
    //    PARAMETRS:      none
    //    RETURN:         none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CThread::Cancel()
    {
      CScopedCriticalSection cs(m_cs);
      
      LogWarning() << "Уничтожение потока без вызова деструкторов локально созданных обьектов, освобожения памяти, дескрипторов и средств межпотоковых синхронизаций. Возможна нестабильная работа приложения!";
    
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

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       JoinThread
    //    DESCRIPTION:    ожидание завершения потока
    //    PARAMETRS:      hThread - дескриптор потока
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CThread::JoinThread()
    {
      osJoinThread(m_hThread);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Stop
    //    DESCRIPTION:    устанавливает флаг остановки потока
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CThread::Stop( ulong ulTimeout /*= 0ul*/ )
    {
      CScopedCriticalSection cs(m_cs);
      RISE_ASSERTES(IsWorking(), CLogicNoInitException, "Поток не запущен");
      m_bStopping = true;
      OnStop();
      if(GetId() != GetCurrentId())
      {
        RISE_ASSERTE(osWaitForThreadExit(m_hThread, ulTimeout), CLogicTimeoutException);
        m_bStopping = false;
      }
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       IsStopping
    //    DESCRIPTION:    возвращает признак необходимости остановки потока
    //    PARAMETRS:      none
    //    RETURN:         true - поток необходимо остановить
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CThread::IsStopping()
    {
      return m_bStopping;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Sleep
    //    DESCRIPTION:    приостановить выполнение текущего потока на ulMSec милисекунд
    //    PARAMETRS:      ulMSec - количество милисекунд
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CThread::Sleep(unsigned long ulMSec)
    {
      osSleep(ulMSec);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       InternalRun
    //    DESCRIPTION:    внутренняя функция для запуска пользовательской
    //    PARAMETRS:      none
    //    RETURN:         none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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
