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

#ifndef _SESSIONMANAGER_H_
#define _SESSIONMANAGER_H_

#include "staffcomponentexport.h"
#include <string>

namespace staff
{
  //! session manager
  class STAFF_COMPONENT_EXPORT SessionManager
  {
  public:
    //! get session manager instance
    /*! \return session manager instance
    */
    static SessionManager& Inst();

    //! start sessions watching
    void Start();

    //! stop sessions watching
    void Stop();

    //! login user and create session, get existing session if user alredy logged in
    /*! \param  sUserName - user name
        \param  sPassword - user password
        \param  sSessionId - resulting session id
        */
    void Login(const std::string& sUserName, const std::string& sPassword, std::string& sSessionId);

    //! login and create session
    /*! \param  sUserName - user name
        \param  sPassword - user password
        \param  bCloseExisting - close existing session if exists, else throw exception
        \param  sSessionId - resulting session id
        */
    void Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId);

    //! close session
    /*! \param  sSessionId - session id
      */
    void Close(const std::string& sSessionId);

    //! is session opened
    /*! \param  sSessionId - session id
        */
    bool IsOpened(const std::string& sSessionId);

    //! validate session (using db query)
    /*! \param  sSessionId - session id
        */
    bool Validate(const std::string& sSessionId);

    //! get number of seconds after which session will be expired
    /*! \param  sSessionId - session id
        \return remaining time of session
        */
    int GetExpiresIn(const std::string& sSessionId);

    //! keepalive session
    /*! \param  sSessionId - session id
        */
    int Keepalive(const std::string& sSessionId);

  private:
    SessionManager();
    ~SessionManager();
    SessionManager(const SessionManager&);
    SessionManager& operator=(const SessionManager&);

  private:
    class SessionManagerImpl;
    SessionManagerImpl* m_pImpl;
  };

}

#endif // _SESSIONMANAGER_H_
