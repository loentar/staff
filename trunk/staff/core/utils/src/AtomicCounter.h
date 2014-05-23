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

#ifndef _STAFF_UTILS_ATOMICCOUNTER_H_
#define _STAFF_UTILS_ATOMICCOUNTER_H_

#if defined __GNUC__ && __GNUC__ >= 4 && __GNUC_MINOR__ >= 4
#define STAFF_USE_GNUC_BUILTINS
#endif

#ifndef WIN32
#ifndef STAFF_USE_GNUC_BUILTINS
#include "Mutex.h"
#endif
#endif
#include "staffutilsexport.h"

namespace staff
{

  //! atomic counter
  class STAFF_UTILS_EXPORT AtomicCounter
  {
  public:
    //! constructor
    /*! \param  lValue - value
      */
    AtomicCounter(long lValue = 0);

    //! destructor
    ~AtomicCounter();

    //! atomic preincrement
    /*! \return new value
    */
    long operator++();

    //! atomic predecrement
    /*! \return new value
    */
    long operator--();

    //! atomic post-increment
    /*! \return old value
    */
    long operator++(int);

    //! atomic predecrement
    /*! \return old value
    */
    long operator--(int);

    //! get current value
    /*! \return current value
    */
    operator long() const;

  private:
    AtomicCounter(const AtomicCounter&);
    AtomicCounter& operator=(const AtomicCounter&);

  private:
    long m_lValue;
#ifndef WIN32
#ifndef STAFF_USE_GNUC_BUILTINS
    Mutex m_tMutex;
#endif
#endif
  };

}

#endif // _STAFF_UTILS_ATOMICCOUNTER_H_
