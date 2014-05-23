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


#ifndef _STAFF_UTILS_MUTEX_H_
#define _STAFF_UTILS_MUTEX_H_

#include "staffutilsexport.h"

namespace staff
{

  //! mutex
  class STAFF_UTILS_EXPORT Mutex
  {
  public:
    //! mutex recursion mode
    enum RecursionMode
    {
      Recursive,
      NonRecursive
    };

    //! constructor.
    /*! \param eMode - mutex mode
     *  if eMode == Recursive, create recursive mutex,
     *  if eMode == NonRecursive - create non-recursive mutex
     */
    Mutex(RecursionMode eMode = NonRecursive);

    //! destructor
    ~Mutex();

    //! lock the mutex
    void Lock();

    //! unlock the mutex
    void Unlock();

    //! try lock the mutex
    /*! \return true, if mutex was successfully locked
      */
    bool TryLock();

    //! is mutex recursive?
    /*! \return Recursive - if mutex is recursive
      */
    RecursionMode GetRecursionMode() const;

  private:
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

    struct MutexImpl;
    MutexImpl* m_pImpl;
  };


  //! scoped lock
  class ScopedLock
  {
  public:
    //! constructor: locks the mutex
    /*! \param  rMutex - mutex
      */
    inline ScopedLock(Mutex& rMutex):
      m_rMutex(rMutex)
    {
      m_rMutex.Lock();
    }

    //! destructor: unlocks the mutex
    inline ~ScopedLock()
    {
      m_rMutex.Unlock();
    }

  private:
    ScopedLock(const ScopedLock&);
    ScopedLock& operator=(const ScopedLock&);

  private:
    Mutex& m_rMutex;
  };

}

#endif // _STAFF_UTILS_MUTEX_H_
