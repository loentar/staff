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

#ifndef _SERVICEDISPATCHER_H_
#define _SERVICEDISPATCHER_H_

#include <rise/string/String.h>

namespace rise { template<typename> class CMutablePtr; }

namespace staff
{
  class CService;
  class COperation;
  class CRemoteService;

  typedef rise::CMutablePtr<CRemoteService> PRemoteService;

  //!  диспетчер сервисов
  class CServiceDispatcher
  {
  public:
    struct SEvents
    {
      void (*pOnConnect)(const std::string& sService, const CService* pService); //!< функция обработки события при присоединении сервисного клиента
      void (*pOnDisconnect)(const std::string& sServiceName); //!< функция обработки события при отсоединении сервисного клиента

      SEvents();
      SEvents(void (*pOnConnectInit)(const std::string&, const CService*), void (*pOnDisconnectInit)(const std::string&));
      SEvents& operator=(const SEvents& rEvents);
    };

  public:
    //!         получить ссылку на реализацию диспетчера
    /*! \return ссылка на диспетчер
    */
    static CServiceDispatcher& Inst();

    //!         освободить экземпляр диспетчера    
    static void FreeInst();

    //!         инициализация/запуск диспетчера
    /*! \param  stEvents - события
        */
    void Init(const SEvents& stEvents);

    //!         деинициализация/останов диспетчера
    void Deinit();

    //!         вызвать свою функцию(получение информации о сервисах)
    /*! \param  rOperation - операция
        */
    void InvokeSelf(COperation& rOperation);

    void GetRemoteService( const rise::CString& sServiceName, const rise::CString& sSessionId, PRemoteService& rpService );

  private:
    CServiceDispatcher();
    ~CServiceDispatcher();
  private:
    class CLocalServiceDispatcherImpl;
    class CRemoteServiceDispatcherImpl;
    CLocalServiceDispatcherImpl* m_pLocalImpl;
    CRemoteServiceDispatcherImpl* m_pRemoteImpl;
    static CServiceDispatcher* m_pInst;
  };
}

#endif // _SERVICEDISPATCHER_H_
