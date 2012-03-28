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
#include "DbConn.h"
#include "Users.h"

namespace staff
{
  namespace security
  {
    Users& Users::Inst()
    {
      static Users tInst;
      return tInst;
    }

    void Users::GetById(int nId, User& rstUser)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM users WHERE id=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_ROW, "User with id is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstUser.nId = sqlite3_column_int(pVm, 0);
        const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        STAFF_ASSERT(szTmp, "Failed to get user name");
        rstUser.sName = szTmp;

        szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
        rstUser.sDescription = szTmp != NULL ? szTmp : "";
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Users::GetByName(const std::string& sUserName, User& rstUser)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM users WHERE name=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_ROW, "User with name is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstUser.nId = sqlite3_column_int(pVm, 0);
        const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        STAFF_ASSERT(szTmp, "Failed to get user name");
        rstUser.sName = szTmp;

        szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
        rstUser.sDescription = szTmp != NULL ? szTmp : "";
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Users::GetList(UsersList& rlsUsers)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM users", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      rlsUsers.clear();

      try
      {
        // get data
        while (sqlite3_step(pVm) == SQLITE_ROW)
        {
          User stUser;
          stUser.nId = sqlite3_column_int(pVm, 0);
          const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
          STAFF_ASSERT(szTmp, "Failed to get user name");
          stUser.sName = szTmp;

          szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
          stUser.sDescription = szTmp != NULL ? szTmp : "";
          rlsUsers.push_back(stUser);
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Users::Add(const std::string& sName, const std::string& sPassword, const std::string& sDescription, int& nId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // add user
      int nResult = sqlite3_prepare_v2(pDb, "INSERT INTO users(name, password, description) VALUES(?, ?, ?)", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sName.c_str(), sName.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 3, sDescription.c_str(), sDescription.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to create user: " + std::string(sqlite3_errmsg(pDb)));

        // get inserted user id
        nId = static_cast<int>(sqlite3_last_insert_rowid(pDb));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Users::Remove(int nId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // remove user
      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM users WHERE id=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to remove user: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Users::SetPassword(int nId, const std::string& sPassword)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // set user password
      int nResult = sqlite3_prepare_v2(pDb, "UPDATE users SET password=? WHERE id=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to set user password: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Users::GetPassword(int nId, std::string& sPassword)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // get user password
      int nResult = sqlite3_prepare_v2(pDb, "SELECT password from users WHERE id=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_ROW, "Failed to get user password: " + std::string(sqlite3_errmsg(pDb)));

        const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 0));
        STAFF_ASSERT(szTmp, "Failed to get user password");
        sPassword = szTmp;
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Users::SetDescription(int nId, const std::string& sDescription)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // set user password
      int nResult = sqlite3_prepare_v2(pDb, "UPDATE users SET description=? WHERE id=?", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sDescription.c_str(), sDescription.size(), SQLITE_STATIC);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nId);
        STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to set user description: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    Users::Users()
    {
    }

    Users::~Users()
    {
    }

  }
}
