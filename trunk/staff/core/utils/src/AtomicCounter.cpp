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
#endif
#include "AtomicCounter.h"

namespace staff
{

  AtomicCounter::AtomicCounter(long lValue /*= 0*/):
    m_lValue(lValue)
  {
  }

  AtomicCounter::~AtomicCounter()
  {
  }


  long AtomicCounter::operator++()
  {
#ifdef WIN32
    InterlockedIncrement(&m_lValue);
#elif defined STAFF_USE_GNUC_BUILTINS
    __sync_fetch_and_add(&m_lValue, 1);
#else
    m_tMutex.Lock();
    ++m_lValue;
    m_tMutex.Unlock();
#endif
    return m_lValue;
  }

  long AtomicCounter::operator--()
  {
#ifdef WIN32
    InterlockedDecrement(&m_lValue);
#elif defined STAFF_USE_GNUC_BUILTINS
    __sync_fetch_and_sub(&m_lValue, 1);
#else
    m_tMutex.Lock();
    --m_lValue;
    m_tMutex.Unlock();
#endif
    return m_lValue;
  }

  long AtomicCounter::operator++(int)
  {
    long lPrev = m_lValue;
#ifdef WIN32
    InterlockedIncrement(&m_lValue);
#elif defined STAFF_USE_GNUC_BUILTINS
    __sync_fetch_and_add(&m_lValue, 1);
#else
    m_tMutex.Lock();
    ++m_lValue;
    m_tMutex.Unlock();
#endif
    return lPrev;
  }

  long AtomicCounter::operator--(int)
  {
    long lPrev = m_lValue;
#ifdef WIN32
    InterlockedDecrement(&m_lValue);
#elif defined STAFF_USE_GNUC_BUILTINS
    __sync_fetch_and_sub(&m_lValue, 1);
#else
    m_tMutex.Lock();
    --m_lValue;
    m_tMutex.Unlock();
#endif
    return lPrev;
  }

  AtomicCounter::operator long() const
  {
    return m_lValue;
  }
}
