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

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <rise/string/String.h>
#include "staffcomponentexport.h"

namespace staff
{
  class CDataObject;
  class COperation;
  class CComponent;

  //!  сервис диспетчера
  class STAFF_COMPONENT_EXPORT CService
  {
  public:
    //!        деструктор
    virtual ~CService();

    //!         получить имя сервиса
    /*! \return имя сервиса
    */
    virtual const rise::CString& GetName() const = 0;

    //!         получить описание сервиса
    /*! \return описание сервиса
    */
    virtual const rise::CString& GetDescr() const = 0;
    
    //!         получить описание операций
    /*! \return описание операций
    */
    virtual CDataObject GetOperations() const = 0;
    
    //!         вызвать операцию сервиса
    /*! \param  rOperation - операция
        \param  sID - идентификатор сервиса
        */
    virtual void Invoke(COperation& rOperation, const rise::CString& sID) = 0;

    //!         получить связанный с сервисом компонент
    /*! \return связанный с сервисом компонент
    */
    virtual const CComponent* GetComponent() const = 0;

    //!         получить связанный с сервисом компонент
    /*! \return связанный с сервисом компонент
    */
    virtual CComponent* GetComponent() = 0;

    //!         получить указатель на реализацию сервиса(только для локальных сервисов)
    /*! \param  sID - идентификатор сервиса в группе
        \return указатель на реализацию сервиса
        */
    virtual void* GetImpl(const rise::CString& sID) = 0;
  };
}

#endif // _SERVICE_H_
