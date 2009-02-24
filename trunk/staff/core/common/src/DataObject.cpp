#include <axutil_utils.h>
#include <axiom.h>
#include <axiom_node.h>
#include <axiom_element.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include "Exception.h"
#include "Runtime.h"
#include "QName.h"
#include "Attribute.h"
#include "Value.h"
#include "DataObject.h"

namespace staff
{
  CDataObject::CDataObject(axiom_node_t* pAxiomNode /*= NULL*/):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    if (pAxiomNode != NULL)
    {
      Attach(pAxiomNode);
    }
  }

  CDataObject::CDataObject(const std::string& sLocalName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName);
  }

  CDataObject::CDataObject(const CQName& stQName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(stQName);
  }

  CDataObject::CDataObject(const CDataObject& rDataObject):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    operator=(const_cast<CDataObject&>(rDataObject));
  }

  CDataObject::CDataObject( const std::string& sLocalName, const std::string& sText ):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName, sText);
  }

  CDataObject::CDataObject( const std::string& sLocalName, const CValue& rValue ):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName, rValue);
  }

  CDataObject::~CDataObject()
  {
    Detach();
  }

  void CDataObject::Attach( axiom_node_t* pAxiomNode, bool bOwner /*= false*/ )
  {
    RISE_ASSERTP(pAxiomNode);
    RISE_ASSERTES(axiom_node_get_node_type(pAxiomNode, m_pEnv) == AXIOM_ELEMENT, 
      CDomTypeException, "Указанный узел не является элементом");

    axiom_element_t* pAxiomElement = 
      reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pAxiomNode, m_pEnv));
    RISE_ASSERTES(pAxiomElement != NULL, CDomNoItemException, "Не найден элемент данных");

    Detach();
    m_pAxiomNode = pAxiomNode;
    m_pAxiomElement = pAxiomElement;
    m_bOwner = bOwner;
  }

  void CDataObject::Detach()
  {
    if (m_bOwner)
    {
      Free();
      m_bOwner = false;
    }
  }

  bool CDataObject::IsOwner()
  {
    return m_bOwner;
  }

  void CDataObject::SetOwner(bool bOwner)
  {
    m_bOwner = bOwner;
  }

  CDataObject::operator axiom_node_t*()
  {
    return m_pAxiomNode;
  }

  CQName CDataObject::GetQName()
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);

    axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, 
      m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Невозможно получить составное имя узла");

//    return pqName;
    CQName tqName(pqName);
    return tqName;
  }

  void CDataObject::SetQName( const CQName& stQName ) const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);

//     axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, 
//       m_pAxiomNode);
//     RISE_ASSERTES(pqName != NULL, CDomFormatException, "Невозможно получить QName");
//     axiom_attribute_t* pAttr = axiom_element_get_attribute(m_pAxiomElement, 
//       m_pEnv, pqName);
//     RISE_ASSERTES(pAttr != NULL, CDomFormatException, "Невозможно получить attr");

    axiom_element_set_localname(m_pAxiomElement, m_pEnv, stQName.GetLocalPart().c_str());

    axiom_namespace_t* pNewNamespace = axiom_namespace_create(m_pEnv, stQName.GetNamespaceUri().c_str(), 
      stQName.GetPrefix().c_str());
    RISE_ASSERTES(pNewNamespace != NULL, CDomFormatException, "Невозможно создать namespace");

    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNewNamespace, m_pAxiomNode);

//    axiom_element_remove_attribute(m_pAxiomElement, m_pEnv, pAttr);
  }

  std::string CDataObject::GetLocalName() const
  {
    RISE_ASSERT(m_pAxiomElement);

    const axis2_char_t* szLocalName = axiom_element_get_localname(m_pAxiomElement, m_pEnv);

    RISE_ASSERTES(szLocalName != NULL, CDomFormatException, "Невозможно получить локальное имя узла");
    
    return szLocalName;
  }

  void CDataObject::SetLocalName( const std::string& sLocalName ) const
  {
    RISE_ASSERT(m_pAxiomElement);

    axiom_element_set_localname(m_pAxiomElement, m_pEnv, sLocalName.c_str());
  }

  std::string CDataObject::GetPrefix() const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);

    axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, 
      m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Невозможно получить составное имя узла");

    const axis2_char_t* szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);

    return szPrefix == NULL ? "" : szPrefix;
  }

  void CDataObject::SetPrefix( const std::string& sPrefix ) const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);

    axiom_namespace_t* pNewNamespace = NULL;
    axiom_namespace_t* pOldNamespace = axiom_element_get_namespace(m_pAxiomElement, m_pEnv, 
      m_pAxiomNode);
    const char* szUri = NULL;

    if (pOldNamespace != NULL)
    {
      szUri = axiom_namespace_get_uri(pOldNamespace, m_pEnv);
//      RISE_ASSERTES(szUri != NULL, CDomFormatException, "Невозможно получить URI узла");

    }

    pNewNamespace = axiom_namespace_create(m_pEnv, szUri != NULL ? szUri : "", sPrefix.c_str());
    RISE_ASSERTES(pNewNamespace != NULL, CDomFormatException, "Невозможно создать namespace");

    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNewNamespace, m_pAxiomNode);
  }

  std::string CDataObject::GetNamespaceUri() const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);

    axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, 
      m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Невозможно получить составное имя узла");

    const axis2_char_t* szUri = axutil_qname_get_uri(pqName, m_pEnv);
//    RISE_ASSERTES(szUri != NULL, CDomFormatException, "Невозможно получить URI узла");

    return szUri != NULL ? szUri : "";
  }

  void CDataObject::SetNamespaceUri( const std::string& sUri ) const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);

    axiom_namespace_t* pOldNamespace = axiom_element_get_namespace(m_pAxiomElement, m_pEnv, 
      m_pAxiomNode);
    axiom_namespace_t* pNewNamespace = NULL;
    const char* szPrefix = NULL;

    if (pOldNamespace != NULL)
    {
      szPrefix = axiom_namespace_get_prefix(pOldNamespace, m_pEnv);
      //RISE_ASSERTES(szPrefix != NULL, CDomFormatException, "Невозможно получить префикс узла");
    }

    pNewNamespace = axiom_namespace_create(m_pEnv, sUri.c_str(), szPrefix != NULL ? "" : szPrefix );

    RISE_ASSERTES(pNewNamespace != NULL, CDomFormatException, "Невозможно создать namespace");

    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNewNamespace, m_pAxiomNode);
  }

  //////////////////////////////////////////////////////////////////////////
  // управление узлом

  void CDataObject::Create()
  {
    Detach();
    axiom_node_t* pAxiomNode = axiom_node_create(m_pEnv);
    Attach(pAxiomNode, true);
  }

  void CDataObject::Create( const std::string& sLocalName )
  {
    Detach();
    axiom_node_t* pAxiomNode = axiom_node_create(m_pEnv);
    RISE_ASSERT(pAxiomNode != NULL);

    axiom_element_create(m_pEnv, NULL, sLocalName.c_str(), NULL, &pAxiomNode);

    Attach(pAxiomNode, true);
    SetLocalName(sLocalName);
  }

  void CDataObject::Create( const CQName& rQName )
  {
    Detach();

    axiom_node_t* pAxiomNode = axiom_node_create(m_pEnv);
    RISE_ASSERT(pAxiomNode != NULL);

    axiom_namespace_t* pNamespace = axiom_namespace_create(m_pEnv, 
      rQName.GetNamespaceUri().c_str(), rQName.GetPrefix().c_str());
    RISE_ASSERT(pNamespace != NULL);

    axiom_element_create(m_pEnv, NULL, rQName.GetLocalPart().c_str(), pNamespace, &pAxiomNode);

    Attach(pAxiomNode, true);
    SetQName(rQName);
  }

  void CDataObject::Create( const std::string& sLocalName, const std::string& sText )
  {
    Create(sLocalName);
    SetText(sText);
  }

  void CDataObject::Create( const std::string& sLocalName, const CValue& rValue )
  {
    Create(sLocalName);
    SetValue(rValue);
  }

  void CDataObject::Free()
  {
    if(m_pAxiomNode != NULL)
    {
      axiom_node_free_tree(m_pAxiomNode, m_pEnv);
    }

    m_pAxiomNode = NULL;
    m_pAxiomElement = NULL;
  }

  CDataObject& CDataObject::DetachNode()
  {
    RISE_ASSERTES(axiom_node_detach(m_pAxiomNode, m_pEnv), CDomNoItemException, 
      "Ошибка отсоединения узла");
    return *this;
  }

  CDataObject& CDataObject::Clone( const CDataObject& rDataObject )
  {
    Detach();
    FromString(rDataObject.ToString());
    return *this;
  }

  CDataObject CDataObject::Clone()
  {
    CDataObject tdoClone;
    tdoClone.FromString(ToString());
    return tdoClone;
  }

  //////////////////////////////////////////////////////////////////////////
  // управление подузлами

  CDataObject CDataObject::CreateChild()
  {
    CDataObject tdoChild;
    return AppendChild(tdoChild);
  }

  CDataObject CDataObject::CreateChild( const std::string& sLocalName )
  {
    CDataObject tdoChild(sLocalName);
    return AppendChild(tdoChild);
  }

  CDataObject CDataObject::CreateChild( const CQName& rstQName )
  {
    CDataObject tdoChild(rstQName);
    return AppendChild(tdoChild);
  }

  CDataObject CDataObject::CreateChild( const std::string& sLocalName, const CValue& rValue )
  {
    CDataObject tdoChild(sLocalName, rValue);
    return AppendChild(tdoChild);
  }

  CDataObject CDataObject::CreateChildOnce( const std::string& sLocalName )
  {
    Iterator itFind = FindChildByLocalName(sLocalName);
    if (itFind != End())
      return *itFind;

    CDataObject tdoChild(sLocalName);
    return AppendChild(tdoChild);
  }

  CDataObject CDataObject::AppendChild( CDataObject& rDataObject )
  {
    RISE_ASSERT(m_pAxiomNode);

    axiom_node_add_child(m_pAxiomNode, m_pEnv, rDataObject.m_pAxiomNode);
    rDataObject.m_bOwner = false;
    
    return rDataObject;
  }

  staff::CDataObject CDataObject::DetachChild( Iterator& itChild )
  {
    RISE_ASSERTES(itChild.m_pDataObject != NULL && *(itChild.m_pDataObject) == *this, 
      CDomNoItemException, "Итератор не пренадлежит указанному узлу данных");
    RISE_ASSERTES(itChild.m_pAxiomNode != NULL, CDomNoItemException, "Итератор == End()");

    return CDataObject(axiom_node_detach(itChild.m_pAxiomNode, m_pEnv));
  }


  void CDataObject::RemoveChild( Iterator& itChild )
  {
    RISE_ASSERTES(itChild.m_pDataObject != NULL && *(itChild.m_pDataObject) == *this, 
      CDomNoItemException, "Итератор не пренадлежит указанному узлу данных");
    RISE_ASSERTES(itChild.m_pAxiomNode != NULL, CDomNoItemException, "Итератор == End()");

    axiom_node_free_tree(itChild.m_pAxiomNode, m_pEnv);
  }

  void CDataObject::RemoveAllChildren()
  {
    axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
    axiom_node_t* pNodeToFree = NULL;

    while(pNode != NULL)
    {
      pNodeToFree = pNode;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv);
      axiom_node_free_tree(pNodeToFree, m_pEnv);
    }
  }

  CDataObject::Iterator CDataObject::FindChildByQName( const CQName& rQName )
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    for ( axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        pqName = axiom_element_get_qname(pElem, m_pEnv, m_pAxiomNode);
        if (pqName == NULL)
          continue;

        szLocalName = axutil_qname_get_localpart(pqName, m_pEnv);
        if (szLocalName == NULL)
          continue;

        szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);
        if (szPrefix == NULL)
          continue;

        szNamespaceUri = axutil_qname_get_uri(pqName, m_pEnv);
        if (szNamespaceUri == NULL)
          continue;

        if (axutil_strcmp(szLocalName, sLocalPart.c_str()) == 0 &&
            axutil_strcmp(szPrefix, sPrefix.c_str()) == 0 &&
            axutil_strcmp(szNamespaceUri, sNamespaceUri.c_str()) == 0)
        {
          return Iterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject::ConstIterator CDataObject::FindChildByQName( const CQName& rQName ) const
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    for ( axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        pqName = axiom_element_get_qname(pElem, m_pEnv, m_pAxiomNode);
        if (pqName == NULL)
          continue;

        szLocalName = axutil_qname_get_localpart(pqName, m_pEnv);
        if (szLocalName == NULL)
          continue;

        szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);
        if (szPrefix == NULL)
          continue;

        szNamespaceUri = axutil_qname_get_uri(pqName, m_pEnv);
        if (szNamespaceUri == NULL)
          continue;

        if (axutil_strcmp(szLocalName, sLocalPart.c_str()) == 0 &&
          axutil_strcmp(szPrefix, sPrefix.c_str()) == 0 &&
          axutil_strcmp(szNamespaceUri, sNamespaceUri.c_str()) == 0)
        {
          return ConstIterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject::Iterator CDataObject::FindChildByQName( const CQName& rQName, const Iterator& itStart )
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    RISE_ASSERTES(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this, 
      CDomNoItemException, "Итератор не пренадлежит указанному узлу данных");

    for ( axiom_node_t* pNode = itStart.m_pAxiomNode;
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        pqName = axiom_element_get_qname(pElem, m_pEnv, m_pAxiomNode);
        if (pqName == NULL)
          continue;

        szLocalName = axutil_qname_get_localpart(pqName, m_pEnv);
        if (szLocalName == NULL)
          continue;

        szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);
        if (szPrefix == NULL)
          continue;

        szNamespaceUri = axutil_qname_get_uri(pqName, m_pEnv);
        if (szNamespaceUri == NULL)
          continue;

        if (axutil_strcmp(szLocalName, sLocalPart.c_str()) == 0 &&
          axutil_strcmp(szPrefix, sPrefix.c_str()) == 0 &&
          axutil_strcmp(szNamespaceUri, sNamespaceUri.c_str()) == 0)
        {
          return Iterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject::ConstIterator CDataObject::FindChildByQName( const CQName& rQName, const ConstIterator& itStart ) const
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    RISE_ASSERTES(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this, 
      CDomNoItemException, "Итератор не пренадлежит указанному узлу данных");

    for ( axiom_node_t* pNode = itStart.m_pAxiomNode;
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        pqName = axiom_element_get_qname(pElem, m_pEnv, m_pAxiomNode);
        if (pqName == NULL)
          continue;

        szLocalName = axutil_qname_get_localpart(pqName, m_pEnv);
        if (szLocalName == NULL)
          continue;

        szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);
        if (szPrefix == NULL)
          continue;

        szNamespaceUri = axutil_qname_get_uri(pqName, m_pEnv);
        if (szNamespaceUri == NULL)
          continue;

        if (axutil_strcmp(szLocalName, sLocalPart.c_str()) == 0 &&
          axutil_strcmp(szPrefix, sPrefix.c_str()) == 0 &&
          axutil_strcmp(szNamespaceUri, sNamespaceUri.c_str()) == 0)
        {
          return ConstIterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject::Iterator CDataObject::FindChildByLocalName( const std::string& sLocalName )
  {
    axiom_element_t* pElem = NULL;
    const axis2_char_t* szLocalName = NULL;

    for ( axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        szLocalName = axiom_element_get_localname(pElem, m_pEnv);
        if (szLocalName == NULL)
          continue;
        
        if (axutil_strcmp(szLocalName, sLocalName.c_str()) == 0)
        {
          return Iterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject::ConstIterator CDataObject::FindChildByLocalName( const std::string& sLocalName ) const
  {
    axiom_element_t* pElem = NULL;
    const axis2_char_t* szLocalName = NULL;

    for ( axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        szLocalName = axiom_element_get_localname(pElem, m_pEnv);
        if (szLocalName == NULL)
          continue;

        if (axutil_strcmp(szLocalName, sLocalName.c_str()) == 0)
        {
          return ConstIterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject::Iterator CDataObject::FindChildByLocalName( const std::string& sLocalName, const Iterator& itStart )
  {
    axiom_element_t* pElem = NULL;
    const axis2_char_t* szLocalName = NULL;

    RISE_ASSERTES(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this, 
      CDomNoItemException, "Итератор не пренадлежит указанному узлу данных");

    for ( axiom_node_t* pNode = itStart.m_pAxiomNode;
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        szLocalName = axiom_element_get_localname(pElem, m_pEnv);
        if (szLocalName == NULL)
          continue;

        if (axutil_strcmp(szLocalName, sLocalName.c_str()) == 0)
        {
          return Iterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject::ConstIterator CDataObject::FindChildByLocalName( const std::string& sLocalName, const ConstIterator& itStart ) const
  {
    axiom_element_t* pElem = NULL;
    const axis2_char_t* szLocalName = NULL;

    RISE_ASSERTES(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this, 
      CDomNoItemException, "Итератор не пренадлежит указанному узлу данных");

    for ( axiom_node_t* pNode = itStart.m_pAxiomNode;
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv) )
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem == NULL)
          continue;

        szLocalName = axiom_element_get_localname(pElem, m_pEnv);
        if (szLocalName == NULL)
          continue;

        if (axutil_strcmp(szLocalName, sLocalName.c_str()) == 0)
        {
          return ConstIterator(this, pNode);
        }
      }
    }
    return End();
  }

  CDataObject CDataObject::GetChildByQName(const CQName& stQName)
  {
    axiom_node_t* pNode = FindChildByQName(stQName).m_pAxiomNode;
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Элемент не найден");
    return CDataObject(pNode);
  }

  const CDataObject CDataObject::GetChildByQName(const CQName& stQName) const
  {
    axiom_node_t* pNode = FindChildByQName(stQName).m_pAxiomNode;
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Элемент не найден");
    return CDataObject(pNode);
  }

  CDataObject CDataObject::GetChildByLocalName(const std::string& sLocalName)
  {
    axiom_node_t* pNode = FindChildByLocalName(sLocalName).m_pAxiomNode;
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Элемент не найден");
    return CDataObject(pNode);
  }

  const CDataObject CDataObject::GetChildByLocalName(const std::string& sLocalName) const
  {
    axiom_node_t* pNode = FindChildByLocalName(sLocalName).m_pAxiomNode;
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Элемент не найден");
    return CDataObject(pNode);
  }


  //////////////////////////////////////////////////////////////////////////
  // управление данными

  //////////////////////////////////////////////////////////////////////////
  // управление атрибутами

  //////////////////////////////////////////////////////////////////////////
  // итерация

  CDataObject::Iterator CDataObject::Begin()
  {
    axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);

    while (pNode != NULL && 
      (axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT))
    {
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv);
    }

    return Iterator(this, pNode);
  }

  CDataObject::ConstIterator CDataObject::Begin() const
  {
    axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);

    while (pNode != NULL && 
      (axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT))
    {
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv);
    }

    return ConstIterator(this, pNode);
  }

  CDataObject::Iterator CDataObject::End()
  {
    return Iterator(this, NULL);
  }

  CDataObject::ConstIterator CDataObject::End() const
  {
    return ConstIterator(this, NULL);
  }

  //!         получить итератор на первый атрибут
  /*! \return итератор на первый атрибут
  */
  CDataObject::AttributeIterator CDataObject::AttributeBegin()
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return AttributeIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);

    return AttributeIterator(this, pIndex);
  }

  //!         получить итератор на следующий за последним атрибут
  /*! \return итератор на следующий за последним атрибут
  */
  CDataObject::AttributeIterator CDataObject::AttributeEnd()
  {
    return AttributeIterator(this, NULL);
  }

  //////////////////////////////////////////////////////////////////////////
  // операторы поддержки

  CDataObject& CDataObject::operator=( const CDataObject& rDataObject )
  {
    m_bOwner = rDataObject.m_bOwner;
    const_cast<CDataObject&>(rDataObject).m_bOwner = false;
    m_pAxiomNode = const_cast<CDataObject&>(rDataObject).m_pAxiomNode;
    m_pAxiomElement = const_cast<CDataObject&>(rDataObject).m_pAxiomElement;
    return *this;
  }

  bool CDataObject::operator==( const CDataObject& rDataObject ) const
  {
    return m_pAxiomNode == rDataObject.m_pAxiomNode &&
      m_pAxiomElement == rDataObject.m_pAxiomElement;
  }

  bool CDataObject::operator!=( const CDataObject& rDataObject ) const
  {
    return !operator==(rDataObject);
  }

  CDataObject& CDataObject::FromString( const std::string& sXml )
  {
    axiom_xml_reader_t* pAxiomXmlReader = NULL;
    axiom_stax_builder_t* pStaxBuilder = NULL;
    axiom_node_t* pAxiomNode = NULL;

    pAxiomXmlReader = axiom_xml_reader_create_for_memory(
      m_pEnv, 
      const_cast<char*>(sXml.c_str()),  // TODO: const cast!
      static_cast<int>(sXml.size()),
      static_cast<const axis2_char_t*>("UTF-8"),
      AXIS2_XML_PARSER_TYPE_BUFFER);
    RISE_ASSERTES(pAxiomXmlReader != NULL, CDomInternalException, "No Axis XML Reader");

    try
    {
      pStaxBuilder = axiom_stax_builder_create(m_pEnv, pAxiomXmlReader);
      RISE_ASSERTES(pStaxBuilder != NULL, CDomInternalException, "No Stax Builder");
    }
    catch (...)
    {
      axiom_xml_reader_free(pAxiomXmlReader, m_pEnv);
      throw;
    }

    try
    {
      axiom_document_t* pAxiomDoc = axiom_stax_builder_get_document(pStaxBuilder, m_pEnv);
      RISE_ASSERTES(pAxiomDoc != NULL, CDomFormatException, "No Axis Document");

      pAxiomNode = axiom_document_build_all(pAxiomDoc, m_pEnv);
      RISE_ASSERTES(pAxiomNode != NULL, CDomFormatException, "No Root Element in the pAxiomDoc");

    }
    catch (...)
    {
      axiom_stax_builder_free(pStaxBuilder, m_pEnv);
      throw;
    }

    try
    {
      pAxiomNode = axiom_node_detach(pAxiomNode, m_pEnv);
      Attach(pAxiomNode, true);
    }
    catch(...)
    {
      axiom_node_free_tree(pAxiomNode, m_pEnv);
      throw;
    }

    return *this;
  }

  std::string CDataObject::ToString() const
  {
    char* szXml = axiom_node_to_string(m_pAxiomNode, m_pEnv);
    RISE_ASSERTES(szXml != NULL, CDomFormatException, "Ошибка преобразования ОМ в строку");
    return szXml;
  }

  CDataObject* CDataObject::operator->()
  {
    return this;
  }

  const CDataObject* CDataObject::operator->() const
  {
    return this;
  }

  void CDataObject::AppendAttribute( CAttribute& rAttribute )
  {
    axiom_element_add_attribute(m_pAxiomElement, m_pEnv, rAttribute, m_pAxiomNode);
  }

  void CDataObject::RemoveAttribute( AttributeIterator& itAttribute )
  {
    axiom_element_remove_attribute(m_pAxiomElement, m_pEnv, *itAttribute);
  }

  CValue CDataObject::Value()
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return CValue(this);
  }

  const CValue CDataObject::Value() const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return CValue(const_cast<CDataObject*>(this));
  }

  CValue CDataObject::GetValue() const
  {
    return GetText();
  }

  void CDataObject::SetValue( const CValue& rValue )
  {
    SetText(rValue.AsString());
  }

  std::string CDataObject::GetText() const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    axis2_char_t* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);

    return szText != NULL ? szText : "";
  }

  void CDataObject::SetText( const std::string& sText )
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    axiom_element_set_text(m_pAxiomElement, m_pEnv, sText.c_str(), m_pAxiomNode);
  }

  CValue CDataObject::operator[]( const std::string& sName )
  {
    return GetChildByLocalName(sName).GetValue();
  }

  const CValue CDataObject::operator[]( const std::string& sName ) const
  {
    return GetChildByLocalName(sName).GetValue();
  }

  CDataObject CDataObject::GetOrAdd( const std::string& sName )
  {
    return CreateChildOnce(sName);
  }

  void CDataObject::RemoveChildByLocalName( const std::string& sName )
  {
    Iterator itFind = FindChildByLocalName(sName);
    if (itFind != End())
      RemoveChild(itFind);
  }

  CDataObject::Iterator CDataObject::Find( const std::string& sLocalName )
  {
    return FindChildByLocalName(sLocalName);
  }

  CDataObject::ConstIterator CDataObject::Find( const std::string& sLocalName ) const
  {
    return FindChildByLocalName(sLocalName);
  }

  const CDataObject CDataObject::operator()( const std::string& sLocalName ) const
  {
    return const_cast<CDataObject*>(this)->GetChildByLocalName(sLocalName);
  }

  CDataObject CDataObject::operator()( const std::string& sLocalName )
  {
    return GetChildByLocalName(sLocalName);
  }

  std::string CDataObject::GetNamespace() const
  {
    return GetNamespaceUri();
  }

  void CDataObject::SetNamespace(const std::string& sNamespace)
  {
    SetNamespaceUri(sNamespace);
  }

  const std::string CDataObject::Name() const
  {
    return GetLocalName();
  }

  void CDataObject::SetName( const std::string& sName )
  {
    SetLocalName(sName);
  }

  CDataObject CDataObject::Add( const std::string& sName )
  {
    return CreateChild(sName);
  }

  CDataObject CDataObject::Add( const std::string& sName, const CValue& rValue )
  {
    return CreateChild(sName, rValue);
  }

  CDataObject CDataObject::Add( CDataObject rDataObject )
  {
    return AppendChild(rDataObject);
  }

  bool CDataObject::IsInit() const
  {
    return (m_pAxiomNode != NULL && m_pAxiomElement != NULL);
  }

  axutil_env_t* CDataObject::m_pEnv = CRuntime::Inst().GetAxis2Env();


  //////////////////////////////////////////////////////////////////////////
  // Iterator

  CDataObject::Iterator::Iterator():
    m_pDataObject(NULL),
    m_pAxiomNode(NULL)
  {
  }

  CDataObject::Iterator::Iterator(CDataObject* pDataObject, axiom_node_t* pAxiomNode):
    m_pDataObject(pDataObject),
    m_pAxiomNode(pAxiomNode)
  {
  }

  CDataObject::Iterator::Iterator(const Iterator& rIter)
  {
    operator=(rIter);
  }

  CDataObject::Iterator::~Iterator()
  {
  }


  CDataObject::Iterator& CDataObject::Iterator::operator=(const Iterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAxiomNode = rIter.m_pAxiomNode;
    return *this;
  }

  CDataObject::Iterator& CDataObject::Iterator::operator--()
  {
    RISE_ASSERT(m_pDataObject);
    
    axiom_node_t* pNode = m_pAxiomNode;

    if (m_pAxiomNode == NULL) // iterator == end()
    {
      pNode = axiom_node_get_last_child(m_pDataObject->m_pAxiomNode, m_pDataObject->m_pEnv);
        
      do
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
      while (pNode != NULL && 
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));
    } else
    {
      do
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
      while (pNode != NULL && 
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));
    }
    
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Попытка выйти за первый элемент");
    m_pAxiomNode = pNode;
    return *this;
  }

  CDataObject::Iterator& CDataObject::Iterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAxiomNode != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    axiom_node_t* pNode = m_pAxiomNode;
    
    do
    {
      pNode = axiom_node_get_next_sibling(pNode, m_pDataObject->m_pEnv);
    }
    while (pNode != NULL && 
      (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));

    m_pAxiomNode = pNode;
    return *this;
  }

  CDataObject::Iterator CDataObject::Iterator::operator--(int)
  {
    Iterator itRes = *this;
    operator--();
    return itRes;
  }

  CDataObject::Iterator CDataObject::Iterator::operator++(int)
  {
    Iterator itRes = *this;
    operator++();
    return itRes;
  }

  bool CDataObject::Iterator::operator==(const Iterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject && 
           m_pAxiomNode == rIter.m_pAxiomNode;
  }

  bool CDataObject::Iterator::operator!=(const Iterator& rIter) const
  {
    return !operator==(rIter);
  }

  CDataObject CDataObject::Iterator::operator*()
  {
    return CDataObject(m_pAxiomNode);
  }

  const CDataObject CDataObject::Iterator::operator*() const
  {
    return CDataObject(m_pAxiomNode);
  }

  CDataObject CDataObject::Iterator::operator->()
  {
    return CDataObject(m_pAxiomNode);
  }

  const CDataObject CDataObject::Iterator::operator->() const
  {
    return CDataObject(m_pAxiomNode);
  }

  //////////////////////////////////////////////////////////////////////////
  // Iterator

  CDataObject::ConstIterator::ConstIterator():
    m_pDataObject(NULL),
    m_pAxiomNode(NULL)
  {
  }

  CDataObject::ConstIterator::ConstIterator(const CDataObject* pDataObject, axiom_node_t* pAxiomNode):
    m_pDataObject(pDataObject),
    m_pAxiomNode(pAxiomNode)
  {
  }

  CDataObject::ConstIterator::ConstIterator(const ConstIterator& rIter)
  {
    operator=(rIter);
  }

  CDataObject::ConstIterator::~ConstIterator()
  {
  }


  CDataObject::ConstIterator& CDataObject::ConstIterator::operator=(const ConstIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAxiomNode = rIter.m_pAxiomNode;
    return *this;
  }

  CDataObject::ConstIterator& CDataObject::ConstIterator::operator--()
  {
    RISE_ASSERT(m_pDataObject);
    
    // !! CONST HACK !!
    axiom_node_t* pNode = m_pAxiomNode;

    if (m_pAxiomNode == NULL) // iterator == end()
    {
      pNode = axiom_node_get_last_child(m_pDataObject->m_pAxiomNode, m_pDataObject->m_pEnv);
        
      do
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
      while (pNode != NULL && 
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));
    } else
    {
      do
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
      while (pNode != NULL && 
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));
    }
    
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Попытка выйти за первый элемент");
    m_pAxiomNode = pNode;
    return *this;
  }

  CDataObject::ConstIterator& CDataObject::ConstIterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAxiomNode != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    axiom_node_t* pNode = m_pAxiomNode;
    
    do
    {
      pNode = axiom_node_get_next_sibling(pNode, m_pDataObject->m_pEnv);
    }
    while (pNode != NULL && 
      (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));

    m_pAxiomNode = pNode;
    return *this;
  }

  CDataObject::ConstIterator CDataObject::ConstIterator::operator--(int)
  {
    ConstIterator itRes = *this;
    operator--();
    return itRes;
  }

  CDataObject::ConstIterator CDataObject::ConstIterator::operator++(int)
  {
    ConstIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool CDataObject::ConstIterator::operator==(const ConstIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject && 
           m_pAxiomNode == rIter.m_pAxiomNode;
  }

  bool CDataObject::ConstIterator::operator!=(const ConstIterator& rIter) const
  {
    return !operator==(rIter);
  }

  const CDataObject CDataObject::ConstIterator::operator*() const
  {
    return m_pAxiomNode;
  }

  const CDataObject CDataObject::ConstIterator::operator->() const
  {
    return m_pAxiomNode;
  }

  //////////////////////////////////////////////////////////////////////////
  // AttributeIterator

  CDataObject::AttributeIterator::AttributeIterator():
    m_pDataObject(NULL),
    m_pAttributeIndex(NULL)
  {
  }

  CDataObject::AttributeIterator::AttributeIterator(CDataObject* pDataObject, axutil_hash_index_t* pAttributeIndex):
    m_pDataObject(pDataObject),
    m_pAttributeIndex(pAttributeIndex)
  {
  }

  CDataObject::AttributeIterator::AttributeIterator(const CDataObject::AttributeIterator& rIter)
  {
    operator=(rIter);
  }

  CDataObject::AttributeIterator::~AttributeIterator()
  {
  }

  CDataObject::AttributeIterator& CDataObject::AttributeIterator::operator=(const CDataObject::AttributeIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAttributeIndex = rIter.m_pAttributeIndex;
    return *this;
  }

  CDataObject::AttributeIterator& CDataObject::AttributeIterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    m_pAttributeIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pAttributeIndex);
    return *this;
  }

  CDataObject::AttributeIterator CDataObject::AttributeIterator::operator++(int)
  {
    AttributeIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool CDataObject::AttributeIterator::operator==(const CDataObject::AttributeIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pAttributeIndex == rIter.m_pAttributeIndex;
  }

  bool CDataObject::AttributeIterator::operator!=(const CDataObject::AttributeIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  CAttribute CDataObject::AttributeIterator::operator*()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Невозможно получить атрибут");

    CAttribute tAttr(m_pDataObject, reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  CAttribute CDataObject::AttributeIterator::operator->()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Невозможно получить атрибут");

    CAttribute tAttr(m_pDataObject, reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  //////////////////////////////////////////////////////////////////////////
  // ConstAttributeIterator

  CDataObject::ConstAttributeIterator::ConstAttributeIterator():
    m_pDataObject(NULL),
    m_pAttributeIndex(NULL)
  {
  }

  CDataObject::ConstAttributeIterator::ConstAttributeIterator(const CDataObject* pDataObject, axutil_hash_index_t* pAttributeIndex):
    m_pDataObject(pDataObject),
    m_pAttributeIndex(pAttributeIndex)
  {
  }

  CDataObject::ConstAttributeIterator::ConstAttributeIterator(const CDataObject::ConstAttributeIterator& rIter)
  {
    operator=(rIter);
  }

  CDataObject::ConstAttributeIterator::~ConstAttributeIterator()
  {
  }

  CDataObject::ConstAttributeIterator& CDataObject::ConstAttributeIterator::operator=(const CDataObject::ConstAttributeIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAttributeIndex = rIter.m_pAttributeIndex;
    return *this;
  }

  CDataObject::ConstAttributeIterator& CDataObject::ConstAttributeIterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    m_pAttributeIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pAttributeIndex);
    return *this;
  }

  CDataObject::ConstAttributeIterator CDataObject::ConstAttributeIterator::operator++(int)
  {
    ConstAttributeIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool CDataObject::ConstAttributeIterator::operator==(const CDataObject::ConstAttributeIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pAttributeIndex == rIter.m_pAttributeIndex;
  }

  bool CDataObject::ConstAttributeIterator::operator!=(const CDataObject::ConstAttributeIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  const CAttribute CDataObject::ConstAttributeIterator::operator*() const
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Невозможно получить атрибут");

    //!!! const HACK
    CAttribute tAttr(const_cast<CDataObject*>(m_pDataObject), reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  const CAttribute CDataObject::ConstAttributeIterator::operator->() const
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Итератор не итерируемый");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Попытка выйти за последний элемент");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Невозможно получить атрибут");

    CAttribute tAttr(const_cast<CDataObject*>(m_pDataObject), reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  CDataObject& operator<<( CDataObject& rDataObject, const CValue& rValue )
  {
    rDataObject.SetValue(rValue);
    return rDataObject;
  }

  const CDataObject& operator>>( const CDataObject& rDataObject, CValue& rValue )
  {
    rValue = rDataObject.Value();
    return rDataObject;
  }
}
