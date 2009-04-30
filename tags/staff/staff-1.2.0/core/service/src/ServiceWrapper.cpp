#include <staff/security/Security.h>
#include "ServiceWrapper.h"

namespace staff
{
  CServiceWrapper::~CServiceWrapper()
  {
  }

  const rise::CString& CServiceWrapper::GetSessionId() const
  {
    return m_sSessionId;
  }

  void CServiceWrapper::SetSessionId( const rise::CString& sSessionId )
  {
    if(sSessionId == "")
    {
      m_sSessionId = STAFF_SECURITY_GUEST_SESSION_ID;
    } 
    else
    {
      m_sSessionId = sSessionId;
    }
  }
}
