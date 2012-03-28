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

#include <staff/utils/Log.h>
#include <staff/utils/stringutils.h>
#include <staff/xml/Element.h>
#include <staff/xml/Document.h>
#include <staff/xml/XmlReader.h>
#include <staff/xml/XmlWriter.h>
#include <staff/common/Runtime.h>
#include "ComponentConfig.h"


namespace staff
{
  class ComponentConfig::ComponentConfigImpl
  {
  public:
    std::string m_sComponent;
    std::string m_sConfig;
    std::string m_sFileName;
    xml::Document m_tXmlConfig;
  };

  ComponentConfig::ComponentConfig():
    m_pImpl(new ComponentConfigImpl)
  {
  }

  ComponentConfig::~ComponentConfig()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  const std::string& ComponentConfig::GetComponent() const
  {
    return m_pImpl->m_sComponent;
  }

  const std::string& ComponentConfig::GetConfig() const
  {
    return m_pImpl->m_sConfig;
  }

  xml::Element& ComponentConfig::ReloadConfig(bool bCreate /*= false*/)
  {
    try
    {
      xml::XmlFileReader tXmlReader(m_pImpl->m_sFileName);
      tXmlReader.ReadDocument(m_pImpl->m_tXmlConfig);
    }
    catch(...)
    {
      if (bCreate)
      {
        LogDebug() << "Creating new configuration for: "
                   << m_pImpl->m_sComponent << ":" << m_pImpl->m_sConfig;
      }
      else
      {
        throw;
      }
    }

    return m_pImpl->m_tXmlConfig.GetRootElement();
  }

  void ComponentConfig::SaveConfig()
  {
    if (m_pImpl->m_tXmlConfig.GetRootElement().GetName().empty())
    {
      m_pImpl->m_tXmlConfig.GetRootElement().SetName("Config");
    }

    xml::XmlFileWriter tXmlWriter(m_pImpl->m_sFileName);
    tXmlWriter.WriteDocument(m_pImpl->m_tXmlConfig);
  }

  xml::Element& ComponentConfig::Config()
  {
    return m_pImpl->m_tXmlConfig.GetRootElement();
  }

  const xml::Element& ComponentConfig::Config() const
  {
    return m_pImpl->m_tXmlConfig.GetRootElement();
  }

  void ComponentConfig::Init(const std::string& sComponent, const std::string& sConfig, bool bCreate)
  {
    m_pImpl->m_sComponent = sComponent;
    m_pImpl->m_sConfig = sConfig;
    m_pImpl->m_sFileName = Runtime::Inst().GetComponentHome(sComponent) + "/" + sConfig;
    ReloadConfig(bCreate);
  }

}
