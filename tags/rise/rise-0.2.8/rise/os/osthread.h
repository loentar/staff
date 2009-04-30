#ifndef _OSTHREAD_H_
#define _OSTHREAD_H_

#include "oscommon.h"

#ifndef WIN32
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#endif

namespace rise
{
  //! указатель на процедуру потока
  typedef void *(*PThreadProc) (void *);
#ifdef WIN32
  //! дескриптор потока
  typedef HANDLE HThread;
#else
  //! дескриптор потока
  typedef pthread_t HThread;
#endif

  //!     создание потока
  /*
     \param  pProc - процедура потока
     \param  pParam - параметр, передаваемый в поток
     \return идентификатор потока
  */
  bool osCreateThread(PThreadProc pProc, HThread* hThread, void* pParam = NULL);
  
  //!     получение текущего потока
  /*
     \return идентификатор текущего потока
  */
  HThread osGetCurrentThread();
  
  //!     завершение текущего потока
  void osExitThread();
  
  //!     уничтожение потока
  /*
     \param  hThread - дескриптор потока
     \return  true - если завершение потока произошло успешно
  */
  bool osTerminateThread(HThread hThread);
  
  //!     ожидание завершения потока
  /*
     \param  hThread - дескриптор потока
     \return none
  */
  void osJoinThread(HThread hThread);
  
  //!     существует ли поток с заданным идентификатором
  /*
     \return true, если существует
  */
  bool osIsThreadExists(HThread hThread);
  
  //!     ожидание завершения потока ограниченное время
  /*
     \param  ulMSec - количество милисекунд
     \return  true - поток завершился за указанный таймаут
  */
  bool osWaitForThreadExit(HThread hThread, unsigned long ulMSec);
  
  //    Locking
#ifdef WIN32
  //! дескриптор критической сессии
  typedef CRITICAL_SECTION HCriticalSection;
  //! указатель на дескриптор критической сессии
  typedef LPCRITICAL_SECTION PCriticalSection;
#else
  //! дескриптор критической сессии
  typedef pthread_mutex_t HCriticalSection;
  //! указатель на дескриптор критической сессии
  typedef pthread_mutex_t* PCriticalSection;
#endif
  
  //!   инициализация критической секции
  /*
     (WIN32) STATUS_NO_MEMORY - при отсутствии своб. памяти
     \param  hCriticalSection - критическая секция
  */
  void osInitializeCriticalSection(PCriticalSection pCriticalSection);
  
  //!     удаление критической секции
  void osDeleteCriticalSection(PCriticalSection pCriticalSection);
  
  //!     вход в критическую секцию
  void osEnterCriticalSection(PCriticalSection pCriticalSection);
  
  //!     попытка войти в критическую секцию без блокирования
  /*
     \return true, если попытка успешна
  */
  bool osTryEnterCriticalSection(PCriticalSection pCriticalSection);
  
  //!     выход из критической секции
  void osLeaveCriticalSection(PCriticalSection pCriticalSection);
  
  //!     приостановить выполнение потока на ulMSec милисекунд
  /*
     \param  ulMSec - количество милисекунд
     \return none
  */
  void osSleep(unsigned long ulMSec);
  
  //!     запросить значение счетчика микросекунд
  /*
     \return счетчик в микросекундах
  */
  long long osQueryPerfomance();

}; //namespace rise  
  
#endif  // _OSTHREAD_H_

