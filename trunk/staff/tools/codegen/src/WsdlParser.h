#ifndef _WSDLPARSER_H_
#define _WSDLPARSER_H_

#include <string>

struct SInterface;

//!  WsdlParser
class CWsdlParser
{
public:
  ~CWsdlParser();

  //!         get parser instance
  /*! \return reference to wsdl parser instance
  */
  static CWsdlParser& Inst();

  //!         parse wsdl file to interface description
  /*! \param  sFileName - wsdl file name
      \param  rInterface - resulting interface
      */
  void Parse(const std::string& sFileName, SInterface& rInterface);

private:
  CWsdlParser();

private:
  static CWsdlParser* m_pInst;
  class CWsdlParserImpl;
  CWsdlParserImpl* m_pImpl;
};

#endif // _WSDLPARSER_H_
