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
#include "Exception.h"
#include "Runtime.h"
#include "QName.h"

namespace staff
{
  CQName::CQName():
    m_bOwner(false),
    m_pAxutilQName(NULL)
  {
  }

  CQName::CQName( CQName& rQName )
  {
    operator=(rQName);
  }

  CQName::CQName( const std::string& sLocalPart, const std::string& sNamespaceUri):
    m_bOwner(false),
    m_pAxutilQName(NULL)
  {
    Create(sLocalPart, sNamespaceUri);
  }

  CQName::CQName( const std::string& sLocalPart, const std::string& sNamespaceUri, const std::string& sPrefix ):
    m_bOwner(false),
    m_pAxutilQName(NULL)
  {
    Create(sLocalPart, sNamespaceUri, sPrefix);
  }

  CQName::CQName( axutil_qname_t* pQName )
  {
    Attach(pQName);
  }

  CQName::~CQName()
  {
    if (m_bOwner)
    {
      Detach();
    }
  }

  CQName& CQName::operator=( CQName& rstQName )
  {
    Detach();
    m_bOwner = rstQName.m_bOwner;
    m_pAxutilQName = rstQName.m_pAxutilQName;
    return *this;
  }

  CQName& CQName::operator=( axutil_qname_t* pQName )
  {
    Detach();
    m_bOwner = false;
    m_pAxutilQName = pQName;
    return *this;
  }


  bool CQName::operator==( const CQName& rQName ) const
  {
    return axutil_qname_equals(m_pAxutilQName, m_pEnv, rQName.m_pAxutilQName) != 0;
  }

  bool CQName::operator!=( const CQName& rstQName ) const
  {
    return !(operator==(rstQName));
  }

  bool CQName::operator==(axutil_qname_t* pQName) const
  {
    return axutil_qname_equals(m_pAxutilQName, m_pEnv, pQName) != 0;
  }

  bool CQName::operator!=(axutil_qname_t* pQName) const
  {
    return !(operator==(pQName));
  }

  CQName::operator std::string() const
  {
    const std::string& sPrefix = GetPrefix();
    return sPrefix != "" ? sPrefix + ":" + GetLocalPart() : GetLocalPart();
  }

  std::string CQName::ToString() const
  {
    const std::string& sPrefix = GetPrefix();
    return sPrefix != "" ? sPrefix + ":" + GetLocalPart() : GetLocalPart();
  }

  std::string CQName::GetLocalPart() const
  {
    axis2_char_t* szResult = axutil_qname_get_localpart(m_pAxutilQName, m_pEnv);
    RISE_ASSERTES(szResult != NULL, CDomFormatException, "Can\'t get local part");
    return szResult;
  }

  std::string CQName::GetNamespaceUri() const
  {
    axis2_char_t* szResult = axutil_qname_get_uri(m_pAxutilQName, m_pEnv);
    RISE_ASSERTES(szResult != NULL, CDomFormatException, "Can\'t get Uri");
    return szResult;
  }

  std::string CQName::GetPrefix() const
  {
    axis2_char_t* szResult = axutil_qname_get_prefix(m_pAxutilQName, m_pEnv);
    RISE_ASSERTES(szResult != NULL, CDomFormatException, "Can\'t get prefix");
    return szResult;
  }

  CQName& CQName::Create( const std::string& sLocalPart, const std::string& sNamespaceUri, const std::string& sPrefix /*= ""*/ )
  {
    axutil_qname_t* pAxutilQName = axutil_qname_create(m_pEnv,
        sLocalPart.c_str(), sNamespaceUri.c_str(), sPrefix.c_str());
    RISE_ASSERTES(pAxutilQName != NULL, CDomFormatException, "Can\'t create AxiOM qname");

    m_pAxutilQName = pAxutilQName;
    m_bOwner = true;
    return *this;
  }

  void CQName::Free()
  {
    axutil_qname_free(m_pAxutilQName, m_pEnv);
    m_pAxutilQName = NULL;
    m_bOwner = false;
  }

  CQName& CQName::Attach( axutil_qname_t* pQName )
  {
    Detach();
    m_pAxutilQName = pQName;
    m_bOwner = false;
    return *this;
  }

  void CQName::Detach()
  {
    if (m_pAxutilQName != NULL)
    {
      if(m_bOwner)
        Free();
      else
      {
        m_pAxutilQName = NULL;
        m_bOwner = false;
      }
    }
  }

  CQName& CQName::Clone( const CQName& rQName )
  {
    m_pAxutilQName = axutil_qname_clone(rQName.m_pAxutilQName, m_pEnv);
    m_bOwner = true;
    return *this;
  }

  CQName CQName::Clone()
  {
    CQName tqClone(axutil_qname_clone(m_pAxutilQName, m_pEnv));
    return tqClone;
  }

  CQName::operator axutil_qname_t*() const
  {
    return m_pAxutilQName;
  }

  CQName::operator axutil_qname_t*()
  {
    return m_pAxutilQName;
  }

  axutil_env_t* CQName::m_pEnv = CRuntime::Inst().GetAxis2Env();

} // namespace staff

