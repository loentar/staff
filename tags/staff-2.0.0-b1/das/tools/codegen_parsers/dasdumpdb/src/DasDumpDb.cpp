/*
 *  Copyright 2012 Utkin Dmitry
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

#include <memory>
#include <algorithm>
#include <staff/utils/Log.h>
#include <staff/utils/SharedPtr.h>
#include <staff/utils/PluginExport.h>
#include <staff/utils/stringutils.h>
#include <staff/common/Exception.h>
#include <staff/common/DataObject.h>
#include <staff/common/Exception.h>
#include <staff/xml/Element.h>
#include <staff/codegen/tools.h>
#include <staff/das/common/ProviderFactory.h>
#include <staff/das/common/Provider.h>
#include <staff/das/common/Executor.h>
#include "DasDumpDb.h"

STAFF_DECLARE_PLUGIN(staff::codegen::DasDumpDb)

namespace staff
{
namespace codegen
{

  DasDumpDb::DasDumpDb()
  {
  }

  DasDumpDb::~DasDumpDb()
  {
  }


  const std::string& DasDumpDb::GetId()
  {
    return m_sId;
  }

  void DasDumpDb::Process(const ParseSettings& rParseSettings, Project& rProject)
  {
    std::string sRootNs = "::";
    StringMap::const_iterator itRootNs = rParseSettings.mEnv.find("rootns");
    if (itRootNs != rParseSettings.mEnv.end() && !itRootNs->second.empty())
    {
      sRootNs = "::" + itRootNs->second + "::";
      StringReplace(sRootNs, ".", "::", true);
    }

    StringMap::const_iterator itProvider = rParseSettings.mEnv.find("provider");
    STAFF_ASSERT(itProvider != rParseSettings.mEnv.end() && !itProvider->second.empty(),
                 "provider is not set");
    const std::string& sProvider = itProvider->second;

    xml::Element tProviderConfig("provider");
    xml::Element& rNodeConn = tProviderConfig.CreateChildElement("connection");
    for (StringMap::const_iterator itVar = rParseSettings.mEnv.begin();
         itVar != rParseSettings.mEnv.end(); ++itVar)
    {
      if (itVar->first != "rootns" && itVar->first != "provider")
      {
        rNodeConn.CreateChildElement(itVar->first, itVar->second);
      }
    }


    das::PProvider pProvider = das::ProviderFactory::Inst().Allocate(sProvider);

    for (StringList::const_iterator itDbName = rParseSettings.lsFiles.begin();
         itDbName != rParseSettings.lsFiles.end(); ++itDbName)
    {
      Interface stInterface;
      const std::string& sDb = rParseSettings.sInDir + *itDbName;

      rNodeConn.CreateChildElementOnce("db").SetValue(sDb);

      pProvider->Deinit();
      pProvider->Init(tProviderConfig);

      stInterface.sName = *itDbName;
      stInterface.sNamespace = sRootNs;
      std::string::size_type nPos = stInterface.sName.find('.');
      if (nPos != std::string::npos)
      {
        stInterface.sName.erase(nPos);
      }

      das::PExecutor pExecutor = pProvider->GetExecutor();
      STAFF_ASSERT(pExecutor->GetType() == das::IExecutor::TypeQuery, "Executor for provider ["
                   + sProvider + "] is not Query type");
      das::IQueryExecutor* pExec = static_cast<das::IQueryExecutor*>(pExecutor.Get());

      DumpDbModel(sProvider, sRootNs, *pExec, stInterface);

      for (StringMap::const_iterator itVar = rParseSettings.mEnv.begin();
           itVar != rParseSettings.mEnv.end(); ++itVar)
      {
        if (itVar->first != "rootns" && itVar->first != "provider")
        {
          stInterface.mOptions["das-dbconn-" + itVar->first] = itVar->second;
        }
      }
      stInterface.mOptions["das-provider"] = sProvider;
      stInterface.mOptions["das-db"] = sDb;

      rProject.lsInterfaces.push_back(stInterface);
    }
  }

  void DasDumpDb::DumpDbModel(const std::string& sProvider, const std::string& sNamespace,
                              das::IQueryExecutor& rpExecutor, Interface& rstInterface)
  {
    if (sProvider == "staff.das.Sqlite")
    {
      DumpDbModelSqlite(sNamespace, rpExecutor, rstInterface);
    }
    else
    if (sProvider == "staff.das.MySql")
    {
      DumpDbModelMySql(sNamespace, rpExecutor, rstInterface);
    }
    else
    if (sProvider == "staff.das.Postgres")
    {
      DumpDbModelPostgres(sNamespace, rpExecutor, rstInterface);
    }
    else
    {
      STAFF_THROW_ASSERT("Provider [" + sProvider + "] is not supported.");
    }
  }


  void DasDumpDb::DumpDbModelSqlite(const std::string& sNamespace, das::IQueryExecutor& rpExecutor,
                                    Interface& rstInterface)
  {
    StringList lsResult;
    rpExecutor.Execute("SELECT tbl_name, sql FROM sqlite_master "
                       "WHERE type='table' AND tbl_name NOT IN ('sqlite_sequence')");

    while (rpExecutor.GetNextResult(lsResult))
    {
      STAFF_ASSERT(lsResult.size() == 2, "Invalid result");

      std::string sSql = *(++lsResult.begin());

      Struct stStruct;
      stStruct.sName = lsResult.front();
      stStruct.sNamespace = sNamespace;

      std::string::size_type nPos = sSql.find('(');
      std::string::size_type nPos1 = 0;

      STAFF_ASSERT(nPos != std::string::npos, "can't find '('");
      sSql = sSql.substr(nPos + 1);
      sSql.resize(sSql.size() - 1); // remove last ')'
      NormalizeString(sSql);
      StringTrim(sSql);

      std::string::size_type nPosBegin = 0;
      std::string::size_type nPosEnd = 0;
      std::string sCol;
      std::string sColUp;
      Param stParam;

      while (nPosEnd !=  std::string::npos)
      {
        nPosEnd = sSql.find('\n', nPosBegin);
        sCol = sSql.substr(nPosBegin, nPosEnd - nPosBegin);
        nPosBegin = nPosEnd + 1;

        StringTrim(sCol);

        nPos = sCol.find("--");
        if (nPos != std::string::npos)
        {
          stParam.sDescr = sCol.substr(nPos + 2);
          StringTrim(stParam.sDescr);
          nPos1 = sCol.find('\n', nPos);
          if (nPos1 != std::string::npos)
          {
            sCol.erase(nPos, nPos1 - nPos);
          }
          else
          {
            sCol.erase(nPos);
          }
        }

        sColUp = sCol;
        std::transform(sColUp.begin(), sColUp.end(), sColUp.begin(), ::toupper);
        if (sCol.empty() || sColUp.find("FOREIGN KEY") != std::string::npos)
        {
          continue;
        }

        stParam.mOptions["pk"] = (sColUp.find("PRIMARY KEY") != std::string::npos) ? "true" : "false";

        nPos = sCol.find(" ");
        STAFF_ASSERT(nPos != std::string::npos, "failed to get field name [" + sCol + "]");
        stParam.sName = sCol.substr(0, nPos);
        sCol.erase(0, nPos);
        StringTrimLeft(sCol);

        nPos = sCol.find_first_of(" ,(");
        if (nPos != std::string::npos)
        {
          stParam.stDataType.sName = sCol.substr(0, nPos);
        }
        else
        {
          stParam.stDataType.sName = sCol;
        }

        FixSqlType(stParam.stDataType);

        stStruct.lsMembers.push_back(stParam);
      }
      while (nPosEnd != std::string::npos);

      stStruct.bForward = false;

      rstInterface.lsStructs.push_back(stStruct);
    }
  }

  void DasDumpDb::DumpDbModelPostgres(const std::string& sNamespace, das::IQueryExecutor& rpExecutor,
                                      Interface& rstInterface)
  {
    StringList lsTables;

    StringList lsResult;
    rpExecutor.Execute("SELECT relname FROM pg_class "
                       "WHERE relname !~ '^(pg_|sql_)' AND relkind = 'r'");

    while (rpExecutor.GetNextResult(lsResult))
    {
      lsTables.push_back(lsResult.front());
    }

    for (StringList::const_iterator itTable = lsTables.begin(); itTable != lsTables.end(); ++itTable)
    {
      Struct stStruct;
      stStruct.sName = *itTable;
      stStruct.sNamespace = sNamespace;

      rpExecutor.Execute("SELECT a.attname, t.typname FROM pg_class c, pg_attribute a, pg_type t "
                         "WHERE c.relname = '" + *itTable + "' AND a.attnum > 0 "
                         "AND a.attrelid = c.oid AND a.atttypid = t.oid");

      while (rpExecutor.GetNextResult(lsResult))
      {
        Param stParam;
        STAFF_ASSERT(lsResult.size() == 2, "Invalid columns count");
        stParam.sName = lsResult.front();
        stParam.stDataType.sName = *(++lsResult.begin());
        FixSqlType(stParam.stDataType);
        stStruct.lsMembers.push_back(stParam);
      }

      stStruct.bForward = false;

      rstInterface.lsStructs.push_back(stStruct);
    }
  }

  void DasDumpDb::DumpDbModelMySql(const std::string& sNamespace, das::IQueryExecutor& rpExecutor,
                                   Interface& rstInterface)
  {
    StringList lsTables;

    StringList lsResult;
    rpExecutor.Execute("SELECT `table_name` FROM `information_schema`.`tables` "
                       "WHERE `table_schema`='" + rstInterface.sName + "'");

    while (rpExecutor.GetNextResult(lsResult))
    {
      lsTables.push_back(lsResult.front());
    }

    for (StringList::const_iterator itTable = lsTables.begin(); itTable != lsTables.end(); ++itTable)
    {
      Struct stStruct;
      stStruct.sName = *itTable;
      stStruct.sNamespace = sNamespace;

      rpExecutor.Execute("SELECT `column_name`, `data_type` FROM `information_schema`.`columns` "
                         "WHERE `table_name` = '" + *itTable + "' "
                         "AND `table_schema`='" + rstInterface.sName + "'");

      while (rpExecutor.GetNextResult(lsResult))
      {
        Param stParam;
        STAFF_ASSERT(lsResult.size() == 2, "Invalid columns count");
        stParam.sName = lsResult.front();
        stParam.stDataType.sName = *(++lsResult.begin());
        FixSqlType(stParam.stDataType);
        stStruct.lsMembers.push_back(stParam);
      }

      stStruct.bForward = false;

      rstInterface.lsStructs.push_back(stStruct);
    }
  }

  void DasDumpDb::FixSqlType(DataType& rstDataType)
  {
    rstDataType.eType = DataType::TypeGeneric;
    rstDataType.sNamespace.clear();
    std::transform(rstDataType.sName.begin(), rstDataType.sName.end(), rstDataType.sName.begin(), ::tolower);

    if (rstDataType.sName == "tinyint" ||
        rstDataType.sName == "smallint" ||
        rstDataType.sName == "int2")
    {
      rstDataType.sName = "short";
    }
    else
    if (rstDataType.sName == "integer" ||
        rstDataType.sName == "mediumint" ||
        rstDataType.sName == "int4")
    {
      rstDataType.sName = "int";
    }
    else
    if (rstDataType.sName == "bigint" ||
        rstDataType.sName == "big int" ||
        rstDataType.sName == "int8")
    {
      rstDataType.sName = "long";
    }
    else
    if (rstDataType.sName == "real" ||
        rstDataType.sName == "numeric" ||
        rstDataType.sName == "decimal")
    {
      rstDataType.sName = "float";
    }
    else
    if (rstDataType.sName == "money")
    {
      rstDataType.sName = "double";
    }
    else
    if (rstDataType.sName == "boolean")
    {
      rstDataType.sName = "bool";
    }
    else
    if (rstDataType.sName == "blob")
    {
      rstDataType.sName = "Base64Binary";
      rstDataType.sNamespace = "staff::";
    }
    else
    if (rstDataType.sName != "short" &&
        rstDataType.sName != "int" &&
        rstDataType.sName != "long" &&
        rstDataType.sName != "double" &&
        rstDataType.sName != "float")
    {
      rstDataType.sName = "string";
      rstDataType.sNamespace = "std::";
      rstDataType.eType = DataType::TypeString;
    }

  }


  void DasDumpDb::NormalizeString(std::string& sString)
  {
    std::string::size_type nPosBegin = 0;
    std::string::size_type nPosEnd = 0;
    while ((nPosBegin = sString.find_first_of(" \t", nPosEnd)) != std::string::npos)
    {
      if (sString[nPosBegin] == '\t')
      {
        sString[nPosBegin] = ' ';
      }

      ++nPosBegin;
      nPosEnd = sString.find_first_not_of(" \t", nPosBegin);
      if (nPosEnd == std::string::npos)
      {
        break;
      }

      if (nPosEnd != nPosBegin)
      {
        sString.erase(nPosBegin, nPosEnd - nPosBegin);
      }

      nPosEnd = nPosBegin;
    }
  }


  const std::string DasDumpDb::m_sId = "dasdumpdb";
}
}

