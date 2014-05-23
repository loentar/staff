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
#include <windows.h>
#else
#if (!defined SUNOS_MAJOR) || (SUNOS_MAJOR < 11)
#if !defined _XOPEN_SOURCE || (_XOPEN_SOURCE - 0) < 600
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#define _STDC_C99
#define _POSIX_C_SOURCE 200112L
#endif
#include <pthread.h>
#undef _STDC_C99
#endif
#include "Mutex.h"

namespace staff
{

  struct Mutex::MutexImpl
  {
#ifdef WIN32
    union
    {
      CRITICAL_SECTION hCriticalSection;
      HANDLE hMutex;
    }; //!< mutex handle
#else
    pthread_mutex_t hMutex; //!< mutex handle
#endif
    RecursionMode eMode;

    MutexImpl(RecursionMode eMode_):
      eMode(eMode_)
    {
    }
  };


  Mutex::Mutex(RecursionMode eMode):
    m_pImpl(new MutexImpl(eMode))
  {
#ifdef WIN32
    if (eMode == Recursive)
    {
      m_pImpl->hMutex = CreateMutex(NULL, FALSE, NULL);
    }
    else
    {
      InitializeCriticalSection(&m_pImpl->hCriticalSection);
    }
#else
    pthread_mutexattr_t tAttr;
    pthread_mutexattr_init(&tAttr);
    if (eMode == Recursive)
    {
      pthread_mutexattr_settype(&tAttr, PTHREAD_MUTEX_RECURSIVE_NP);
    }
    pthread_mutex_init(&m_pImpl->hMutex, &tAttr);
#endif
  }

  Mutex::~Mutex()
  {
#ifdef WIN32
    if (m_pImpl->eMode == Recursive)
    {
      CloseHandle(m_pImpl->hMutex);
    }
    else
    {
      DeleteCriticalSection(&m_pImpl->hCriticalSection);
    }
#else
    pthread_mutex_destroy(&m_pImpl->hMutex);
#endif
    delete m_pImpl;
  }

  void Mutex::Lock()
  {
#ifdef WIN32
    if (m_pImpl->eMode == Recursive)
    {
      WaitForSingleObject(m_pImpl->hMutex, INFINITE);
    }
    else
    {
      EnterCriticalSection(&m_pImpl->hCriticalSection);
    }
#else
    pthread_mutex_lock(&m_pImpl->hMutex);
#endif
  }

  void Mutex::Unlock()
  {
#ifdef WIN32
    if (m_pImpl->eMode == Recursive)
    {
      ReleaseMutex(m_pImpl->hMutex);
    }
    else
    {
      LeaveCriticalSection(&m_pImpl->hCriticalSection);
    }
#else
    pthread_mutex_unlock(&m_pImpl->hMutex);
#endif
  }

  bool Mutex::TryLock()
  {
#ifdef WIN32
    if (m_pImpl->eMode == Recursive)
    {
      return WaitForSingleObject(m_pImpl->hMutex, 0) == WAIT_OBJECT_0;
    }
    else
    {
#if(_WIN32_WINNT >= 0x0400)
      return TryEnterCriticalSection(&m_pImpl->hCriticalSection) != 0;
#else
#pragma message ("To include support of TryLock you must set preproccessor definition _WIN32_WINNT=0x0400 (or above)")
      return false;
#endif
    }
#else
    return !pthread_mutex_trylock(&m_pImpl->hMutex);
#endif
  }

  Mutex::RecursionMode Mutex::GetRecursionMode() const
  {
    return m_pImpl->eMode;
  }

}
