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

#ifndef _THREAD_H_
#define _THREAD_H_

#include <rise/os/osthread.h>
#include <rise/common/types.h>
#include <rise/threading/CriticalData.h>
#include "CriticalSection.h"

namespace rise
{
  //!        threading classes
  namespace threading
  {
    //!        Thread class
    class RISE_EXPORT CThread
    {
      public:
        //!        constructor
        CThread();

        //!        destructor
        virtual ~CThread();
    
        //!         create and start thread
        /*! \param  pParam - pointer to thread params
            \return true, if thread created
        */
        virtual bool Start(void* pParam = NULL, bool bDetached = true);
    
        //!         is thread running
        /*! \return true, if thread running
        */
        bool IsWorking() const;
    
        //!         get current thread handle
        /*! \return current thread handle
        */
        static HThread GetCurrentId();

        //!         get thread handle
        /*! \return thread handle
        */
        HThread GetId() const;
    
        //!        exit from current thread
        static void Exit();
    
        //!        terminate thread
        bool Cancel();
    
        //!         wait for thread end (only for joined threads)
        void JoinThread();
        
        //!         wait for thread end (for joined and detached threads, using waitpid)
        /*! \return true, if thread has finished
        */
        bool Wait();

        //!         wait for thread end (for joined and detached threads, using pthread_kill in cycle)
        /*! \param  ulTimeout - timeout in milliseconds, if = 0 wait infinite
            \return true, if thread has finished
        */
        bool Wait(unsigned long ulTimeout);

        //!        stop thread
        /*! set Stop flag and waits for thread end
            \param  ulTimeout - wait timeout, 0 - wait infinite
        */
        void Stop(ulong ulTimeout = 0ul);

        //!         is thread stopping
        /*! \return  true - thread is stopping
        */
        bool IsStopping();

        //!        sleeps current thread to ulMSec msec
        /*! \param  ulMSec - msec
        */
        static void Sleep(unsigned long ulMSec);

        //!         get thread's critical section
        /*! \return thread's critical section
        */
        CCriticalSection& GetCS();

      protected:
        //!        thread function
        /*!        child class must override this function
                   \param pParam - thread parameter
                   \sa Start
        */
        virtual void Run(void* pParam) = 0;

        //!        on stop handler
        virtual void OnStop();


      private:
        //!        internal function
        static void* InternalRun(void* pParam);

      private:
        HThread m_hThread;      //!< thread handle
        void*   m_pParam;       //!< thread param
        CCriticalSection m_cs;  //!< critical section
        CCriticalData<bool> m_bStopping; //!< thread is stopping flag

        CThread(const CThread&);
        CThread& operator=(const CThread&);
    };  

  } // namespace threading
}  // namespace rise
  
#endif
