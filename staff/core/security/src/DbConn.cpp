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

#ifdef WIN32
#pragma warning (disable : 4267)
#endif

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <staff/common/Runtime.h>
#include "sqlite3.h"
#include "DbConn.h"

namespace staff
{
  namespace security
  {
    void CDbConn::Open()
    {
      ++m_nCounter;
      if (m_nCounter > 1)
      {
        return;
      }

      RISE_ASSERTES(m_pDb == NULL, rise::CLogicAlreadyExistsException, "Staff database is already opened");
      sqlite3_enable_shared_cache(1);

      // open db
      int nResult = sqlite3_open((CRuntime::Inst().GetStaffHome() + "/db/staff.db").c_str(), &m_pDb);
      RISE_ASSERTES(nResult == SQLITE_OK, rise::CFileOpenException, "Failed to open staff database");

      sqlite3_stmt* pVm = NULL;

      nResult = sqlite3_prepare_v2(m_pDb, "PRAGMA foreign_keys = ON;", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(m_pDb));

      try
      {
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to enable foreign keys: "
                     + std::string(sqlite3_errmsg(m_pDb)));
      }
      catch(...)
      {
        sqlite3_finalize(pVm);
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(m_pDb));
    }

    void CDbConn::Close()
    {
      if (m_nCounter > 1)
      {
        --m_nCounter;
        return;
      }

      RISE_ASSERTES(m_pDb != NULL, rise::CLogicNoItemException, "Staff database is not opened");

      // free all prepared ops
      sqlite3_stmt* pStmt = NULL;
      while ((pStmt = sqlite3_next_stmt(m_pDb, 0)) != NULL)
      {
        sqlite3_finalize(pStmt);
      }

      // close db
      int nResult = sqlite3_close(m_pDb);
      RISE_ASSERTES(nResult == SQLITE_OK, rise::CFileCloseException, "Failed to close staff database");
      m_pDb = NULL;

      --m_nCounter;
    }

    sqlite3* CDbConn::GetDb()
    {
      return m_pDb;
    }

    sqlite3* CDbConn::m_pDb = NULL;
    int CDbConn::m_nCounter = 0;
  }
}
