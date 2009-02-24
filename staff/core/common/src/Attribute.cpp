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
      "Непроинициализированный атрибут");

    axis2_char_t* szLocalName = axiom_attribute_get_localname(m_pAxiomAttribute, 
      CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(szLocalName != NULL, CDomNoItemException, "Ошибка получения атрибута");
    
    return szLocalName;
  }

  CValue CAttribute::GetValue() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      CDomNoItemException, "Непроинициализированный атрибут");

    axis2_char_t* szValue = axiom_attribute_get_value(m_pAxiomAttribute, 
      CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(szValue != NULL, CDomNoItemException, "Ошибка получения атрибута");

    return szValue;
  }

  const CQName CAttribute::GetQName() const
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      CDomNoItemException, "Непроинициализированный атрибут");

    axutil_qname_t* pqName = axiom_attribute_get_qname(m_pAxiomAttribute, CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(pqName != NULL, CDomNoItemException, "Ошибка получения составного имени атрибута");
    
    CQName tqName(pqName);
    return tqName;

//    return pqName; // old stdcpp
  }

  CQName CAttribute::GetQName()
  {
    RISE_ASSERTES(m_pDataObject != NULL && m_pAxiomAttribute != NULL,
      CDomNoItemException, "Непроинициализированный атрибут");

    axutil_qname_t* pqName = axiom_attribute_get_qname(m_pAxiomAttribute, CRuntime::Inst().GetAxis2Env());
    RISE_ASSERTES(pqName != NULL, CDomNoItemException, "Ошибка получения составного имени атрибута");

    CQName tqName(pqName);
    return tqName;
//    return pqName; // old stdcpp
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
      CDomNoItemException, "Попытка сравнения с непроинициализированным атрибутом");

    //! TODO: прямой вызов функций
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

