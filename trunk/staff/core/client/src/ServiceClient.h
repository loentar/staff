/*
 *  Copyright 2010 Utkin Dmitry
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


#ifndef _SERVICECLIENT_H_
#define _SERVICECLIENT_H_

#include "staffclientexport.h"
#include <string>
#include <memory>

typedef struct axutil_env axutil_env_t;
typedef struct axis2_svc_client axis2_svc_client_t;
typedef struct axis2_options axis2_options_t;

namespace staff
{
  class CDataObject;
  class COptions;

  template <typename> class ICallback; //!< client callback
  typedef std::auto_ptr< ICallback<const CDataObject&> > PICallback;

  //! service client
  class STAFF_CLIENT_EXPORT CServiceClient
  {
  public:
    //! constructor
    CServiceClient();

    //! destructor
    ~CServiceClient();

    //! initialize client with default options
    /*! default options are:
        * given service uri
        * SOAP version: 1.1
        \param  sServiceUri - service URI
        */
    void Init(const std::string& sServiceUri = "");

    //! initialize client with given options
    /*! \param  rOptions - options
        */
    void Init(COptions& rOptions);

    //! deinitialize client
    void Deinit();

    //! checks is client initialized or not
    /*! \return true if client is initialized
      */
    bool IsInit() const;

    //! set client options
    /*! \param  rOptions - client options
      */
    void SetOptions(COptions& rOptions);

    //! get client options
    /*! \return client options
      */
    COptions& GetOptions();

    //! engage axis2/c module
    /*! \param sModuleName
      */
    void EngageModule(const std::string& sModuleName);

    //! disengage axis2/c module
    /*! \param sModuleName
      */
    void DisengageModule(const std::string& sModuleName);

    //! add SOAP header for next request
    /*! \param rdoHeader - SOAP header
        */
    void AddHeader(CDataObject& rdoHeader);

    //! remove all added SOAP headers
    void RemoveAllHeaders();

    //! sets the proxy with authentication support
    /*! \param sProxyHost - proxy server host
        \param sProxyPort - proxy server port
        \param sUserName - user name
        \param sPassword - password
        */
    void SetProxyWithAuth(const std::string& sProxyHost, const std::string& sProxyPort,
                          const std::string& sUserName, const std::string& sPassword);


    //! Tests Proxy Authentication is required or not
    /*! sends robust authentication test message
        */
    void TestAuthRequired();

    //! Gets the boolean value indicating whether Proxy Authentication is required.
    /*! \return true, if proxy auth is required
        to invoke auth test call TestAuthRequired before this function
        \sa TestAuthRequired
        \sa COptions::SetTestProxyAuth
        */
    bool GetProxyAuthRequired();

    //! Gets the boolean value indicating whether Http Authentication is required.
    /*! \return true, if http auth is required
        to invoke auth test call TestAuthRequired before this function
        \sa TestAuthRequired
        \sa COptions::SetTestHttpAuth
        */
    bool GetHttpAuthRequired();

    //! Gets the authentication type required
    /*! \return authentication type
        to invoke auth test call TestAuthRequired before this function
        \sa TestAuthRequired
      */
    std::string GetAuthType();


    //! invoke service synchronously
    /*! MEP: In-Out
        \param  rdoPayload - request to service
        \return service response
        */
    CDataObject Invoke(CDataObject& rdoPayload);

    //! invoke service asynchronously
    /*! MEP: In-Out
        \param  rdoPayload - request to service
        \param  rCallback - result callback
        */
    void Invoke(CDataObject& rdoPayload, ICallback<const CDataObject&>& rCallback);

    //! invoke service asynchronously with dynamicaly allocated callback
    /*! MEP: In-Out
        callback will be freed after calling
        \param  rdoPayload - request to service
        \param  rpCallback - result callback
        */
    void Invoke(CDataObject& rdoPayload, PICallback& rpCallback);

    //! send payload and check result, returned by server
    /*! MEP: Robust Out-Only
        \param  rdoPayload - request to service
        */
    void SendRobust(CDataObject& rdoPayload);

    //! send payload without checking result
    /*! MEP: In-Only
        \param  rdoPayload - request to service
        */
    void Send(CDataObject& rdoPayload);

    //! get last response has fault or not
    /*! \return true if last response has fault
      */
    bool GetLastResponseHasFault();

    //! get last response or last soap:Fault
    /*! \return last response or last fault
      */
    CDataObject GetLastResponse();

    //! get last axis2/c service client error string
    /*! \return error string
      */
    std::string GetLastErrorStr();

    //! axis2/c service client cast operator
    operator axis2_svc_client_t*();

  private:
    void PrepareToSend(CDataObject& rdoPayload);

  private:
    axis2_svc_client_t* m_pSvcClient;
    axutil_env_t* m_pEnv;
    COptions* m_pOptions;
    bool m_bOptOwner;
    bool m_bInit;
  };
}

#endif // _SERVICECLIENT_H_
