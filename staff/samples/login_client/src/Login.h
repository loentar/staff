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

namespace staff
{

  //!  Login service
  class CLogin
  {
  public:
    //!        destructor
    virtual ~CLogin() {}

    //!         login user and create session
    /*! this operation must be called from guest session
        \param  sUserName - username
        \param  sPassword - password
        \return created session id
        */
    virtual std::string Login(const std::string& sUserName, const std::string& sPassword) = 0;

    //!         login and create session
    /*! this operation must be called from guest session
        \param  sUserName - username
        \param  sPassword - password
        \param  bCloseExisting - close existing session if exists
        \return created session id
        */
    virtual std::string OpenSession(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting) = 0;

    //!         open extra session
    /*! \param  nExtraSessionId - extra session number
        */
    virtual std::string OpenExtraSession(int nExtraSessionId) = 0;

    //!         logout and close session
    virtual void Logout() = 0;

    //!         keepalive session
    virtual void KeepAliveSession() = 0;

    //!         validate session
    /*! \return true - session is valid
        */
    virtual bool ValidateSession() = 0;
    
    //!         get current user name
    /*! \return current use name
        */
    virtual std::string GetUserName() = 0;

    //!         get session expiration time
    /*! \return session expiration time in minutes
        */
    virtual int GetSessionExpiration() const = 0;

  };
  
}

#endif // _LOGIN_H_
