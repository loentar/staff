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

#ifndef _CRITICALSECTION_H_
#define _CRITICALSECTION_H_

#include <rise/os/osdllexport.h>
#include <rise/os/osthread.h>

namespace rise
{
  namespace threading
  {
    //!        Critical section
    class RISE_EXPORT CCriticalSection  
    {
    public:
      //!        constructor
      CCriticalSection();

      //!        destructor
      virtual ~CCriticalSection();

      //!        enter critical section
      void Enter();

      //!        try enter critical section without locking
      /*! \return true, enter successes
      */
      bool TryEnter();

      //!        leave critical section
      void Leave();

    private:
      HCriticalSection m_hCriticalSection; //!< critical section handle
    
      CCriticalSection(const CCriticalSection&);
      CCriticalSection operator=(const CCriticalSection&);
    };  // CCriticalSection




    //!  Scope critical section
    class RISE_EXPORT CScopedCriticalSection
    {
      public:
        //!        constructor
        /*! \param cs - ref to existing critical section
        */
        CScopedCriticalSection(CCriticalSection& cs);

        //!        destructor
        ~CScopedCriticalSection();

      private:
        CCriticalSection& m_cs;  //! reference to critical section

       CScopedCriticalSection(const CScopedCriticalSection&);
       CScopedCriticalSection& operator=(const CScopedCriticalSection&);
    };
  } // namespace threading
} // namespace rise

#endif // _CRITICALSECTION_H_
