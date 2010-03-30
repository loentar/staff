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

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/Log.h>
#include "sqlite3.h"
#include "DbConn.h"
#include "UsersToGroups.h"

namespace staff
{
  namespace security
  {
    CUsersToGroups& CUsersToGroups::Inst()
    {
      if (!m_pInst)
      {
        m_pInst = new CUsersToGroups;
      }

      return *m_pInst;
    }

    void CUsersToGroups::FreeInst()
    {
      if (m_pInst)
      {
        delete m_pInst;
        m_pInst = NULL;
      }
    }

    void CUsersToGroups::GetUserGroups(int nUserId, TIntList& rlsGroups)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT groupid FROM users_to_groups WHERE userid=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      rlsGroups.clear();

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nUserId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        while (sqlite3_step(pVm) == SQLITE_ROW)
        {
          rlsGroups.push_back(sqlite3_column_int(pVm, 0));
        }
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsersToGroups::GetGroupUsers(int nGroupId, TIntList& rlsUsers)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT userid FROM users_to_groups WHERE groupid=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      rlsUsers.clear();

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nGroupId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        while (sqlite3_step(pVm) == SQLITE_ROW)
        {
          rlsUsers.push_back(sqlite3_column_int(pVm, 0));
        }
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsersToGroups::AddUserToGroup(int nUserId, int nGroupId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "INSERT INTO users_to_groups(userid, groupid) VALUES(?, ?)", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nUserId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nGroupId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, sqlite3_errmsg(pDb));
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsersToGroups::RemoveUserFromGroup(int nUserId, int nGroupId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM users_to_groups WHERE userid=? AND groupid=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nUserId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nGroupId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, sqlite3_errmsg(pDb));
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    bool CUsersToGroups::IsUserMemberOfGroup(int nUserId, int nGroupId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;
      bool bResult = false;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT (SELECT id FROM users_to_groups WHERE userid=? AND groupid=?) NOT NULL", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nUserId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nGroupId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, sqlite3_errmsg(pDb));

        // get data
        bResult = sqlite3_column_int(pVm, 0) != 0;
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
      return bResult;
    }

    CUsersToGroups::CUsersToGroups()
    {
    }

    CUsersToGroups::~CUsersToGroups()
    {
    }

    CUsersToGroups* CUsersToGroups::m_pInst = NULL;

  }
}
