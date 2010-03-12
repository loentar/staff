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

#ifndef STAFF_NO_DEPRECATED

#ifndef _DATAOBJECTHELPER_H_
#define _DATAOBJECTHELPER_H_

#include "staffcommonexport.h"

namespace rise
{
  class CStreamBuffer;
  namespace xml
  {
    class CXMLNode;
  }
}

namespace staff
{
  class CDataObject;

  //!  DataObject Helper
  class STAFF_COMMON_EXPORT CDataObjectHelper
  {
  public:
    //!         convert rise xml to DataObject
    /*! \param  rXmlNode - rise xml node
        \param  rDataObject - resulting DataObject
        */
    static void XmlToDataObject(const rise::xml::CXMLNode& rXmlNode, CDataObject& rDataObject);

    //!         convert DataObject to rise xml
    /*! \param  rDataObject - DataObject
        \param  rXmlNode - resulting rise xml
        */
    static void DataObjectToXml(const CDataObject& rDataObject, rise::xml::CXMLNode& rXmlNode);

#if defined _DEBUG || defined DEBUG
    //!         dump DataObject
    /*! \param  rDataObject - DataObject
        */
    static void Dump(const CDataObject& rDataObject);
#endif
  };  

  //!         serialize DataObject into buffer
  /*! \param  rBuffer - serialization buffer
      \param  rDataObject - DataObject
      \return serialization buffer
      */
  STAFF_COMMON_EXPORT rise::CStreamBuffer& operator<<(rise::CStreamBuffer& rBuffer, const CDataObject& rDataObject);

  //!         deserialize DataObject from buffer
  /*! \param  rBuffer - deserialization buffer
      \param  rDataObject - DataObject
      \return deserialization buffer
      */
  STAFF_COMMON_EXPORT rise::CStreamBuffer& operator>>(rise::CStreamBuffer& rBuffer, CDataObject& rDataObject);
}

#endif // _DATAOBJECTHELPER_H_

#endif // STAFF_NO_DEPRECATED
