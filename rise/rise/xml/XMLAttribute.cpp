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

#include <rise/xml/XMLException.h>
#include "XMLAttribute.h"

namespace rise
{
  namespace xml
  {
    SXMLAttribute::SXMLAttribute()
    {
    }

    SXMLAttribute::SXMLAttribute(const SXMLAttribute& rAttribute):
      sAttrName(rAttribute.sAttrName), 
      sAttrValue(rAttribute.sAttrValue)
    {
    }

    SXMLAttribute::SXMLAttribute(const CString& rAttrName, const CXMLValue& rAttrValue):
      sAttrName(rAttrName), 
      sAttrValue(rAttrValue) 
    {
    }

    SXMLAttribute& SXMLAttribute::operator=(const SXMLAttribute& rAttribute)
    { 
      sAttrName = rAttribute.sAttrName;
      sAttrValue = rAttribute.sAttrValue;  
      return *this;
    }

    bool SXMLAttribute::operator==(const SXMLAttribute& rAttribute) const
    { 
      return sAttrName == rAttribute.sAttrName && sAttrValue == rAttribute.sAttrValue;  
    }

    bool SXMLAttribute::operator!=(const SXMLAttribute& rAttribute) const
    { 
      return !(operator==(rAttribute));
    }


    CXMLOStream& operator<<(CXMLOStream& rStream, const SXMLAttribute& rXMLAttr)
    {
      return rStream << " " << rXMLAttr.sAttrName << "=\"" << rXMLAttr.sAttrValue << "\"";
    }

    CXMLIStream& operator>>(CXMLIStream& rStream, SXMLAttribute& rXMLAttr)
    {
      TChar chTmp = '\0';
      rStream.ReadId(rXMLAttr.sAttrName);
      rStream.SkipWhitespace();
      chTmp = rStream.ReadChar();
      ASSERTXMLS(chTmp == '=', CXMLFormatException, "expected \'=\' while reading attribute \"" + 
        rXMLAttr.sAttrName + "\"", rStream.GetFileName(), rStream.GetLine());

      rStream.SkipWhitespace();
      chTmp = rStream.ReadChar();
      ASSERTXMLS(chTmp == '\"' || chTmp == '\'', CXMLFormatException, 
        "expected quote while reading attribute \"" + 
        rXMLAttr.sAttrName + "\"", rStream.GetFileName(), rStream.GetLine());
      
      rStream.ReadString(rXMLAttr.sAttrValue.AsString(), CString(1, chTmp));
      return rStream;
    }

    RISE_EXPORT COStream& operator<<(COStream& rStream, const SXMLAttribute& rXMLAttr)
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLAttr);
    }

    RISE_EXPORT CIStream& operator>>(CIStream& rStream, SXMLAttribute& rXMLAttr)
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLAttr);
    }

  } // namespace xml
} // namespace rise

