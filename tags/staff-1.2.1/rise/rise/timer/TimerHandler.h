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

#ifndef _TIMERHANDLER_H_
#define _TIMERHANDLER_H_

#include <rise/common/types.h>
#include <list>

namespace rise
{
  //!        Timer handler class
  class RISE_EXPORT CTimerHandler  
  {
  public:
    typedef word TTimerID; //! timer id

    enum EREPEAT //! timer repeats
    {
      EINFINITE = 0  //!< infinite
    };

  public:
    //!         constructor
    CTimerHandler();

    //!         destructor
    virtual ~CTimerHandler();

    //!         add new timer
    /*! \param  dwMSec - timer period
        \param  dwRepeat - repeat count
        \return timer id
    */
    TTimerID AddTimer( dword dwMSec, dword dwRepeat = EINFINITE );

    //!        delete timer
    /*! \param  wID - timer id
    */
    void DelTimer( TTimerID wID );

    //!        delete all timers
    void DeleteAllTimers();

  protected:
    //!         timer handler
    /*! child class must override this function
        \param  wID - timer id
    */
    virtual void OnTimer( word wID ) = 0;

    friend class CTimeCatcher;
  };
} // namespace rise
  
#endif // #ifdef _TIMERHANDLER_H_
