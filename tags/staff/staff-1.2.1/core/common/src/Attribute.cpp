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

#include <axiom_attribute.h>
#include "Exception.h"
#include "Runtime.h"
#include "DataObject.h"
#include "QName.h"
#include "Value.h"
#include "Attribute.h"

namespace staff
{
  CAttribute::CAttribute():
    m_bOwner(false),
    m_pDataObject(NULL), 
    m_pAxiomAttribute(NULL)
  {
  }

  CAttribute::CAttribute( CAttribute& rAttribute ):
    m_bOwner(rAttribute.m_bOwner),
    m_pDataObject(rAttribute.m_pDataObject), 
    m_pAxiomAttribute(rAttribute.m_pAxiomAttribute)
  {
    rAttribute.m_bOwner = false;
  }

  CAttribute::CAttribute( CDataObject* pDataObject, axiom_attribute_t* pAxiomAttribute ):
    m_bOwner(false),
    m_pDataObject(pDataObject), 
    m_pAxiomAttribute(pAxiomAttribute)
  {
  }

  CAttribute::CAttribute( const std::string& sLocalName, const CValue& tValue):
    m_bOwner(true),
    m_pDataObject(NULL)
  {
    m_pAxiomAttribute = axiom_attribute_create(CRuntime::Inst().GetAxis2Env(), sLocalName.c_str(), tValue.AsString().c_str(), NULL);
  }

  std::string CAttribute::GetLocalName() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL, CDomNoItemException, 
      "Not Initialized");

    axis2_char_t* szLocalName = axiom_attribute_get_localname(m_pAxiomAttribute, 
      CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(szLocalName != NULL, CDomNoItemException, "Can\'t get attribute");
    
    return szLocalName;
  }

  CValue CAttribute::GetValue() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      CDomNoItemException, "Not Initialized");

    axis2_char_t* szValue = axiom_attribute_get_value(m_pAxiomAttribute, 
      CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(szValue != NULL, CDomNoItemException, "Can\'t get attribute");

    return szValue;
  }

  const CQName CAttribute::GetQName() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      CDomNoItemException, "Not Initialized");

    axutil_qname_t* pqName = axiom_attribute_get_qname(m_pAxiomAttribute, CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(pqName != NULL, CDomNoItemException, "Can\'t get attribute\'s QName");
    
    CQName tqName(pqName);
    return tqName;

//    return pqName; // old stdcpp
  }

  CQName CAttribute::GetQName()
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      CDomNoItemException, "Not Initialized");

    axutil_qname_t* pqName = axiom_attribute_get_qname(m_pAxiomAttribute, CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(pqName != NULL, CDomNoItemException, "Can\'t get attribute\'s QName");

    CQName tqName(pqName);
    return tqName;
  }

  CAttribute& CAttribute::operator=( CAttribute& rAttribute )
  {
    m_bOwner = rAttribute.m_bOwner;
    m_pDataObject = rAttribute.m_pDataObject;
    m_pAxiomAttribute = rAttribute.m_pAxiomAttribute;
    rAttribute.m_bOwner = false;
    return *this;
  }

  bool CAttribute::operator==( const CAttribute& rAttribute ) const
  {
    RISE_ASSERTES(m_pDataObject != NULL && rAttribute.m_pDataObject != NULL
      && m_pAxiomAttribute != NULL && rAttribute.m_pAxiomAttribute != NULL,
      CDomNoItemException, "Not Initialized");

    //! TODO: compare using AxiOM functions
    return GetLocalName() == rAttribute.GetLocalName() && 
      GetValue() == rAttribute.GetValue();
  }

  bool CAttribute::operator!=( const CAttribute& rAttribute ) const
  {
    return !(operator==(rAttribute));
  }

  CAttribute::operator axiom_attribute_t*()
  {
    return m_pAxiomAttribute;
  }

  CAttribute* CAttribute::operator->()
  {
    return this;
  }

  const CAttribute* CAttribute::operator->() const
  {
    return this;
  }

} // namespace staff

