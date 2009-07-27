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
#include "osthread.h"

namespace rise
{

#ifdef WIN32
  LARGE_INTEGER g_llFrequency = {0};
  BOOL g_bQueryResult = QueryPerformanceFrequency(&g_llFrequency);
#endif


bool osCreateThread(PThreadProc pProc, HThread* hThread, void* pParam /*= NULL*/ )
{
#ifdef WIN32
  DWORD dwTreadId;
  *hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pProc, pParam, 0, &dwTreadId);
  return hThread != 0;
#else
  pthread_attr_t tAttr;
  pthread_attr_init(&tAttr);
  pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
  return pthread_create(hThread, &tAttr, pProc, pParam) == 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osGetCurrentThread(...)
//    DESCRIPTION:    получение текущего потока
//    PARAMETRS:      none
//    RETURN:         идентификатор текущего потока
//    EXCEPTIONS:     none
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
HThread osGetCurrentThread()
{
  return 
#ifdef WIN32
  GetCurrentThread();
#else
  pthread_self();
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osExitThread
//    DESCRIPTION:    завершение текущего потока
//    PARAMETRS:      none
//    RETURN:         none
//    EXCEPTIONS:     none
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
void osExitThread()
{
#ifdef WIN32
  ExitThread(0);
#else
  pthread_exit(NULL);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osTerminateThread
//    DESCRIPTION:    уничтожение потока
//    PARAMETRS:      hThread - дескриптор потока
//    RETURN:         true - если завершение потока произошло успешно
//    EXCEPTIONS:     none
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
bool osTerminateThread(HThread hThread)
{
  return 
#ifdef WIN32
  TerminateThread(hThread, 0) == TRUE;
#else
  pthread_cancel(hThread) == 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osJoinThread
//    DESCRIPTION:    ожидание завершения потока
//    PARAMETRS:      hThread - дескриптор потока
//    RETURN:         none
//    EXCEPTIONS:     none
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
void osJoinThread(HThread hThread)
{
#ifdef WIN32
  WaitForSingleObject(hThread, INFINITE);
#else
  pthread_join(hThread, NULL);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osIsThreadExists
//    DESCRIPTION:    существует ли поток с заданным идентификатором
//    PARAMETRS:      none
//    RETURN:         true, если существует
//    EXCEPTIONS:     none
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
bool osIsThreadExists(HThread hThread)
{
  return
#ifdef WIN32
    WaitForSingleObject(hThread, 1) != WAIT_FAILED;
#else
    pthread_kill(hThread, 0) == 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osWaitForThreadExit()
//    DESCRIPTION:    ожидание завершения потока ограниченное время
//    PARAMETRS:      ulMSec - количество милисекунд
//    RETURN:         true - поток завершился за указанный таймаут
//    EXCEPTIONS:     
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
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
      if (pthread_kill(hThread, 0) == 0)
        return true;
      osSleep(10);
    }
  }
  
  for(; ulMSec != 0;)
  {
    if (pthread_kill(hThread, 0) == 0)
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


//////////////////////////////////////////////////////////////////////////////
//    Threading
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osInitializeCriticalSection(...)
//    DESCRIPTION:    инициализация критической секции
//    PARAMETRS:      hCriticalSection - критическая секция
//    RETURN:         none
//    EXCEPTIONS:     (WIN32) STATUS_NO_MEMORY - при отсутствии своб. памяти
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////  
void osInitializeCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  InitializeCriticalSection(pCriticalSection);
#else
#ifdef OS_MCBC
  pthread_mutexattr_t tAttr = {PTHREAD_MUTEX_RECURSIVE_NP};
#else
  pthread_mutexattr_t tAttr;
  pthread_mutexattr_settype(&tAttr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
  pthread_mutex_init(pCriticalSection, &tAttr);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osDeleteCriticalSection(...)
//    DESCRIPTION:    удаление критической секции
//    PARAMETRS:      none
//    RETURN:         none
//    EXCEPTIONS:     
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
void osDeleteCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  DeleteCriticalSection(pCriticalSection);
#else
  pthread_mutex_destroy(pCriticalSection);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osEnterCriticalSection(...)
//    DESCRIPTION:    вход в критическую секцию
//    PARAMETRS:      none
//    RETURN:         none
//    EXCEPTIONS:     
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
void osEnterCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  EnterCriticalSection(pCriticalSection);
#else
  pthread_mutex_lock(pCriticalSection);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osTryEnterCriticalSection(...)
//    DESCRIPTION:    попытка войти в критическую секцию без блокирования
//    PARAMETRS:      none
//    RETURN:         true, если попытка успешна
//    EXCEPTIONS:     
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osLeaveCriticalSection(...)
//    DESCRIPTION:    выход из критической секции
//    PARAMETRS:      none
//    RETURN:         none
//    EXCEPTIONS:     
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
void osLeaveCriticalSection(PCriticalSection pCriticalSection)
{
#ifdef WIN32
  LeaveCriticalSection(pCriticalSection);
#else
  pthread_mutex_unlock(pCriticalSection);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osSleep
//    DESCRIPTION:    приостановить выполнение потока на ulMSec милисекунд
//    PARAMETRS:      ulMSec количество милисекунд
//    RETURN:         none
//    EXCEPTIONS:     none
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
void osSleep(unsigned long ulMSec)
{
#ifdef WIN32
  Sleep(ulMSec);
#else
  usleep(ulMSec * 1000);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//    FUNCTION:       osQueryPerfomance
//    DESCRIPTION:    запросить значение счетчика микросекунд
//    PARAMETRS:      none
//    RETURN:         счетчик в микросекундах
//    EXCEPTIONS:     
//    COMMENT:        none
//////////////////////////////////////////////////////////////////////////////
long long osQueryPerfomance()
{
#ifdef WIN32
  LARGE_INTEGER llPerf = {0};
  QueryPerformanceCounter(&llPerf);
  return llPerf.QuadPart * 1000ll / ( g_llFrequency.QuadPart / 1000ll);
#else
  struct timeval stTimeVal;
  gettimeofday(&stTimeVal, NULL);
  return stTimeVal.tv_sec * 1000000ll + stTimeVal.tv_usec;
#endif
}

}

