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


#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "staffclientexport.h"
#include <string>
#include <map>

typedef struct axutil_env axutil_env_t;
typedef struct axis2_options axis2_options_t;

namespace staff
{
  typedef std::map<std::string, std::string> StringMap;

  //! service client options
  class STAFF_CLIENT_EXPORT Options
  {
  public:
    //! soap version
    enum SoapVersion
    {
      NotSet = 0, //!< soap version is not set
      Soap11,     //!< soap version 1.1
      Soap12      //!< soap version 1.2
    };

  public:
    //! constructor
    /*! sets owner flag to true
        \param pEnv - axis2/c env
      */
    Options(axutil_env_t* pEnv);

    //! constructor
    /*! sets owner flag to false
        \param pOptions - axis2 options
        \param pEnv - axis2/c env
      */
    Options(axis2_options_t* pOptions, axutil_env_t* pEnv);

    //! destructor
    /*! axis2 options will be destroyed only if owner flag is set
      */
    ~Options();

    //! get is current object is owner or not
    /*! \return true if current object is owner
      */
    bool IsOwner() const;

    //! set current object as owner
    /*! \param bOwner - true if current object is owner
      */
    void SetOwner(bool bOwner = true);


    //! get WSA action
    /*! \return WSA action
      */
    std::string GetAction() const;

    //! set WSA action
    /*! \param sAction - WSA action
      */
    void SetAction(const std::string& sAction);

    //! get WSA from address
    /*! \return from address
      */
    std::string GetFromAddress() const;

    //! set WSA from address
    /*! \param sAddress - from address
      */
    void SetFromAddress(const std::string& sAddress);

    //! get WSA to address
    /*! \return to address
      */
    std::string GetToAddress() const;

    //! set WSA to address
    /*! \param sAddress - to address
      */
    void SetToAddress(const std::string& sAddress);

    //! get WSA reply to address
    /*! \return reply to address
      */
    std::string GetReplyToAddress() const;

    //! set WSA reply to address
    /*! \param sAddress - reply to address
      */
    void SetReplyToAddress(const std::string& sAddress);

    //! get WSA fault to address
    /*! \return fault to address
      */
    std::string GetFaultToAddress() const;

    //! set WSA fault to address
    /*! \param sAddress - fault to address
      */
    void SetFaultToAddress(const std::string& sAddress);

    //! set WSA use separate listener
    /*! \param bUseSeparateListener - true to use separate listener
      */
    void UseSeparateListener(bool bUseSeparateListener = true);

    //! get WSA is usung separate listener
    /*! \return true if using separate listener
      */
    bool IsUsingSeparateListener();


    //! set HTTP authentication information
    /*! \param sUserName - http username
        \param sPassword - http password
        \param sAuthType - http authentication type
        */
    void SetHttpAuthInfo(const std::string& sUserName, const std::string& sPassword,
                         const std::string& sAuthType);

    //! set the bool value indicating whether to test whether HTTP Authentication is required or not
    /*! \param bAuth - true - enable test
      */
    void SetTestHttpAuth(bool bAuth);


    //! set proxy authentication information
    /*! \param sUserName - proxy username
        \param sPassword - proxy password
        \param sAuthType - proxy authentication type
        */
    void SetProxyAuthInfo(const std::string& sUserName, const std::string& sPassword,
                          const std::string& sAuthType);

    //! set the bool value indicating whether to test whether proxy Authentication is required or not
    /*! \param bAuth - true - enable test
      */
    void SetTestProxyAuth(bool bAuth);


    //! set NTLM authentication information
    /*! \param sUserName    - ntlm username
        \param sPassword    - ntlm password
        \param sDomain      - ntlm domain (optional)
        \param sWorkstation - ntlm workstation (optional)
        \param nFlags       - ntlm flags (optional)
        */
    void SetNtlmAuthInfo(const std::string& sUserName, const std::string& sPassword,
                         const std::string& sDomain = "", const std::string& sWorkstation = "",
                         int nFlags = 0);


    //! set timeout
    /*! \param lTimeout - timeout in milli seconds
      */
    void SetTimeout(long lTimeout);


    //! get SOAP version URI
    /*! \return SOAP version URI
      */
    std::string GetSoapVersionUri() const;

    //! set SOAP version URI
    /*! \param sSoapVersionUri - SOAP version URI
      */
    void SetSoapVersionUri(const std::string& sSoapVersionUri);

    //! get SOAP version
    /*! \return SOAP version
      */
    SoapVersion GetSoapVersion() const;

    //! get SOAP version
    /*! \return SOAP version
      */
    void SetSoapVersion(SoapVersion nSoapVersion);

    //! get SOAP action
    /*! \return SOAP action
      */
    std::string GetSoapAction() const;

    //! set SOAP action
    /*! \param sSoapAction - SOAP action
      */
    void SetSoapAction(const std::string& sSoapAction);


    //! enable or disable REST
    /*! \param bEnable = true - enable REST
      */
    void EnableRest(bool bEnable = true);

    //! set http method to be used
    /*! \param sHttpMethod - http method
      */
    void SetHttpMethod(const std::string& sHttpMethod);


    //! set http headers for the next client request
    /*! \param rmHeaders - headers.
        after client request list of headers will be cleared
     */
    void SetHttpHeaders(const StringMap& rmHeaders);


    //! get is MTOM enabled or not
    /*! \return true - MTOM is enabled
      */
    bool IsMtomEnabled() const;

    //! enable or disable MTOM
    /*! \param bEnable = true - enable MTOM
      */
    void EnableMtom(bool bEnable = true);


    //! set property
    /*! \param sName - property name
        \param sValue - property value
     */
    void SetProperty(const std::string& sName, const std::string& sValue);


    //! get session id
    /*! \return session id
      */
    const std::string& GetSessionId() const;

    //! set session id
    /*! \param  sSessionId - session id
      */
    void SetSessionId(const std::string& sSessionId);

    //! get instance id
    /*! \return instance id
      */
    const std::string& GetInstanceId() const;

    //! set instance id
    /*! \param  sInstanceId - instance id
      */
    void SetInstanceId(const std::string& sInstanceId);

    //! set default namespace for all outgoing messages
    /*! \param sUri - namespace uri
        \param sPrefix - namespace prefix
        */
    void SetDefaultNamespace(const std::string& sUri, const std::string& sPrefix = "");

    //! get default namespace uri for all outgoing messages
    /*! \return default namespace uri for all outgoing messages
      */
    const std::string& GetDefaultNamespaceUri() const;

    //! get default namespace prefix for all outgoing messages
    /*! \return default namespace prefix for all outgoing messages
      */
    const std::string& GetDefaultNamespacePrefix() const;


    //! options init operator
    /*! sets owner flag to false
        \param pOptions - axis2/c options
      */
    Options& operator=(axis2_options_t* pOptions);

    //! axis2 options cast operator
    operator axis2_options_t*();

  private:
    axis2_options_t* m_pOptions;
    axutil_env_t* m_pEnv;
    bool m_bOwner;
    std::string m_sSessionId;
    std::string m_sInstanceId;
    std::string m_sDefaultNsUri;
    std::string m_sDefaultNsPrefix;
  };

}

#endif // _OPTIONS_H_
