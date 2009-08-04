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

#ifndef _ATOMICCOUNT_H_
#define _ATOMICCOUNT_H_

#include <rise/os/osdllexport.h>
#include <rise/threading/CriticalSection.h>

namespace rise
{
  namespace threading
  {

    //!  atomic counter(thread-safe)
    class RISE_EXPORT CAtomicCount  
    {
    public:
      //!        constructor
      CAtomicCount() throw();

      //!        constructor with initialization
      CAtomicCount(long lValue) throw();

      //!         atomic increment
      /*! \return new value
      */
      long Increment() throw();

      //!         atomic decrement
      /*! \return new value
      */
      long Decrement() throw();

      //!         get value
      /*! \return current value
      */
      long GetValue() const throw();

      //!         atomic preincrement
      /*! \return new value
      */
      long operator++() throw();

      //!         atomic predecrement
      /*! \return new value
      */
      long operator--() throw();

      //!         atomic post-increment
      /*! \return old value
      */
      long operator++(int) throw();
    
      //!         atomic predecrement
      /*! \return old value
      */
      long operator--(int) throw();
    
      //!         get current value
      /*! \return current value
      */
      operator long() const throw();

    private:
      CAtomicCount(CAtomicCount&);
      CAtomicCount operator=(const CAtomicCount&);
    
      volatile long m_lValue;  //! current value
#ifndef WIN32
      CCriticalSection m_csLock;
#endif
    };
  }
}
  
#endif // _ATOMICCOUNT_H_
