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
#ifdef OS_FreeBSD
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#include "osprocess.h"

namespace rise
{
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

  void osSetSignalHandler(PSignalProc pSignalProc, int nSignal)
  {
  #ifdef WIN32
    #pragma message ( "WIN32 osSetSignalHandler not realized!" ) 
    #ifndef __MINGW32__ // warning: statement has no effect
      pSignalProc;
      nSignal;
    #endif
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

  HProcess osExecute( const CString& sCmdArgs )
  {
    HProcess hProcess = PID_0;

    CStringA asCmdArgs = sCmdArgs;

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

  HProcess osGetCurrentProcessID()
  {
  #ifdef WIN32
    return GetCurrentProcess();
  #else
    return getpid();
  #endif
  }

  int osDaemon()
  {
  #ifdef WIN32
  #pragma message ( "WIN32 osDaemon not realized!" ) 
    return 0;
  #else
    return daemon(1, 1);
  #endif
  }

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
#ifdef OS_FreeBSD
    TChar szPath[PATH_MAX];
    size_t nPathSize = PATH_MAX;
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    sysctl(mib, 4, szPath, &nPathSize, NULL, 0);

    if(nPathSize > 0)
    {
      szPath[nPathSize] = '\0';
    } else
#else
#ifdef OS_Darwin
    TChar szPath[PATH_MAX];
    size_t nPathSize = PATH_MAX;
    _NSGetExecutablePath(path, &nPathSize);
    if(nPathSize > 0)
    {
      szPath[nPathSize] = '\0';
    } else
#else
    TChar szPath[PATH_MAX];
    int nReaded = readlink("/proc/self/exe", szPath, PATH_MAX);

    if(nReaded > 0)
    {
      szPath[nReaded] = '\0';
    } else
#endif
#endif
#endif
      return "";

    return szPath;
  }
}

