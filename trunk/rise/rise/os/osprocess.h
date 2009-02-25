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
     \return идентификатор процесса, в случае ошибки -1 или 0
  */
  HProcess osWaitPid(HProcess hProcess, int* piStatus, bool bWait = false);

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
