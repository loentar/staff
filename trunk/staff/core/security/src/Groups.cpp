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

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/Log.h>
#include <staff/sqlite3/sqlite3.h>
#include "DbConn.h"
#include "Groups.h"

namespace staff
{
  namespace security
  {
    Groups& Groups::Inst()
    {
      static Groups tInst;
      return tInst;
    }

    void Groups::GetById(int nId, Group& rstGroup)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM groups WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "Group with id is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstGroup.nId = sqlite3_column_int(pVm, 0);
        const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        RISE_ASSERTS(szTmp, "Failed to get group name");
        rstGroup.sName = szTmp;

        szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
        rstGroup.sDescription = szTmp != NULL ? szTmp : "";
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Groups::GetByName(const std::string& sGroupName, Group& rstGroup)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM groups WHERE name=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sGroupName.c_str(), sGroupName.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "Group with name is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstGroup.nId = sqlite3_column_int(pVm, 0);
        const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        RISE_ASSERTS(szTmp, "Failed to get group name");
        rstGroup.sName = szTmp;

        szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
        rstGroup.sDescription = szTmp != NULL ? szTmp : "";
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Groups::GetList(GroupsList& rlsGroups)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM groups", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      rlsGroups.clear();

      try
      {
        // get data
        while (sqlite3_step(pVm) == SQLITE_ROW)
        {
          Group stGroup;
          stGroup.nId = sqlite3_column_int(pVm, 0);
          const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
          RISE_ASSERTS(szTmp, "Failed to get group name");
          stGroup.sName = szTmp;

          szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
          stGroup.sDescription = szTmp != NULL ? szTmp : "";
          rlsGroups.push_back(stGroup);
        }
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Groups::Add(const std::string& sName, const std::string& sDescription, int& nId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // add group
      int nResult = sqlite3_prepare_v2(pDb, "INSERT INTO groups(name, description) VALUES(?, ?)", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sName.c_str(), sName.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sDescription.c_str(), sDescription.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to create group: " + std::string(sqlite3_errmsg(pDb)));

        // get inserted group id
        nId = static_cast<int>(sqlite3_last_insert_rowid(pDb));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Groups::Remove(int nId)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // remove group
      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM groups WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to remove group: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void Groups::SetDescription(int nId, const std::string& sDescription)
    {
      sqlite3* pDb = DbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // set group password
      int nResult = sqlite3_prepare_v2(pDb, "UPDATE groups SET description=? WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sDescription.c_str(), sDescription.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to set group description: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    Groups::Groups()
    {
    }

    Groups::~Groups()
    {
    }

  }
}
