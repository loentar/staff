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

#ifndef _STAFF_UTILS_THREAD_H_
#define _STAFF_UTILS_THREAD_H_

#include "staffutilsexport.h"

namespace staff
{

  //! thread class
  class STAFF_UTILS_EXPORT Thread
  {
  public:
    //! constructor
    Thread();

    //! destructor
    virtual ~Thread();

    //! create and start thread
    /*! \return true, if thread created
      */
    virtual bool Start(bool bDetached = false);

    //! is thread running
    /*! \return true, if thread running
      */
    bool IsRunning() const;

    //! get current thread handle
    /*! \return current thread handle
      */
    static unsigned long GetCurrentId();

    //! get thread handle
    /*! \return thread handle
      */
    unsigned long GetId() const;

    //! finish current thread
    static void Exit();

    //! force thread to terminate
    /*! dangerous function, use Stop instead if possible.
        \sa Stop
        \return true, if thread was successfully terminated
      */
    bool Terminate();

    //! join thread
    void Join();

    //! wait for thread end (for joined and detached threads, using pthread_kill in cycle)
    /*! \param  ulTimeout - timeout in milliseconds, if = 0 wait infinite
        \return true, if thread has finished, false of timeout occured
      */
    bool Wait(unsigned long ulTimeout = 0);

    //! stop thread
    /*! set Stop flag and wait while thread finishing
        \param  ulTimeout - wait timeout, 0 - wait infinite
      */
    bool Stop(unsigned long ulTimeout = 0);

    //! is thread stopping
    /*! \return  true - thread is stopping
      */
    bool IsStopping();

    //! sleeps current thread up to ulMSec msec
    /*! \param  ulMSec - msec
      */
    static void Sleep(unsigned long ulMSec);

  protected:
    //! thread function
    /*! child class must override this function
        \sa Start
      */
    virtual void Run() = 0;

    //! on stop handler
    virtual void OnStop();

  private:
    Thread(const Thread&);
    Thread& operator=(const Thread&);

  private:
    struct ThreadImpl;
    ThreadImpl* m_pImpl;
  };

}

#endif // _STAFF_UTILS_THREAD_H_
