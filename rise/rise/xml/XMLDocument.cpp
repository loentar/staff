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

#include <rise/common/Log.h>
#include <rise/xml/XMLException.h>
#include <rise/xml/XMLFileStream.h>
#include "XMLDocument.h"

namespace rise
{
  namespace xml
  {
    void CXMLDocument::LoadFromFile(const CString& sFileName)
    {
      CXMLFileIStream fsStream;
      fsStream.Open(sFileName);
      fsStream >> *this;
      fsStream.Close();
    }

    void CXMLDocument::LoadFromStream( CIStream& rStream )
    {
      rStream >> *this;
    }

    void CXMLDocument::SaveToFile(const CString& sFileName) const
    {
      CXMLFileOStream fsStream;
      fsStream.Open(sFileName);
      fsStream << *this;
      fsStream.Close();
    }

    void CXMLDocument::SaveToStream( COStream& rStream ) const
    {
      rStream << *this;
    }

    const CXMLDecl& CXMLDocument::GetDecl() const
    {
      return m_tDecl;
    }

    CXMLDecl& CXMLDocument::GetDecl()
    {
      return m_tDecl;
    }

    const CXMLNode& CXMLDocument::GetRoot() const
    {
      return m_tRoot;
    }

    CXMLNode& CXMLDocument::GetRoot()
    {
      return m_tRoot;
    }

    RISE_EXPORT CXMLOStream& operator<<( CXMLOStream& rStream, const CXMLDocument& rXMLDoc )
    {
      rStream << rXMLDoc.GetDecl();
      rStream << rXMLDoc.GetRoot();
      return rStream;
    }

    RISE_EXPORT CXMLIStream& operator>>( CXMLIStream& rStream, CXMLDocument& rXMLDoc )
    {
      try
      {
        rStream >> rXMLDoc.GetDecl();
      }
      catch (CXMLNoItemException& )
      {
        rise::LogWarning() << "XML declaration not found";
      }

      rXMLDoc.GetRoot().Clear();
      rStream >> rXMLDoc.GetRoot();
      return rStream;
    }

    RISE_EXPORT CIStream& operator>>( CIStream& rStream, CXMLDocument& rXMLDoc )
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLDoc);
    }

    RISE_EXPORT COStream& operator<<( COStream& rStream, const CXMLDocument& rXMLDoc )
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLDoc);
    }

  } // namespace xml
} // namespace rise
