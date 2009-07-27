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

#ifndef _XMLDOCUMENT_H_
#define _XMLDOCUMENT_H_

#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLDecl.h>

namespace rise
{
  namespace xml
  {
    //! класс XML-Документ
    class RISE_EXPORT CXMLDocument
    {
    public:
      //!         загрузка XML-документа из файла
      /*! CXMLOpenException - ошибка открытия файла
          CXMLFormatException - неверный формат файла
          \param  sFileName - имя файла для загрузки
          */
      void LoadFromFile(const CString& sFileName);

      //!         загрузка xml-документа из потока
      /*! \param  rStream - поток
          */
      void LoadFromStream(CIStream& rStream);

      //!         сохранение XML-документа в файл 
      /*! CXMLOpenException - ошибка открытия файла
          \param  sFileName - имя файла для сохранения
          */
      void SaveToFile(const CString& sFileName) const;

      //!         сохранение XML-документа в поток 
      /*! \param  rStream - поток
          */
      void SaveToStream(COStream& rStream) const;

      //!         получение декларации документа
      /*! \return ссылка на декларацию документа
          */
      const CXMLDecl& GetDecl() const;

      //!         получение декларации документа
      /*! \return ссылка на декларацию документа
          */
      CXMLDecl& GetDecl();

      //!         получение корневого узла XML
      /*! \return ссылка на корневой узел XML
          */
      const CXMLNode& GetRoot() const;

      //!         получение корневого узла XML
      /*! \return ссылка на корневой узел XML
          */
      CXMLNode& GetRoot();

    private:
      CXMLDecl m_tDecl;  //!  декларация 
      CXMLNode m_tRoot;  //!  корневой узел
    };

    //!         оператор вывода XML-документа в xml-поток
    /*! \param  rStream - поток вывода
        \param  rXMLDoc - XML-документ
        \return поток вывода
        */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLDocument& rXMLDoc);
    
    //!         оператор ввода XML-документа из xml-потока
    /*! \param  rStream - поток ввода
        \param  rXMLDoc - XML-документ
        \return поток ввода
        */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLDocument& rXMLDoc);

    //!         оператор вывода XML-документа в поток
    /*! \param  rStream - поток вывода
        \param  rXMLDoc - XML-документ
        \return поток вывода
        */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLDocument& rXMLDoc);

    //!         оператор ввода XML-документа из потока
    /*! \param  rStream - поток ввода
        \param  rXMLDoc - XML-документ
        \return поток ввода
        */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLDocument& rXMLDoc);

  } // namespace xml
} // namespace rise


#endif // _XMLDOCUMENT_H_
