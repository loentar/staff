#include <iostream>
#include <strstream>
#include <string>
#include <rise/common/ExceptionTemplate.h>
#include <rise/xml/XMLAttribute.h>
#include <rise/xml/XMLException.h>
#include "XMLDecl.h"

namespace rise
{
  namespace xml
  {

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLDecl
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLDecl::CXMLDecl():
      m_sVersion("1.0"), m_eStandalone(EXMLSAUndefined)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CXMLDecl
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLDecl::~CXMLDecl()
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       <<
    //    DESCRIPTION:    оператор сериализации
    //    PARAMETRS:      (in/out) rStream - поток сериализации
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLDecl& rXMLDecl)
    {
      rStream << "<?xml";
      rStream << SXMLAttribute("version", rXMLDecl.m_sVersion);
      
      if (rXMLDecl.m_sEncoding != "")
        rStream << SXMLAttribute("encoding", rXMLDecl.m_sEncoding);
      
      if (rXMLDecl.m_eStandalone == CXMLDecl::EXMLSAYes )
        rStream << SXMLAttribute("standalone", "yes");
      else
      if (rXMLDecl.m_eStandalone == CXMLDecl::EXMLSANo)
        rStream << SXMLAttribute("standalone", "no");
      
      rStream << " ?>\n";
      return rStream;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       >>
    //    DESCRIPTION:    оператор десериализации
    //    PARAMETRS:      (in/out) rStream - поток десериализации
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLIStream& operator>>(CXMLIStream& rStream, CXMLDecl& rXMLDecl)
    {
      CString sTmp;
      int nLine = 0;

      rStream.SkipWhitespace();

      rXMLDecl.m_sVersion = "1.0";
      rXMLDecl.m_sEncoding = "";
      rXMLDecl.m_eStandalone = CXMLDecl::EXMLSAUndefined;

      ASSERTXMLS( rStream.Test("<?xml"), CXMLNoItemException, "File does not begins with xml-declaration!", 
        rStream.GetFileName(), rStream.GetLine());

      SXMLAttribute stAttribute;
      for(;;)
      {
        rStream.SkipWhitespace();
        if ( rStream.Test("?>") )
          break;

        rStream >> stAttribute;

        if ( stAttribute.sAttrName == "version" )
           rXMLDecl.m_sVersion = stAttribute.sAttrValue.AsString();
        else
        if ( stAttribute.sAttrName == "encoding" )
          rXMLDecl.m_sEncoding = stAttribute.sAttrValue.AsString();
        else
        if ( stAttribute.sAttrName == "standalone" )
        {
          if ( stAttribute.sAttrValue == "yes" )
            rXMLDecl.m_eStandalone = CXMLDecl::EXMLSAYes;
          else
          if ( stAttribute.sAttrValue == "no" )
            rXMLDecl.m_eStandalone = CXMLDecl::EXMLSANo;
          else
            THROWXML(CXMLFormatException, "Invalid Attribute Value: " + stAttribute.sAttrValue.AsString(), , "", nLine);
        }
        else
          THROWXML(CXMLFormatException, "Invalid Attribute Name: " + stAttribute.sAttrName, , "", nLine);
      }
      return rStream;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       <<
    //    DESCRIPTION:    оператор сериализации
    //    PARAMETRS:      (in/out) rStream - поток сериализации
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLDecl& rXMLDecl)
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLDecl);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       >>
    //    DESCRIPTION:    оператор десериализации
    //    PARAMETRS:      (in/out) rStream - поток десериализации
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     CLogicFormatException - неверный формат файла
    //                    CXMLNoItemException - файл не начинается с декларации XML
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLDecl& rXMLDecl)
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLDecl);
    }



  } // namespace xml
} // namespace rise
