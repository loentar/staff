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

#ifndef _OSTHREAD_H_
#define _OSTHREAD_H_

#include "oscommon.h"

#ifndef WIN32
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#endif

namespace rise
{
  //! pointer to thread proc
  typedef void *(*PThreadProc) (void *);
#ifdef WIN32
  //! thread handle
  typedef HANDLE HThread;
#else
  //! thread handle
  typedef pthread_t HThread;
#endif

  //!     creating a thread
  /*! \param  pProc - thread proc
      \param  hThread - resulting thread id
      \param  pParam - thread param proc
      \return thread handle
  */
  bool osCreateThread(PThreadProc pProc, HThread* hThread, void* pParam = NULL, bool bDetached = true);
  
  //!     get current thread handle
  /*! \return current thread handle
  */
  HThread osGetCurrentThread();

  //!     is hThread current thread
  bool osIsCurrentThread(HThread hThread);
  
  //!     ends and exit from current thread
  void osExitThread();
  
  //!     terminate thread
  /*! \param  hThread - thread handle
      \return true - thread was successfully terminated
  */
  bool osTerminateThread(HThread hThread);
  
  //!     wait for thread ends
  /*! \param  hThread - thread handle
  */
  void osJoinThread(HThread hThread);
  
  //!     is thread exists
  /*! \param  hThread - thread id
      \return true - thread exists
  */
  bool osIsThreadExists(HThread hThread);
  
  //!     wait for thread exit
  /*! \param  hThread - thread id
      \param  ulMSec - wait milliseconds
      \return true - thread was ended
  */
  bool osWaitForThreadExit(HThread hThread, unsigned long ulMSec);
  
  //!     wait for thread
  /*! \param  hThread - thread id
      \return true - thread was ended
  */
  bool osWaitForThread(HThread hThread);
  
  //    Locking
#ifdef WIN32
  //! critical section handle
  typedef CRITICAL_SECTION HCriticalSection;
  //! critical section handle pointer
  typedef LPCRITICAL_SECTION PCriticalSection;
#else
  //! critical section handle
  typedef pthread_mutex_t HCriticalSection;
  //! critical section handle pointer
  typedef pthread_mutex_t* PCriticalSection;
#endif
  
  //!         initializing critical section
  /*! \param  pCriticalSection - pointer to critical section
  */
  void osInitializeCriticalSection(PCriticalSection pCriticalSection);
  
  //!         delete critical section
  /*! \param  pCriticalSection - critical section
      */
  void osDeleteCriticalSection(PCriticalSection pCriticalSection);
  
  //!         enter into critical section
  /*! \param  pCriticalSection - critical section
      */
  void osEnterCriticalSection(PCriticalSection pCriticalSection);
  
  //!         enter into critical section without locking
  /*! \param  pCriticalSection - critical section
      \return true if success
      */
  bool osTryEnterCriticalSection(PCriticalSection pCriticalSection);
  
  //!         leave critical section
  /*! \param  pCriticalSection - critical section
      */
  void osLeaveCriticalSection(PCriticalSection pCriticalSection);
  
  //!         sleep for ulMSec millisecond
  /*! \param  ulMSec - millisecond to sleep
      */
  void osSleep(unsigned long ulMSec);
  
  //!         query millisecond counter
  /*! \return millisecond counter
  */
  long long osQueryPerfomance();

}; //namespace rise  
  
#endif  // _OSTHREAD_H_

