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
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLDocument.h>
#include "Runtime.h"
#include "Config.h"

#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif

namespace staff
{
  class Config::ConfigImpl
  {
  public:
    rise::xml::CXMLDocument m_tConfig;
  };

  Config::Config():
    m_pImpl(new ConfigImpl)
  {
    try
    {
      m_pImpl->m_tConfig.LoadFromFile(Runtime::Inst().GetStaffHome() + "/staff.xml");
    }
    RISE_CATCH_ALL
  }

  Config::~Config()
  {
rise::LogNotice() << "*";
    delete m_pImpl;
  }

  rise::xml::CXMLNode& Config::GetModuleConfig(const std::string& sModuleName)
  {
    return m_pImpl->m_tConfig.GetRoot().Subnode(sModuleName);
  }

  Config& Config::Inst()
  {
    static Config tInst;
    return tInst;
  }

}
