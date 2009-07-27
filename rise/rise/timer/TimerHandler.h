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

#ifndef _TIMERHANDLER_H_
#define _TIMERHANDLER_H_

#include <rise/common/types.h>
#include <list>

namespace rise
{
  //!        класс для работы с таймером
  class RISE_EXPORT CTimerHandler  
  {
  public:
    typedef word TTimerID; //! идентификатор таймера

    enum EREPEAT //! количество срабатываний таймера
    {
      EINFINITE = 0  //!< бесконечное кол-во повторений
    };

  public:
    //!          конструктор
    CTimerHandler();

    //!          деструктор
    virtual ~CTimerHandler();

    //!        добавление таймера
    /*
       CAssertException - неверные параметры вызова
       \param  PARAMETERS:     dwMSec - время милисекундах до срабатывания таймера
       \param  dwRepeat  - количество срабатываний таймера
       \return идентификатор добалвенного таймера
    */
    TTimerID AddTimer( dword dwMSec, dword dwRepeat = EINFINITE );

    //!        удаление таймера
    /*
       \param  wID - идентификатор таймера
    */
    void DelTimer( TTimerID wID );

    //!        удаление всех таймеров
    void DeleteAllTimers();

  protected:
    //!        обработчик таймера
    /* эту функцию должен перекрыть унаследованный класс */
    virtual void OnTimer( word wID ) = 0;

    friend class CTimeCatcher;
  };
} // namespace rise
  
#endif // #ifdef _TIMERHANDLER_H_
