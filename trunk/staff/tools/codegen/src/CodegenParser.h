#ifndef _CODEGENPARSER_H_
#define _CODEGENPARSER_H_

#include <string>
#include <iosfwd>
#include "Interface.h"

namespace staff
{
  //! source code parse settings
  struct SParseSettings
  {
    std::string    sInDir;          //!<  input dir
    std::string    sOutDir;         //!<  output dir
    TStringList    lsFiles;         //!<  input files
    TStringMap     mEnv;            //!<  environment - arguments passed through -d option
    bool           bNoServiceError; //!<  error if no service found

    SParseSettings():
      bNoServiceError(true)
    {
    }
  };

  //! codegen source code parser plugin
  class ICodegenParser
  {
  public:
    virtual ~ICodegenParser() {}

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
                    const std::string& sSourceFile, int nSourceLine):
      m_sFile(sFile), m_nLine(nLine), m_sMessage(sMessage),
      m_sSourceFile(sSourceFile), m_nSourceLine(nSourceLine)
    {
    }

    std::ostream& operator<<(std::ostream& rStream) const
    {
      return rStream << m_sFile << "[" << m_nLine << "]: " << m_sMessage
              << "\n While parsing " << m_sSourceFile << "[" << m_nSourceLine << "]\n";
    }

    std::string& Message()
    {
      return m_sMessage;
    }

  private:
    std::string m_sFile;
    int m_nLine;
    std::string m_sMessage;
    std::string m_sSourceFile;
    int m_nSourceLine;
  };

  std::ostream& operator<<(std::ostream& rStream, const CParseException& rParseException)
  {
    return rParseException.operator<<(rStream);
  }

#define CSP_THROW(CSP_MESSAGE, CSP_, CSP_LINE)\
  throw ::staff::CParseException(__FILE__, __LINE__, CSP_MESSAGE, CSP_, CSP_LINE)

}

#endif // _CODEGENPARSER_H_
