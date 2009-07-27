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

#ifndef _XMLNAMESPACE_H_
#define _XMLNAMESPACE_H_

#include <rise/os/osdllexport.h>
#include <map>
#include <rise/common/containertypes.h>
#include <rise/xml/XMLFileStream.h>
#include <rise/string/String.h>

namespace rise
{
  namespace xml
  {

    //! пространство имен XML
    struct RISE_EXPORT SXMLNamespace
    {
      CString  sNs;    //!< имя пространства имен
      CString  sUri;   //!< URI

      //!         конструктор по умолчанию
      SXMLNamespace();

      //!         копирующий конструктор
      SXMLNamespace(const SXMLNamespace& rNamespace);

      //!         инициализирующий конструктор
      /*! \param  sNsInit - имя пространства имен
          \param  sUriInit - URI
          \return 
          */
      SXMLNamespace(const CString& sNsInit, const CString& sUriInit);

      //!         оператор копирования
      /*! \param  rNamespace - исходный 
          \return 
          */
      SXMLNamespace& operator=(const SXMLNamespace& rNamespace);

      //!         оператор проверки на равенство
      bool operator==(const SXMLNamespace& rNamespace) const;

      //!         оператор проверки на неравенство
      bool operator!=(const SXMLNamespace& rNamespace) const;
    };

    //!         оператор сериализации
    /*! \param  rStream - поток сериализации
        \param  rXMLNs - ссылка на пространство имен
        \return поток сериализации
        */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const SXMLNamespace& rXMLNs);

    //!         оператор сериализации
    /*! \param  rStream - поток сериализации
        \param  rXMLNs - ссылка на пространство имен
        \return поток сериализации
        */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const SXMLNamespace& rXMLNs);

  } // namespace xml
} // namespace rise

#endif // _XMLNAMESPACE_H_
