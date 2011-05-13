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

#if defined WIN32 && !defined __MINGW32__
#pragma warning (disable : 4267)
#endif

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/Log.h>
#include <staff/sqlite3/sqlite3.h>
#include "tools.h"
#include "Time.h"
#include "DbConn.h"
#include "Sessions.h"

namespace staff
{
  namespace security
  {
    CSessions& CSessions::Inst()
    {
      static CSessions tInst;
      return tInst;
    }

    void CSessions::GetById(int nId, SSession& rstSession)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, sessionid, userid, expires FROM sessions "
                                       "WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "Session by id is not found: " + std::string(sqlite3_errmsg(pDb)));

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

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CSessions::GetBySessionId(const std::string& sSessionId, SSession& rstSession)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, sessionid, userid, expires FROM sessions "
                                       "WHERE sessionid=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "Session by id is not found: " + std::string(sqlite3_errmsg(pDb)));

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

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CSessions::GetList(TSessionsList& rlsSessions)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, sessionid, userid, expires FROM sessions", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        rlsSessions.clear();

        // get data
        while (sqlite3_step(pVm) == SQLITE_ROW)
        {
          SSession stSession;
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

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CSessions::Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      int nUserId = -1;

      // get user id
      int nResult = sqlite3_prepare_v2(pDb, "SELECT id FROM users WHERE name=? AND password=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) != SQLITE_DONE, "User with name and password is not found: " + std::string(sqlite3_errmsg(pDb)));

        nUserId = sqlite3_column_int(pVm, 0);
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));

      const char* szRequest = bCloseExisting ? "REPLACE INTO sessions(userid, expires) VALUES(?, ?)"
                                             : "INSERT INTO sessions(userid, expires) VALUES(?, ?)";
      // open session
      nResult = sqlite3_prepare_v2(pDb, szRequest, -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nUserId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, CTime::Get() + m_nSessionExpiration);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));


        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to create session: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));


      // get inserted session id
      nResult = sqlite3_prepare_v2(pDb, "SELECT sessionid FROM sessions WHERE id = last_insert_rowid()", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "Failed to get last created session id: " + std::string(sqlite3_errmsg(pDb)));

        const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
        RISE_ASSERTS(szSessionId != NULL, "session id == NULL: " + std::string(sqlite3_errmsg(pDb)));
        sSessionId = szSessionId;
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CSessions::Close(const std::string& sSessionId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      RISE_ASSERTP(sSessionId.size() != 0);
      RISE_ASSERTS(sSessionId != sNobodySessionId, "closing nobody's session is not allowed");

      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM sessions WHERE sessionid=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to close session: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    bool CSessions::Validate(const std::string& sSessionId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bValid = false;

      RISE_ASSERTP(sSessionId.size() != 0);
//      if (sSessionId == sNobodySessionId)
//      {
//        return true;
//      }

      // validate session
      int nResult = sqlite3_prepare_v2(pDb, "SELECT (expires > ?2) FROM sessions WHERE sessionid = ?1", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, CTime::Get());
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

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

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));

      return bValid;
    }

    void CSessions::Keepalive(const std::string& sSessionId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      if (sSessionId == sNobodySessionId)
      {
        // ignore request to keepalive nobody's session
        return;
      }

      // TODO: optimize
      RISE_ASSERTS(Validate(sSessionId), "Session expired");

      // update session
      int nResult = sqlite3_prepare_v2(pDb,
          "UPDATE sessions SET expires=? WHERE sessionid=?", -1, &pVm, NULL);

      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, CTime::Get() + m_nSessionExpiration);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to keepalive session: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    int CSessions::GetExpiration() const
    {
      return m_nSessionExpiration;
    }

    int CSessions::GetExpiresById(int nId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nExpires = -1;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT expires FROM sessions WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "Session by id is not found: " + std::string(sqlite3_errmsg(pDb)));

        nExpires = sqlite3_column_int(pVm, 0);
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));

      return nExpires;
    }


    bool CSessions::GetUserId(const std::string& sSessionId, int& nUserId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      RISE_ASSERTP(sSessionId.size() != 0);

      // get user id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT userid FROM sessions WHERE sessionid = ?1", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

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

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }

    bool CSessions::GetUserName(const std::string& sSessionId, std::string& sUserName)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      RISE_ASSERTP(sSessionId.size() != 0);

      // get user id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT name FROM users WHERE id=(SELECT userid FROM sessions WHERE sessionid = ?1)", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szUserName = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          RISE_ASSERTS(szUserName != NULL, "Can't get username");
          sUserName = szUserName;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }

    bool CSessions::GetUserDescription(const std::string& sSessionId, std::string& sUserDescription)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      RISE_ASSERTP(sSessionId.size() != 0);

      // get user id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT description FROM users WHERE id=(SELECT userid FROM sessions WHERE sessionid = ?1)", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sSessionId.c_str(), sSessionId.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szUserDescription = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          RISE_ASSERTS(szUserDescription != NULL, "Can't get user description");
          sUserDescription = szUserDescription;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }


    bool CSessions::GetSessionIdByUserName(const std::string& sUserName, std::string& sSessionId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      RISE_ASSERTP(sUserName.size() != 0);

      // get session id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT sessionid FROM sessions WHERE userid=(SELECT id FROM users WHERE name = ?1)", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          RISE_ASSERTS(szSessionId != NULL, "Can't get session id");
          sSessionId = szSessionId;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }

    bool CSessions::GetSessionIdByUserNameAndPassword(const std::string& sUserName, const std::string& sPassword, std::string& sSessionId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      RISE_ASSERTP(sUserName.size() != 0);

      // get session id
      int nResult = sqlite3_prepare_v2(pDb,
          "SELECT sessionid FROM sessions WHERE userid=(SELECT id FROM users WHERE name = ?1 AND password = ?2)", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        if ((sqlite3_step(pVm) == SQLITE_ROW) &&
            (sqlite3_column_type(pVm, 0) != SQLITE_NULL))
        {
          const char* szSessionId = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
          RISE_ASSERTS(szSessionId != NULL, "Can't get session id");
          sSessionId = szSessionId;
          bResult = true;
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }


    void CSessions::CloseExpiredSessions()
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM sessions WHERE expires < ?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, CTime::Get());
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to close expired sessions: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    CSessions::CSessions():
      m_nSessionExpiration(600)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT value FROM settings WHERE name='sessionexpiration'", -1, &pVm, NULL);
      if (nResult == SQLITE_OK && sqlite3_step(pVm) == SQLITE_ROW)
      {
        m_nSessionExpiration = sqlite3_column_int(pVm, 0);
      }
      else
      {
        rise::LogWarning() << "Can't get session expiration from settings, using defaults(600s): " << sqlite3_errmsg(pDb);
      }

      sqlite3_finalize(pVm);
    }

    CSessions::~CSessions()
    {
    }

    const std::string CSessions::sNobodySessionId = STAFF_SECURITY_NOBODY_SESSION_ID;
  }
}
