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
namespace xml
{
  class Element;
}
class DataObject;

namespace codegen
{
  //!  DasParser
  class DasDataSourceParser: public ICodegenParser
  {
  public:
    DasDataSourceParser();
    ~DasDataSourceParser();

    virtual const std::string& GetId();
    virtual void Process(const ParseSettings& rParseSettings, Project& rProject);

  private:
    void ParseProject(xml::Element& rDataSourceNode, Project& rProject,
                      const std::string& sRootNs);
    void ImportInterface(Interface& rInterface, const Interface& rImportedInterface);
    void ParseTypes(const xml::Element& rNodeTypes, Project& rProject, Interface& rInterface,
                    const std::string& sNamespace);
    void ParseDescr(const xml::Element& rNode, std::string& sDescr);
    bool FixDataType(DataType& rDataType, const Interface& rInterface, const std::string& sNamespace);
    void ProcessIncludes(xml::Element& rNode);

  private:
    static const std::string m_sId;
    std::string m_sDir;
  };
}
}

#endif // _DASPARSER_H_
