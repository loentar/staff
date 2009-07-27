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
    //! атрибут
    struct RISE_EXPORT SXMLAttribute
    {
      CString sAttrName;  //! имя атрибута
      CXMLValue sAttrValue; //! значение атрибута

      //!        конструктор
      SXMLAttribute();

      //!        копирующий конструктор
      SXMLAttribute(const SXMLAttribute& rAttribute);

      //!        инициализирующий конструктор
      SXMLAttribute(const CString& rAttrName, const CXMLValue& rAttrValue);

      //!        оператор копирования
      /*
         \param  rAttribute - другой атрибут
         \return ссылка на текущий объект
      */
      SXMLAttribute& operator=(const SXMLAttribute& rAttribute);

      //!        оператор проверки на равенство
      /*
         \param  rAttribute - другой атрибут
         \return true, если атрибуты эквивалентны
      */
      bool operator==(const SXMLAttribute& rAttribute) const;

      //!        оператор проверки на неравенство
      /*
         \param  rAttribute - другой атрибут
         \return true, если атрибуты не эквивалентны
      */
      bool operator!=(const SXMLAttribute& rAttribute) const;
    };

    //!        оператор сериализации
    /*
       CLogicFormatException
       \param   rStream - поток сериализации
       \param   rXMLAttr - ссылка на атрибут
       \return результирующий поток сериализации
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const SXMLAttribute& rXMLAttr);

    //!        оператор десериализации
    /*
       CLogicFormatException
       \param   rStream - поток десериализации
       \param   rXMLAttr - ссылка на атрибут
       \return результирующий поток десериализации
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, SXMLAttribute& rXMLAttr);

    //!        оператор сериализации
    /*
       CLogicFormatException
       \param   rStream - поток сериализации
       \param   rXMLAttr - ссылка на атрибут
       \return результирующий поток сериализации
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const SXMLAttribute& rXMLAttr);

    //!        оператор десериализации
    /*
       CLogicFormatException
       \param   rStream - поток десериализации
       \param   rXMLAttr - ссылка на атрибут
       \return результирующий поток десериализации
    */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, SXMLAttribute& rXMLAttr);


  } // namespace xml
} // namespace rise

#endif // _XMLATTRIBUTE_H_

