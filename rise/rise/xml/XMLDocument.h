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
  //!        rise xml-DOM classes
  namespace xml
  {
    //! XML-Document
    class RISE_EXPORT CXMLDocument
    {
    public:
      //!         load xml document from file
      /*! \param  sFileName - filename
          */
      void LoadFromFile(const CString& sFileName);

      //!         load xml document from stream
      /*! \param  rStream - stream
          */
      void LoadFromStream(CIStream& rStream);

      //!         save xml document to file
      /*! \param  sFileName - filename
          */
      void SaveToFile(const CString& sFileName) const;

      //!         save xml document to stream
      /*! \param  rStream - stream
          */
      void SaveToStream(COStream& rStream) const;

      //!         get document declaration
      /*! \return document declaration
          */
      const CXMLDecl& GetDecl() const;

      //!         get document declaration
      /*! \return document declaration
          */
      CXMLDecl& GetDecl();

      //!         get root node
      /*! \return root node
          */
      const CXMLNode& GetRoot() const;

      //!         get root node
      /*! \return root node
          */
      CXMLNode& GetRoot();

    private:
      CXMLDecl m_tDecl;  //!< xml-declaration
      CXMLNode m_tRoot;  //!< root node
    };

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLDoc - reference to xml document
        \return  serializing stream
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLDocument& rXMLDoc);
    
    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLDoc - reference to xml document
        \return  deserializing stream
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLDocument& rXMLDoc);

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLDoc - reference to xml document
        \return  serializing stream
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLDocument& rXMLDoc);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLDoc - reference to xml document
        \return  deserializing stream
    */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLDocument& rXMLDoc);

  } // namespace xml
} // namespace rise


#endif // _XMLDOCUMENT_H_
