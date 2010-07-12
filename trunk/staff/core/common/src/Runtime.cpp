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
          if (itEnv->second.pEnv != NULL)
          {
            axutil_env_free(itEnv->second.pEnv);
          }
        }
        m_mEnv.clear();
      }
    }
    
  public:
    struct SAxutilEnvCounted
    {
      int nCounter;
      axutil_env_t* pEnv;

      SAxutilEnvCounted():
        nCounter(1), pEnv(NULL)
      {
      }
    };

    typedef std::map<std::string, SAxutilEnvCounted> TAxutilEnvMap;

  public:
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
      rise::LogWarning() << "Creating new env: " << sEnvComponent;
      axutil_env_t* pEnv = axutil_env_create_all((sEnvComponent + ".log").c_str(), AXIS2_LOG_LEVEL_TRACE);
      m_pImpl->m_mEnv[sEnvComponent].pEnv = pEnv;
      return pEnv;
    }

    ++itEnv->second.nCounter;
    return itEnv->second.pEnv;
  }

  void CRuntime::FreeAxis2Env(const std::string& sEnvComponent /*= "staff"*/)
  {
    CRuntimeImpl::TAxutilEnvMap::iterator itEnv = m_pImpl->m_mEnv.find(sEnvComponent);
    if (itEnv != m_pImpl->m_mEnv.end())
    {
      --itEnv->second.nCounter;
      if (itEnv->second.nCounter == 0)
      {
        rise::LogWarning() << "Freeing env: " << sEnvComponent;
        axutil_env_free(itEnv->second.pEnv);
        m_pImpl->m_mEnv.erase(itEnv);
      }
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
