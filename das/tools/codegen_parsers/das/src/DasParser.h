/*
 *  Copyright 2009 Utkin Dmitry
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

#ifndef _DASPARSER_H_
#define _DASPARSER_H_

#include <staff/codegen/Interface.h>
#include <staff/codegen/CodegenParser.h>

namespace staff
{
  class CDataObject;

namespace codegen
{
  //!  DasParser
  class CDasParser: public ICodegenParser
  {
  public:
    CDasParser();
    ~CDasParser();

    virtual const std::string& GetId();
    virtual void Process(const SParseSettings& rParseSettings, SProject& rProject);

  private:
    void ParseTypes(const CDataObject& rdoTypes, const std::string& sNamespace, SInterface& rInterface);
    void Parse(const CDataObject& rdoInterface, SInterface& rInterface, SProject& rProject);
    bool FixDataType(SDataType& rDataType, const SInterface& rInterface, const std::string& sNamespace);
    void OptimizeCppNs(std::string& sOptimizeNs, const std::string& sCurrentNs);

  private:
    static const std::string m_sId;
  };
}
}

#endif // _DASPARSER_H_
