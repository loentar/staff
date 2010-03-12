#include "sqlite3.h"
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <staff/common/Runtime.h>
#include "DbConn.h"

namespace staff
{
  namespace security
  {
    void CDbConn::Open()
    {
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
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(m_pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(m_pDb));
    }

    void CDbConn::Close()
    {
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
    }

    sqlite3* CDbConn::GetDb()
    {
      return m_pDb;
    }

    sqlite3* CDbConn::m_pDb = NULL;
  }
}
