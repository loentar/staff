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
#include <rise/common/StreamBuffer.h>
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLNamespace.h>
#include "DataObject.h"
#include "DataObjectHelper.h"
#if defined _DEBUG || defined DEBUG
#include <iostream>
#endif

namespace staff
{

  void CDataObjectHelper::XmlToDataObject( const rise::xml::CXMLNode& rXmlNode, CDataObject& rDataObject )
  {
    rise::COStringStream ssOut;
    ssOut << rXmlNode;
    rDataObject.FromString(ssOut.str());
  }

  void CDataObjectHelper::DataObjectToXml( const CDataObject& rDataObject, rise::xml::CXMLNode& rXmlNode )
  {
    rise::CIStringStream tStream(rDataObject.ToString());
    tStream >> rXmlNode;
  }

#if defined _DEBUG || defined DEBUG
  void CDataObjectHelper::Dump( const CDataObject& rDataObject )
  {
    std::cout << rDataObject.ToString();
  }
#endif

  rise::CStreamBuffer& operator<<( rise::CStreamBuffer& rBuffer, const CDataObject& rDataObject )
  {
    rBuffer << rDataObject.ToString();
    return rBuffer;
  }

  rise::CStreamBuffer& operator>>( rise::CStreamBuffer& rBuffer, CDataObject& rDataObject )
  {
    std::string sTmpData;
    rBuffer >> sTmpData;
    rDataObject.FromString(sTmpData);

    return rBuffer;
  }
}

#endif // STAFF_NO_DEPRECATED
