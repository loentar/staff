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

#ifndef _SESSIONS_H_
#define _SESSIONS_H_

#include "staffsecurityexport.h"
#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    //! session
    struct STAFF_SECURITY_EXPORT SSession
    {
      int nId;                //!< id
      std::string sSessionId; //!< session id
      int nUserId;            //!< session opener id
      int nExpires;           //!< datetime of session expiration (unix datetime)
    };

    typedef std::list<SSession> TSessionsList; //!< list of sessions

    //! sessions
    class STAFF_SECURITY_EXPORT CSessions
    {
    public:
      static const std::string sNobodySessionId;  //!< default session id

    public:
      //! get sessions instance
      /*! \return session instance
        */
      static CSessions& Inst();

      //! free sessions instance
      static void FreeInst();

      //! get session by id
      /*! \param nId - id
          \param rstSession - resulting session
        */
      void GetById(int nId, SSession& rstSession);

      //! get session by session id
      /*! \param sSessionId - session id
          \param rstSession - resulting session
        */
      void GetBySessionId(const std::string& sSessionId, SSession& rstSession);

      //! get sessions list
      /*! \param rlsSessions - resulting sessions list
        */
      void GetList(TSessionsList& rlsSessions);

      //! auth user and create new session
      /*! \param sUserName - user name
          \param sPassword - user password
          \param bCloseExisting - close existing session if exists
          \param sSessionId - created session id
          */
      void Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId);

      //! close session
      /*! \param sSessionId - session id to close
          */
      void Close(const std::string& sSessionId);

      //! validate session
      /*! checks session is expired or not
          \param sSessionId - session id
          \return true, session is not expired
          */
      bool Validate(const std::string& sSessionId);

      //! keep alive session
      /*! \param sSessionId - session id
        */
      void Keepalive(const std::string& sSessionId);

      //! get session expiration
      /*! \return session expiration (seconds)
        */
      int GetExpiration() const;

      // misc functions

      //! get number of seconds before session expires
      /*! \param nId - session id
          \return number of seconds before session expires
        */
      int GetExpiresById(int nId);

      //! get user id by session id
      /*! \param sSessionId - session id
          \param nUserId - resulting user id
          \return true if function successed
          */
      bool GetUserId(const std::string& sSessionId, int& nUserId);

      //! get user name by session id
      /*! \param sSessionId - session id
          \param sUserName - resulting user name
          \return true if function successed
          */
      bool GetUserName(const std::string& sSessionId, std::string& sUserName);

      //! get user description by session id
      /*! \param sSessionId - session id
          \param sUserDescription - resulting user description
          \return true if function successed
          */
      bool GetUserDescription(const std::string& sSessionId, std::string& sUserDescription);

      //! get session id by user name
      /*! \param sUserName - user name
          \param sSessionId - resulting session id
          \return true if function successed
          */
      bool GetSessionIdByUserName(const std::string& sUserName, std::string& sSessionId);

      //! get session id by user name and password
      /*! \param sUserName - user name
          \param sPassword - user password
          \param sSessionId - resulting session id
          \return true if function successed
          */
      bool GetSessionIdByUserNameAndPassword(const std::string& sUserName, const std::string& sPassword, std::string& sSessionId);


      //! close expired sessions
      /*! cleanup
          */
      void CloseExpiredSessions();

    private:
      CSessions();
      ~CSessions();

    private:
      static CSessions* m_pInst;
      int m_nSessionExpiration; //!< session expiration
    };
  }
}

#endif // _SESSIONS_H_
