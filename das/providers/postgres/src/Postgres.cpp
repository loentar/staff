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

#include <libpq-fe.h>
#include <rise/xml/XMLDocument.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
#include <rise/string/String.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Value.h>
#include <staff/das/common/DataSource.h>
#include <staff/das/common/Executor.h>
#include "Postgres.h"

namespace staff
{
namespace das
{

  class PostgresProvider::PostgresImpl
  {
  public:
    PostgresImpl():
      m_pConn(NULL),
      m_sHost("localhost"),
      m_sPort("5432"),
      m_sDataBase("template0"),
      m_sLogin("postgres"),
      m_sPassword("postgres")
    {
    }


  public:
    static const std::string m_sName;
    static const std::string m_sDescr;

    PGconn* m_pConn;
    std::string m_sHost;
    std::string m_sPort;
    std::string m_sDataBase;
    std::string m_sLogin;
    std::string m_sPassword;
  };

  const std::string PostgresProvider::PostgresImpl::m_sName = "staff.das.Postgres";
  const std::string PostgresProvider::PostgresImpl::m_sDescr = "Postgres data access provider";

  //  ---------------------------------------------------------------

  class PostgresQueryExecutor: public IQueryExecutor
  {
  public:
    PostgresQueryExecutor(PostgresProvider* pProvider):
      m_pProvider(pProvider), m_pResult(NULL),
      m_nFieldsCount(0), m_nRowsCount(0), m_nCurrentRow(0)
    {
    }

    virtual ~PostgresQueryExecutor()
    {
      Reset();
    }

    virtual void Reset()
    {
      if (m_pResult)
      {
        PQclear(m_pResult);
        m_pResult = NULL;
        m_nFieldsCount = 0;
        m_nRowsCount = 0;
        m_nCurrentRow = 0;
      }
    }

    virtual void Execute(const std::string& sExecute)
    {
      RISE_ASSERTS(m_pProvider != NULL && m_pProvider->m_pImpl->m_pConn != NULL, "Not Initialized");

      Reset();

      m_pResult = PQexec(m_pProvider->m_pImpl->m_pConn, sExecute.c_str());

      ExecStatusType tQueryStatus = PQresultStatus(m_pResult);
      if (tQueryStatus != PGRES_COMMAND_OK)
      {
        RISE_ASSERTS(tQueryStatus == PGRES_TUPLES_OK, "error executing query #" + rise::ToStr(tQueryStatus) + ": \n"
                     + std::string(PQerrorMessage(m_pProvider->m_pImpl->m_pConn))
                     + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

        m_nRowsCount = static_cast<unsigned>(PQntuples(m_pResult));
        m_nFieldsCount = static_cast<unsigned>(PQnfields(m_pResult));
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
        szFieldName = PQfname(m_pResult, nField);
        RISE_ASSERTS(szFieldName, "Error while getting field name");
        *itItem = szFieldName;
      }
    }

    virtual bool GetNextResult(StringList& rResult)
    {
      RISE_ASSERTS(m_pResult, "Execute was not called");

      if (m_nCurrentRow == m_nRowsCount)
      {
        return false;
      }

      if (rResult.size() != m_nFieldsCount)
      {
        rResult.resize(m_nFieldsCount);
      }

      int nField = 0;
      const char* szResult = NULL;
      for (StringList::iterator itResult = rResult.begin();
          itResult != rResult.end(); ++itResult, ++nField)
      {
        szResult = PQgetvalue(m_pResult, m_nCurrentRow, nField);
        *itResult = szResult ? szResult : "(NULL)";
      }

      ++m_nCurrentRow;
      return true;
    }

  private:
    PostgresProvider* m_pProvider;
    PGresult* m_pResult;
    unsigned m_nFieldsCount;
    unsigned m_nRowsCount;
    unsigned m_nCurrentRow;
  };

  PostgresProvider::PostgresProvider()
  {
    m_pImpl = new PostgresImpl;
  }

  PostgresProvider::~PostgresProvider()
  {
    delete m_pImpl;
  }

  void PostgresProvider::Init(const rise::xml::CXMLNode& rConfig)
  {
    // initialize connection
    const rise::xml::CXMLNode& rConnection = rConfig.Subnode("connection");

    m_pImpl->m_sHost = rConnection["host"].AsString();
    m_pImpl->m_sPort = rConnection["port"].AsString();
    m_pImpl->m_sDataBase = rConnection["db"].AsString();
    m_pImpl->m_sLogin = rConnection["login"].AsString();
    m_pImpl->m_sPassword = rConnection["password"].AsString();

    RISE_ASSERTS(!m_pImpl->m_pConn, "Already connected");
    m_pImpl->m_pConn = PQsetdbLogin(m_pImpl->m_sHost.c_str(), m_pImpl->m_sPort.c_str(), "", "",
                                    m_pImpl->m_sDataBase.c_str(), m_pImpl->m_sLogin.c_str(),
                                    m_pImpl->m_sPassword.c_str());

    RISE_ASSERTS(m_pImpl->m_pConn, "Failed to set db login");
    if (PQstatus(m_pImpl->m_pConn) != CONNECTION_OK)
    {
      std::string sError = std::string("Failed to login: ") + PQerrorMessage(m_pImpl->m_pConn);
      PQfinish(m_pImpl->m_pConn);
      m_pImpl->m_pConn = NULL;
      RISE_THROWS(rise::CLogicNoItemException, sError);
    }

    int nResult = PQsetClientEncoding(m_pImpl->m_pConn, "UTF8");
    RISE_ASSERTS(nResult == 0, std::string("error setting encoding: ") + PQerrorMessage(m_pImpl->m_pConn));
  }

  void PostgresProvider::Deinit()
  {
    if (m_pImpl->m_pConn)
    {
      PQfinish(m_pImpl->m_pConn);
      m_pImpl->m_pConn = NULL;
    }
  }

  const std::string& PostgresProvider::GetName() const
  {
    return PostgresImpl::m_sName;
  }

  const std::string& PostgresProvider::GetDescr() const
  {
    return PostgresImpl::m_sDescr;
  }

  PExecutor PostgresProvider::GetExecutor()
  {
    return new PostgresQueryExecutor(this);
  }

}
}
