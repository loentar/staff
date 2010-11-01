#include <sqlite3.h>
#include <rise/xml/XMLDocument.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Value.h>
#include <staff/das/common/DataSource.h>
#include "Sqlite.h"

namespace staff
{
namespace das
{

  class Sqlite::SqliteImpl
  {
  public:
    SqliteImpl():
      m_pDataSource(NULL),
      m_pConn(NULL)
    {
    }


  public:
    static const std::string m_sName;
    static const std::string m_sDescr;

    const DataSource* m_pDataSource;

    sqlite3* m_pConn;
    std::string m_sDataBase;
  };

  const std::string Sqlite::SqliteImpl::m_sName = "staff.das.Sqlite";
  const std::string Sqlite::SqliteImpl::m_sDescr = "Sqlite data access provider";


  //  ---------------------------------------------------------------

  Sqlite::Sqlite()
  {
    m_pImpl = new SqliteImpl;
  }

  Sqlite::~Sqlite()
  {
    delete m_pImpl;
  }

  void Sqlite::Init(const DataSource& rDataSource)
  {
    m_pImpl->m_pDataSource = &rDataSource;

    const rise::xml::CXMLNode& rConfig = rDataSource.GetProviderConfig();

    // initialize connection
    const rise::xml::CXMLNode& rConnection = rConfig.Subnode("connection");

    m_pImpl->m_sDataBase = rConnection["db"].AsString();


    RISE_ASSERTS(m_pImpl->m_pConn == NULL, "Already connected");
    sqlite3_enable_shared_cache(1);

    // open db
    int nResult = sqlite3_open(m_pImpl->m_sDataBase.c_str(), &m_pImpl->m_pConn);
    RISE_ASSERTS(nResult == SQLITE_OK, "Failed to open staff database");

    sqlite3_stmt* pStmt = NULL;

    nResult = sqlite3_prepare_v2(m_pImpl->m_pConn, "PRAGMA foreign_keys = ON;", -1, &pStmt, NULL);
    RISE_ASSERTS(nResult == SQLITE_OK, sqlite3_errmsg(m_pImpl->m_pConn));

    try
    {
      RISE_ASSERTS(sqlite3_step(pStmt) == SQLITE_DONE, "Failed to enable foreign keys: "
                   + std::string(sqlite3_errmsg(m_pImpl->m_pConn)));
    }
    catch(...)
    {
      sqlite3_finalize(pStmt);
      throw;
    }

    RISE_ASSERTS(sqlite3_finalize(pStmt) == SQLITE_OK, sqlite3_errmsg(m_pImpl->m_pConn));
  }

  void Sqlite::Deinit()
  {
    if (m_pImpl->m_pConn)
    {
      // free all prepared ops
      sqlite3_stmt* pStmt = NULL;
      while ((pStmt = sqlite3_next_stmt(m_pImpl->m_pConn, 0)) != NULL)
      {
        sqlite3_finalize(pStmt);
      }

      // close db
      int nResult = sqlite3_close(m_pImpl->m_pConn);
      RISE_ASSERTES(nResult == SQLITE_OK, rise::CFileCloseException, "Failed to close database");
      m_pImpl->m_pConn = NULL;
    }
  }

  const std::string& Sqlite::GetName() const
  {
    return SqliteImpl::m_sName;
  }

  const std::string& Sqlite::GetDescr() const
  {
    return SqliteImpl::m_sDescr;
  }

  void Sqlite::Invoke(const std::string& sExecute, const Type& rstReturnType, staff::CDataObject& rdoResult)
  {
    RISE_ASSERTS(m_pImpl->m_pConn != NULL && m_pImpl->m_pDataSource != NULL, "Not Initialized");

    sqlite3_stmt* pStmt = NULL;
    const char* szResult = NULL;

    int nResult = sqlite3_prepare_v2(m_pImpl->m_pConn, sExecute.c_str(), sExecute.size(), &pStmt, NULL);
    RISE_ASSERTS(nResult == SQLITE_OK, "error executing query #" + rise::ToStr(nResult) + ": \n"
                 + std::string(sqlite3_errmsg(m_pImpl->m_pConn))
                 + "Query was:\n----------\n" + sExecute + "\n----------\n");

    try
    {
      while ((nResult = sqlite3_step(pStmt)) == SQLITE_ROW)
      {
        int nColumnsCount = sqlite3_column_count(pStmt);
        if (rstReturnType.eType == Type::Generic)
        {
          RISE_ASSERTS(nColumnsCount == 1, "Columns count does not match: " + rise::ToStr(nColumnsCount) + " expected: 1");
          szResult = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 0));
          RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
          rdoResult.SetText(szResult);
        }
        else
        if (rstReturnType.eType == Type::Struct)
        {
          RISE_ASSERTS(nColumnsCount == static_cast<int>(rstReturnType.lsChilds.size()), "Columns count does not match: " +
              rise::ToStr(nColumnsCount) + " expected: " + rise::ToStr(rstReturnType.lsChilds.size()));

          int nColumn = 0;
          for (TypesList::const_iterator itType = rstReturnType.lsChilds.begin();
              itType != rstReturnType.lsChilds.end(); ++itType, ++nColumn)
          {
            szResult = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, nColumn));
            RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
            rdoResult.CreateChild(itType->sName).SetText(szResult);
          }
        }
        else
        if (rstReturnType.eType == Type::List)
        {
          const Type& rItemType = rstReturnType.lsChilds.front();
          if (rItemType.eType == Type::Generic) // list of generics
          {
            int nColumnsCount = sqlite3_column_count(pStmt);
            RISE_ASSERTS(nColumnsCount == 1, "Columns count does not match: " + rise::ToStr(nColumnsCount)
                         + " expected: 1 (generic)");

            staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
            szResult = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 0));
            RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
            tdoItem.SetText(szResult);
          }
          else
          if (rItemType.eType == Type::Struct) // list of structs
          {
            RISE_ASSERTS(nColumnsCount == static_cast<int>(rItemType.lsChilds.size()),
               "Columns count does not match: " + rise::ToStr(nColumnsCount) + " expected: "
               + rise::ToStr(rItemType.lsChilds.size()));
            staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
            int nColumn = 0;

            for (TypesList::const_iterator itType = rItemType.lsChilds.begin();
                itType != rItemType.lsChilds.end(); ++itType, ++nColumn)
            {
              szResult = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, nColumn));
              RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
              tdoItem.CreateChild(itType->sName).SetText(szResult);
            }
          }
          else
          if (rItemType.eType == Type::DataObject)
          {
            staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
            for (int nColumn = 0; nColumn < nColumnsCount; ++nColumn)
            {
              szResult = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, nColumn));
              RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
              szResult = sqlite3_column_name(pStmt, nColumn);
              RISE_ASSERTS(szResult, "Error while getting Column name");

              tdoItem.CreateChild(szResult).SetText(szResult);
            }
          }
        }
        else
        if (rstReturnType.eType == Type::DataObject)
        {
          staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
          for (int nColumn = 0; nColumn < nColumnsCount; ++nColumn)
          {
            szResult = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, nColumn));
            RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
            szResult = sqlite3_column_name(pStmt, nColumn);
            RISE_ASSERTS(szResult, "Error while getting Column name");

            tdoItem.CreateChild(szResult).SetText(szResult);
          }
        }
        else
        {
          RISE_ASSERTS(rstReturnType.eType == Type::Void, "Invalid return type: " + rise::ToStr(rstReturnType.eType));
        }
      }

      RISE_ASSERTS(nResult == SQLITE_DONE, "error#" + rise::ToStr(nResult) + " while executing query: \n"
                   + std::string(sqlite3_errmsg(m_pImpl->m_pConn))
                   + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

    }
    catch(...)
    {
      sqlite3_finalize(pStmt);
      throw;
    }

    sqlite3_finalize(pStmt);
  }

}
}

