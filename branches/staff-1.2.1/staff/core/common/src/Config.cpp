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
#include "Runtime.h"
#include "Config.h"

#ifdef WIN32
#pragma warning (disable:4996)
#endif

namespace staff
{
  class CConfig::CConfigImpl
  {
  public:
    rise::xml::CXMLDocument m_tConfig;
  };

  CConfig::CConfig()
  {
    m_pImpl = new CConfigImpl;

    try
    {
      m_pImpl->m_tConfig.LoadFromFile(CRuntime::Inst().GetStaffHome() + "/staff.xml");
    }
    RISE_CATCH_ALL
  }

  CConfig::~CConfig()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  rise::xml::CXMLNode& CConfig::GetModuleConfig( const rise::CString& sModuleName )
  {
    return m_pImpl->m_tConfig.GetRoot().Subnode(sModuleName);
  }

  CConfig& CConfig::Inst()
  {
    if (m_pInst == NULL)
      m_pInst = new CConfig;
    return *m_pInst;
  }

  CConfig* CConfig::m_pInst = NULL;
}
