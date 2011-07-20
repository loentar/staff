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
  Attribute::Attribute():
    m_bOwner(false),
    m_pDataObject(NULL), 
    m_pAxiomAttribute(NULL)
  {
  }

  Attribute::Attribute(Attribute& rAttribute):
    m_bOwner(rAttribute.m_bOwner),
    m_pDataObject(rAttribute.m_pDataObject), 
    m_pAxiomAttribute(rAttribute.m_pAxiomAttribute)
  {
    rAttribute.m_bOwner = false;
  }

  Attribute::Attribute(const Attribute& rAttribute):
    m_bOwner(rAttribute.m_bOwner),
    m_pDataObject(rAttribute.m_pDataObject),
    m_pAxiomAttribute(rAttribute.m_pAxiomAttribute)
  {
    const_cast<Attribute&>(rAttribute).m_bOwner = false;
  }

  Attribute::Attribute(DataObject* pDataObject, axiom_attribute_t* pAxiomAttribute):
    m_bOwner(false),
    m_pDataObject(pDataObject), 
    m_pAxiomAttribute(pAxiomAttribute)
  {
  }

  bool Attribute::IsOwner() const
  {
    return m_bOwner;
  }

  void Attribute::SetOwner(bool bOwner)
  {
    m_bOwner = bOwner;
  }

  Attribute::Attribute(const std::string& sLocalName, const char* szValue):
      m_bOwner(true),
      m_pDataObject(NULL)
  {
    m_pAxiomAttribute = axiom_attribute_create(m_pEnv, sLocalName.c_str(), szValue, NULL);
  }

  Attribute::Attribute(const std::string& sLocalName, const std::string& sValue):
      m_bOwner(true),
      m_pDataObject(NULL)
  {
    m_pAxiomAttribute = axiom_attribute_create(m_pEnv, sLocalName.c_str(), sValue.c_str(), NULL);
  }

  Attribute::Attribute(const std::string& sLocalName, const Value& tValue):
    m_bOwner(true),
    m_pDataObject(NULL)
  {
    m_pAxiomAttribute = axiom_attribute_create(m_pEnv, sLocalName.c_str(), tValue.AsString().c_str(), NULL);
  }

  Attribute::~Attribute()
  {
    if (m_bOwner && m_pAxiomAttribute != NULL)
    {
      axiom_attribute_free(m_pAxiomAttribute, m_pEnv);
    }
  }


  std::string Attribute::GetLocalName() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL, DomNoItemException, 
      "Not Initialized");

    axis2_char_t* szLocalName = axiom_attribute_get_localname(m_pAxiomAttribute, 
      m_pEnv);
    RISE_ASSERTES(szLocalName != NULL, DomNoItemException, "Can\'t get attribute name");
    
    return szLocalName;
  }

  void Attribute::SetLocalName(const std::string& sLocalName)
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL, DomNoItemException,
      "Not Initialized");

    axiom_attribute_set_localname(m_pAxiomAttribute, m_pEnv,
                                   reinterpret_cast<const axis2_char_t*>(sLocalName.c_str()));
  }

  std::string Attribute::GetText() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      DomNoItemException, "Not Initialized");

    axis2_char_t* szText = axiom_attribute_get_value(m_pAxiomAttribute,
      m_pEnv);
    RISE_ASSERTES(szText != NULL, DomNoItemException, "Can\'t get attribute text");

    return szText;
  }

  void Attribute::SetText(const std::string& sText)
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      DomNoItemException, "Not Initialized");

    axiom_attribute_set_value(m_pAxiomAttribute, m_pEnv,
                              reinterpret_cast<const axis2_char_t*>(sText.c_str()));
  }

  Value Attribute::GetValue() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      DomNoItemException, "Not Initialized");

    axis2_char_t* szValue = axiom_attribute_get_value(m_pAxiomAttribute, 
      m_pEnv);
    RISE_ASSERTES(szValue != NULL, DomNoItemException, "Can\'t get attribute text");

    return szValue;
  }

  void Attribute::SetValue(const Value& rValue)
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      DomNoItemException, "Not Initialized");

    axiom_attribute_set_value(m_pAxiomAttribute, m_pEnv,
                              reinterpret_cast<const axis2_char_t*>(rValue.AsString().c_str()));
  }

  const QName Attribute::GetQName() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      DomNoItemException, "Not Initialized");

    axutil_qname_t* pqName = axiom_attribute_get_qname(m_pAxiomAttribute, m_pEnv);
    RISE_ASSERTES(pqName != NULL, DomNoItemException, "Can\'t get attribute\'s QName");
    
    QName tqName(pqName);
    return tqName;

//    return pqName; // old stdcpp
  }

  QName Attribute::GetQName()
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      DomNoItemException, "Not Initialized");

    axutil_qname_t* pqName = axiom_attribute_get_qname(m_pAxiomAttribute, m_pEnv);
    RISE_ASSERTES(pqName != NULL, DomNoItemException, "Can\'t get attribute\'s QName");

    QName tqName(pqName);
    return tqName;
  }

  Attribute& Attribute::operator=(Attribute& rAttribute)
  {
    m_bOwner = rAttribute.m_bOwner;
    m_pDataObject = rAttribute.m_pDataObject;
    m_pAxiomAttribute = rAttribute.m_pAxiomAttribute;
    rAttribute.m_bOwner = false;
    return *this;
  }

  Attribute& Attribute::operator=(const Attribute& rAttribute)
  {
    m_bOwner = rAttribute.m_bOwner;
    m_pDataObject = rAttribute.m_pDataObject;
    m_pAxiomAttribute = rAttribute.m_pAxiomAttribute;
    const_cast<Attribute&>(rAttribute).m_bOwner = false;
    return *this;
  }

  bool Attribute::operator==(const Attribute& rAttribute) const
  {
    RISE_ASSERTES(m_pDataObject != NULL && rAttribute.m_pDataObject != NULL
      && m_pAxiomAttribute != NULL && rAttribute.m_pAxiomAttribute != NULL,
      DomNoItemException, "Not Initialized");

    //! TODO: compare using AxiOM functions
    return GetLocalName() == rAttribute.GetLocalName() && 
      GetValue() == rAttribute.GetValue();
  }

  bool Attribute::operator!=(const Attribute& rAttribute) const
  {
    return !(operator==(rAttribute));
  }

  Attribute::operator axiom_attribute_t*()
  {
    return m_pAxiomAttribute;
  }

  Attribute* Attribute::operator->()
  {
    return this;
  }

  const Attribute* Attribute::operator->() const
  {
    return this;
  }

  axutil_env_t* Attribute::m_pEnv = Runtime::Inst().GetAxis2Env();

} // namespace staff

