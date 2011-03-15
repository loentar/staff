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

#include <signal.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#endif
#include <errno.h>
#include "osthread.h"

namespace rise
{
  //////////////////////////////////////////////////////////////////////////////
  //    Threading
  //////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
  LARGE_INTEGER g_llFrequency = {{0}};
  BOOL g_bQueryResult = QueryPerformanceFrequency(&g_llFrequency);

  // windows version is lower 2003 server
#if _WIN32_WINNT < 0x0502

#ifdef __MINGW32__
  typedef ULONG_PTR KAFFINITY;
#endif

  typedef struct _CLIENT_ID
  {
    DWORD UniqueProcess; 
    DWORD UniqueThread;
  } CLIENT_ID, *PCLIENT_ID;

  typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS) 0x00000000)

  typedef LONG KPRIORITY;

  typedef struct _THREAD_BASIC_INFORMATION
  {
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    KAFFINITY AffinityMask;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
  } THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

  typedef enum _THREAD_INFORMATION_CLASS
  {
    ThreadBasicInformation = 0,
  } THREAD_INFORMATION_CLASS, *PTHREAD_INFORMATION_CLASS;

  class CThreadInfoLoader
  {
  public:
    DWORD GetThreadId(HANDLE hThread)
    {
      if (m_pNtQueryInformationThread)
      {
        THREAD_BASIC_INFORMATION tInfo;
        if (m_pNtQueryInformationThread(hThread, ThreadBasicInformation, &tInfo, sizeof(tInfo), NULL) == STATUS_SUCCESS)
        {
          return tInfo.ClientId.UniqueThread;
        }
      }
      return static_cast<DWORD>(-1);
    }

    static CThreadInfoLoader& Inst()
    {
      if (!m_pInst)
      {
        m_pInst = new CThreadInfoLoader;
      }
      return *m_pInst;
    }

    static void FreeInst()
    {
      if (m_pInst)
      {
        delete m_pInst;
        m_pInst = NULL;
      }
    }

  private:
    typedef NTSTATUS (__stdcall *pfnNtQueryInformationThread) (HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG, PULONG);

  private:
    CThreadInfoLoader():
      m_hModule(LoadLibrary("ntdll.dll")),
      m_pNtQueryInformationThread(NULL)
    {
      m_pNtQueryInformationThread = reinterpret_cast<pfnNtQueryInformationThread>(GetProcAddress(m_hModule, "NtQueryInformationThread"));
    }

    ~CThreadInfoLoader()
    {
      if (m_hModule)
      {
        FreeLibrary(m_hModule);
      }
    }

  private:
    HMODULE m_hModule;
    pfnNtQueryInformationThread m_pNtQueryInformationThread;
    static CThreadInfoLoader* m_pInst;
  };

  CThreadInfoLoader* CThreadInfoLoader::m_pInst = NULL;

#define rise_GetThreadId CThreadInfoLoader::Inst().GetThreadId
#else
#define rise_GetThreadId ::GetThreadId
#endif
#endif


bool osCreateThread(PThreadProc pProc, HThread* hThread, void* pParam /*= NULL*/, bool bDetached /* = true*/ )
{
#ifdef WIN32
  DWORD dwTreadId;
  *hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pProc, pParam, 0, &dwTreadId);
  return hThread != 0;
#else
  pthread_attr_t tAttr;
  pthread_attr_init(&tAttr);
  if (bDetached)
  {
    pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
  }
  return pthread_create(hThread, &tAttr, pProc, pParam) == 0;
#endif
}

HThread osGetCurrentThread()
{
  return 
#ifdef WIN32
  GetCurrentThread();
#else
  pthread_self();
#endif
}

bool osIsCurrentThread(HThread hThread)
{
  return
#ifndef WIN32
    hThread == pthread_self();
#else
    GetCurrentThreadId() == rise_GetThreadId(hThread);
#endif
}

void osExitThread()
{
#ifdef WIN32
  ExitThread(0);
#else
  pthread_exit(NULL);
#endif
}

bool osTerminateThread(HThread hThread)
{
  return 
#ifdef WIN32
  TerminateThread(hThread, 0) == TRUE;
#else
  pthread_cancel(hThread) == 0;
#endif
}

void osJoinThread(HThread hThread)
{
#ifdef WIN32
  WaitForSingleObject(hThread, INFINITE);
#else
  pthread_join(hThread, NULL);
#endif
}

bool osIsThreadExists(HThread hThread)
{
  return
#ifdef WIN32
    WaitForSingleObject(hThread, 1) != WAIT_FAILED;
#else
    pthread_kill(hThread, 0) == 0;
#endif
}

bool osWaitForThreadExit( HThread hThread, unsigned long ulMSec )
{
#ifdef WIN32
  if (ulMSec == 0)
    ulMSec = INFINITE;  
  return WaitForSingleObject(hThread, ulMSec) == WAIT_OBJECT_0;
#else
  if(ulMSec == 0)
  {
    for(;;)
    {
      if (pthread_kill(hThread, 0) != 0)
        return true;
      osSleep(10);
    }
  }
  
  for(; ulMSec != 0;)
  {
    if (pthread_kill(hThread, 0) != 0)
      return true;

    if (ulMSec > 10)
    {
      osSleep(10);
      ulMSec -= 10;
    } else
    {
      osSleep(ulMSec);
      ulMSec = 0;
    }
  }

  return false;
#endif
}

bool osWaitForThread( HThread hThread )
{
#ifdef WIN32
  return WaitForSingleObject(hThread, INFINITE) == WAIT_OBJECT_0;
#else
#ifdef OS_Linux
  int nStatus = 0;
  if (waitpid(hThread, &nStatus, 0))
  {
    return false;
  }
  return WIFEXITED(nStatus) != 0;
#else
  return osWaitForThreadExit(hThread, 0);
#endif
#endif
}


//////////////////////////////////////////////////////////////////////////////
//    Critical section
//////////////////////////////////////////////////////////////////////////////

void osInitializeCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  InitializeCriticalSection(pCriticalSection);
#else

#ifdef LINUX_RELEASE_MCBC
  pthread_mutexattr_t tAttr = {PTHREAD_MUTEX_RECURSIVE_NP};
  pthread_mutex_init(pCriticalSection, &tAttr);
#else
  pthread_mutexattr_t tAttr;
  pthread_mutexattr_init(&tAttr);
#if defined OS_Linux
  pthread_mutexattr_settype(&tAttr, PTHREAD_MUTEX_RECURSIVE_NP);
#else
  pthread_mutexattr_settype(&tAttr, PTHREAD_MUTEX_RECURSIVE);
#endif
  pthread_mutex_init(pCriticalSection, &tAttr);
  pthread_mutexattr_destroy(&tAttr);
#endif
#endif
}

void osDeleteCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  DeleteCriticalSection(pCriticalSection);
#else
  pthread_mutex_destroy(pCriticalSection);
#endif
}

void osEnterCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  EnterCriticalSection(pCriticalSection);
#else
  pthread_mutex_lock(pCriticalSection);
#endif
}

bool osTryEnterCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
#if(_WIN32_WINNT >= 0x0400)
  return TryEnterCriticalSection(pCriticalSection) != 0;
#else
#pragma message ("To include support of osTryEnterCriticalSection set preproccessor definition _WIN32_WINNT=0x0400 (or above)")
  return false;
#endif
#else
  return pthread_mutex_trylock(pCriticalSection) == 0;
#endif
}

void osLeaveCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  LeaveCriticalSection(pCriticalSection);
#else
  pthread_mutex_unlock(pCriticalSection);
#endif
}


//////////////////////////////////////////////////////////////////////////////
//    Events
//////////////////////////////////////////////////////////////////////////////

void osThreadEventCreate(PEvent pThreadEvent)
{
#ifdef WIN32
  *pThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#else
  sem_init(pThreadEvent, 0, 0);
#endif
}

void osThreadEventFree(PEvent pThreadEvent)
{
#ifdef WIN32
  CloseHandle(*pThreadEvent);
#else
  sem_destroy(pThreadEvent);
#endif
}


EEventWaitResult osThreadEventWait(PEvent pThreadEvent, unsigned long ulTimeout /*= RISE_WAIT_INFINITE*/)
{
  EEventWaitResult eResult;
#ifdef WIN32
  DWORD dwWaitResult = WaitForSingleObject(pThreadEvent, 5000);
  switch (dwWaitResult)
  {
  case WAIT_OBJECT_0:
    eResult = EEventWaitSignalled;
    break;

  case WAIT_TIMEOUT:
    eResult = EEventWaitTimeout;
    break;

  default:
    eResult = EEventWaitError;
  }
#else
  int nResult = 0;
  struct timeval tvNow;
  struct timespec tvAbsTimeout;

  gettimeofday(&tvNow, NULL);

  tvAbsTimeout.tv_sec = tvNow.tv_sec + ulTimeout / 1000;
  tvAbsTimeout.tv_nsec = ((ulTimeout % 1000) * 1000 + tvNow.tv_usec) * 1000;

  // check for overflow
  if (tvAbsTimeout.tv_nsec > 1000000000)
  {
    ++tvAbsTimeout.tv_sec;
    tvAbsTimeout.tv_nsec -= 1000000000;
  }

  // Wait for the event be signaled
  nResult = sem_timedwait(pThreadEvent, &tvAbsTimeout);

  if (nResult == 0)
  {
    eResult = EEventWaitSignalled;
  }
  else
  {
    eResult = (errno == ETIMEDOUT) ? EEventWaitTimeout : EEventWaitError;
  }
#endif
  return eResult;
}

void osThreadEventSignal(PEvent pThreadEvent)
{
#ifdef WIN32
  SetEvent(*pThreadEvent);
#else
  sem_post(pThreadEvent);
#endif
}


void osSleep(unsigned long ulMSec)
{
#ifdef WIN32
  Sleep(ulMSec);
#else
  usleep(ulMSec * 1000);
#endif
}

long long osQueryPerfomance()
{
#ifdef WIN32
  LARGE_INTEGER llPerf = {{0}};
  QueryPerformanceCounter(&llPerf);
  return llPerf.QuadPart * 1000ll / ( g_llFrequency.QuadPart / 1000ll);
#else
  struct timeval stTimeVal;
  gettimeofday(&stTimeVal, NULL);
  return stTimeVal.tv_sec * 1000000ll + stTimeVal.tv_usec;
#endif
}

}

