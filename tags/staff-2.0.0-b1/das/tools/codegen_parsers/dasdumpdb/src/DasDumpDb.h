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

#ifndef _DASDUMPDB_H_
#define _DASDUMPDB_H_

#include <staff/codegen/Interface.h>
#include <staff/codegen/CodegenParser.h>

namespace staff
{
  class DataObject;

namespace das
{
  class IQueryExecutor;
}

namespace codegen
{
  //!  DasDumpDb
  class DasDumpDb: public ICodegenParser
  {
  public:
    DasDumpDb();
    ~DasDumpDb();

    virtual const std::string& GetId();
    virtual void Process(const ParseSettings& rParseSettings, Project& rProject);

  private:
    void DumpDbModel(const std::string& sProvider, const std::string& sNamespace,
                     das::IQueryExecutor& rpExecutor, Interface& rstInterface);
    void DumpDbModelSqlite(const std::string& sNamespace, staff::das::IQueryExecutor& rpExecutor,
                           Interface& rstInterface);
    void DumpDbModelPostgres(const std::string& sNamespace, das::IQueryExecutor& rpExecutor,
                             Interface& rstInterface);
    void DumpDbModelMySql(const std::string &sNamespace, das::IQueryExecutor &rpExecutor,
                          Interface &rstInterface);

    void FixSqlType(DataType& sType);
    void NormalizeString(std::string& sString);

  private:
    static const std::string m_sId;
  };
}
}

#endif // _DASDUMPDB_H_
