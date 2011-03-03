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

#ifndef _CODEGENPARSER_H_
#define _CODEGENPARSER_H_

#include <string>
#include <iosfwd>
#include "Interface.h"

namespace staff
{
namespace codegen
{
  //! source code parse settings
  struct SParseSettings
  {
    std::string    sInDir;          //!<  input dir
    std::string    sOutDir;         //!<  output dir
    TStringList    lsFiles;         //!<  input files
    TStringMap     mEnv;            //!<  environment - arguments passed through -d option
    bool           bNoServiceError; //!<  throw error if no service found

    SParseSettings();
  };

  //! codegen source code parser plugin
  class ICodegenParser
  {
  public:
    virtual ~ICodegenParser();

    //! get parser's id
    /*! \return parser's id
      */
    virtual const std::string& GetId() = 0;

    //! process project
    /*! \param rParseSettings - parse settings
        \param rProject - resulting project
      */
    virtual void Process(const SParseSettings& rParseSettings, SProject& rProject) = 0;
  };

  //! parse exception
  class CParseException
  {
  public:
    CParseException(const std::string& sFile, int nLine, const std::string& sMessage,
                    const std::string& sSourceFile, int nSourceLine);

    std::ostream& operator<<(std::ostream& rStream) const;

    std::string& Message();

  private:
    std::string m_sFile;
    int m_nLine;
    std::string m_sMessage;
    std::string m_sSourceFile;
    int m_nSourceLine;
  };

  std::ostream& operator<<(std::ostream& rStream, const CParseException& rParseException);

#define CSP_THROW(CSP_MESSAGE, CSP_FILE, CSP_LINE)\
  throw ::staff::codegen::CParseException(__FILE__, __LINE__, CSP_MESSAGE, CSP_FILE, CSP_LINE)

#define CSP_ASSERT(CSP_EXPRESSION, CSP_MESSAGE, CSP_FILE, CSP_LINE)\
  if (!(CSP_EXPRESSION)) CSP_THROW(CSP_MESSAGE, CSP_FILE, CSP_LINE)

}
}

#endif // _CODEGENPARSER_H_
