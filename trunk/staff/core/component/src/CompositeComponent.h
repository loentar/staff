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

#ifndef _COMPOSITECOMPONENT_H_
#define _COMPOSITECOMPONENT_H_

#include "staffcomponentexport.h"
#include <rise/string/String.h>
#include <staff/common/DataObject.h>
#include "Component.h"

namespace staff
{
  //!  составной компонент
  class STAFF_COMPONENT_EXPORT CCompositeComponent: public CComponent
  {
  public:
    //!        конструктор
    CCompositeComponent();

    //!        деструктор
    virtual ~CCompositeComponent();

    //!         составить компонент с уже имеющимся
    /*! \param  pComponent - компонент
        */
    void Compose(CComponent* pComponent);

    //!         получить имя компонента
    /*! \return имя компонента
    */
    const rise::CString& GetName() const;

    //!         получить сервис
    /*! \param  sName - имя сервиса
        \return указатель на обьект для работы с сервисом, NULL если сервис не найден
        */
    const CService* GetService(const rise::CString& sName) const;

    //!         получить сервис
    /*! \param  sName - имя сервиса
        \return указатель на обьект для работы с сервисом, NULL если сервис не найден
        */
    CService* GetService(const rise::CString& sName);

    //!         добавить сервис
    /*! \param  pService - указатель на сервис
        */
    void AddService(CService* pService);

    //!         удалить сервис
    /*! \param  sName - имя сервиса
        */
    void RemoveService(const rise::CString& sName);

    //!         получить список сервисов
    /*! \return список сервисов
    */
    const TServiceMap& GetServices() const;

    //!         получить свойства компонента
    /*! \return свойства компонента
    */
    CDataObject& GetProperties();
  
  private:
    rise::CString m_sName;
    TServiceMap m_mServices;
    CDataObject m_doProperties;
  };
}

#endif // _COMPOSITECOMPONENT_H_
