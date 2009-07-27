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

#ifndef _THREAD_H_
#define _THREAD_H_

#include <rise/os/osthread.h>
#include <rise/common/types.h>
#include <rise/threading/CriticalData.h>
#include "CriticalSection.h"

namespace rise
{
  namespace threading
  {
    //!        класс потока
    class RISE_EXPORT CThread
    {
      public:
        //!        конструктор
        CThread();

        //!        деструктор
        virtual ~CThread();
    
        //!        создание и запуск потока
        /*
           \param  pparam - указатель на параметр передаваемый потоку
           \return 0, если создание потока произошло успешно
        */
        virtual bool Start(void* pParam = NULL);
    
        //!        запущен ли поток
        /*
           \return true если работает
        */
        bool IsWorking() const;
    
        //!        получение идентификатора вызывающего потока
        /*
           \return идентификатор
        */
        static HThread GetCurrentId();

        //!        получение идентификатора потока, связаного с обьектом класса
        /*
           \return идентификатор
        */
        HThread GetId() const;
    
        //!        выход из потока, в котором была вызвана эта функция
        static void Exit();
    
        //!        завершает работу потока, связанным с обьектом класса
        /*
              ВНИМАНИЕ! опасная функция! 
              память выделенная локальными обьектами, критические секции,
                дескрипторы, не будет освобождены!
        */
        bool Cancel();
    
        //!        ожидание завершения потока
        /*
           \param  hThread - дескриптор потока
        */
        void JoinThread();

        //!        устанавливает флаг остановки потока
        /*
           CLogicNoInitException
           \param  ulTimeout - таймаут, если равен=0 ждать бесконечно
        */
        void Stop(ulong ulTimeout = 0ul);

        //!        возвращает признак необходимости остановки потока
        /*
           \return  true - поток необходимо остановить
        */
        bool IsStopping();

        //!        приостановить выполнение текущего потока на ulMSec милисекунд
        /*
           \param  ulMSec - количество милисекунд
        */
        static void Sleep(unsigned long ulMSec);

        //!        получить критическую секцию для межпотоковой синхронизации
        CCriticalSection& GetCS();

      protected:
        //!        функция потока
        /*         функция, которую должен перекрывать унаследованный класс
                   pParam - параметр
        */
        virtual void Run(void* pParam) = 0;

        //!        функция обработчик события при остановке
        virtual void OnStop();


      private:
        //!        внутренняя функция для запуска пользовательской функции потока
        static void* InternalRun(void* pParam);

      private:
        HThread m_hThread;      //! идентификатор потока
        void*   m_pParam;       //! параметр, передаваемый потоку
        CCriticalSection m_cs;  //! критическая секция
        CCriticalData<bool> m_bStopping; //! признак остановки потока

        // запрет копирования
        CThread(const CThread&);
        CThread& operator=(const CThread&);
        
    };  

  } // namespace threading
}  // namespace rise
  
#endif
