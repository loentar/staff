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

#ifndef _CRITICALDATA_H_
#define _CRITICALDATA_H_

#include <rise/threading/CriticalSection.h>

namespace rise
{
  namespace threading
  {
    //!         data with critical section
    template<typename Type>
    class CCriticalData  
    {
    public:
      //!         initializing constructor
      /*! \param  rValue - object
      */
      CCriticalData(const Type& rValue);

      //!         get object operator
      /*! \return reference to object
      */
      operator Type&();
      
      //!         atomic preincrement
      /*! \return new object
      */
      Type& operator++();

      //!         atomic post-increment
      /*! \return old object
      */
      Type& operator++(int);

      //!         atomic predecrement
      /*! \return new object
      */
      Type& operator--();

      //!         atomic predecrement
      /*! \return old object
      */
      Type& operator--(int);

      //!         init operator
      /*! \param  rValue - object
          \return reference to current object
          */
      Type& operator=(const Type& rValue);

    private:
      Type m_tValue;          //!< value
      CCriticalSection m_cs;  //!< critical section
    };
  } // namespace threading
}  // namespace rise

#include "CriticalData.hpp"

#endif // _CRITICALDATA_H_
