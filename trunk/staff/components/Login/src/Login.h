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

#ifndef _LOGIN_H_
#define _LOGIN_H_

#include <string>
#include <staff/common/IService.h>

namespace staff
{
  //!  Login service
  class Login: public IService
  {
  public:
    //! login user and create session, get existing session if user alredy logged in
    /*! this operation must be called from guest session
        \param  sUserName - username
        \param  sPassword - password
        \return created/existing session id
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: login
    virtual std::string LoginUser(const std::string& sUserName, const std::string& sPassword) = 0;

    //! login and create session
    /*! this operation must be called from guest session
        \param  sUserName - username
        \param  sPassword - password
        \param  bCloseExisting - close existing session if exists
        \return created session id
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: openSession
    virtual std::string OpenSession(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting) = 0;

    //! logout and close session
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: logout
    virtual void Logout() = 0;

    //! keepalive session
    /*! \return remaining duration of session in seconds
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: keepAlive
    virtual int KeepAliveSession() = 0;

    //! validate session
    /*! \return true - session is valid
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: validate
    virtual bool ValidateSession() = 0;

    //! get current user id
    /*! \return current user id
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: getUserId
    virtual int GetUserId() = 0;

    //! get current user name
    /*! \return current user name
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: getUserName
    virtual std::string GetUserName() = 0;

    //! get current user description
    /*! \return current user description
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: getUserDescription
    virtual std::string GetUserDescription() = 0;

    //! get standard duration of session expiration
    /*! \return standard duration of session expiration time in minutes
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: getSessionExpiration
    virtual int GetSessionExpiration() const = 0;

    //! get number of seconds after which session will be expired
    /*! \param  sSessionId - session id
        \return remaining duration of session in seconds
        */
    // *restEnable: true
    // *restMethod: POST
    // *restLocation: getSessionExpiresIn
    virtual int GetSessionExpiresIn() const = 0;

  };

}

#endif // _LOGIN_H_
