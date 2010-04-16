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

#ifndef _XMLDecl_h_
#define _XMLDecl_h_

#include <rise/os/osdllexport.h>
#include <iostream>
#include <rise/xml/XMLFileStream.h>

namespace rise
{
  namespace xml
  {
    //!        xml-document declaration
    class RISE_EXPORT CXMLDecl
    {
    public:
      enum EXMLDeclStandAlone //! standalone
      {
        EXMLSAUndefined = -1, //!< not defined
        EXMLSANo,             //!< standalone="no"
        EXMLSAYes             //!< standalone="yes"
      };

      CString              m_sVersion;      //!< xml document version(1.0)
      CString              m_sEncoding;     //!< encoding
      EXMLDeclStandAlone   m_eStandalone;   //!< standalone declaration

    public:
      //!        constructor
      CXMLDecl();

      //!        destructor
      ~CXMLDecl();
    };

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLDecl - reference to declaration
        \return  serializing stream
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLDecl& rXMLDecl);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLDecl - reference to declaration
        \return  deserializing stream
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLDecl& rXMLDecl);

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLDecl - reference to declaration
        \return  serializing stream
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLDecl& rXMLDecl);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLDecl - reference to declaration
        \return  deserializing stream
    */
     RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLDecl& rXMLDecl);

  } // namespace xml
} // namespace rise

#endif // _XMLDecl_h_

