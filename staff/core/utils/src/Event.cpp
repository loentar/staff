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

#ifndef WIN32
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#else
#include <windows.h>
#endif
#include "Error.h"
#include "Exception.h"
#include "Event.h"

namespace staff
{

  struct Event::EventImpl
  {
#ifdef WIN32
    HANDLE hEvent;
#else
    sem_t hEvent;
#endif
  };

  Event::Event():
    m_pImpl(new EventImpl)
  {
#ifdef WIN32
    m_pImpl->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#else
    sem_init(&m_pImpl->hEvent, 0, 0);
#endif
  }

  Event::~Event()
  {
#ifdef WIN32
    CloseHandle(m_pImpl->hEvent);
#else
    sem_destroy(&m_pImpl->hEvent);
#endif
    delete m_pImpl;
  }

  bool Event::Wait(unsigned long ulTimeout /*= STAFF_EVENT_WAIT_INFINITE*/)
  {
#ifdef WIN32
    DWORD dwWaitResult = WaitForSingleObject(m_pImpl->hEvent, 
                            ulTimeout != STAFF_EVENT_WAIT_INFINITE ? ulTimeout : INFINITE);

    switch (dwWaitResult)
    {
    case WAIT_OBJECT_0:
      return true;

    case WAIT_TIMEOUT:
      return false;
    }
#else
    if (ulTimeout != STAFF_EVENT_WAIT_INFINITE)
    {
      int nResult = 0;
      struct timeval tvNow;
      struct timespec tvAbsTimeout;

      gettimeofday(&tvNow, NULL);

      tvAbsTimeout.tv_sec = tvNow.tv_sec + ulTimeout / 1000;
      tvAbsTimeout.tv_nsec = ((ulTimeout % 1000) * 1000 + tvNow.tv_usec) * 1000;

      // check for overflow
      if (tvAbsTimeout.tv_nsec > 1000000000)
      {
        ++tvAbsTimeout.tv_sec;
        tvAbsTimeout.tv_nsec -= 1000000000;
      }

#if defined __APPLE__ || defined __FreeBSD__
      while ((nResult = sem_trywait(&m_pImpl->hEvent) == -1) &&
             (errno == EINTR || errno == EAGAIN) && (ulTimeout != 0))
      {
        usleep(1000 * ulTimeout);
        --ulTimeout;
      }

      if (!nResult)
      {
        return true;
      }
      else
      if (!ulTimeout)
      {
        return false;
      }
#else
      // Wait for the event be signaled
      while ((nResult = sem_timedwait(&m_pImpl->hEvent, &tvAbsTimeout)) == -1 && errno == EINTR);

#if defined PTHREAD_VERSION_MAJOR && defined PTHREAD_VERSION_MINOR && \
    PTHREAD_VERSION_MAJOR == 0 && PTHREAD_VERSION_MINOR < 10

      // pthread 0.9 returns ETIMEDOUT instead of -1 on timeout
      if (nResult == ETIMEDOUT && errno == EINTR)
      {
        return false;
      }
#else

      if (!nResult)
      {
        return true;
      }
      else
      if (errno == ETIMEDOUT)
      {
        return false;
      }

#endif // ptherad version
#endif // __APPLE__ || __FreeBSD__
    }
    else
    { // wait infinite
      int nResult = 0;
      while ((nResult = sem_wait(&m_pImpl->hEvent)) == -1 && errno == EINTR);
      if (!nResult)
      {
        return true;
      }
    }
#endif // not win32

    STAFF_THROW_ASSERT("Error while waiting event: " + Error::GetLastErrorStr());
  }

  void Event::Signal()
  {
#ifdef WIN32
    SetEvent(m_pImpl->hEvent);
#else
    sem_post(&m_pImpl->hEvent);
#endif
  }

}
