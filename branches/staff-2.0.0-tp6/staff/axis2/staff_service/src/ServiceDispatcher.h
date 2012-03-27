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

#include <string>

namespace staff
{
  class ServiceWrapper;
  class Operation;

  //!  Service dispatcher
  class ServiceDispatcher
  {
  public:
    struct Events
    {
      void (*pOnConnect)(const ServiceWrapper* pService);
      void (*pOnDisconnect)(const std::string& sServiceName);

      Events();
      Events(void (*pOnConnectInit)(const ServiceWrapper*), void (*pOnDisconnectInit)(const std::string&));
      Events& operator=(const Events& rEvents);
    };

  public:
    //!         get reference to dispatcher instance
    /*! \return reference to dispatcher instance
    */
    static ServiceDispatcher& Inst();

    //!         initialize dispatcher
    /*! \param  stEvents - events struct
        */
    void Init(const Events& stEvents);

    //!         deinitialize dispatcher
    void Deinit();

    //!         invoke dispatcher's function
    /*! \param  rOperation - operation
        */
    void InvokeSelf(Operation& rOperation);

  private:
    ServiceDispatcher();
    ~ServiceDispatcher();
    ServiceDispatcher(const ServiceDispatcher&);
    ServiceDispatcher& operator=(const ServiceDispatcher&);
  
  private:
    class ServiceDispatcherImpl;
    ServiceDispatcherImpl* m_pImpl;
  };
}

#endif // _SERVICEDISPATCHER_H_
