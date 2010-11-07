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
#include <rise/xml/XMLDocument.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Value.h>
#include <staff/das/common/DataSource.h>
#include "MySql.h"

namespace staff
{
namespace das
{

  class MySql::MySqlImpl
  {
  public:
    MySqlImpl():
      m_pDataSource(NULL),
      m_sHost("localhost"),
      m_sPort("3306"),
      m_bConnected(false)
    {
    }


  public:
    static const std::string m_sName;
    static const std::string m_sDescr;

    const DataSource* m_pDataSource;

    MYSQL m_tConn;
    std::string m_sHost;
    std::string m_sPort;
    std::string m_sDataBase;
    std::string m_sLogin;
    std::string m_sPassword;
    bool m_bConnected;
  };

  const std::string MySql::MySqlImpl::m_sName = "staff.das.MySql";
  const std::string MySql::MySqlImpl::m_sDescr = "MySql data access provider";


  //  ---------------------------------------------------------------

  MySql::MySql()
  {
    m_pImpl = new MySqlImpl;
  }

  MySql::~MySql()
  {
    delete m_pImpl;
  }

  void MySql::Init(const DataSource& rDataSource)
  {
    m_pImpl->m_pDataSource = &rDataSource;

    const rise::xml::CXMLNode& rConfig = rDataSource.GetProviderConfig();

    // initialize connection
    const rise::xml::CXMLNode& rConnection = rConfig.Subnode("connection");

    m_pImpl->m_sHost = rConnection["host"].AsString();
    m_pImpl->m_sPort = rConnection["port"].AsString();
    m_pImpl->m_sDataBase = rConnection["db"].AsString();
    m_pImpl->m_sLogin = rConnection["login"].AsString();
    m_pImpl->m_sPassword = rConnection["password"].AsString();

    RISE_ASSERTS(!m_pImpl->m_bConnected, "Already connected");
    unsigned short ushPort = 0;
    rise::FromStr(m_pImpl->m_sPort, ushPort);
    mysql_init(&m_pImpl->m_tConn);
    MYSQL* pResult = mysql_real_connect(&m_pImpl->m_tConn,
                           m_pImpl->m_sHost.c_str(), m_pImpl->m_sLogin.c_str(),
                           m_pImpl->m_sPassword.c_str(), m_pImpl->m_sDataBase.c_str(),
                           ushPort, NULL, 0);

    RISE_ASSERTS(pResult, std::string("Failed to connect to db: ") + mysql_error(&m_pImpl->m_tConn));

    m_pImpl->m_bConnected = true;

    int nResult = mysql_set_character_set(&m_pImpl->m_tConn, "UTF8");
    RISE_ASSERTS(nResult == 0, std::string("error setting encoding: ") + mysql_error(&m_pImpl->m_tConn));
  }

  void MySql::Deinit()
  {
    if (m_pImpl->m_bConnected)
    {
      mysql_close(&m_pImpl->m_tConn);
      m_pImpl->m_bConnected = false;
    }
  }

  const std::string& MySql::GetName() const
  {
    return MySqlImpl::m_sName;
  }

  const std::string& MySql::GetDescr() const
  {
    return MySqlImpl::m_sDescr;
  }

  void MySql::Invoke(const std::string& sExecute, const Type& rstReturnType, staff::CDataObject& rdoResult)
  {
    RISE_ASSERTS(m_pImpl->m_bConnected && m_pImpl->m_pDataSource != NULL, "Not Initialized");

    int nStatus = 0;
    MYSQL_ROW pRow;

    nStatus = mysql_query(&m_pImpl->m_tConn, sExecute.c_str());
    RISE_ASSERTS(nStatus == 0, "error executing query #" + rise::ToStr(nStatus) + ": \n"
                + std::string(mysql_error(&m_pImpl->m_tConn))
                + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

    if (mysql_field_count(&m_pImpl->m_tConn) > 0)
    {
      MYSQL_RES* pResult = NULL;
      pResult = mysql_store_result(&m_pImpl->m_tConn);
      RISE_ASSERTS(pResult, "Cannot retreive result: \n" + std::string(mysql_error(&m_pImpl->m_tConn)));

      try
      {
        unsigned nFieldsCount = mysql_num_fields(pResult);
        unsigned long long nRowsCount = mysql_num_rows(pResult);

        if (rstReturnType.eType == Type::Generic)
        {
          RISE_ASSERTS(nRowsCount == 1, "Rows count does not match: " + rise::ToStr(nRowsCount) + " expected: 1");
          RISE_ASSERTS(nFieldsCount == 1, "Fields count does not match: " + rise::ToStr(nFieldsCount) + " expected: 1");
          pRow = mysql_fetch_row(pResult);
          RISE_ASSERTS(pRow, "Error while fetching row: " + std::string(mysql_error(&m_pImpl->m_tConn)));
          const char* szResult = pRow[0];
          RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
          rdoResult.SetText(szResult);
        }
        else
        if (rstReturnType.eType == Type::Struct)
        {
          RISE_ASSERTS(nRowsCount == 1, "Rows count does not match: " + rise::ToStr(nRowsCount) + " expected: 1");
          RISE_ASSERTS(nFieldsCount == static_cast<int>(rstReturnType.lsChilds.size()), "Fields count does not match: " +
              rise::ToStr(nFieldsCount) + " expected: " + rise::ToStr(rstReturnType.lsChilds.size()));

          pRow = mysql_fetch_row(pResult);
          RISE_ASSERTS(pRow, "Error while fetching row");

          int nField = 0;
          for (TypesList::const_iterator itType = rstReturnType.lsChilds.begin();
              itType != rstReturnType.lsChilds.end(); ++itType, ++nField)
          {
            const char* szResult = pRow[nField];
            RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
            rdoResult.CreateChild(itType->sName).SetText(szResult);
          }
        }
        else
        if (rstReturnType.eType == Type::List)
        {
          RISE_ASSERTS(nRowsCount >= 0, "Invalid rows count");

          if (nRowsCount > 0)
          {
            const Type& rItemType = rstReturnType.lsChilds.front();
            if (rItemType.eType == Type::Generic) // list of generics
            {
              RISE_ASSERTS(nFieldsCount == 1, "Fields count does not match: " + rise::ToStr(nFieldsCount)
                           + " expected: 1 (generic)");
              for (int nTuple = 0; nTuple < nRowsCount; ++nTuple)
              {
                pRow = mysql_fetch_row(pResult);
                RISE_ASSERTS(pRow, "Error while fetching row");

                staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
                const char* szResult = pRow[0];
                RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
                tdoItem.SetText(szResult);
              }
            }
            else
            if (rItemType.eType == Type::Struct) // list of structs
            {
              RISE_ASSERTS(nFieldsCount == static_cast<int>(rItemType.lsChilds.size()),
                 "Fields count does not match: " + rise::ToStr(nFieldsCount) + " expected: "
                 + rise::ToStr(rItemType.lsChilds.size()));
              for (int nTuple = 0; nTuple < nRowsCount; ++nTuple)
              {
                pRow = mysql_fetch_row(pResult);
                RISE_ASSERTS(pRow, "Error while fetching row");

                staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
                int nField = 0;

                for (TypesList::const_iterator itType = rItemType.lsChilds.begin();
                    itType != rItemType.lsChilds.end(); ++itType, ++nField)
                {
                  const char* szResult = pRow[nField];
                  RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
                  tdoItem.CreateChild(itType->sName).SetText(szResult);
                }
              }
            }
            else
            if (rItemType.eType == Type::DataObject)
            {
              MYSQL_FIELD* pFields = mysql_fetch_fields(pResult);
              for (int nTuple = 0; nTuple < nRowsCount; ++nTuple)
              {
                pRow = mysql_fetch_row(pResult);
                RISE_ASSERTS(pRow, "Error while fetching row");

                staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
                for (unsigned nField = 0; nField < nFieldsCount; ++nField)
                {
                  const char* szResult = pRow[nField];
                  RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
                  const char* szFieldName = pFields[nField].name;
                  RISE_ASSERTS(szFieldName, "Error while getting field name");

                  tdoItem.CreateChild(szFieldName).SetText(szResult);
                }
              }
            }
          }
        }
        else
        if (rstReturnType.eType == Type::DataObject)
        {
          MYSQL_FIELD* pFields = mysql_fetch_fields(pResult);
          for (int nTuple = 0; nTuple < nRowsCount; ++nTuple)
          {
            pRow = mysql_fetch_row(pResult);
            RISE_ASSERTS(pRow, "Error while fetching row");

            staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
            for (unsigned nField = 0; nField < nFieldsCount; ++nField)
            {
              const char* szResult = pRow[nField];
              RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
              const char* szFieldName = pFields[nField].name;
              RISE_ASSERTS(szFieldName, "Error while getting field name");

              tdoItem.CreateChild(szFieldName).SetText(szResult);
            }
          }
        }
      }
      catch(...)
      {
        mysql_free_result(pResult);
        throw;
      }

      mysql_free_result(pResult);
    }
  }

}
}

