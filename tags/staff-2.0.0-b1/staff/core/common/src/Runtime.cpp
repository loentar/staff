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

#include <axutil_env.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/File.h>
#include "Exception.h"
#include "Runtime.h"

namespace staff
{
  class Runtime::RuntimeImpl
  {
  public:
    RuntimeImpl():
      m_pEnv(CreateAxis2Env("staff.log"))
    {
    }

    ~RuntimeImpl()
    {
      axutil_env_free(m_pEnv);
    }

    static axutil_env_t* CreateAxis2Env(const axis2_char_t* szLogName)
    {
      static axis2_bool_t bEnableLog = !getenv("STAFF_AXIS2C_DISABLE_LOG") ? AXIS2_TRUE : AXIS2_FALSE;

      axutil_env_t* pEnv = axutil_env_create_all(szLogName, GetAxis2LogLevel());
      axutil_env_enable_log(pEnv, bEnableLog);
      return pEnv;
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
    axutil_env_t* m_pEnv;
    std::string m_sAxis2cHome;
    std::string m_sStaffHome;
  };


  Runtime::Runtime():
    m_pImpl(new RuntimeImpl)
  {
    m_pImpl->m_sAxis2cHome = GetEnvOpt("AXIS2C_HOME");
    m_pImpl->m_sStaffHome = GetEnvOpt("STAFF_HOME");

    // for dpkg
#ifndef WIN32
    if (m_pImpl->m_sAxis2cHome.empty())
    {
      m_pImpl->m_sAxis2cHome = "/usr/lib/axis2";
    }

    if (m_pImpl->m_sStaffHome.empty())
    {
      m_pImpl->m_sStaffHome = "/usr/lib/staff";
    }
#endif
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

  axutil_env_t* Runtime::CreateAxis2Env(const char* szLogName)
  {
    return RuntimeImpl::CreateAxis2Env(szLogName);
  }

  axutil_env_t* Runtime::CreateAxis2Env(const std::string& sLogName)
  {
    return RuntimeImpl::CreateAxis2Env(sLogName.c_str());
  }

  void Runtime::FreeAxis2Env(axutil_env_t* pEnv)
  {
    axutil_env_free(pEnv);
  }

  std::string Runtime::GetEnv(const std::string& sVariable) const
  {
    const char* szEnv = getenv(sVariable.c_str());
    STAFF_ASSERT(szEnv, "Environment variable " + sVariable + " not found");
    return szEnv;
  }

  std::string Runtime::GetEnvOpt(const std::string& sVariable) const
  {
    const char* szEnv = getenv(sVariable.c_str());
    return szEnv ? szEnv : "";
  }

  const std::string& Runtime::GetAxis2Home() const
  {
    return m_pImpl->m_sAxis2cHome;
  }

  const std::string& Runtime::GetStaffHome() const
  {
    return m_pImpl->m_sStaffHome;
  }

  std::string Runtime::GetComponentsHome() const
  {
    return m_pImpl->m_sStaffHome + STAFF_PATH_SEPARATOR + std::string("components");
  }

  std::string Runtime::GetComponentHome(const std::string& sComponent) const
  {
    return GetComponentsHome() + STAFF_PATH_SEPARATOR + sComponent;
  }
 
}
