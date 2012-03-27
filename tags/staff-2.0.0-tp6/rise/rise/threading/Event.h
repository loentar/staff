/*
 *  Copyright 2011 Utkin Dmitry
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

#ifndef _RISE_EVENT_H_
#define _RISE_EVENT_H_

#include <rise/os/osthread.h>
#include <rise/os/osdllexport.h>

namespace rise
{
namespace threading
{
  //!        Thread class
  class RISE_EXPORT CEvent
  {
    public:
      //!        constructor
      CEvent();

      //!        destructor
      virtual ~CEvent();

      //!        wait for the event be signaled
      /*! \param  ulTimeout - timeout in msec
          \return true, is event was signalled, false if timeout,
           throws an exception if error occurs
        */
      bool Wait(unsigned long ulTimeout = RISE_WAIT_INFINITE);

      //!         signal event
      void Signal();

      //!         operator HEvent
      operator HEvent();

    private:
      HEvent m_hEvent;      //!< event handle

      CEvent(const CEvent&);
      CEvent& operator=(const CEvent&);
  };

} // namespace threading
}  // namespace rise
  
#endif
