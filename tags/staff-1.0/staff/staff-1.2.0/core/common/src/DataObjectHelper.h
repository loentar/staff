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

  //!  помощник для работы с DataObject
  class STAFF_COMMON_EXPORT CDataObjectHelper
  {
  public:
    //!         преобразовать xml-объект в объект данных
    /*! \param  rXmlNode - ссылка на xml-объект
        \param  rDataObject - объект данных
        */
    static void XmlToDataObject(const rise::xml::CXMLNode& rXmlNode, CDataObject& rDataObject);

    //!         преобразовать объект данных в xml-объект
    /*! \param  rDataObject - объект данных
        \param  rXmlNode - ссылка на xml-объект
        */
    static void DataObjectToXml(const CDataObject& rDataObject, rise::xml::CXMLNode& rXmlNode);

#if defined _DEBUG || defined DEBUG
    //!         вывести отладочную информацию(структуру) об обьекте данных
    /*! \param  rDataObject - объект данных
        */
    static void Dump(const CDataObject& rDataObject);
#endif
  };  

  //!         оператор сериализаиции
  /*! \param  rBuffer - буфер сериализации
      \param  rDataObject - обьект данных для сериализации
      \return буфер сериализации
      */
  STAFF_COMMON_EXPORT rise::CStreamBuffer& operator<<(rise::CStreamBuffer& rBuffer, const CDataObject& rDataObject);

  //!         оператор десериализаиции
  /*! \param  rBuffer - буфер десериализации
      \param  rDataObject - обьект данных для десериализации
      \return буфер сериализации
      */
  STAFF_COMMON_EXPORT rise::CStreamBuffer& operator>>(rise::CStreamBuffer& rBuffer, CDataObject& rDataObject);
}

#endif // _DATAOBJECTHELPER_H_

#endif // STAFF_NO_DEPRECATED
