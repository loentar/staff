/*
 *  Copyright 2012 Utkin Dmitry
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

#include <sstream>
#include "Attribute.h"
#include "Exception.h"
#include "Declaration.h"

namespace staff
{
namespace xml
{

  Declaration::Declaration():
    m_sVersion("1.0"),
    m_sEncoding("UTF-8"),
    m_eStandalone(StandaloneUndefined)
  {
  }

  Declaration::~Declaration()
  {
  }

  const std::string& Declaration::GetVersion() const
  {
    return m_sVersion;
  }

  void Declaration::SetVersion(const std::string& sVersion)
  {
    m_sVersion = sVersion;
  }

  const std::string& Declaration::GetEncoding() const
  {
    return m_sEncoding;
  }

  void Declaration::SetEncoding(const std::string& sEncoding)
  {
    m_sEncoding = sEncoding;
  }

  Declaration::Standalone Declaration::GetStandalone() const
  {
    return m_eStandalone;
  }

  void Declaration::SetStandalone(Declaration::Standalone eStandalone)
  {
    m_eStandalone = eStandalone;
  }

} // namespace xml
} // namespace staff
