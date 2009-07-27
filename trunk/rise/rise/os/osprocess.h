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
#include <linux/limits.h>
#endif

namespace rise
{
#ifdef WIN32
  //!   дескриптор процесса
  typedef HANDLE HProcess;
  #define PID_0 NULL
#else
  //!   дескриптор процесса
  typedef pid_t HProcess;
  #define PID_0 0
#endif

  //! ошибка
  const int g_nResultError = 1000;

  //! процедура обработки сигнала(внутренняя)
  typedef void (*PSignalProc) (int nSignal);

  //!        послать сигнал процессу
  /*
     \param  hProcess - номер процесса
     \param  nSignal - номер сигнала
     \return 0, если успешно
  */
  int osKill(HProcess hProcess, int nSignal);

  //!        установка обработчика сигналов
  /*
     \param pSignalProc - процедура обработки сигналов
     \param nSignal - номер сигнала
  */
  void osSetSignalHandler(PSignalProc pSignalProc, int nSignal);

  //!        выполнение приложения
  /*
             аргументы разделяются символом табуляции '\t'
     \param  szCommand - команда с параметрами
     \return дескриптор процесса
  */
  HProcess osExecute(const CString& sCmdArgs);

  //!        ждать завершения процесса и получить его код возврата
  /*
     \param  hProcess - ид. процесса
     \param  piStatus - статус процесса
     \param  bWait = true - ждать завершения процесса
     \return -1: ошибка, 0 - процесс еще запущен
  */
  int osWaitPid(HProcess hProcess, int* piStatus, bool bWait = false);

  //!        получить идентификатор текущего процесса
  /*
     \return идентификатор процесса из которого была вызвана эта функция
  */
  HProcess osGetCurrentProcessID();

  //!         получить имя исполняемого файла с полным путем текущего процесса
  /*! \return имя исполняемого файла с полным путем текущего процесса
  */
  rise::CString osGetCurrentExecPath();

  //!        создание демона
  /*
     \return 0, если успешно
  */
  int osDaemon();

  //!        приостановить выполнение процесса до прихода сообщения/сигнала
  void osPause();

  //!        завершить процесс
  /*
     \param  hProcess - идентификатор процесса
     \return 0 - ok,  1 - процесс не найден,  2 - нет прав
  */
  int osTerminateProcess(HProcess hProcess);
};


#endif  // _OSPROCESS_H_
