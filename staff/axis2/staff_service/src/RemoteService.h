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

   //!  client part for service
  class CRemoteService: public rise::sockets::CRecvSocket
  {
  public:
    //!         constructor
    CRemoteService();
    
    //!         destructor
    virtual ~CRemoteService();

    //!         initialize
    void Init();

    //!         get service name
    /*! \return service name
    */
    const rise::CString& GetName() const;

    //!         get service session id
    /*! \return service session id
    */
    const rise::CString& GetID() const;

    //!         get service description
    /*! \return service description
    */
    const rise::CString& GetDescr() const;

    //!         get service operations
    /*! \return service operations
    */
    CDataObject GetOperations() const;

    //!         create service with given session id
    /*! \param  sID - session id
        */
    void CreateServiceID(const rise::CString& sID);

    //!         invoke operation
    /*! \param  rOperation - operation
        */
    void Invoke(COperation& rOperation);

    //!         check connection
    /*! \return true - connection is good
        */
    bool CheckConnection();

    //!         set service wrapper
    /*! \param  pRemoteServiceWrapper - service wrapper
        */
    void SetRemoteServiceWrapper(CRemoteServiceWrapper* pRemoteServiceWrapper);

    //!         get service wrapper
    /*! \return service wrapper
    */
    CRemoteServiceWrapper* GetRemoteServiceWrapper();
  
  private:
    class CRemoteServiceImpl;
    CRemoteServiceImpl* m_pImpl;
  };
}

#endif // _REMOTESERVICE_H_
