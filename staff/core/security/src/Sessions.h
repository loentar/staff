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

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    struct SSession
    {
      int nId;
      std::string sSessionId;
      int nUserId;
      int nExpires; //!< unix datetime
    };

    typedef std::list<SSession> TSessionsList;

    class CSessions
    {
    public:
      static const std::string sNobodySessionId;

    public:
      static CSessions& Inst();

      static void FreeInst();

      void GetById(int nId, SSession& rstSession);

      void GetBySessionId(const std::string& sSessionId, SSession& rstSession);

      void GetList(TSessionsList& rlsSessions);

      void Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId);

      //! close session
      void Close(const std::string& sSessionId);

      bool Validate(const std::string& sSessionId);

      void Keepalive(const std::string& sSessionId);

      int GetExpiration() const;

      int GetExpiresById(int nId);

      bool GetUserId(const std::string& sSessionId, int& nUserId);

      bool GetUserName(const std::string& sSessionId, std::string& sUserName);

      void CloseExpiredSessions();

    private:
      CSessions();
      ~CSessions();

    private:
      static CSessions* m_pInst;
      int m_nSessionExpiration;
    };
  }
}

#endif // _SESSIONS_H_
