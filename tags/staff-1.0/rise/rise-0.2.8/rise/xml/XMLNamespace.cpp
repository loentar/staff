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

