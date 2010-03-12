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
