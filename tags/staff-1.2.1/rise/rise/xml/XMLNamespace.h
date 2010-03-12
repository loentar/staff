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
    //! xml namespace
    struct RISE_EXPORT SXMLNamespace
    {
      CString  sNs;    //!< namespace
      CString  sUri;   //!< URI

      //!         constructor
      SXMLNamespace();

      //!         copying constructor
      SXMLNamespace(const SXMLNamespace& rNamespace);

      //!         initializing
      /*! \param  sNsInit - namespace
          \param  sUriInit - URI
          \return 
          */
      SXMLNamespace(const CString& sNsInit, const CString& sUriInit);

      //!         copy operator
      /*! \param  rNamespace - source namespace
          \return *this
          */
      SXMLNamespace& operator=(const SXMLNamespace& rNamespace);

      //!         check namespaces for equality
      /*! \param  rNamespace - other namespace
          \return true, if namespaces are equal
          */
      bool operator==(const SXMLNamespace& rNamespace) const;

      //!         check namespaces for inequality
      /*! \param  rNamespace - other namespace
          \return true, if namespaces are inequal
          */
      bool operator!=(const SXMLNamespace& rNamespace) const;
    };

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLNs - reference to namespace
        \return  serializing stream
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const SXMLNamespace& rXMLNs);

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLNs - reference to namespace
        \return  serializing stream
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const SXMLNamespace& rXMLNs);

  } // namespace xml
} // namespace rise

#endif // _XMLNAMESPACE_H_
