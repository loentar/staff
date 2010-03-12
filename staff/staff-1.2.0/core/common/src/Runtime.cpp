#include <axiom.h>
#include <axis2_util.h>
#include <axiom_soap.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include "Runtime.h"

namespace staff
{
  class CRuntime::CRuntimeImpl
  {
  public:
    CRuntimeImpl():
      m_pEnv(axutil_env_create_all("staff.log", AXIS2_LOG_LEVEL_TRACE))
    {
    }

    ~CRuntimeImpl()
    {
      if (m_pEnv != NULL)
      {
        axutil_env_free(m_pEnv);
        m_pEnv = NULL;
      }
    }
    
  public:
    axutil_env_t* m_pEnv;
  };


  CRuntime::CRuntime():
    m_pImpl(new CRuntimeImpl)
  {
  }

  CRuntime::~CRuntime()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  CRuntime& CRuntime::Inst()
  {
    if (m_pInst == NULL)
    {
      m_pInst = new CRuntime;
    }

    return *m_pInst;
  }

  axutil_env_t* CRuntime::GetAxis2Env()
  {
    return m_pImpl->m_pEnv;
  }

  rise::CString CRuntime::GetEnv( const rise::CString& rEnvVariable ) const
  {
    const rise::TChar* szEnv = getenv(rEnvVariable.c_str());
    RISE_ASSERTES(szEnv != NULL, rise::CLogicNoItemException, "Environment variable " + rEnvVariable + " not found");
    return szEnv;
  }

  rise::CString CRuntime::GetAxis2Home() const
  {
    return GetEnv("AXIS2C_HOME");
  }

  rise::CString CRuntime::GetStaffHome() const
  {
    return GetEnv("STAFF_HOME");
  }

  rise::CString CRuntime::GetComponentsHome() const
  {
    return GetStaffHome() + "/components";
  }

  rise::CString CRuntime::GetComponentHome( const rise::CString& sComponent ) const
  {
    return GetComponentsHome() + "/" + sComponent;
  }
 
  CRuntime* CRuntime::m_pInst = NULL;
}
