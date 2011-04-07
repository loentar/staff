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

#include <iostream>
#include <sstream>
#include <string>
#include <rise/common/ExceptionTemplate.h>
#include <rise/xml/XMLAttribute.h>
#include <rise/xml/XMLException.h>
#include "XMLDecl.h"

namespace rise
{
  namespace xml
  {

    CXMLDecl::CXMLDecl():
      m_sVersion("1.0"), m_eStandalone(EXMLSAUndefined)
    {
    }

    CXMLDecl::~CXMLDecl()
    {
    }

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

    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLDecl& rXMLDecl)
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLDecl);
    }

    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLDecl& rXMLDecl)
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLDecl);
    }

  } // namespace xml
} // namespace rise
