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
  class Runtime::RuntimeImpl
  {
  public:
    RuntimeImpl():
      m_pEnv(axutil_env_create_all("staff.log", AXIS2_LOG_LEVEL_TRACE))
    {
    }

    ~RuntimeImpl()
    {
      if (m_mEnv.size() != 0)
      {
        for (AxutilEnvMap::iterator itEnv = m_mEnv.begin(); itEnv != m_mEnv.end(); ++itEnv)
        {
          if (itEnv->second.pEnv)
          {
            axutil_env_free(itEnv->second.pEnv);
          }
        }
        m_mEnv.clear();
      }
      axutil_env_free(m_pEnv);
    }
    
  public:
    struct AxutilEnvCounted
    {
      int nCounter;
      axutil_env_t* pEnv;

      AxutilEnvCounted():
        nCounter(1), pEnv(NULL)
      {
      }
    };

    typedef std::map<std::string, AxutilEnvCounted> AxutilEnvMap;

  public:
    AxutilEnvMap m_mEnv;
    axutil_env_t* m_pEnv;
  };


  Runtime::Runtime():
    m_pImpl(new RuntimeImpl)
  {
  }

  Runtime::~Runtime()
  {
    delete m_pImpl;
  }

  Runtime& Runtime::Inst()
  {
    static Runtime tInst;
    return tInst;
  }

  axutil_env_t* Runtime::GetAxis2Env()
  {
    return m_pImpl->m_pEnv;
  }

  axutil_env_t* Runtime::GetAxis2Env(const std::string& sEnvComponent)
  {
    if (sEnvComponent == "staff")
    {
      return m_pImpl->m_pEnv;
    }

    RuntimeImpl::AxutilEnvMap::iterator itEnv = m_pImpl->m_mEnv.find(sEnvComponent);
    if (itEnv == m_pImpl->m_mEnv.end())
    {
      axutil_env_t* pEnv = axutil_env_create_all((sEnvComponent + ".log").c_str(), AXIS2_LOG_LEVEL_TRACE);
      m_pImpl->m_mEnv[sEnvComponent].pEnv = pEnv;
      return pEnv;
    }

    ++itEnv->second.nCounter;
    return itEnv->second.pEnv;
  }

  void Runtime::FreeAxis2Env(const std::string& sEnvComponent)
  {
    RuntimeImpl::AxutilEnvMap::iterator itEnv = m_pImpl->m_mEnv.find(sEnvComponent);
    if (itEnv != m_pImpl->m_mEnv.end())
    {
      --itEnv->second.nCounter;
      if (itEnv->second.nCounter == 0)
      {
        axutil_env_free(itEnv->second.pEnv);
        m_pImpl->m_mEnv.erase(itEnv);
      }
    }
  }

  std::string Runtime::GetEnv(const std::string& rEnvVariable) const
  {
    const char* szEnv = getenv(rEnvVariable.c_str());
    RISE_ASSERTES(szEnv, rise::CLogicNoItemException,
                  "Environment variable " + rEnvVariable + " not found");
    return szEnv;
  }

  std::string Runtime::GetAxis2Home() const
  {
    return GetEnv("AXIS2C_HOME");
  }

  std::string Runtime::GetStaffHome() const
  {
    return GetEnv("STAFF_HOME");
  }

  std::string Runtime::GetComponentsHome() const
  {
    return GetStaffHome() + RISE_PATH_SEPARATOR + std::string("components");
  }

  std::string Runtime::GetComponentHome(const std::string& sComponent) const
  {
    return GetComponentsHome() + RISE_PATH_SEPARATOR + sComponent;
  }
 
}
