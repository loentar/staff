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

#ifndef _SERVICEDISPATCHERCLIENT_H_
#define _SERVICEDISPATCHERCLIENT_H_

#include "staffserviceexport.h"

namespace staff
{
  class COperation;
  class CServiceWrapper;
  
  //!  часть sdk для сервиса staff. принимает запросы от диспетчера staff и передает их сервису staff
  class STAFF_SERVICE_EXPORT CServiceDispatcherClient
  {
  public:
    //!         получить ссылку на реализацию
    static CServiceDispatcherClient& ServiceDispatcherClient();

    //!         запустить клиент
    /*! \param  bSingleThread - не запускать отдельный поток для клиента, обрабатывать запросы в текущем потоке
        */
    void Start(bool bSingleThread = true);

    //!         остановить клиент
    void Stop();

    //!         установить обертку
    /*! \param  rServiceWrapper - обертка сервиса
        */
    void SetWrapper(CServiceWrapper& rServiceWrapper);

  protected:
    CServiceDispatcherClient();
  private:
    CServiceDispatcherClient& operator=(const CServiceDispatcherClient&);
    class CServiceDispatcherClientImpl;
    static CServiceDispatcherClientImpl* m_pImpl;
  };
}

#endif // _SERVICEDISPATCHERCLIENT_H_
