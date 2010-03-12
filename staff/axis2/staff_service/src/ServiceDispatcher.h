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

namespace rise { template<typename> class CMutablePtr; }

namespace staff
{
  class CService;
  class COperation;
  class CRemoteService;

  typedef rise::CMutablePtr<CRemoteService> PRemoteService;

  //!  Service dispatcher
  class CServiceDispatcher
  {
  public:
    struct SEvents
    {
      void (*pOnConnect)(const std::string& sService, const CService* pService);
      void (*pOnDisconnect)(const std::string& sServiceName);

      SEvents();
      SEvents(void (*pOnConnectInit)(const std::string&, const CService*), void (*pOnDisconnectInit)(const std::string&));
      SEvents& operator=(const SEvents& rEvents);
    };

  public:
    //!         get reference to dispatcher instance
    /*! \return reference to dispatcher instance
    */
    static CServiceDispatcher& Inst();

    //!         free instance
    static void FreeInst();

    //!         initialize dispatcher
    /*! \param  stEvents - events struct
        */
    void Init(const SEvents& stEvents);

    //!         deinitialize dispatcher
    void Deinit();

    //!         invoke dispatcher's function
    /*! \param  rOperation - operation
        */
    void InvokeSelf(COperation& rOperation);

    //!         get remote service
    /*! \param  sServiceName - service name
        \param  sSessionId - session id
        \param  rpService - service
        */
    void GetRemoteService( const std::string& sServiceName, const std::string& sSessionId, PRemoteService& rpService );

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
