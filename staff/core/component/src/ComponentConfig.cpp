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
#include <rise/common/Exception.h>
#include <rise/common/exmacros.h>
#include <rise/string/String.h>
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLDocument.h>
#include <staff/common/Runtime.h>
#include "ComponentConfig.h"


namespace staff
{
  class CComponentConfig::CComponentConfigImpl
  {
  public:
    rise::CString m_sComponent;
    rise::CString m_sConfig;
    rise::CString m_sFileName;
    rise::xml::CXMLDocument m_tConfig;
  };

  CComponentConfig::CComponentConfig():
    m_pImpl(new CComponentConfigImpl)
  {
  }

  CComponentConfig::~CComponentConfig()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  const rise::CString& CComponentConfig::GetComponent() const
  {
    return m_pImpl->m_sComponent;
  }

  const rise::CString& CComponentConfig::GetConfig() const
  {
    return m_pImpl->m_sConfig;
  }

  rise::xml::CXMLNode& CComponentConfig::ReloadConfig( bool bCreate /*= false*/ )
  {
    try
    {
      m_pImpl->m_tConfig.LoadFromFile(m_pImpl->m_sFileName);
    }
    catch(...)
    {
      if (bCreate)
      {
        rise::LogDebug() << "Создание новой конфигурации для: " << m_pImpl->m_sComponent << ":" 
              << m_pImpl->m_sConfig << " (при сохранении)";
      }
      else
      {
        throw;
      }
    }

    return m_pImpl->m_tConfig.GetRoot();
  }

  void CComponentConfig::SaveConfig()
  {
    if (m_pImpl->m_tConfig.GetRoot().NodeName() == "")
    {
      m_pImpl->m_tConfig.GetRoot().NodeName() = "Config";
    }

    m_pImpl->m_tConfig.SaveToFile(m_pImpl->m_sFileName);
  }

  rise::xml::CXMLNode& CComponentConfig::Config()
  {
    return m_pImpl->m_tConfig.GetRoot();
  }

  const rise::xml::CXMLNode& CComponentConfig::Config() const
  {
    return m_pImpl->m_tConfig.GetRoot();
  }

  void CComponentConfig::Init( const rise::CString& sComponent, const rise::CString& sConfig, bool bCreate )
  {
    m_pImpl->m_sComponent = sComponent;
    m_pImpl->m_sConfig = sConfig;
    m_pImpl->m_sFileName = CRuntime::Inst().GetComponentHome(sComponent) + "/" + sConfig;
    ReloadConfig(bCreate);
  }

}
