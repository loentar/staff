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

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <string>
#include <rise/common/types.h>
#include <rise/os/osprocess.h>
#include <rise/os/osdllexport.h>
#include <rise/threading/SharedPtr.h>

namespace rise
{
  //!        process classes
  namespace process
  {
    //!        Process
    class RISE_EXPORT CProcess
    {
    public:
      enum ESTATUS     //! return code
      {
        EESERROREXEC = g_nResultError,  //!<  error while executing
        EESRUNNING,                     //!<  process is running
        EESNOTRUNNING,                  //!<  process is not running
        EESUNKNOWN                      //!<  unknown state
      };

    public:
      //!        constructor
      CProcess();

      //!        destructor
      virtual ~CProcess();

      //!        attach to existing process
      /*! \param  hProcess - process id
          \return true, if attaching successes
      */
      bool Attach(HProcess hProcess);

      //!        execute process
      /*! arguments are splitted with '\\t'
          example: 
            CProcess tLsProc;
            tLsProc.Exec("/bin/ls\t-l\t/home");
          \param  sCommand - command to exec
          \return true, if process started
      */
      bool Exec(const CString& sCommand);

      //!         is process running
      /*! \return true, if process running
      */
      bool IsExists();

      //!         get process exit status
      /*! \param  bWait - wait for process ends
          \return process exit status
      */
      int GetExitStatus(bool bWait = false);

      //!         get process handle
      /*! \return process handle
      */
      HProcess GetProcessID();

      //!         get current process handle
      /*! \return current process handle
      */
      static HProcess GetCurrentProcessID();

      //!         get current process exec path
      /*! \return current process exec path
      */
      static rise::CString GetCurrentExecPath();

      //!         daemonize process(run process in background)
      /*!         WIN32: no implementation
          \return 0, if daemonizing was successed
      */
      static void Daemon();

      //!         wait for some signal
      static void Pause();

      //!         terminate process
      void Terminate();

      //!         terminate given process
      /*! \param  hProcess - process handle
      */
      static void Terminate(HProcess hProcess);

    private:
      int              m_nExitStatus; //!< exit status
      HProcess         m_hProcess;    //!< process description

    };
  } // namespace process
}  // namespace rise

#endif // _EXEC_H_
