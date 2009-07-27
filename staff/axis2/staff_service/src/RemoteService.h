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

#ifndef _REMOTESERVICE_H_
#define _REMOTESERVICE_H_

#include <rise/socket/RecvSocket.h>

namespace staff
{
  class COperation;
  class CDataObject;
  class CRemoteServiceWrapper;
  class CService;

   //!  класс клиентской части для stateful сервиса
  class CRemoteService: public rise::sockets::CRecvSocket
  {
  public:
    //!         конструктор
    CRemoteService();
    
    //!         деструктор
    virtual ~CRemoteService();

    void Init();

    //!         получить имя сервиса
    /*! \return имя сервиса
    */
    const rise::CString& GetName() const;

    //!         получить ID сервиса
    /*! \return ID сервиса
    */
    const rise::CString& GetID() const;

    //!         получить описание сервиса
    /*! \return описание сервиса
    */
    const rise::CString& GetDescr() const;

    //!         получить список операций сервиса
    /*! \return список операций сервиса
    */
    CDataObject GetOperations() const;

    //!         создать сервис с указанным идентифиактором
    /*! \param  sID - идентификатор сервиса
        */
    void CreateServiceID(const rise::CString& sID);

    //!         выполнить операцию
    /*! \param  rOperation - операция
        */
    void Invoke(COperation& rOperation);

    //!         проверка соединения
    /*! \return true - соединение в норме
        */
    bool CheckConnection();

    //!         установить ссылку на обьект для работы с удаленным сервисом
    void SetRemoteServiceWrapper(CRemoteServiceWrapper* pRemoteServiceWrapper);

    //!         получить ссылку на обьект для работы с удаленным сервисом
    /*! \return обьект для работы с удаленным сервисом
    */
    CRemoteServiceWrapper* GetRemoteServiceWrapper();
  
  private:
    class CRemoteServiceImpl;
    CRemoteServiceImpl* m_pImpl;
  };
}

#endif // _REMOTESERVICE_H_
