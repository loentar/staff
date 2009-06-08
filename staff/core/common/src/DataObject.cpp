#include <axutil_utils.h>
#include <axiom.h>
#include <axiom_node.h>
#include <axiom_element.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include "Exception.h"
#include "Runtime.h"
#include "QName.h"
#include "Namespace.h"
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

  axiom_node_t* CDataObject::Clone(axiom_node_t* pNodeIn, axiom_node_t* pNodeOutParent)
  {
    axiom_types_t tNodeType = axiom_node_get_node_type(pNodeIn, m_pEnv);
    axiom_node_t* pNodeOut = NULL;
    
    switch (tNodeType)
    {
      case AXIOM_ELEMENT:
      {
        axiom_element_t* pElemIn = 
          reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNodeIn, m_pEnv));
        axis2_char_t* szLocalName = axiom_element_get_localname(pElemIn, m_pEnv);
        axiom_namespace_t* pNsIn = axiom_element_get_namespace(pElemIn, m_pEnv, pNodeIn);
        axiom_namespace_t* pNsOut = NULL;

        axiom_element_t* pElemOut = axiom_element_create(m_pEnv, pNodeOutParent, szLocalName, 
          NULL, &pNodeOut);

        // clone attributes
        axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(pElemIn, m_pEnv);
        if (pAttrHash != NULL)
        {
          void* pHashValue = NULL;

          for (axutil_hash_index_t* pAttrIndex = axutil_hash_first(pAttrHash, m_pEnv);
            pAttrIndex != NULL; 
            pAttrIndex = axutil_hash_next(m_pEnv, pAttrIndex))
          {
            axutil_hash_this(pAttrIndex, NULL, NULL, &pHashValue);
            if (pHashValue != NULL)
            {
              axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
              axiom_element_add_attribute(pElemOut, m_pEnv, axiom_attribute_clone(pAttr, m_pEnv), pNodeOut);
            }
          }
        }

        // clone namespaces
        axutil_hash_t* pNsHash = axiom_element_get_namespaces(pElemIn, m_pEnv);
        if (pNsHash != NULL)
        {
          axiom_namespace_t* pNs = NULL;
          axiom_namespace_t* pNsClone = NULL;
          void* pHashValue = NULL;

          for (axutil_hash_index_t* pNsIndex = axutil_hash_first(pNsHash, m_pEnv);
            pNsIndex != NULL; 
            pNsIndex = axutil_hash_next(m_pEnv, pNsIndex))
          {
            axutil_hash_this(pNsIndex, NULL, NULL, &pHashValue);
            if (pHashValue != NULL)
            {
              pNs = reinterpret_cast<axiom_namespace_t*>(pHashValue);
              pNsClone = axiom_namespace_clone(pNs, m_pEnv);
              axiom_element_declare_namespace(pElemOut, m_pEnv, pNodeOut, pNsClone);
              if (pNs == pNsIn)
              {
                pNsOut = pNsClone;
              }
            }
          }
        }

        // set current namespace
        // if current namespace is owned by element
        if (pNsOut != NULL)
        {
          axiom_element_set_namespace(pElemOut, m_pEnv, pNsOut, pNodeOut);
        }
        else
        if (pNsIn != NULL)
        { // find namespace in parents
          axis2_char_t* szPrefix = axiom_namespace_get_prefix(pNsIn, m_pEnv);
          axis2_char_t* szUri = axiom_namespace_get_uri(pNsIn, m_pEnv);
          pNsOut = axiom_element_find_namespace(pElemOut, m_pEnv, pNodeOut, szUri, szPrefix);
          axiom_element_set_namespace(pElemOut, m_pEnv, pNsOut, pNodeOut);
        }

        // clone subnodes
        for (axiom_node_t* pItNode = axiom_node_get_first_child(pNodeIn, m_pEnv);
          pItNode != NULL;
          pItNode = axiom_node_get_next_sibling(pItNode, m_pEnv))
        {
          Clone(pItNode, pNodeOut);
        }

        break;
      }

      case AXIOM_DOCTYPE:
      {
        axiom_doctype_t* pDocTypeIn = 
          reinterpret_cast<axiom_doctype_t*>(axiom_node_get_data_element(pNodeIn, m_pEnv));

        if (pDocTypeIn != NULL)
        {
          axiom_doctype_create(m_pEnv, pNodeOutParent, 
            axiom_doctype_get_value(pDocTypeIn, m_pEnv), &pNodeOut);
        }
        
        break;
      }

      case AXIOM_COMMENT:
      {
        axiom_comment_t* pCommentIn = 
          reinterpret_cast<axiom_comment_t*>(axiom_node_get_data_element(pNodeIn, m_pEnv));
        if (pCommentIn != NULL)
        {
          axiom_comment_create(m_pEnv, pNodeOutParent, 
              axiom_comment_get_value(pCommentIn, m_pEnv), &pNodeOut);
        }

        break;
      }

      case AXIOM_PROCESSING_INSTRUCTION:
      {
        axiom_processing_instruction_t* pProcInstrIn = 
          reinterpret_cast<axiom_processing_instruction_t*>(axiom_node_get_data_element(pNodeIn, m_pEnv));

        if (pProcInstrIn != NULL)
        {
          axiom_processing_instruction_create(m_pEnv, pNodeOutParent, 
            axiom_processing_instruction_get_target(pProcInstrIn, m_pEnv),
            axiom_processing_instruction_get_value(pProcInstrIn, m_pEnv),
            &pNodeOut);
        }

        break;
      }

      case AXIOM_TEXT:
      {
        axiom_text_t* pTextIn = 
          reinterpret_cast<axiom_text_t*>(axiom_node_get_data_element(pNodeIn, m_pEnv));
        if (pTextIn != NULL)
        {
          axiom_text_t* pTextOut = axiom_text_create(m_pEnv, pNodeOutParent, 
            axiom_text_get_text(pTextIn, m_pEnv), &pNodeOut);

          axiom_text_set_content_id(pTextOut, m_pEnv, axiom_text_get_content_id(pTextIn, m_pEnv));
        }
        break;
      }

      case AXIOM_DATA_SOURCE:
      {
        // axiom handle CDATA as TEXT?
        break;
      }
      
      default:
      {
      }
    }

    return pNodeOut;
  }

  CDataObject& CDataObject::Clone( const CDataObject& rDataObject )
  {
    axiom_node_t* pNode = Clone(const_cast<CDataObject&>(rDataObject).m_pAxiomNode, NULL);
    Attach(pNode);
    return *this;
  }

  CDataObject CDataObject::Clone() const
  {
    CDataObject tdoClone;
    return tdoClone.Clone(*this);
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

  CDataObject CDataObject::AppendChild( const CDataObject& rDataObject )
  {
    RISE_ASSERT(m_pAxiomNode);

    axiom_node_add_child(m_pAxiomNode, m_pEnv, rDataObject.Clone().m_pAxiomNode);
    
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

  CDataObject::NamespaceIterator CDataObject::NamespaceBegin()
  {
    axutil_hash_t* pNsHash = axiom_element_get_namespaces(m_pAxiomElement, m_pEnv);
    if (pNsHash == NULL)
    {
      return NamespaceIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pNsHash, m_pEnv);

    return NamespaceIterator(this, pIndex);
  }

  CDataObject::NamespaceIterator CDataObject::NamespaceEnd()
  {
    return NamespaceIterator(this, NULL);
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

  CNamespace CDataObject::GetDefaultNamespace() const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return axiom_element_get_default_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode);
  }

  void CDataObject::DeclareDefaultNamespace(const std::string& sUri)
  {
    RISE_ASSERT(m_pAxiomElement);
    axiom_element_declare_default_namespace(m_pAxiomElement, m_pEnv, const_cast<axis2_char_t*>(sUri.c_str()));
  }

  CNamespace CDataObject::GetNamespace() const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode);
  }

  void CDataObject::DeclareNamespace(CNamespace& rNamespace)
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    axiom_element_declare_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, rNamespace);
    rNamespace.SetDataObject(this);
  }

  void CDataObject::SetNamespace(CNamespace& rNamespace)
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, rNamespace, m_pAxiomNode);
    rNamespace.SetDataObject(this);
  }

  CNamespace CDataObject::FindNamespace( const std::string& sUri )
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, sUri.c_str(), NULL);
  }

  CNamespace CDataObject::FindNamespace(const std::string& sUri, const std::string& sPrefix)
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, sUri.c_str(), sPrefix.c_str());
  }

  CNamespace CDataObject::FindDeclaredNamespace(const std::string& sUri)
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return axiom_element_find_declared_namespace(m_pAxiomElement, m_pEnv, sUri.c_str(), NULL);
  }

  CNamespace CDataObject::FindDeclaredNamespace(const std::string& sUri, const std::string& sPrefix)
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return axiom_element_find_declared_namespace(m_pAxiomElement, m_pEnv, sUri.c_str(), sPrefix.c_str());
  }

  CNamespace CDataObject::FindNamespaceUri(const std::string& sPrefix) const
  {
    RISE_ASSERT(m_pAxiomNode);
    RISE_ASSERT(m_pAxiomElement);
    return axiom_element_find_namespace_uri(m_pAxiomElement, m_pEnv, 
      const_cast<axis2_char_t*>(sPrefix.c_str()), m_pAxiomNode);
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
  // NamespaceIterator

  CDataObject::NamespaceIterator::NamespaceIterator():
    m_pDataObject(NULL),
    m_pNamespaceIndex(NULL)
  {
  }

  CDataObject::NamespaceIterator::NamespaceIterator(CDataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex):
    m_pDataObject(pDataObject),
    m_pNamespaceIndex(pNamespaceIndex)
  {
  }

  CDataObject::NamespaceIterator::NamespaceIterator(const CDataObject::NamespaceIterator& rIter)
  {
    operator=(rIter);
  }

  CDataObject::NamespaceIterator::~NamespaceIterator()
  {
  }

  CDataObject::NamespaceIterator& CDataObject::NamespaceIterator::operator=(const CDataObject::NamespaceIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pNamespaceIndex = rIter.m_pNamespaceIndex;
    return *this;
  }

  CDataObject::NamespaceIterator& CDataObject::NamespaceIterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pNamespaceIndex != NULL, CDomNoItemException, "++ operator after end");

    m_pNamespaceIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pNamespaceIndex);
    return *this;
  }

  CDataObject::NamespaceIterator CDataObject::NamespaceIterator::operator++(int)
  {
    NamespaceIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool CDataObject::NamespaceIterator::operator==(const CDataObject::NamespaceIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pNamespaceIndex == rIter.m_pNamespaceIndex;
  }

  bool CDataObject::NamespaceIterator::operator!=(const CDataObject::NamespaceIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  CNamespace CDataObject::NamespaceIterator::operator*()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pNamespaceIndex != NULL, CDomNoItemException, "* operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Cannot get namespace");

    CNamespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), m_pDataObject);
    return tNs;
  }

  CNamespace CDataObject::NamespaceIterator::operator->()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pNamespaceIndex != NULL, CDomNoItemException, "-> operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Cannot get namespace");

    CNamespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), m_pDataObject);
    return tNs;
  }

  //////////////////////////////////////////////////////////////////////////
  // ConstNamespaceIterator

  CDataObject::ConstNamespaceIterator::ConstNamespaceIterator():
    m_pDataObject(NULL),
    m_pNamespaceIndex(NULL)
  {
  }

  CDataObject::ConstNamespaceIterator::ConstNamespaceIterator(CDataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex):
    m_pDataObject(pDataObject),
    m_pNamespaceIndex(pNamespaceIndex)
  {
  }

  CDataObject::ConstNamespaceIterator::ConstNamespaceIterator(const CDataObject::ConstNamespaceIterator& rIter)
  {
    operator=(rIter);
  }

  CDataObject::ConstNamespaceIterator::~ConstNamespaceIterator()
  {
  }

  CDataObject::ConstNamespaceIterator& CDataObject::ConstNamespaceIterator::operator=(const CDataObject::ConstNamespaceIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pNamespaceIndex = rIter.m_pNamespaceIndex;
    return *this;
  }

  CDataObject::ConstNamespaceIterator& CDataObject::ConstNamespaceIterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pNamespaceIndex != NULL, CDomNoItemException, "++ operator after end");

    m_pNamespaceIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pNamespaceIndex);
    return *this;
  }

  CDataObject::ConstNamespaceIterator CDataObject::ConstNamespaceIterator::operator++(int)
  {
    ConstNamespaceIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool CDataObject::ConstNamespaceIterator::operator==(const CDataObject::ConstNamespaceIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pNamespaceIndex == rIter.m_pNamespaceIndex;
  }

  bool CDataObject::ConstNamespaceIterator::operator!=(const CDataObject::ConstNamespaceIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  const CNamespace CDataObject::ConstNamespaceIterator::operator*() const
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pNamespaceIndex != NULL, CDomNoItemException, "* operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Cannot get namespace");

    CNamespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), const_cast<CDataObject*>(m_pDataObject));
    return tNs;
  }

  const CNamespace CDataObject::ConstNamespaceIterator::operator->() const
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pNamespaceIndex != NULL, CDomNoItemException, "-> operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Cannot get namespace");

    CNamespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), const_cast<CDataObject*>(m_pDataObject));
    return tNs;
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
