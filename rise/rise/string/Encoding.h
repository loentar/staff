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

#ifndef _ENCODING_H_
#define _ENCODING_H_

#include <rise/os/osdllexport.h>
#include <rise/string/String.h>
#include <map>

namespace rise
{
  //! описание кодировки
  struct RISE_EXPORT SEncodingType
  {
    int       nID;       //!< идентификатор кодировки
    CString   sName;     //!< название кодировки
    CString   sAltName;  //!< название кодировки для XML
    CStringA  asTable;   //!< набор символов в однобайтной кодировке
    CStringW  wsTable;   //!< набор символов многобайтной кодировке
    CStringA  asMagic;   //!< Magic-байты для определения типа кодировки

    //!         конструктор
    /*! \param  nIDInit - идентификатор кодировки
        \param  sNameInit - название кодировки
        \param  sAltNameInit - название кодировки для XML
        \param  asTableInit - набор символов в однобайтной кодировке
        \param  wsTableInit - набор символов многобайтной кодировке
        \param  asMagicInit - Magic-байты для определения типа кодировки
        */
    SEncodingType(int nIDInit, const CString& sNameInit, const CString& sAltNameInit, const CStringA& asTableInit, const CStringW& wsTableInit = L"", const CStringA& asMagicInit = "");
  };

  //!  класс преобразования кодировок
  class RISE_EXPORT CEncoding
  {
  public:
    //! тип кодировки
    enum EType
    {
      ET_UNKNOWN,    //!<  неизвестная
      ET_UTF_8,      //!<  utf-8
      ET_KOI8R,      //!<  koi8-r
      ET_WIN1251,    //!<  windows-1251
      ET_CP866,      //!<  dos
      ET_MACINTOSH,  //!<  macintosh
      ET_ISO_8859_5, //!<  iso8859-5
      ET_LAST,       //!<  последняя используемая кодировка
      ET_USER        //!<  пользовательские кодировки
    };

  public:
    //! тип: ассоциативный список поддерживаемых кодировок
    typedef std::map<int, SEncodingType> TEncMap;

  public:
    //!         Добавить пользовательскую кодировку
    /*! \param  rstEncType - описание кодировки
        */
    static void AddUserEncodingType(const SEncodingType& rstEncType);
    
    //!         получить ассоциативный список поддерживаемых кодировок
    /*! \return ассоциативный список поддерживаемых кодировок
    */
    static const TEncMap& GetEncMap();

    //!         найти описание кодировки по идентификатору
    /*! \param  nID - идентификатор
        \return описание кодировки
        */
    static const SEncodingType& FindEncodingTypeByID(int nID);
    
    //!         найти описание кодировки по имени
    /*! \param  sEncName - имя
        \return описание кодировки
        */
    static const SEncodingType& FindEncodingTypeByName(const CString& sEncName);

    //!         преобразовать строку из одной кодировки в другую
    /*! \param  sFrom - исходная строка
        \param  sTo - результирующая строка
        \param  nIdFrom - кодировка исходной строки
        \param  nIdTo - кодировка результирующей строки
        \return 
        */
    static void Convert(const CString& sFrom, CString& sTo, EType nIdFrom, EType nIdTo);

  private:
    static TEncMap  m_mEncMap; //!<  ассоциативный массив поддерживаемых кодировок
  };
}

#endif // _ENCODING_H_
