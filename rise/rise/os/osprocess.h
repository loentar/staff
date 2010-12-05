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

#ifndef _OSPROCESS_H_
#define _OSPROCESS_H_

#include "oscommon.h"
#include <rise/os/osdllexport.h>
#include <rise/common/types.h>
#include <signal.h>
#include <sys/types.h>
#include <rise/string/String.h>

#ifdef WIN32
#include <process.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#include <cstdarg>
#include <stdlib.h>
#include <errno.h>
#ifdef OS_Linux
#include <linux/limits.h>
#endif
#endif

namespace rise
{
#ifdef WIN32
  //!   process handle
  typedef HANDLE HProcess;
  #define PID_0 NULL
#else
  //!   process handle
  typedef pid_t HProcess;
  #define PID_0 0
#endif

  //! error
  const int g_nResultError = 1000;

  //! internal function for signal handling
  typedef void (*PSignalProc) (int nSignal);

  //!         send signal to process
  /*! \param  hProcess - process handle
      \param  nSignal - signal
      \return 0, if success
  */
  int osKill(HProcess hProcess, int nSignal);

  //!         set signal handler
  /*! \param  pSignalProc - signal handler proc
      \param  nSignal - signal
  */
  void osSetSignalHandler(PSignalProc pSignalProc, int nSignal);

  //!         execute process
  /*! arguments must be splitted with '\\t'
      example: 
      HProcess hProc = osExecute("ls\\t-l\\t/home");
      \param  sCmdArgs - command and arguments
      \return process handle
  */
  HProcess osExecute(const CString& sCmdArgs);

  //!         wait for process end and get return code
  /*! \param  hProcess - process handle
      \param  piStatus - pointer to status
      \param  bWait = true - wait for process
      \return -1: error, 0 - process is running(if bWait = false)
  */
  int osWaitPid(HProcess hProcess, int* piStatus, bool bWait = false);

  //!         get current process handle
  /*! \return current process handle
  */
  HProcess osGetCurrentProcessID();

  //!         get current process executable file name
  /*! \return current process executable file name
  */
  rise::CString osGetCurrentExecPath();

  //!         switch to daemon
  /*! \return 0, if daemonizing is sucessed
  */
  int osDaemon();

  //!         wait for signal
  void osPause();

  //!         terminate process
  /*! \param  hProcess - process handle
      \return 0 - ok,  1 - process is not found,  2 - access denied
  */
  int osTerminateProcess(HProcess hProcess);
};


#endif  // _OSPROCESS_H_
