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

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <rise/common/Log.h>
#include <rise/signal/SignalHandler.h>
#endif

#include <rise/common/ExceptionTemplate.h>
#include "Process.h"

namespace rise
{
  namespace process
  {

    CProcess::CProcess():
      m_nExitStatus(EESUNKNOWN), m_hProcess(PID_0)
    {
    }

    CProcess::~CProcess()
    {
    }

    bool CProcess::Exec( const CString& sCommand )
    {
      m_hProcess = osExecute(sCommand);

      return m_hProcess != PID_0;
    }

    bool CProcess::IsExists()
    {
      return m_hProcess == PID_0 ? false : osKill(m_hProcess, 0) == 0;
    }

    int CProcess::GetExitStatus( bool bWait /*= false*/ )
    {
      int nStatus = 0;
      int nResult = osWaitPid(m_hProcess, &nStatus, bWait);
      return nResult == -1 ? -2 : ((nResult == 0) ? -1 : nStatus);
    }

    bool CProcess::Attach( HProcess hProcess )
    {
      if (osKill(hProcess, 0) == 0)
        return false;
      
      m_hProcess = hProcess;
      m_nExitStatus = (osKill(m_hProcess, 0) == 0) ?  EESRUNNING : EESNOTRUNNING;
      // TODO: read proc info
      return true;
    }

    HProcess CProcess::GetProcessID()
    {
      return m_hProcess;
    }

    HProcess CProcess::GetCurrentProcessID()
    {
      return osGetCurrentProcessID();
    }

    rise::CString CProcess::GetCurrentExecPath()
    {
      return osGetCurrentExecPath();
    }

    void CProcess::Daemon()
    {
      osDaemon();
    }

    void CProcess::Pause()
    {
      osPause();
    }

    void CProcess::Terminate()
    {
      Terminate(m_hProcess);
    }

    void CProcess::Terminate( HProcess hProcess )
    {
      int nRet = osTerminateProcess(hProcess);
      RISE_ASSERTES(nRet != 1, CLogicNoItemException, "No such PID ");
      RISE_ASSERTES(nRet != 2, CLogicNoItemException, "Permission denied");
      RISE_ASSERTES(nRet == 0, CInternalInternalException, "Unknown error");
    }

  } // namespace process
} // namespace rise
