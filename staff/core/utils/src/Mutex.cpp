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
#if defined __APPLE__
#include <libkern/OSAtomic.h>
#else
#if !defined _XOPEN_SOURCE || (_XOPEN_SOURCE - 0) < 600
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#include <pthread.h>
#endif
#endif
#include "Mutex.h"

namespace staff
{

  struct Mutex::MutexImpl
  {
#ifdef WIN32
    HANDLE hMutex; //!< mutex handle
#else
#ifdef __APPLE__
    OSSpinLock hMutex; //!< mutex handle

    MutexImpl():
      hMutex(0)
    {}
#else
    pthread_spinlock_t hMutex; //!< mutex handle
#endif
#endif
  };


  Mutex::Mutex():
    m_pImpl(new MutexImpl)
  {
#ifdef WIN32
    m_pImpl->hMutex = CreateMutex(NULL, FALSE, NULL);
#elif !defined __APPLE__
    pthread_spin_init(&m_pImpl->hMutex, 0);
#endif
  }

  Mutex::~Mutex()
  {
#ifdef WIN32
    CloseHandle(m_pImpl->hMutex);
#elif !defined __APPLE__
    pthread_spin_destroy(&m_pImpl->hMutex);
#endif
    delete m_pImpl;
  }

  void Mutex::Lock()
  {
#ifdef WIN32
    WaitForSingleObject(m_pImpl->hMutex, INFINITE);
#else
#ifdef __APPLE__
    OSSpinLockLock(&m_pImpl->hMutex);
#else
    pthread_spin_lock(&m_pImpl->hMutex);
#endif
#endif
  }

  void Mutex::Unlock()
  {
#ifdef WIN32
    ReleaseMutex(m_pImpl->hMutex);
#else
#ifdef __APPLE__
    OSSpinLockUnlock(&m_pImpl->hMutex);
#else
    pthread_spin_unlock(&m_pImpl->hMutex);
#endif
#endif
  }

  bool Mutex::TryLock()
  {
#ifdef WIN32
    return WaitForSingleObject(m_pImpl->hMutex, 0) == WAIT_OBJECT_0;
#else
#ifdef __APPLE__
    return OSSpinLockTry(&m_pImpl->hMutex);
#else
    return !pthread_spin_trylock(&m_pImpl->hMutex);
#endif
#endif
  }

}
