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

#include <axiom_namespace.h>
#include "Exception.h"
#include "Runtime.h"
#include "DataObject.h"
#include "QName.h"
#include "Value.h"
#include "Namespace.h"

namespace staff
{
  Namespace::Namespace():
    m_bOwner(false),
    m_pDataObject(NULL), 
    m_pAxiomNamespace(NULL)
  {
  }

  Namespace::Namespace(Namespace& rNamespace):
    m_bOwner(rNamespace.m_bOwner),
    m_pDataObject(rNamespace.m_pDataObject), 
    m_pAxiomNamespace(rNamespace.m_pAxiomNamespace)
  {
    rNamespace.m_bOwner = false;
  }

  Namespace::Namespace(axiom_namespace_t* pAxiomNamespace, DataObject* pDataObject /*= NULL*/, bool bOwner /*= false*/):
    m_bOwner(false),
    m_pDataObject(NULL),
    m_pAxiomNamespace(NULL)
  {
    Attach(pAxiomNamespace, pDataObject, bOwner);
  }

  Namespace::Namespace(const std::string& sUri, const std::string& sPrefix):
    m_bOwner(true),
    m_pDataObject(NULL)
  {
    m_pAxiomNamespace = axiom_namespace_create(m_pEnv, sUri.c_str(), sPrefix.c_str());
  }

  void Namespace::Attach(axiom_namespace_t* pAxiomNamespace, DataObject* pDataObject /*= NULL*/, bool bOwner /*= false*/)
  {
    Detach();
    m_pAxiomNamespace = pAxiomNamespace;
    m_pDataObject = pDataObject;
    m_bOwner = bOwner;
  }

  void Namespace::Detach()
  {
    if (m_bOwner)
    {
      Free();
      m_bOwner = false;
    }
  }

  void Namespace::Free()
  {
    if(m_pAxiomNamespace != NULL)
    {
      axiom_namespace_free(m_pAxiomNamespace, m_pEnv);
    }

    m_pAxiomNamespace = NULL;
    m_pDataObject = NULL;
  }

  Namespace Namespace::Clone() const
  {
    Namespace tNamespace(axiom_namespace_clone(m_pAxiomNamespace, m_pEnv));
    return tNamespace;
  }

  const Namespace& Namespace::Clone(Namespace& rNamespace) const
  {
    rNamespace.Attach(axiom_namespace_clone(m_pAxiomNamespace, m_pEnv));
    return *this;
  }

  std::string Namespace::GetPrefix() const
  {
    STAFF_ASSERT_DOM(m_pAxiomNamespace != NULL, "Namespace is not initialized");

    axis2_char_t* szPrefix = axiom_namespace_get_prefix(m_pAxiomNamespace, m_pEnv);
    return szPrefix == NULL ? "" : szPrefix;
  }

  std::string Namespace::GetUri() const
  {
    STAFF_ASSERT_DOM(m_pAxiomNamespace != NULL, "Namespace is not initialized");

    axis2_char_t* szUri = axiom_namespace_get_uri(m_pAxiomNamespace, m_pEnv);
    STAFF_ASSERT_DOM(szUri != NULL, "Error while getting namespace URI");

    return szUri;
  }

  DataObject* Namespace::GetDataObject() const
  {
    return m_pDataObject;
  }

  void Namespace::SetDataObject(DataObject* pDataObject)
  {
    m_pDataObject = pDataObject;
    m_bOwner = false;
  }

  bool Namespace::IsOwner() const
  {
    return m_bOwner;
  }

  void Namespace::SetOwner(bool bOwner)
  {
    m_bOwner = bOwner;
  }

  bool Namespace::IsNull()
  {
    return m_pAxiomNamespace == NULL;
  }

  Namespace& Namespace::operator=(Namespace& rNamespace)
  {
    m_bOwner = rNamespace.m_bOwner;
    m_pDataObject = rNamespace.m_pDataObject;
    m_pAxiomNamespace = rNamespace.m_pAxiomNamespace;
    rNamespace.m_bOwner = false;
    return *this;
  }

  bool Namespace::operator==(const Namespace& rNamespace) const
  {
    return axiom_namespace_equals(m_pAxiomNamespace, m_pEnv, rNamespace.m_pAxiomNamespace) == AXIS2_TRUE;
  }

  bool Namespace::operator!=(const Namespace& rNamespace) const
  {
    return !(operator==(rNamespace));
  }

  Namespace::operator axiom_namespace_t*()
  {
    return m_pAxiomNamespace;
  }

  Namespace* Namespace::operator->()
  {
    return this;
  }

  const Namespace* Namespace::operator->() const
  {
    return this;
  }

  axutil_env_t* Namespace::m_pEnv = Runtime::Inst().GetAxis2Env();

} // namespace staff

