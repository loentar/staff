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
