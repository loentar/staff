/*
 *  Copyright 2011 Utkin Dmitry
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

#ifndef _PROTOBUFPARSER_H_
#define _PROTOBUFPARSER_H_

#include <staff/codegen/Interface.h>
#include <staff/codegen/CodegenParser.h>

namespace staff
{
  class CProtobufParser: public ICodegenParser
  {
  public:
    virtual const std::string& GetId();
    virtual void Process(const SParseSettings& rParseSettings, SProject& rProject);
  private:
    static const std::string m_sId;
  };
}

#endif // _PROTOBUFPARSER_H_
