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
  class CServiceWrapper;
  class COperation;

  //!  Service dispatcher
  class CServiceDispatcher
  {
  public:
    struct SEvents
    {
      void (*pOnConnect)(const CServiceWrapper* pService);
      void (*pOnDisconnect)(const std::string& sServiceName);

      SEvents();
      SEvents(void (*pOnConnectInit)(const CServiceWrapper*), void (*pOnDisconnectInit)(const std::string&));
      SEvents& operator=(const SEvents& rEvents);
    };

  public:
    //!         get reference to dispatcher instance
    /*! \return reference to dispatcher instance
    */
    static CServiceDispatcher& Inst();

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

  private:
    CServiceDispatcher();
    ~CServiceDispatcher();
    CServiceDispatcher(const CServiceDispatcher&);
    CServiceDispatcher& operator=(const CServiceDispatcher&);
  
  private:
    class CServiceDispatcherImpl;
    CServiceDispatcherImpl* m_pImpl;
  };
}

#endif // _SERVICEDISPATCHER_H_
