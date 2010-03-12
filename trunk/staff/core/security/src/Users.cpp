#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/Log.h>
#include "sqlite3.h"
#include "DbConn.h"
#include "Users.h"

namespace staff
{
  namespace security
  {
    CUsers& CUsers::Inst()
    {
      if (!m_pInst)
      {
        m_pInst = new CUsers;
      }

      return *m_pInst;
    }

    void CUsers::FreeInst()
    {
      if (m_pInst)
      {
        delete m_pInst;
        m_pInst = NULL;
      }
    }

    void CUsers::GetById(int nId, SUser& rstUser)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM users WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "User with id is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstUser.nId = sqlite3_column_int(pVm, 0);
        const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        RISE_ASSERTS(szTmp, "Failed to get user name");
        rstUser.sName = szTmp;

        szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
        rstUser.sDescription = szTmp != NULL ? szTmp : "";
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsers::GetByName(const std::string& sUserName, SUser& rstUser)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM users WHERE name=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sUserName.c_str(), sUserName.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        // get data
        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_ROW, "User with name is not found: " + std::string(sqlite3_errmsg(pDb)));

        rstUser.nId = sqlite3_column_int(pVm, 0);
        const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
        RISE_ASSERTS(szTmp, "Failed to get user name");
        rstUser.sName = szTmp;

        szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
        rstUser.sDescription = szTmp != NULL ? szTmp : "";
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsers::GetList(TUsersList& rlsUsers)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      int nResult = sqlite3_prepare_v2(pDb, "SELECT id, name, description FROM users", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      rlsUsers.clear();

      try
      {
        // get data
        while (sqlite3_step(pVm) == SQLITE_ROW)
        {
          SUser stUser;
          stUser.nId = sqlite3_column_int(pVm, 0);
          const char* szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 1));
          RISE_ASSERTS(szTmp, "Failed to get user name");
          stUser.sName = szTmp;

          szTmp = reinterpret_cast<const char*>(sqlite3_column_text(pVm, 2));
          stUser.sDescription = szTmp != NULL ? szTmp : "";
          rlsUsers.push_back(stUser);
        }
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsers::Add(const std::string& sName, const std::string& sPassword, const std::string& sDescription, int& nId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // add user
      int nResult = sqlite3_prepare_v2(pDb, "INSERT INTO users(name, password, description) VALUES(?, ?, ?)", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sName.c_str(), sName.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 2, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_text(pVm, 3, sDescription.c_str(), sDescription.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to create user: " + std::string(sqlite3_errmsg(pDb)));

        // get inserted user id
        nId = sqlite3_last_insert_rowid(pDb);
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsers::Remove(int nId)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // remove user
      int nResult = sqlite3_prepare_v2(pDb, "DELETE FROM users WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_int(pVm, 1, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to remove user: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsers::SetPassword(int nId, const std::string& sPassword)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // set user password
      int nResult = sqlite3_prepare_v2(pDb, "UPDATE users SET password=? WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sPassword.c_str(), sPassword.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to set user password: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    void CUsers::SetDescription(int nId, const std::string& sDescription)
    {
      sqlite3* pDb = CDbConn::GetDb();
      sqlite3_stmt* pVm = NULL;

      // set user password
      int nResult = sqlite3_prepare_v2(pDb, "UPDATE users SET description=? WHERE id=?", -1, &pVm, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

      try
      {
        nResult = sqlite3_bind_text(pVm, 1, sDescription.c_str(), sDescription.size(), SQLITE_STATIC);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        nResult = sqlite3_bind_int(pVm, 2, nId);
        RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(pDb));

        RISE_ASSERTS(sqlite3_step(pVm) == SQLITE_DONE, "Failed to set user description: " + std::string(sqlite3_errmsg(pDb)));
      }
      catch(...)
      {
        RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
        throw;
      }

      RISE_ASSERTS(sqlite3_finalize(pVm) == SQLITE_OK, sqlite3_errmsg(pDb));
    }

    CUsers::CUsers()
    {
    }

    CUsers::~CUsers()
    {
    }

    CUsers* CUsers::m_pInst = NULL;
  }
}
