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
#include <rise/threading/CriticalSection.h>
#include "Runtime.h"

namespace staff
{
  class Runtime::RuntimeImpl
  {
  public:
    RuntimeImpl():
      m_pEnv(axutil_env_create_all("staff.log", GetAxis2LogLevel()))
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

    static axutil_log_levels_t GetAxis2LogLevel()
    {
      static axutil_log_levels_t eLogLevel = static_cast<axutil_log_levels_t>(-1);
      if (eLogLevel == static_cast<axutil_log_levels_t>(-1))
      {
        const char* szAxis2LogEnv = getenv("STAFF_AXIS2C_LOG_LEVEL");
        if (szAxis2LogEnv)
        {
          std::string sAxis2LogEnv(szAxis2LogEnv);
          if (sAxis2LogEnv == "CRITICAL")
          {
            eLogLevel = AXIS2_LOG_LEVEL_CRITICAL;
          }
          else
          if (sAxis2LogEnv == "WARNING")
          {
            eLogLevel = AXIS2_LOG_LEVEL_WARNING;
          }
          else
          if (sAxis2LogEnv == "INFO")
          {
            eLogLevel = AXIS2_LOG_LEVEL_INFO;
          }
          else
          if (sAxis2LogEnv == "DEBUG")
          {
            eLogLevel = AXIS2_LOG_LEVEL_DEBUG;
          }
          else
          if (sAxis2LogEnv == "USER")
          {
            eLogLevel = AXIS2_LOG_LEVEL_USER;
          }
          else
          if (sAxis2LogEnv == "TRACE")
          {
            eLogLevel = AXIS2_LOG_LEVEL_TRACE;
          }
          else
          {
            eLogLevel = AXIS2_LOG_LEVEL_WARNING;
          }
        }
        else
        {
          eLogLevel = AXIS2_LOG_LEVEL_WARNING;
        }
      }
      return eLogLevel;
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
    rise::threading::CCriticalSection m_tLock;
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

    {
      rise::threading::CScopedCriticalSection tScopedLock(m_pImpl->m_tLock);

      RuntimeImpl::AxutilEnvMap::iterator itEnv = m_pImpl->m_mEnv.find(sEnvComponent);
      if (itEnv == m_pImpl->m_mEnv.end())
      {
        axutil_env_t* pEnv = axutil_env_create_all((sEnvComponent + ".log").c_str(),
                                                   RuntimeImpl::GetAxis2LogLevel());
        m_pImpl->m_mEnv[sEnvComponent].pEnv = pEnv;
        return pEnv;
      }

      ++itEnv->second.nCounter;
      return itEnv->second.pEnv;
    }
  }

  void Runtime::FreeAxis2Env(const std::string& sEnvComponent)
  {
    rise::threading::CScopedCriticalSection tScopedLock(m_pImpl->m_tLock);

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
