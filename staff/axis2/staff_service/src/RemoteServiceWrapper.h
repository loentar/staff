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

#ifndef _REMOTESERVICEWRAPPER_H_
#define _REMOTESERVICEWRAPPER_H_

#include <map>
#include <rise/common/MutablePtr.h>
#include <staff/component/Service.h>

namespace staff
{
  class CComponent;
  class COperation;
  class CDataObject;
  class CRemoteService;

  typedef rise::CMutablePtr<CRemoteService> PRemoteService;
  typedef std::map<rise::CString, PRemoteService> TRemoteServiceMap;

   //!  класс клиентской части для stateful сервиса
  class CRemoteServiceWrapper: public CService
  {
  public:
    //!         конструктор
    CRemoteServiceWrapper(CComponent* pComponent);
    
    //!         деструктор
    virtual ~CRemoteServiceWrapper();

    //!         получить имя сервиса
    /*! \return имя сервиса
    */
    const rise::CString& GetName() const;

    //!         получить описание сервиса
    /*! \return описание сервиса
    */
    const rise::CString& GetDescr() const;

    //!         получить список операций сервиса
    /*! \return список операций сервиса
    */
    CDataObject GetOperations() const;

    //!         выполнить операцию
    /*! \param  rOperation - операция
        */
    void Invoke(COperation& rOperation, const rise::CString& sID);

    //!         получить связанный с сервисом компонент
    /*! \return связанный с сервисом компонент
    */
    const CComponent* GetComponent() const;

    //!         получить связанный с сервисом компонент
    /*! \return связанный с сервисом компонент
    */
    CComponent* GetComponent();

    //!         получить указатель на реализацию сервиса(только для локальных сервисов)
    /*! \param  sID - идентификатор сервиса в группе
        \return указатель на реализацию сервиса
        */
    void* GetImpl(const rise::CString& sID);

    TRemoteServiceMap& GetServices();

  private:
    TRemoteServiceMap m_mServices;
    CComponent* m_pComponent;
  };
}

#endif // _REMOTESERVICEWRAPPER_H_
