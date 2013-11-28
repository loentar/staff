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

#ifndef _STAFF_UTILS_EVENT_H_
#define _STAFF_UTILS_EVENT_H_

#include "staffutilsexport.h"

namespace staff
{

  static const unsigned long STAFF_EVENT_WAIT_INFINITE = static_cast<unsigned long>(-1);

  //! thread event
  class STAFF_UTILS_EXPORT Event
  {
  public:
    //! constructor
    Event();

    //! destructor
    virtual ~Event();

    //! wait for the event to be signaled
    /*! throws exception on error
        \param  ulTimeout - timeout in msec
        \return true, if event was signalled, false on timeout,
        */
    bool Wait(unsigned long ulTimeout = STAFF_EVENT_WAIT_INFINITE);

    //! signal event
    void Signal();

  private:
    Event(const Event&);
    Event& operator=(const Event&);

  private:
    struct EventImpl;
    EventImpl* m_pImpl;
  };

}

#endif // _STAFF_UTILS_EVENT_H_

