#include <axiom.h>
#include <axis2_util.h>
#include <axiom_soap.h>
#include <map>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include "Runtime.h"

namespace staff
{
  class CRuntime::CRuntimeImpl
  {
  public:
    CRuntimeImpl()
    {
    }

    ~CRuntimeImpl()
    {
      if (m_mEnv.size() != 0)
      {
        for (TAxutilEnvMap::iterator itEnv = m_mEnv.begin(); itEnv != m_mEnv.end(); ++itEnv)
        {
          if (itEnv->second != NULL)
          {
            axutil_env_free(itEnv->second);
          }
        }
        m_mEnv.clear();
      }
    }
    
  public:
    typedef std::map<std::string, axutil_env_t*> TAxutilEnvMap;
    TAxutilEnvMap m_mEnv;
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

  axutil_env_t* CRuntime::GetAxis2Env(const std::string& sEnvComponent /*= "staff"*/)
  {
    CRuntimeImpl::TAxutilEnvMap::iterator itEnv = m_pImpl->m_mEnv.find(sEnvComponent);
    if (itEnv == m_pImpl->m_mEnv.end())
    {
      axutil_env_t* pEnv = axutil_env_create_all((sEnvComponent + ".log").c_str(), AXIS2_LOG_LEVEL_TRACE);
      m_pImpl->m_mEnv[sEnvComponent] = pEnv;
      return pEnv;
    }

    return itEnv->second;
  }

  void CRuntime::FreeAxis2Env(const std::string& sEnvComponent /*= "staff"*/)
  {
    CRuntimeImpl::TAxutilEnvMap::iterator itEnv = m_pImpl->m_mEnv.find(sEnvComponent);
    if (itEnv != m_pImpl->m_mEnv.end())
    {
      axutil_env_free(itEnv->second);
      itEnv->second = NULL;
      m_pImpl->m_mEnv.erase(itEnv);
    }
  }

  std::string CRuntime::GetEnv( const std::string& rEnvVariable ) const
  {
    const rise::TChar* szEnv = getenv(rEnvVariable.c_str());
    RISE_ASSERTES(szEnv != NULL, rise::CLogicNoItemException, "Environment variable " + rEnvVariable + " not found");
    return szEnv;
  }

  std::string CRuntime::GetAxis2Home() const
  {
    return GetEnv("AXIS2C_HOME");
  }

  std::string CRuntime::GetStaffHome() const
  {
    return GetEnv("STAFF_HOME");
  }

  std::string CRuntime::GetComponentsHome() const
  {
    return GetStaffHome() + RISE_PATH_SEPARATOR + std::string("components");
  }

  std::string CRuntime::GetComponentHome( const std::string& sComponent ) const
  {
    return GetComponentsHome() + RISE_PATH_SEPARATOR + sComponent;
  }
 
  CRuntime* CRuntime::m_pInst = NULL;
}
