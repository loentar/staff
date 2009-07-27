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

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "staffcomponentexport.h"
#include <map>
#include <rise/string/String.h>

namespace rise
{
  template<typename Type> class CMutablePtr;
}

namespace staff
{
  class CService;
  typedef rise::CMutablePtr<CService> PService; //!< Указатель на обьект сервиса
  typedef std::map<rise::CString, PService> TServiceMap; //!< Ассоциативный список сервисов

  //!  компонент диспетчера
  class STAFF_COMPONENT_EXPORT CComponent
  {
  public:
    //!        деструктор
    virtual ~CComponent() {}

    //!         получить имя компонента
    /*! \return имя компонента
    */
    virtual const rise::CString& GetName() const = 0;

    //!         получить сервис
    /*! \param  sService - имя сервиса
        \return указатель на обьект для работы с сервисом, NULL если сервис не найден
        */
    virtual const CService* GetService(const rise::CString& sService) const = 0;

    //!         получить сервис
    /*! \param  sService - имя сервиса
        \return указатель на обьект для работы с сервисом, NULL если сервис не найден
        */
    virtual CService* GetService(const rise::CString& sService) = 0;

    //!         получить список сервисов
    /*! \return список сервисов
    */
    virtual const TServiceMap& GetServices() const = 0;
  };
}

#endif // _COMPONENT_H_
