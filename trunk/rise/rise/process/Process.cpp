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

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CProcess
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CProcess::CProcess():
      m_nExitStatus(EESUNKNOWN), m_hProcess(PID_0)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CProcess
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CProcess::~CProcess()
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Exec(...)
    //    DESCRIPTION:    запуск приложения
    //    PARAMETRS:      sCommand - приложение
    //    RETURN:         none
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CProcess::Exec( const CString& sCommand )
    {
      m_hProcess = osExecute(sCommand);

      return m_hProcess != PID_0;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       IsExists()
    //    DESCRIPTION:    получить состояние работы процесса
    //    PARAMETRS:      none
    //    RETURN:         ESTATE
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CProcess::IsExists()
    {
      return m_hProcess == PID_0 ? false : osKill(m_hProcess, 0) == 0;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetExitCodeProcess(...)
    //    DESCRIPTION:    получить код возврата процесса
    //    PARAMETRS:      none
    //    RETURN:         код возврата
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    int CProcess::GetExitStatus( bool bWait /*= false*/ )
    {
      int nStatus = 0;
      int nResult = osWaitPid(m_hProcess, &nStatus, bWait);
      return nResult == -1 ? -2 : ((nResult == 0) ? -1 : nStatus);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Attach
    //    DESCRIPTION:    присоединиться к уже запущенному процессу
    //    PARAMETRS:      hProcess - идентификатор процесса
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CProcess::Attach( HProcess hProcess )
    {
      if (osKill(hProcess, 0) == 0)
        return false;
      
      m_hProcess = hProcess;
      m_nExitStatus = (osKill(m_hProcess, 0) == 0) ?  EESRUNNING : EESNOTRUNNING;
      // TODO: считать информацию о процессе
      return true;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetProcessID
    //    DESCRIPTION:    получить идентификатор процесса связанного с обьектом класса
    //    PARAMETRS:      none
    //    RETURN:         идентификатор процесса связанного с обьектом класса
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    HProcess CProcess::GetProcessID()
    {
      return m_hProcess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetCurrentProcessID
    //    DESCRIPTION:    получить идентификатор текущего процесса
    //    PARAMETRS:      none
    //    RETURN:         идентификатор процесса из которого была вызвана эта функция
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    HProcess CProcess::GetCurrentProcessID()
    {
      return osGetCurrentProcessID();
    }

    rise::CString CProcess::GetCurrentExecPath()
    {
      return osGetCurrentExecPath();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Daemon
    //    DESCRIPTION:    переключить процесс в фоновый режим
    //    PARAMETRS:      none
    //    RETURN:         0, если успешно
    //    EXCEPTIONS:     none
    //    COMMENT:        нет реализации в WIN32. всегда игнорируется
    //////////////////////////////////////////////////////////////////////////////
    void CProcess::Daemon()
    {
      osDaemon();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Pause
    //    DESCRIPTION:    приостановить выполнение процесса до прихода сообщения/сигнала
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CProcess::Pause()
    {
      osPause();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Terminate
    //    DESCRIPTION:    завершить процесс, связанный с обьектом класса
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicNoItemException - процесс не найден
    //                    CLogicPermissionException - нет прав
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CProcess::Terminate()
    {
      Terminate(m_hProcess);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Terminate
    //    DESCRIPTION:    завершить процесс
    //    PARAMETRS:      hProcess - идентификатор процесса
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicNoItemException - процесс не найден
    //                    CLogicPermissionException - нет прав
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CProcess::Terminate( HProcess hProcess )
    {
      int nRet = osTerminateProcess(hProcess);
      RISE_ASSERTES(nRet != 1, CLogicNoItemException, "No such PID ");
      RISE_ASSERTES(nRet != 2, CLogicNoItemException, "Permission denied");
      RISE_ASSERTES(nRet == 0, CInternalInternalException, "Unknown error");
    }

  } // namespace process
} // namespace rise
