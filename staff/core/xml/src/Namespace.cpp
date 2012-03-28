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

#include "Namespace.h"

namespace staff
{
  namespace xml
  {
    Namespace::Namespace(const Namespace& rNamespace):
      m_sPrefix(rNamespace.m_sPrefix),
      m_sUri(rNamespace.m_sUri),
      m_pNextSibling(NULL)
    {
    }

    Namespace::Namespace(const std::string& sPrefix, const std::string& sUri):
      m_sPrefix(sPrefix),
      m_sUri(sUri),
      m_pNextSibling(NULL)
    {
    }

    Namespace& Namespace::operator=(const Namespace& rNamespace)
    {
      m_sPrefix = rNamespace.m_sPrefix;
      m_sUri = rNamespace.m_sUri;
      return *this;
    }


    const std::string& Namespace::GetPrefix() const
    {
      return m_sPrefix;
    }

    void Namespace::SetPrefix(const std::string& sPrefix)
    {
      m_sPrefix = sPrefix;
    }


    const std::string& Namespace::GetUri() const
    {
      return m_sUri;
    }

    void Namespace::SetUri(const std::string& sUri)
    {
      m_sUri = sUri;
    }

    const Namespace* Namespace::GetNextSibling() const
    {
      return m_pNextSibling;
    }

    Namespace* Namespace::GetNextSibling()
    {
      return m_pNextSibling;
    }


    bool Namespace::operator==(const Namespace& rNamespace) const
    {
      return m_sPrefix == rNamespace.m_sPrefix && m_sUri == rNamespace.m_sUri;
    }

    bool Namespace::operator!=(const Namespace& rNamespace) const
    {
      return !(operator==(rNamespace));
    }

  } // namespace xml
} // namespace staff

