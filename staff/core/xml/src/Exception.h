/*
 *  Copyright 2012 Utkin Dmitry
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

#ifndef _STAFF_XML_EXCEPTION_H_
#define _STAFF_XML_EXCEPTION_H_

#include <staff/utils/Exception.h>
#include <staff/utils/tostring.h>
#include "staffxmlexport.h"

//! throw xml exception
#define STAFF_XML_THROW(DESCRIPTION, XMLFILE, XMLLINE) \
  throw ::staff::xml::XmlException(STAFF_FILE_LINE, __FUNCTION__, DESCRIPTION, XMLFILE, XMLLINE);

//! assert expression
#define STAFF_XML_ASSERT(EXPRESSION, DESCRIPTION, XMLFILE, XMLLINE) \
  if (!(EXPRESSION)) STAFF_XML_THROW(DESCRIPTION, XMLFILE, XMLLINE)

namespace staff
{
namespace xml
{
  //! staff exception
  class XmlException: public ::staff::Exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
        \param  sXmlFile - xml file name where error occured
        \param  nXmlLine - xml file line where error occured
      */
    inline XmlException(const char* szFileLine, const char* szFunction, std::string sDescr,
                        const std::string& sXmlFile, int nXmlLine):
      Exception(szFileLine, szFunction, sDescr.append(sXmlFile).append(":").append(ToString(nXmlLine)))
    {
    }
  };

} // namespace xml
} // namespace staff

#endif // #ifndef _STAFF_XML_EXCEPTION_H_
