#include <libpq-fe.h>
#include <rise/xml/XMLDocument.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Value.h>
#include <staff/das/common/DataSource.h>
#include "Postgres.h"

namespace staff
{
namespace das
{

  class Postgres::PostgresImpl
  {
  public:
    PostgresImpl():
      m_pDataSource(NULL),
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

    const DataSource* m_pDataSource;

    PGconn* m_pConn;
    std::string m_sHost;
    std::string m_sPort;
    std::string m_sDataBase;
    std::string m_sLogin;
    std::string m_sPassword;
  };

  const std::string Postgres::PostgresImpl::m_sName = "staff.das.Postgres";
  const std::string Postgres::PostgresImpl::m_sDescr = "Postgres data access provider";


  //  ---------------------------------------------------------------

  Postgres::Postgres()
  {
    m_pImpl = new PostgresImpl;
  }

  Postgres::~Postgres()
  {
    delete m_pImpl;
  }

  void Postgres::Init(const DataSource& rDataSource)
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

  void Postgres::Deinit()
  {
    if (m_pImpl->m_pConn)
    {
      PQfinish(m_pImpl->m_pConn);
      m_pImpl->m_pConn = NULL;
    }
  }

  const std::string& Postgres::GetName() const
  {
    return PostgresImpl::m_sName;
  }

  const std::string& Postgres::GetDescr() const
  {
    return PostgresImpl::m_sDescr;
  }

  void Postgres::Invoke(const std::string& sExecute, const Type& rstReturnType, staff::CDataObject& rdoResult)
  {
    RISE_ASSERTS(m_pImpl->m_pConn != NULL && m_pImpl->m_pDataSource != NULL, "Not Initialized");

    ExecStatusType tQueryStatus;
    PGresult* pPgResult = NULL;

    pPgResult = PQexec(m_pImpl->m_pConn, sExecute.c_str());
    try
    {
      tQueryStatus = PQresultStatus(pPgResult);
      if (tQueryStatus != PGRES_COMMAND_OK)
      {
        RISE_ASSERTS(tQueryStatus == PGRES_TUPLES_OK, "error executing query #" + rise::ToStr(tQueryStatus) + ": \n"
                     + std::string(PQerrorMessage(m_pImpl->m_pConn))
                     + "\nQuery was:\n----------\n" + sExecute + "\n----------\n");

        int nTuplesCount = PQntuples(pPgResult);
        int nFieldsCount = PQnfields(pPgResult);

        if (rstReturnType.eType == Type::Generic)
        {
          RISE_ASSERTS(nTuplesCount == 1, "Tuples count does not match: " + rise::ToStr(nTuplesCount) + " expected: 1");
          RISE_ASSERTS(nFieldsCount == 1, "Fields count does not match: " + rise::ToStr(nFieldsCount) + " expected: 1");
          const char* szResult = PQgetvalue(pPgResult, 0, 0);
          RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
          rdoResult.SetText(szResult);
        }
        else
        if (rstReturnType.eType == Type::Struct)
        {
          RISE_ASSERTS(nTuplesCount == 1, "Tuples count does not match: " + rise::ToStr(nTuplesCount) + " expected: 1");
          RISE_ASSERTS(nFieldsCount == static_cast<int>(rstReturnType.lsChilds.size()), "Fields count does not match: " +
              rise::ToStr(nFieldsCount) + " expected: " + rise::ToStr(rstReturnType.lsChilds.size()));

          int nField = 0;
          for (TypesList::const_iterator itType = rstReturnType.lsChilds.begin();
              itType != rstReturnType.lsChilds.end(); ++itType, ++nField)
          {
            const char* szResult = PQgetvalue(pPgResult, 0, nField);
            RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
            rdoResult.CreateChild(itType->sName).SetText(szResult);
          }
        }
        else
        if (rstReturnType.eType == Type::List)
        {
          RISE_ASSERTS(nTuplesCount >= 0, "Invalid tuples count");

          if (nTuplesCount > 0)
          {
            const Type& rItemType = rstReturnType.lsChilds.front();
            if (rItemType.eType == Type::Generic) // list of generics
            {
              RISE_ASSERTS(nFieldsCount == 1, "Fields count does not match: " + rise::ToStr(nFieldsCount)
                           + " expected: 1 (generic)");
              for (int nTuple = 0; nTuple < nTuplesCount; ++nTuple)
              {
                staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
                const char* szResult = PQgetvalue(pPgResult, nTuple, 0);
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
              for (int nTuple = 0; nTuple < nTuplesCount; ++nTuple)
              {
                staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
                int nField = 0;

                for (TypesList::const_iterator itType = rItemType.lsChilds.begin();
                    itType != rItemType.lsChilds.end(); ++itType, ++nField)
                {
                  const char* szResult = PQgetvalue(pPgResult, nTuple, nField);
                  RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
                  tdoItem.CreateChild(itType->sName).SetText(szResult);
                }
              }
            }
            else
            if (rItemType.eType == Type::DataObject)
            {
              for (int nTuple = 0; nTuple < nTuplesCount; ++nTuple)
              {
                staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
                for (int nField = 0; nField < nFieldsCount; ++nField)
                {
                  const char* szResult = PQgetvalue(pPgResult, nTuple, nField);
                  RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
                  const char* szFieldName = PQfname(pPgResult, nField);
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
          for (int nTuple = 0; nTuple < nTuplesCount; ++nTuple)
          {
            staff::CDataObject tdoItem = rdoResult.CreateChild("Item");
            for (int nField = 0; nField < nFieldsCount; ++nField)
            {
              const char* szResult = PQgetvalue(pPgResult, nTuple, nField);
              RISE_ASSERTS(szResult, "Error while getting result value (result is NULL)");
              const char* szFieldName = PQfname(pPgResult, nField);
              RISE_ASSERTS(szFieldName, "Error while getting field name");

              tdoItem.CreateChild(szFieldName).SetText(szResult);
            }
          }
        }
      }
    }
    catch(...)
    {
      PQclear(pPgResult);
      throw;
    }

    PQclear(pPgResult);
  }

}
}

