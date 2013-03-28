/*
 *  Copyright 2012 Utkin Dmitry
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

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#endif
#include <string>
#include "Exception.h"
#include "Error.h"
#include "Log.h"
#include "Mutex.h"
#include "Thread.h"

namespace staff
{

#ifdef WIN32
#define HThread HANDLE
#else
#define HThread pthread_t
#endif

  struct Thread::ThreadImpl
  {
    HThread hThread;
    Mutex tMutex;
    bool bStopping;

    ThreadImpl():
      hThread(0), bStopping(false)
    {
    }

#ifdef WIN32
    static DWORD WINAPI InternalRun(LPVOID pParam)
#else
    static void* InternalRun(void* pParam)
#endif
    {
      Thread* pThread = reinterpret_cast<Thread*>(pParam);

      if (!pThread)
      {
        LogError() << "invalid param: pParam == NULL!";
      }
      else
      {
        ThreadImpl* pThis = pThread->m_pImpl;
        pThis->tMutex.Lock(); // just wait here while Start function finishing
        pThis->tMutex.Unlock();
        try
        {
          pThread->Run();
        }
        STAFF_CATCH_ALL;

        pThis->tMutex.Lock();
        pThis->hThread = 0;
        pThis->bStopping = false;
        pThis->tMutex.Unlock();
      }

      return NULL;
    }

    // should be called inside mutex
    inline bool IsRunning() const
    {
      bool bResult = hThread &&
#ifdef WIN32
          WaitForSingleObject(hThread, 1) != WAIT_FAILED;
#else
          !pthread_kill(hThread, 0);
#endif
      return bResult;
    }
  };

  Thread::Thread():
    m_pImpl(new ThreadImpl)
  {
  }

  Thread::~Thread()
  {
    try
    {
      if (IsRunning())
      {
        Stop(5000);
      }
    }
    STAFF_CATCH_ALL;

    if (IsRunning())
    {
      try
      {
        Terminate();
      }
      STAFF_CATCH_ALL;
    }

    delete m_pImpl;
  }

  bool Thread::Start(bool bDetached /* = true*/)
  {
    ScopedLock tLock(m_pImpl->tMutex);
    STAFF_ASSERT(!m_pImpl->IsRunning(), "Thread already started");
    m_pImpl->bStopping = false;
#ifdef WIN32
    DWORD dwTreadId;
    m_pImpl->hThread = CreateThread(NULL, 0, &ThreadImpl::InternalRun, this, 0, &dwTreadId);
    return m_pImpl->hThread != 0;
#else
    pthread_attr_t tAttr;
    pthread_attr_init(&tAttr);
    if (bDetached)
    {
      pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
    }
    return !pthread_create(&m_pImpl->hThread, &tAttr, ThreadImpl::InternalRun, this);
#endif
  }

  bool Thread::IsRunning() const
  {
    m_pImpl->tMutex.Lock();
    bool bResult = m_pImpl->IsRunning();
    m_pImpl->tMutex.Unlock();
    return bResult;
  }

  unsigned long Thread::GetCurrentId()
  {
#ifdef WIN32
    return reinterpret_cast<unsigned long>(GetCurrentThread());
#else
#if defined __linux__ || defined sun
    return static_cast<unsigned long>(pthread_self());
#else
    return reinterpret_cast<unsigned long>(pthread_self());
#endif
#endif
  }

  unsigned long Thread::GetId() const
  {
#if defined __linux__ || defined sun
    return static_cast<unsigned long>(m_pImpl->hThread);
#else
    return reinterpret_cast<unsigned long>(m_pImpl->hThread);
#endif
  }

  void Thread::Exit()
  {
#ifdef WIN32
    ExitThread(0);
#else
    pthread_exit(NULL);
#endif
  }

  bool Thread::Terminate()
  {
    ScopedLock tLock(m_pImpl->tMutex);
    if (m_pImpl->IsRunning())
    {
      LogWarning() << "Terminating thread #" << m_pImpl->hThread;

#ifdef WIN32
      STAFF_ASSERT(TerminateThread(m_pImpl->hThread, 0) == TRUE, "Failed to terminate thread: "
                   + Error::GetLastErrorStr());
#elif !defined __ANDROID_API__
      STAFF_ASSERT(!pthread_cancel(m_pImpl->hThread), "Failed to terminate thread: "
                   + Error::GetLastErrorStr());
#endif

      m_pImpl->hThread = 0;
      return true;
    }
    return false;
  }

  void Thread::Join()
  {
    HThread hThread = 0;
    {
      ScopedLock tLock(m_pImpl->tMutex);
      if (!m_pImpl->IsRunning())
      {
        return;
      }
      hThread = m_pImpl->hThread;
    }

#ifdef WIN32
    WaitForSingleObject(hThread, INFINITE);
#else
    pthread_join(hThread, NULL);
#endif
  }

  bool Thread::Wait(unsigned long ulTimeout)
  {
    m_pImpl->tMutex.Lock();
    HThread hThread = m_pImpl->hThread; // this handle is always valid
    m_pImpl->tMutex.Unlock();

    if (!hThread)
    {
      return true;
    }

#ifdef WIN32
    if (!ulTimeout)
    {
      ulTimeout = INFINITE;
    }
    return WaitForSingleObject(hThread, ulTimeout) == WAIT_OBJECT_0;
#else
    // wait infinite
    if (!ulTimeout)
    {
      while (m_pImpl->hThread && !pthread_kill(hThread, 0)) // lock-free checking
      {
        Sleep(10);
      }
      return true;
    }

    // wait limited time
    while (ulTimeout && m_pImpl->hThread && !pthread_kill(hThread, 0))
    {
      if (ulTimeout > 10)
      {
        Sleep(10);
        ulTimeout -= 10;
      }
      else
      {
        ulTimeout = 0;
        Sleep(ulTimeout);
      }
    }

    return !(m_pImpl->hThread && !pthread_kill(hThread, 0));
#endif
  }

  bool Thread::Stop(unsigned long ulTimeout /*= 0*/)
  {
    if (!IsRunning())
    {
      return false;
    }

    m_pImpl->bStopping = true;
    OnStop();

    return Wait(ulTimeout);
  }

  bool Thread::IsStopping()
  {
    return m_pImpl->bStopping;
  }

  void Thread::Sleep(unsigned long ulTimeout)
  {
#ifdef WIN32
    ::Sleep(ulTimeout);
#else
    usleep(ulTimeout * 1000);
#endif
  }

  void Thread::OnStop()
  {
  }

}
