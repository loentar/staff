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

#ifndef _SERVICEWRAPPER_H_
#define _SERVICEWRAPPER_H_

#include "staffserviceexport.h"
#include <rise/string/String.h>

namespace staff
{
  class COperation;

  //!  класс обработчика запросов
  /*   от этого класса наследуется упаковщик для обработки запросов(сервисная часть)
  */
  class STAFF_SERVICE_EXPORT CServiceWrapper
  {
  public:
    //!         виртуальный деструктор
    virtual ~CServiceWrapper();
    
    //!         выполнить запрос
    /*!         эту функцию необходимо перекрыть в дочернем классе
        \param  rOperation - операция
        */
    virtual void Invoke(COperation& rOperation) = 0;

    //!         получить SessionId сервиса
    /*! \return SessionId сервиса
    */
    const rise::CString& GetSessionId() const;
  
    //!         установить SessionId сервиса
    /*! \param  sSessionId - SessionId сервиса
        */
    void SetSessionId(const rise::CString& sSessionId);

  private:
    rise::CString m_sSessionId;
  };
}

#endif // _SERVICEWRAPPER_H_
