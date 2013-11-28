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

#ifdef _MSC_VER
#pragma warning (disable : 4267)
#endif

#include <staff/common/Exception.h>
#include <staff/utils/Log.h>
#include <staff/sqlite3/sqlite3.h>
#include "tools.h"
#include "Time.h"
#include "DbConn.h"
#include "Sessions.h"

namespace staff
{
  namespace security
  {
    Sessions& Sessions::Inst()
    {
      static Sessions tInst;
      return tInst;
    }

    void Sessions::GetById(int nId, Session& rstSession)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, sessionid, userid, expires FROM sessions "
                                       "WHERE id=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_ROW, "Session by id is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstSession.nId = sqlite3_column_int(pVm, 0);
        const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        rstSession.sSessionId = szSessionId != NULL ? szSessionId : "";
        rstSession.nUserId = sqlite3_column_int(pVm, 2);
        rstSession.nExpires = sqlite3_column_int(pVm, 3);
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Sessions::GetBySessionId(const std::string& sSessionId, Session& rstSession)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, sessionid, userid, expires FROM sessions "
                                       "WHERE sessionid=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_ROW, "Session by id is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstSession.nId = sqlite3_column_int(pVm, 0);
        const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        rstSession.sSessionId = szSessionId != NULL ? szSessionId : "";
        rstSession.nUserId = sqlite3_column_int(pVm, 2);
        rstSession.nExpires = sqlite3_column_int(pVm, 3);
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Sessions::GetList(SessionsList& rlsSessions)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, sessionid, userid, expires FROM sessions", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        rlsSessions.clear();

        // get data
        while (sqlite3_step(pVm) == SQLITE_ROW)
        {
          Session stSession;
          stSession.nId = sqlite3_column_int(pVm, 0);
          const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
          stSession.sSessionId = szSessionId != NULL ? szSessionId : "";
          stSession.nUserId = sqlite3_column_int(pVm, 2);
          stSession.nExpires = sqlite3_column_int(pVm, 3);
          rlsSessions.push_back(stSession);
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Sessions::Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      int nUserId = -1;

      // get user id
      int nResult = sqlite3_prepare_v2(pDb, "SELECT id FROM users WHERE name=? AND password=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        STAFF_ASSERT(sqlite3_step(pVm) != SQLITE_DONE, "User with name and password is not found: " + std::string(sqlite3_errmsg(pDb)));

        nUserId = sqlite3_column_int(pVm, 0);
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));

      const char* szRequest = bCloseExisting ? "REPLACE INTO sessions(userid, expires) VALUES(?, ?)"
                                             : "INSERT INTO sessions(userid, expires) VALUES(?, ?)";
      // open session
      nResult = sqlite3_prepare_v2(pDb, szRequest, -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nUserId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, Time::Get() + m_nSessionExpiration);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));


        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to create session: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));


      // get inserted session id
      nResult = sqlite3_prepare_v2(pDb, "SELECT sessionid FROM sessions WHERE id = last_insert_rowid()", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_ROW, "Failed to get last created session id: " + std::string(sqlite3_errmsg(pDb)));

        const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
        STAFF_ASSERT(szSessionId != NULL, "session id == NULL: " + std::string(sqlite3_errmsg(pDb)));
        sSessionId = szSessionId;
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Sessions::Close(const std::string& sSessionId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      STAFF_ASSERT_PARAM(sSessionId.size() != 0);
      STAFF_ASSERT(sSessionId != sNobodySessionId, "closing nobody's session is not allowed");

      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM sessions WHERE sessionid=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to close session: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    bool Sessions::Validate(const std::string& sSessionId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bValid = false;

      STAFF_ASSERT_PARAM(sSessionId.size() != 0);
//      if (sSessionId == sNobodySessionId)
//      {
//        return true;
//      }

      // validate session
      int nResult = sqlite3_prepare_v2(pDb, "SELECT (expires > ?2) FROM sessions WHERE sessionid = ?1", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, Time::Get());
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          bValid = sqlite3_column_int(pVm, 0) != 0;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));

      return bValid;
    }

    void Sessions::Keepalive(const std::string& sSessionId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      if (sSessionId == sNobodySessionId)
      {
        // ignore request to keepalive nobody's session
        return;
      }

      // TODO: optimize
      STAFF_ASSERT(Validate(sSessionId), "Session expired");

      // update session
      int nResult = sqlite3_prepare_v2(pDb,
          "UPDATE sessions SET expires=? WHERE sessionid=?", -1, &pVm, NULL);

      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, Time::Get() + m_nSessionExpiration);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to keepalive session: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    int Sessions::GetExpiration() const
    {
      return m_nSessionExpiration;
    }

    int Sessions::GetExpiresById(int nId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nExpires = -1;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT expires FROM sessions WHERE id=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_ROW, "Session by id is not found: " + std::string(sqlite3_errmsg(pDb)));

        nExpires = sqlite3_column_int(pVm, 0);
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));

      return nExpires;
    }


    bool Sessions::GetUserId(const std::string& sSessionId, int& nUserId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      STAFF_ASSERT_PARAM(sSessionId.size() != 0);

      // get user id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT userid FROM sessions WHERE sessionid = ?1", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          nUserId = sqlite3_column_int(pVm, 0);
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }

    bool Sessions::GetUserName(const std::string& sSessionId, std::string& sUserName)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      STAFF_ASSERT_PARAM(sSessionId.size() != 0);

      // get user id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT name FROM users WHERE id=(SELECT userid FROM sessions WHERE sessionid = ?1)", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szUserName = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          STAFF_ASSERT(szUserName != NULL, "Can't get username");
          sUserName = szUserName;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }

    bool Sessions::GetUserDescription(const std::string& sSessionId, std::string& sUserDescription)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      STAFF_ASSERT_PARAM(sSessionId.size() != 0);

      // get user id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT description FROM users WHERE id=(SELECT userid FROM sessions WHERE sessionid = ?1)", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szUserDescription = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          STAFF_ASSERT(szUserDescription != NULL, "Can't get user description");
          sUserDescription = szUserDescription;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }


    bool Sessions::GetSessionIdByUserName(const std::string& sUserName, std::string& sSessionId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      STAFF_ASSERT_PARAM(sUserName.size() != 0);

      // get session id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT sessionid FROM sessions WHERE userid=(SELECT id FROM users WHERE name = ?1)", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          STAFF_ASSERT(szSessionId != NULL, "Can't get session id");
          sSessionId = szSessionId;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }

    bool Sessions::GetSessionIdByUserNameAndPassword(const std::string& sUserName, const std::string& sPassword, std::string& sSessionId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      STAFF_ASSERT_PARAM(sUserName.size() != 0);

      // get session id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT sessionid FROM sessions WHERE userid=(SELECT id FROM users WHERE name = ?1 AND password = ?2)", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          STAFF_ASSERT(szSessionId != NULL, "Can't get session id");
          sSessionId = szSessionId;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }


    void Sessions::CloseExpiredSessions()
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM sessions WHERE expires < ?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, Time::Get());
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to close expired sessions: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    Sessions::Sessions():
      m_nSessionExpiration(600)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT value FROM settings WHERE name='sessionexpiration'", -1, &pVm, NULL);
      if (nResult == SQLITE_OK && sqlite3_step(pVm) == SQLITE_ROW)
      {
        m_nSessionExpiration = sqlite3_column_int(pVm, 0);
      }
      else
      {
        LogWarning() << "Can't get session expiration from settings, using defaults(600s): " << sqlite3_errmsg(pDb);
      }

      sqlite3_finalize(pVm);
    }

    Sessions::~Sessions()
    {
    }

    const std::string Sessions::sNobodySessionId = STAFF_SECURITY_NOBODY_SESSION_ID;
  }
}
