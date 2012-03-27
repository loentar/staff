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

#include <rise/os/osthread.h>
#include <rise/threading/CriticalSection.h>
#include "AtomicCount.h"

namespace rise
{
  namespace threading
  {
    CAtomicCount::CAtomicCount() throw():
      m_lValue(0)
    {
    }

    CAtomicCount::CAtomicCount(long lValue) throw():
      m_lValue(lValue)
    {
    }

    long CAtomicCount::Increment() throw()
    {
#ifdef WIN32
      InterlockedIncrement(const_cast<long*>(&m_lValue));
#else
      CScopedCriticalSection csLock(m_csLock);
      ++m_lValue;
#endif
      return m_lValue; 
    }

    long CAtomicCount::Decrement() throw()
    {
#ifdef WIN32
      InterlockedDecrement(const_cast<long*>(&m_lValue));
#else
      CScopedCriticalSection csLock(m_csLock);
      --m_lValue;
#endif
      return m_lValue; 
    }

    long CAtomicCount::GetValue() const throw()
    {
      return m_lValue; 
    }

    long CAtomicCount::operator++() throw()
    {
#ifdef WIN32
      InterlockedIncrement(const_cast<long*>(&m_lValue));
#else
      CScopedCriticalSection csLock(m_csLock);
      ++m_lValue;
#endif
      return m_lValue; 
    }

    long CAtomicCount::operator--() throw()
    {
#ifdef WIN32
      InterlockedDecrement(const_cast<long*>(&m_lValue));
#else
      CScopedCriticalSection csLock(m_csLock);
      --m_lValue;
#endif
      return m_lValue;
    }

    long CAtomicCount::operator++(int) throw()
    {
      long nPrev = m_lValue;
#ifdef WIN32
      InterlockedIncrement(const_cast<long*>(&m_lValue));
#else
      CScopedCriticalSection csLock(m_csLock);
      ++m_lValue;
#endif
      return nPrev;
    }

    long CAtomicCount::operator--(int) throw()
    {
      long nPrev = m_lValue;
#ifdef WIN32
      InterlockedDecrement(const_cast<long*>(&m_lValue));
#else
      CScopedCriticalSection csLock(m_csLock);
      --m_lValue;
#endif
      return nPrev;
    }

    CAtomicCount::operator long() const throw()
    {
      return m_lValue; 
    }
  }
}
