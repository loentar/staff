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
#include <staff/common/Runtime.h>
#include <staff/sqlite3/sqlite3.h>
#include "DbConn.h"

namespace staff
{
  namespace security
  {
    void DbConn::Open()
    {
      ++m_nCounter;
      if (m_nCounter > 1)
      {
        return;
      }

      STAFF_ASSERT(m_pDb == NULL, "Staff database is already opened");
      sqlite3_enable_shared_cache(1);

      // open db
      int nResult = sqlite3_open((Runtime::Inst().GetStaffHome() + "/db/staff.db").c_str(), &m_pDb);
      STAFF_ASSERT(nResult == SQLITE_OK, "Failed to open staff database");

      sqlite3_stmt* pVm = NULL;

      nResult = sqlite3_prepare_v2(m_pDb, "PRAGMA foreign_keys = ON;", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(m_pDb));

      try
      {
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to enable foreign keys: "
                     + std::string(sqlite3_errmsg(m_pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(m_pDb));
    }

    void DbConn::Close()
    {
      if (m_nCounter > 1)
      {
        --m_nCounter;
        return;
      }

      STAFF_ASSERT(m_pDb != NULL, "Staff database is not opened");

      // free all prepared ops
      sqlite3_stmt* pStmt = NULL;
      while ((pStmt = sqlite3_next_stmt(m_pDb, 0)) != NULL)
      {
        sqlite3_finalize(pStmt);
      }

      // close db
      int nResult = sqlite3_close(m_pDb);
      STAFF_ASSERT(nResult == SQLITE_OK, "Failed to close staff database");
      m_pDb = NULL;

      --m_nCounter;
    }

    void DbConn::BeginTransaction()
    {
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(m_pDb, "BEGIN TRANSACTION", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(m_pDb));

      try
      {
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to begin transaction: "
                     + std::string(sqlite3_errmsg(m_pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(m_pDb));
    }

    void DbConn::EndTransaction()
    {
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(m_pDb, "END TRANSACTION", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(m_pDb));

      try
      {
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to commit transaction: "
                     + std::string(sqlite3_errmsg(m_pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(m_pDb));
    }

    void DbConn::RollbackTransaction()
    {
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(m_pDb, "ROLLBACK TRANSACTION", -1, &pVm, NULL);
      STAFF_ASSERT(nResult == SQLITE_OK, sqlite3_errmsg(m_pDb));

      try
      {
        STAFF_ASSERT(sqlite3_step(pVm) == SQLITE_DONE, "Failed to rollback transaction: "
                     + std::string(sqlite3_errmsg(m_pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      STAFF_ASSERT(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(m_pDb));
    }

    sqlite3* DbConn::GetDb()
    {
      return m_pDb;
    }

    sqlite3* DbConn::m_pDb = NULL;
    int DbConn::m_nCounter = 0;
  }
}
