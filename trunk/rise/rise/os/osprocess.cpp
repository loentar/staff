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

#include <rise/os/oscommon.h>
#include <stdio.h>
#include "osprocess.h"

namespace rise
{

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osKill(...)
  //    DESCRIPTION:    послать сигнал процессу
  //    PARAMETRS:      hProcess - номер процесса
  //                    nSignal - номер сигнала
  //    RETURN:         0, если успешно
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int osKill(HProcess hProcess, int nSignal)
  {
  #ifdef WIN32
    switch(nSignal)
    {
      case 0:
      {
        DWORD dwRetCode = 0;
        GetExitCodeProcess(hProcess, &dwRetCode);
        return dwRetCode == STILL_ACTIVE ? 0 : 1;
      } break;
      
      default:
        return TerminateProcess(hProcess, 1);
    }
  #else
    return kill(hProcess, nSignal);
  #endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osSetSignalHandler
  //    DESCRIPTION:    установка обработчика сигналов
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void osSetSignalHandler(PSignalProc pSignalProc, int nSignal)
  {
  #ifdef WIN32
    #pragma message ( "WIN32 osSetSignalHandler not realized!" ) 
    pSignalProc;
    nSignal;
  #else 
    static struct sigaction stAction; 
    static bool bIsInitialized = false;
    if (!bIsInitialized)
    {
      sigfillset(&(stAction.sa_mask));
      bIsInitialized = true;
    }
    stAction.sa_handler = pSignalProc;
    sigaction(nSignal, &stAction, 0);
  #endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osExecute(...)
  //    DESCRIPTION:    выполнение приложения
  //    PARAMETRS:      szCommand - команда с параметрами
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        аргументы разделяются символом табуляции '\t'
  //////////////////////////////////////////////////////////////////////////////
  HProcess osExecute( const CString& sCmdArgs )
  {
    HProcess hProcess = PID_0;

    CStringA asCmdArgs = sCmdArgs;
//    sCmdArgs.ToA(asCmdArgs);

    TCharA* szCmd = const_cast<TCharA*>(asCmdArgs.c_str());
    TCharA* szArgs[256];
    unsigned nParam = 0;
    unsigned i = 0;

    szArgs[0] = szCmd;
    for (; szCmd[i] != '\0'; ++i)
      if(szCmd[i] == '\t')
      {
        szCmd[i] = '\0';
        szArgs[++nParam] = &szCmd[i + 1];
        if (nParam >= 255)
          break;
      }
    szArgs[++nParam] = NULL;

  #ifdef WIN32
    intptr_t nRes = _spawnvp(_P_NOWAIT, szArgs[0], szArgs);
    hProcess = (HProcess) (nRes == -1 ? 0 : nRes);
  #else 
    hProcess = fork();
    switch(hProcess)
    {
      case 0:
      {
        execvp(szArgs[0], szArgs);
        exit(g_nResultError);
      } break;
      
      case -1:
      {
        perror("error fork");
      } break;
    }
  #endif
    
    return hProcess;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osWaitPid()
  //    DESCRIPTION:    ждать завершения процесса и получить его код возврата
  //    PARAMETRS:      hProcess - ид. процесса
  //                    piStatus - статус процесса
  //                    bWait = true - ждать завершения процесса
  //    RETURN:         идентификатор процесса, в случае ошибки -1 или 0
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int osWaitPid( HProcess hProcess, int* piStatus, bool bWait /*= false*/ )
  {
  #ifdef WIN32
    DWORD dwRetCode = 0;
    BOOL bRes = GetExitCodeProcess(hProcess, &dwRetCode);
    if (!bRes)
    {
      return -1;
    }

    if (dwRetCode == STILL_ACTIVE && bWait)
    {
      WaitForSingleObject(hProcess, INFINITE);
      bRes = GetExitCodeProcess(hProcess, &dwRetCode);
      if (!bRes)
      {
        return -1;
      }
    }

    if (piStatus != NULL)
    {
      *piStatus = (int)dwRetCode;
    }

    return dwRetCode == STILL_ACTIVE ? 0 : 1;
  #else
    return static_cast<int>(waitpid(hProcess, piStatus, bWait ? 0 : WNOHANG));
  #endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetCurrentProcessID
  //    DESCRIPTION:    получить идентификатор текущего процесса
  //    PARAMETRS:      none
  //    RETURN:         идентификатор вызывающего процесса из которого была вызвана эта функция
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  HProcess osGetCurrentProcessID()
  {
  #ifdef WIN32
    return GetCurrentProcess();
  #else
    return getpid();
  #endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osDaemon
  //    DESCRIPTION:    создание демона
  //    PARAMETRS:      none
  //    RETURN:         0, если успешно
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int osDaemon()
  {
  #ifdef WIN32
  #pragma message ( "WIN32 osDaemon not realized!" ) 
    return 0;
  #else
    return daemon(1, 1);
  #endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osPause
  //    DESCRIPTION:    приостановить выполнение процесса до прихода сообщения
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void osPause()
  {
  #ifdef WIN32
    WaitMessage();
    MSG Msg;
    GetMessage(&Msg, NULL, 0, 0);
    DispatchMessage(&Msg);
  //   const HANDLE hThread = GetCurrentThread();
  //   long lTh = MsgWaitForMultipleObjectsEx(1, &hThread, INFINITE, QS_TIMER, 0);
  #else
    pause();
  #endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Terminate
  //    DESCRIPTION:    завершить процесс
  //    PARAMETRS:      hProcess - идентификатор процесса
  //    RETURN:         1 - процесс не найден
  //                    2 - нет прав
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int osTerminateProcess(HProcess hProcess)
  {
  #ifdef WIN32
    if(TerminateProcess(hProcess, g_nResultError))
      return 0;

    switch(errno)
    {
    case ERROR_INVALID_HANDLE:
      return 1;
    case ERROR_ACCESS_DENIED:
      return 2;
    }
    return 3;  
  #else
    if (kill(hProcess, SIGKILL) == 0)
      return 0;
    
    switch(errno)
    {
    case ESRCH:
      return 1;
    case EPERM:
      return 2;
    }
    return 3;  
  #endif
  }

  rise::CString osGetCurrentExecPath()
  {
#ifdef WIN32
    TChar szPath[MAX_PATH];

    if(GetModuleFileName(NULL, szPath, MAX_PATH) == 0)
#else
    TChar szPath[PATH_MAX];
    int nReaded = readlink("/proc/self/exe", szPath, PATH_MAX);
    
    if(nReaded > 0)
    {
      szPath[nReaded] = '\0';
    } else
#endif
      return "";

    return szPath;
  }
}

