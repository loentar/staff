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

#ifndef _XMLATTRIBUTE_H_
#define _XMLATTRIBUTE_H_

#include <rise/os/osdllexport.h>
#include <rise/xml/XMLValue.h>
#include <rise/xml/XMLFileStream.h>
#include <rise/string/String.h>

namespace rise
{
  namespace xml
  {
    //! xml-attribute
    struct RISE_EXPORT SXMLAttribute
    {
      CString sAttrName;  //!< attribute name
      CXMLValue sAttrValue; //!< attribute value

      //!         constructor
      SXMLAttribute();

      //!         copy constructor
      /*! \param  rAttribute - source attribute
          */
      SXMLAttribute(const SXMLAttribute& rAttribute);

      //!         initializing constructor
      /*! \param  rAttrName - attribute name
          \param  rAttrValue - attribute value
          */
      SXMLAttribute(const CString& rAttrName, const CXMLValue& rAttrValue);

      //!         copy operator
      /*! \param  rAttribute - source attribute
          \return *this
      */
      SXMLAttribute& operator=(const SXMLAttribute& rAttribute);

      //!         check attributes for equality
      /*! \param  rAttribute - other attribute
          \return true, if attributes are equal
      */
      bool operator==(const SXMLAttribute& rAttribute) const;

      //!         check attributes for inequality
      /*! \param  rAttribute - other attribute
          \return true, if attributes are inequal
      */
      bool operator!=(const SXMLAttribute& rAttribute) const;
    };

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLAttr - reference to attribute
        \return  serializing stream
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const SXMLAttribute& rXMLAttr);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLAttr - reference to attribute
        \return  deserializing stream
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, SXMLAttribute& rXMLAttr);

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLAttr - reference to attribute
        \return  serializing stream
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const SXMLAttribute& rXMLAttr);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLAttr - reference to attribute
        \return  deserializing stream
    */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, SXMLAttribute& rXMLAttr);


  } // namespace xml
} // namespace rise

#endif // _XMLATTRIBUTE_H_

