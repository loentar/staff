#include <rise/os/osthread.h>
#include <rise/threading/CriticalSection.h>
#include "AtomicCount.h"

namespace rise
{
  namespace threading
  {
    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CAtomicCount
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CAtomicCount::CAtomicCount() throw():
      m_lValue(0)
    {
    }

  
    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CAtomicCount
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CAtomicCount::CAtomicCount(long lValue) throw():
      m_lValue(lValue)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       Increment
    //    DESCRIPTION:    атомарный инкремент
    //    PARAMETRS:      none
    //    RETURN:         новое значение
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       Decrement
    //    DESCRIPTION:    атомарный декремент
    //    PARAMETRS:      none
    //    RETURN:         новое значение
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetValue
    //    DESCRIPTION:    получение значения счетчика
    //    PARAMETRS:      none
    //    RETURN:         текущее значение счетчика
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    long CAtomicCount::GetValue() const throw()
    {
      return m_lValue; 
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       ++
    //    DESCRIPTION:    атомарный преинкремент
    //    PARAMETRS:      none
    //    RETURN:         новое значение
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       --
    //    DESCRIPTION:    атомарный предекремент
    //    PARAMETRS:      none
    //    RETURN:         новое значение
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       ++
    //    DESCRIPTION:    атомарный постинкремент
    //    PARAMETRS:      none
    //    RETURN:         новое значение
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       --
    //    DESCRIPTION:    атомарный постдекремент
    //    PARAMETRS:      none
    //    RETURN:         новое значение
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       long
    //    DESCRIPTION:    получение значения счетчика
    //    PARAMETRS:      none
    //    RETURN:         текущее значение счетчика
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CAtomicCount::operator long() const throw()
    {
      return m_lValue; 
    }
  }
}
