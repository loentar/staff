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
#include <my_global.h>
#endif
#include <mysql.h>
#include <staff/utils/SharedPtr.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/tostring.h>
#include <staff/utils/fromstring.h>
#include <staff/utils/Log.h>
#include <staff/xml/Element.h>
#include <staff/common/Exception.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/das/common/DataSource.h>
#include <staff/das/common/Executor.h>
#include "MySql.h"

namespace staff
{
namespace das
{

  class MySqlProvider::MySqlImpl
  {
  public:
    MySqlImpl():
      m_sHost("localhost"),
      m_sPort("3306"),
      m_bConnected(false)
    {
    }


  public:
    static const std::string m_sName;
    static const std::string m_sDescr;

    MYSQL m_tConn;
    std::string m_sHost;
    std::string m_sPort;
    std::string m_sDataBase;
    std::string m_sLogin;
    std::string m_sPassword;
    bool m_bConnected;
  };

  const std::string MySqlProvider::MySqlImpl::m_sName = "staff.das.MySql";
  const std::string MySqlProvider::MySqlImpl::m_sDescr = "MySql data access provider";


  //  ---------------------------------------------------------------

  class MySqlQueryExecutor: public IQueryExecutor
  {
  public:
    MySqlQueryExecutor(MySqlProvider* pProvider):
      m_pProvider(pProvider), m_pStmt(NULL),
      m_nFieldsCount(0), m_nRowsCount(0), m_nCurrentRow(0)
    {
    }

    virtual ~MySqlQueryExecutor()
    {
      Reset();
    }

    virtual void Reset()
    {
      if (m_pStmt)
      {
        mysql_stmt_close(m_pStmt);
        m_pStmt = NULL;
        m_nFieldsCount = 0;
        m_nRowsCount = 0;
        m_nCurrentRow = 0;
      }
    }

    virtual void Execute(const std::string& sExecute, const StringList& rlsParams)
    {
      STAFF_ASSERT(m_pProvider != NULL && m_pProvider->m_pImpl->m_bConnected, "Not Initialized");

      Reset();

      MYSQL_BIND* paBind = NULL;
      unsigned long* paSizes = NULL;

      m_pStmt = mysql_stmt_init(&m_pProvider->m_pImpl->m_tConn);
      STAFF_ASSERT(m_pStmt, "Can't init STMT: "
                   + std::string(mysql_error(&m_pProvider->m_pImpl->m_tConn))
                   + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

      try
      {
        paBind = reinterpret_cast<MYSQL_BIND*>(malloc(sizeof(MYSQL_BIND) * rlsParams.size()));
        STAFF_ASSERT(paBind, "Memory allocation failed!");
        memset(paBind, 0, sizeof(MYSQL_BIND) * rlsParams.size());

        paSizes = reinterpret_cast<unsigned long*>(malloc(sizeof(unsigned long) * rlsParams.size()));
        STAFF_ASSERT(paSizes, "Memory allocation failed!");
        memset(paSizes, 0, sizeof(unsigned long) * rlsParams.size());

        int nStatus = mysql_stmt_prepare(m_pStmt, sExecute.c_str(), sExecute.size());
        STAFF_ASSERT(nStatus == 0, "Failed to prepare STMT: "
                     + std::string(mysql_stmt_error(m_pStmt))
                     + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

        unsigned long nParamCount = mysql_stmt_param_count(m_pStmt);
        STAFF_ASSERT(nParamCount == rlsParams.size(), "STMT count != params count: "
                     + ToString(nParamCount) + " != " + ToString(rlsParams.size()) );


        int nPos = 0;
        static my_bool bNull = 1;
        static my_bool bNotNull = 0;

        for (StringList::const_iterator itParam = rlsParams.begin();
             itParam != rlsParams.end(); ++itParam, ++nPos)
        {
          MYSQL_BIND* pBind = &paBind[nPos];
          pBind->buffer_type = MYSQL_TYPE_STRING;

          if (*itParam == STAFF_DAS_NULL_VALUE)
          {
            pBind->is_null = &bNull;
          }
          else
          {
            pBind->is_null = &bNotNull;
            pBind->buffer = const_cast<void*>(reinterpret_cast<const void*>(itParam->c_str()));
            pBind->buffer_length = itParam->size();
            paSizes[nPos] = pBind->buffer_length + 1;
            pBind->length = &paSizes[nPos];
          }
        }

        STAFF_ASSERT(mysql_stmt_bind_param(m_pStmt, paBind) == 0,
                     "Failed to bind param: #" + ToString(nStatus) + ": \n"
                     + std::string(mysql_stmt_error(m_pStmt))
                     + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

        nStatus = mysql_stmt_execute(m_pStmt);
        STAFF_ASSERT(nStatus == 0, "error executing query #" + ToString(nStatus) + ": \n"
                    + std::string(mysql_stmt_error(m_pStmt))
                    + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

        free(paBind);
        free(paSizes);
      }
      catch (...)
      {
        mysql_stmt_close(m_pStmt);
        m_pStmt = NULL;
        free(paBind);
        free(paSizes);
        throw;
      }

      int nFieldsCount = mysql_stmt_field_count(m_pStmt);
      if (nFieldsCount > 0)
      {
        my_bool bUpdateMaxLength = 1;
        mysql_stmt_attr_set(m_pStmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bUpdateMaxLength);

        int nRes = mysql_stmt_store_result(m_pStmt);
        STAFF_ASSERT(!nRes, "Can not retrieve result: \n"
                     + std::string(mysql_error(&m_pProvider->m_pImpl->m_tConn)));

        m_nFieldsCount = nFieldsCount;
        m_nRowsCount = mysql_stmt_num_rows(m_pStmt);
      }
    }

    virtual void GetFieldsNames(StringList& rNames)
    {
      if (rNames.size() != m_nFieldsCount)
      {
        rNames.resize(m_nFieldsCount);
      }

      if (m_pStmt)
      {
        MYSQL_FIELD* pFields = m_pStmt->fields;
        const char* szFieldName = NULL;
        int nField = 0;
        for (StringList::iterator itItem = rNames.begin();
            itItem != rNames.end(); ++itItem, ++nField)
        {
          szFieldName = pFields[nField].name;
          STAFF_ASSERT(szFieldName, "Error while getting field name");
          *itItem = szFieldName;
        }
      }
    }

    virtual bool GetNextResult(StringList& rResult)
    {
      if (!m_pStmt || m_nCurrentRow == m_nRowsCount)
      {
        return false;
      }

      if (rResult.size() != m_nFieldsCount)
      {
        rResult.resize(m_nFieldsCount);
      }


      MYSQL_BIND* paBind = reinterpret_cast<MYSQL_BIND*>(malloc(sizeof(MYSQL_BIND) * m_nFieldsCount));
      STAFF_ASSERT(paBind, "Memory allocation failed!");
      memset(paBind, 0, sizeof(MYSQL_BIND) * m_nFieldsCount);

      try
      {
         MYSQL_RES* pMetadata = mysql_stmt_result_metadata(m_pStmt);

        for (unsigned nField = 0; nField < m_nFieldsCount; ++nField)
        {
          unsigned long ulMaxLength = pMetadata->fields[nField].max_length;

          paBind[nField].buffer_type = MYSQL_TYPE_STRING;
          paBind[nField].is_null = &paBind[nField].is_null_value;

          if (ulMaxLength)
          {
            char* szData = reinterpret_cast<char*>(malloc(ulMaxLength));
            STAFF_ASSERT(szData, "Memory allocation failed!");
            memset(szData, 0, ulMaxLength);
            paBind[nField].buffer = szData;
            paBind[nField].buffer_length = ulMaxLength;
          }
        }

        mysql_free_result(pMetadata);

        STAFF_ASSERT(!mysql_stmt_bind_result(m_pStmt, paBind), "Can't bind result: \n"
                     + std::string(mysql_stmt_error(m_pStmt)));

        if (mysql_stmt_fetch(m_pStmt))
        {
          LogWarning() << "can't fetch stmt: " << mysql_stmt_error(m_pStmt);
          Reset();
          return false;
        }

        unsigned nField = 0;
        for (StringList::iterator itResult = rResult.begin();
            itResult != rResult.end(); ++itResult, ++nField)
        {
          if (paBind[nField].is_null_value)
          {
            *itResult = STAFF_DAS_NULL_VALUE;
          }
          else
          {
            STAFF_ASSERT(!mysql_stmt_fetch_column(m_pStmt, &paBind[nField], nField, 0),
                         "Failed to fetch column: " + std::string(mysql_stmt_error(m_pStmt)));

            itResult->assign(static_cast<const char*>(paBind[nField].buffer),
                             paBind[nField].buffer_length);
          }

          free(paBind[nField].buffer);
        }
      }
      catch (...)
      {
        free(paBind);
        throw;
      }

      free(paBind);

      ++m_nCurrentRow;
      return true;
    }

  private:
    MySqlProvider* m_pProvider;
    MYSQL_RES* m_pResult;
    MYSQL_STMT* m_pStmt;
    unsigned m_nFieldsCount;
    unsigned long long m_nRowsCount;
    unsigned long long m_nCurrentRow;
  };


  MySqlProvider::MySqlProvider()
  {
    m_pImpl = new MySqlImpl;
  }

  MySqlProvider::~MySqlProvider()
  {
    delete m_pImpl;
  }

  void MySqlProvider::Init(const xml::Element& rConfig)
  {
    // initialize connection
    const xml::Element& rConnection = rConfig.GetChildElementByName("connection");

    m_pImpl->m_sHost = rConnection.GetChildElementByName("host").GetTextValue();
    m_pImpl->m_sPort = rConnection.GetChildElementByName("port").GetTextValue();
    m_pImpl->m_sDataBase = rConnection.GetChildElementByName("db").GetTextValue();
    m_pImpl->m_sLogin = rConnection.GetChildElementByName("login").GetTextValue();
    m_pImpl->m_sPassword = rConnection.GetChildElementByName("password").GetTextValue();

    STAFF_ASSERT(!m_pImpl->m_bConnected, "Already connected");
    unsigned short ushPort = 0;
    FromString(m_pImpl->m_sPort, ushPort);
    mysql_init(&m_pImpl->m_tConn);
    MYSQL* pResult = mysql_real_connect(&m_pImpl->m_tConn,
                           m_pImpl->m_sHost.c_str(), m_pImpl->m_sLogin.c_str(),
                           m_pImpl->m_sPassword.c_str(), m_pImpl->m_sDataBase.c_str(),
                           ushPort, NULL, 0);

    STAFF_ASSERT(pResult, std::string("Failed to connect to db: ") + mysql_error(&m_pImpl->m_tConn));

    m_pImpl->m_bConnected = true;

    int nResult = mysql_set_character_set(&m_pImpl->m_tConn, "UTF8");
    STAFF_ASSERT(nResult == 0, std::string("error setting encoding: ") + mysql_error(&m_pImpl->m_tConn));
  }

  void MySqlProvider::Deinit()
  {
    if (m_pImpl->m_bConnected)
    {
      mysql_close(&m_pImpl->m_tConn);
      m_pImpl->m_bConnected = false;
    }
  }

  const std::string& MySqlProvider::GetName() const
  {
    return MySqlImpl::m_sName;
  }

  const std::string& MySqlProvider::GetDescr() const
  {
    return MySqlImpl::m_sDescr;
  }

  PExecutor MySqlProvider::GetExecutor()
  {
    return new MySqlQueryExecutor(this);
  }

}
}

