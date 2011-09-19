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

#include <iostream>
#include "CodegenParser.h"

namespace staff
{
namespace codegen
{
  ParseSettings::ParseSettings():
    bNoServiceWarn(false)
  {
  }

  ICodegenParser::~ICodegenParser()
  {
  }

  ParseException::ParseException(const std::string& sFile, int nLine, const std::string& sMessage,
                    const std::string& sSourceFile, int nSourceLine):
    m_sFile(sFile), m_nLine(nLine), m_sMessage(sMessage),
    m_sSourceFile(sSourceFile), m_nSourceLine(nSourceLine)
  {
  }

  std::ostream& ParseException::operator<<(std::ostream& rStream) const
  {
    return rStream << m_sFile << "[" << m_nLine << "]: " << m_sMessage
            << "\n While parsing " << m_sSourceFile << "[" << m_nSourceLine << "]\n";
  }

  std::string& ParseException::Message()
  {
    return m_sMessage;
  }


  std::ostream& operator<<(std::ostream& rStream, const ParseException& rParseException)
  {
    return rParseException.operator<<(rStream);
  }

}
}
