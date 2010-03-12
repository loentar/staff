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
    rise::CString sTmpData;
    rBuffer >> sTmpData;
    rDataObject.FromString(sTmpData);

    return rBuffer;
  }
}

#endif // STAFF_NO_DEPRECATED
