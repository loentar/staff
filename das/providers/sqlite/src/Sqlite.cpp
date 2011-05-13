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

#include <staff/sqlite3/sqlite3.h>
#include <rise/xml/XMLDocument.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
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

  class SqliteProvider::SqliteImpl
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

  const std::string SqliteProvider::SqliteImpl::m_sName = "staff.das.Sqlite";
  const std::string SqliteProvider::SqliteImpl::m_sDescr = "Sqlite data access provider";


  //  ---------------------------------------------------------------

  class SqliteQueryExecutor: public IQueryExecutor
  {
  public:
    SqliteQueryExecutor(SqliteProvider* pProvider):
      m_pProvider(pProvider), m_pResult(NULL), m_nLastStepStatus(0), m_nFieldsCount(0)
    {
    }

    virtual ~SqliteQueryExecutor()
    {
      Reset();
    }

    virtual void Reset()
    {
      if (m_pResult)
      {
        sqlite3_finalize(m_pResult);
        m_pResult = NULL;
        m_nFieldsCount = 0;
      }
    }

    virtual void Execute(const std::string& sExecute)
    {
      RISE_ASSERTS(m_pProvider != NULL && m_pProvider->m_pImpl->m_pConn != NULL
                   && m_pProvider->m_pImpl->m_pDataSource != NULL, "Not Initialized");

      Reset();

      int nResult = sqlite3_prepare_v2(m_pProvider->m_pImpl->m_pConn, sExecute.c_str(), sExecute.size(), &m_pResult, NULL);
      RISE_ASSERTS(nResult == SQLITE_OK, "error #" + rise::ToStr(nResult) + ": "
                   + std::string(sqlite3_errmsg(m_pProvider->m_pImpl->m_pConn))
                   + "\nWhile executing query: \n----------\n" + sExecute + "\n----------\n");

      m_nLastStepStatus = sqlite3_step(m_pResult);
      if (m_nLastStepStatus == SQLITE_ROW)
      {
        m_nFieldsCount = static_cast<unsigned>(sqlite3_column_count(m_pResult));
      }
    }

    virtual void GetFieldsNames(StringList& rNames)
    {
      RISE_ASSERTS(m_pResult, "Execute was not called");

      if (rNames.size() != m_nFieldsCount)
      {
        rNames.resize(m_nFieldsCount);
      }

      const char* szFieldName = NULL;
      int nField = 0;
      for (StringList::iterator itItem = rNames.begin();
          itItem != rNames.end(); ++itItem, ++nField)
      {
        szFieldName = sqlite3_column_name(m_pResult, nField);
        RISE_ASSERTS(szFieldName, "Error while getting field name");
        *itItem = szFieldName;
      }
    }

    virtual bool GetNextResult(StringList& rResult)
    {
      RISE_ASSERTS(m_pResult, "Execute was not called");

      if (m_nLastStepStatus != SQLITE_ROW)
      {
        return false;
      }

      if (rResult.size() != m_nFieldsCount)
      {
        rResult.resize(m_nFieldsCount);
      }

      int nField = 0;
      const char* szResult = NULL;
      for (StringList::iterator itItem = rResult.begin();
          itItem != rResult.end(); ++itItem, ++nField)
      {
        szResult = reinterpret_cast<const char*>(sqlite3_column_text(m_pResult, nField));
        *itItem = szResult ? szResult : "(NULL)";
      }

      m_nLastStepStatus = sqlite3_step(m_pResult);

      return true;
    }

  private:
    SqliteProvider* m_pProvider;
    sqlite3_stmt* m_pResult;
    int m_nLastStepStatus;
    unsigned m_nFieldsCount;
  };


  SqliteProvider::SqliteProvider()
  {
    m_pImpl = new SqliteImpl;
  }

  SqliteProvider::~SqliteProvider()
  {
    Deinit();
    delete m_pImpl;
  }

  void SqliteProvider::Init(const DataSource& rDataSource)
  {
    m_pImpl->m_pDataSource = &rDataSource;

    const rise::xml::CXMLNode& rConfig = rDataSource.GetProviderConfig();

    // initialize connection
    const rise::xml::CXMLNode& rConnection = rConfig.Subnode("connection");

    m_pImpl->m_sDataBase = rConnection["db"].AsString();

    // replace env variable to full path
    std::string::size_type nPosStart = 0;
    std::string::size_type nPosEnd = 0;
    
    while ((nPosStart = m_pImpl->m_sDataBase.find("$(", nPosEnd)) != std::string::npos)
    {
      nPosEnd = m_pImpl->m_sDataBase.find(')', nPosStart);
      RISE_ASSERTS(nPosEnd != std::string::npos, "Invalid Env var declaration: " + m_pImpl->m_sDataBase);
      const std::string& sVar = m_pImpl->m_sDataBase.substr(nPosStart + 2, nPosEnd - nPosStart - 2);
      const std::string& sValue = staff::CRuntime::Inst().GetEnv(sVar);
      m_pImpl->m_sDataBase.replace(nPosStart, nPosEnd - nPosStart + 1, sValue);
      nPosEnd = nPosStart + sValue.size();
    }

    RISE_ASSERTS(m_pImpl->m_pConn == NULL, "Already connected");
    sqlite3_enable_shared_cache(1);

    // open db
    int nResult = sqlite3_open(m_pImpl->m_sDataBase.c_str(), &m_pImpl->m_pConn);
    RISE_ASSERTS(nResult == SQLITE_OK, "Failed to open database: " + m_pImpl->m_sDataBase);

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

  void SqliteProvider::Deinit()
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
      if (nResult != SQLITE_OK)
      {
        rise::LogWarning() << "Failed to close database";
      }
      m_pImpl->m_pConn = NULL;
    }
  }

  const std::string& SqliteProvider::GetName() const
  {
    return SqliteImpl::m_sName;
  }

  const std::string& SqliteProvider::GetDescr() const
  {
    return SqliteImpl::m_sDescr;
  }

  PQueryExecutor SqliteProvider::GetQueryExecutor()
  {
    return new SqliteQueryExecutor(this);
  }

}
}

