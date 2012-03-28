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

#include "Exception.h"
#include "Attribute.h"

namespace staff
{
namespace xml
{
  Attribute::Attribute(const Attribute& rAttr):
    m_sPrefix(rAttr.m_sPrefix),
    m_sName(rAttr.m_sName),
    m_sValue(rAttr.m_sValue),
    m_pNextSibling(NULL)
  {
  }

  Attribute::Attribute(const std::string& sName):
    m_pNextSibling(NULL)
  {
    SetName(sName);
  }

  Attribute::Attribute(const std::string& sName, const std::string& sValue):
    m_sValue(sValue),
    m_pNextSibling(NULL)
  {
    SetName(sName);
  }

  Attribute::Attribute(const std::string& sName, const std::string& sValue,
                       const std::string& sPrefix):
    m_sPrefix(sPrefix),
    m_sName(sName),
    m_sValue(sValue),
    m_pNextSibling(NULL)
  {
  }

  const std::string& Attribute::GetPrefix() const
  {
    return m_sPrefix;
  }

  void Attribute::SetPrefix(const std::string& sPrefix)
  {
    m_sPrefix = sPrefix;
  }

  const std::string& Attribute::GetName() const
  {
    return m_sName;
  }

  void Attribute::SetName(const std::string& sName)
  {
    std::string::size_type nPos = sName.find_last_of(':');
    if (nPos == std::string::npos)
    {
      m_sName = sName;
      m_sPrefix.erase();
    }
    else
    {
      m_sName.assign(sName, nPos, std::string::npos);
      m_sPrefix.assign(sName, 0, nPos - 1);
    }
  }

  std::string Attribute::GetPrefixName() const
  {
    return m_sPrefix.empty() ? m_sName : (m_sPrefix + ":" + m_sName);
  }

  const std::string& Attribute::GetValue() const
  {
    return m_sValue;
  }

  void Attribute::SetValue(const std::string& sValue)
  {
    m_sValue = sValue;
  }


  const Attribute* Attribute::GetNextSibling() const
  {
    return m_pNextSibling;
  }

  Attribute* Attribute::GetNextSibling()
  {
    return m_pNextSibling;
  }


  Attribute& Attribute::operator=(const Attribute& rAttr)
  {
    m_sPrefix = rAttr.m_sPrefix;
    m_sName = rAttr.m_sName;
    m_sValue = rAttr.m_sValue;
    return *this;
  }

  bool Attribute::operator==(const Attribute& rAttr) const
  {
    return m_sName == rAttr.m_sName && m_sValue == rAttr.m_sValue && m_sPrefix == rAttr.m_sPrefix;
  }

  bool Attribute::operator!=(const Attribute& rAttr) const
  {
    return !(operator==(rAttr));
  }

} // namespace xml
} // namespace staff

