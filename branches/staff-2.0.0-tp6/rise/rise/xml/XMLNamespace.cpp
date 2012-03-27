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

#include <rise/common/exmacros.h>
#include <rise/common/ExceptionTemplate.h>
#include "XMLNamespace.h"

namespace rise
{
  namespace xml
  {
    SXMLNamespace::SXMLNamespace()
    {
    }

    SXMLNamespace::SXMLNamespace( const SXMLNamespace& rNamespace ):
      sNs(rNamespace.sNs), sUri(rNamespace.sUri)
    {
    }

    SXMLNamespace::SXMLNamespace( const CString& sNsInit, const CString& sUriInit ):
      sNs(sNsInit), sUri(sUriInit)
    {

    }

    SXMLNamespace& SXMLNamespace::operator=( const SXMLNamespace& rNamespace )
    {
      sNs = rNamespace.sNs;
      sUri = rNamespace.sUri;
      return *this;
    }

    bool SXMLNamespace::operator==( const SXMLNamespace& rNamespace ) const
    {
      return sNs == rNamespace.sNs && sUri == rNamespace.sUri;
    }

    bool SXMLNamespace::operator!=( const SXMLNamespace& rNamespace ) const
    {
      return !(operator==(rNamespace));
    }

    RISE_EXPORT CXMLOStream& operator<<( CXMLOStream& rStream, const SXMLNamespace& rXMLNs )
    {
      return rStream << " xmlns:" << rXMLNs.sNs << "=\"" << rXMLNs.sUri << "\"";
    }

    RISE_EXPORT COStream& operator<<( COStream& rStream, const SXMLNamespace& rXMLNs )
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLNs);
    }
  } // namespace xml
} // namespace rise

