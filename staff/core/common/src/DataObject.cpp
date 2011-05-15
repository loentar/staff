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

#include <axutil_utils.h>
#include <axiom.h>
#include <axiom_node.h>
#include <axiom_element.h>
#include <axiom_attribute.h>
#include <axiom_namespace.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <staff/utils/tocstring.h>
#include <staff/utils/fromcstring.h>
#include "Exception.h"
#include "Runtime.h"
#include "QName.h"
#include "Namespace.h"
#include "Attribute.h"
#include "Value.h"
#include "DataObject.h"

namespace staff
{
  enum { NUM_TO_STR_BUFF_SIZE = 32 };

  CDataObject::CDataObject(axiom_node_t* pAxiomNode /*= NULL*/):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    if (pAxiomNode)
    {
      Attach(pAxiomNode);
    }
  }

  CDataObject::CDataObject(const char* szLocalName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName);
  }

  CDataObject::CDataObject(const std::string& sLocalName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str());
  }

  CDataObject::CDataObject(const CQName& stQName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(stQName);
  }

  CDataObject::CDataObject(const CDataObject& rDataObject):
    m_pAxiomNode(const_cast<CDataObject&>(rDataObject).m_pAxiomNode),
    m_pAxiomElement(const_cast<CDataObject&>(rDataObject).m_pAxiomElement),
    m_bOwner(rDataObject.m_bOwner)
  {
    const_cast<CDataObject&>(rDataObject).m_bOwner = false;
  }

  CDataObject::CDataObject(const char* szLocalName, bool bValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, byte btValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, int nValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, short shValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, long lValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, long long llValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, unsignedByte ubtValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, unsigned int unValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, unsigned short ushValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, unsigned long ulValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, unsigned long long ullValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, float fValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, double dValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }


  CDataObject::CDataObject(const std::string& sLocalName, bool bValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, byte btValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, int nValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, short shValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, long lValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, long long llValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, unsignedByte ubtValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, unsigned int unValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, unsigned short ushValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, unsigned long ulValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, unsigned long long ullValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, float fValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const std::string& sLocalName, double dValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  CDataObject::CDataObject(const char* szLocalName, const char* szText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName, szText);
  }

  CDataObject::CDataObject(const std::string& sLocalName, const char* szText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str(), szText);
  }

  CDataObject::CDataObject(const char* szLocalName, const std::string& sText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName, sText.c_str());
  }

  CDataObject::CDataObject(const std::string& sLocalName, const std::string& sText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str(), sText.c_str());
  }

  CDataObject::CDataObject(const char* szLocalName, const CValue& rValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName, rValue.AsString().c_str());
  }

  CDataObject::CDataObject(const std::string& sLocalName, const CValue& rValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str(), rValue.AsString().c_str());
  }

  CDataObject::CDataObject(axiom_node_t* pAxiomNode, axiom_element_t* pAxiomElement):
    m_pAxiomNode(pAxiomNode),
    m_pAxiomElement(pAxiomElement),
    m_bOwner(false)
  {
  }

  CDataObject::~CDataObject()
  {
    Detach();
  }

  void CDataObject::Attach(axiom_node_t* pAxiomNode, bool bOwner /*= false*/)
  {
    Detach();

    RISE_ASSERTP(pAxiomNode);
    RISE_ASSERTES(axiom_node_get_node_type(pAxiomNode, m_pEnv) == AXIOM_ELEMENT, 
      CDomTypeException, "Given node is not a element");

    axiom_element_t* pAxiomElement = 
      reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pAxiomNode, m_pEnv));
    RISE_ASSERTES(pAxiomElement != NULL, CDomNoItemException, "Can\'t get data element");

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
    else
    {
      m_pAxiomNode = NULL;
      m_pAxiomElement = NULL;
    }
  }

  bool CDataObject::IsOwner() const
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

  CDataObject::operator axiom_element_t*()
  {
    return m_pAxiomElement;
  }

  bool CDataObject::IsNull() const
  {
    return (m_pAxiomNode == NULL || m_pAxiomElement == NULL);
  }

  bool CDataObject::IsInit() const
  {
    return (m_pAxiomNode != NULL && m_pAxiomElement != NULL);
  }

  //////////////////////////////////////////////////////////////////////////
  // Node properties

  CQName CDataObject::GetQName()
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Can\'t get node's QName");

    CQName tqName(pqName);
    return tqName;
  }

  void CDataObject::SetQName(const CQName& stQName) const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_element_set_localname(m_pAxiomElement, m_pEnv, stQName.GetLocalPart().c_str());

    axiom_namespace_t* pNewNamespace = axiom_namespace_create(m_pEnv, stQName.GetNamespaceUri().c_str(), 
      stQName.GetPrefix().c_str());
    RISE_ASSERTES(pNewNamespace != NULL, CDomFormatException, "Can\'t create namespace");

    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNewNamespace, m_pAxiomNode);
  }

  void CDataObject::GetLocalName(std::string& sLocalName) const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    const axis2_char_t* szLocalName = axiom_element_get_localname(m_pAxiomElement, m_pEnv);

    RISE_ASSERTES(szLocalName != NULL, CDomFormatException, "Can\'t get local name");

    sLocalName = szLocalName;
  }

  std::string CDataObject::GetLocalName() const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    const axis2_char_t* szLocalName = axiom_element_get_localname(m_pAxiomElement, m_pEnv);

    RISE_ASSERTES(szLocalName != NULL, CDomFormatException, "Can\'t get local name");
    
    return szLocalName;
  }

  void CDataObject::SetLocalName(const char* szLocalName) const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axis2_status_t nResult =
        axiom_element_set_localname(m_pAxiomElement, m_pEnv, szLocalName);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "Can't set local name")
  }

  void CDataObject::SetLocalName(const std::string& sLocalName) const
  {
    SetLocalName(sLocalName.c_str());
  }

  void CDataObject::GetPrefix(std::string& sPrefix) const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName =
        axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Can\'t get node's QName");

    const axis2_char_t* szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);

    sPrefix = szPrefix ? szPrefix : "";
  }

  std::string CDataObject::GetPrefix() const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Can\'t get node's QName");

    const axis2_char_t* szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);

    return szPrefix == NULL ? "" : szPrefix;
  }

  void CDataObject::SetPrefix(const char* szPrefix)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_namespace_t* pNewNamespace = NULL;
    axiom_namespace_t* pOldNamespace =
        axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    const char* szUri = NULL;

    if (pOldNamespace != NULL)
    {
      szUri = axiom_namespace_get_uri(pOldNamespace, m_pEnv);
    }

    pNewNamespace = axiom_namespace_create(m_pEnv, szUri != NULL ? szUri : "", szPrefix);
    RISE_ASSERTES(pNewNamespace != NULL, CDomFormatException, "Can\'t create namespace");

    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNewNamespace, m_pAxiomNode);
  }

  void CDataObject::SetPrefix(const std::string& sPrefix)
  {
    SetPrefix(sPrefix.c_str());
  }

  void CDataObject::GetNamespaceUri(std::string& sNamespaceUri) const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName =
        axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Can\'t get node's QName");

    const axis2_char_t* szUri = axutil_qname_get_uri(pqName, m_pEnv);

    sNamespaceUri = szUri ? szUri : "";
  }

  std::string CDataObject::GetNamespaceUri() const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName =
        axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTES(pqName != NULL, CDomFormatException, "Can\'t get node's QName");

    const axis2_char_t* szUri = axutil_qname_get_uri(pqName, m_pEnv);

    return szUri != NULL ? szUri : "";
  }


  void CDataObject::SetNamespaceUri(const char* szUri) const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_namespace_t* pNamespace = axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    if (!pNamespace)
    {
      pNamespace = axiom_namespace_create(m_pEnv, szUri, "");
      RISE_ASSERTES(pNamespace, CDomInternalException, "Can't create axiom namespace");
      axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNamespace, m_pAxiomNode);
    }
    else
    {
      axutil_string_t* pStrUri = axutil_string_create(m_pEnv, szUri);
      RISE_ASSERTES(pStrUri, CDomInternalException, "Can't create axuil_string");

      axis2_status_t nResult = axiom_namespace_set_uri_str(pNamespace, m_pEnv, pStrUri);
      if (nResult != AXIS2_SUCCESS)
      {
        axutil_string_free(pStrUri, m_pEnv);
        RISE_THROWS(CDomInternalException, "Failed to axiom_namespace_set_uri_str");
      }

      axutil_string_free(pStrUri, m_pEnv);
    }
  }

  void CDataObject::SetNamespaceUri(const std::string& sUri) const
  {
    SetNamespaceUri(sUri.c_str());
  }

  //////////////////////////////////////////////////////////////////////////
  // node management

  void CDataObject::Create()
  {
    Detach();

    m_pAxiomElement = axiom_element_create(m_pEnv, NULL, "", NULL, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      RISE_THROWS(CDomInternalException, "Failed to create axiom element");
    }

    m_bOwner = true;
  }

  void CDataObject::Create(const char* szLocalName)
  {
    Detach();

    m_pAxiomElement = axiom_element_create(m_pEnv, NULL, szLocalName, NULL, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      RISE_THROWS(CDomInternalException, "Failed to create axiom element");
    }

    m_bOwner = true;
  }

  void CDataObject::Create(const std::string& sLocalName)
  {
    Create(sLocalName.c_str());
  }

  void CDataObject::Create(const CQName& rQName)
  {
    Detach();

    m_pAxiomElement = axiom_element_create_with_qname(m_pEnv, NULL, rQName, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      RISE_THROWS(CDomInternalException, "Failed to create axiom element");
    }

    m_bOwner = true;
  }


  void CDataObject::Create(const char* szLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void CDataObject::Create(const char* szLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }


  void CDataObject::Create(const std::string& sLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void CDataObject::Create(const std::string& sLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }


  void CDataObject::Create(const char* szLocalName, const char* szText)
  {
    Detach();

    m_pAxiomElement = axiom_element_create(m_pEnv, NULL, szLocalName, NULL, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      RISE_THROWS(CDomInternalException, "Failed to create axiom element");
    }

    m_bOwner = true;

    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, szText, m_pAxiomNode);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "can't set text");
  }

  void CDataObject::Create(const std::string& sLocalName, const char* szText)
  {
    Create(sLocalName.c_str(), szText);
  }

  void CDataObject::Create(const char* szLocalName, const std::string& sText)
  {
    Create(szLocalName, sText.c_str());
  }

  void CDataObject::Create(const std::string& sLocalName, const std::string& sText)
  {
    Create(sLocalName.c_str(), sText.c_str());
  }

  void CDataObject::Create(const std::string& sLocalName, const CValue& rValue)
  {
    Create(sLocalName.c_str(), rValue.AsString().c_str());
  }

  void CDataObject::Free()
  {
    if (m_pAxiomNode != NULL)
    {
      axiom_node_free_tree(m_pAxiomNode, m_pEnv);
      m_pAxiomNode = NULL;
      m_pAxiomElement = NULL;
    }
  }

  CDataObject& CDataObject::DetachNode()
  {
    RISE_ASSERTS(m_pAxiomNode != NULL, "Not initialized");

    RISE_ASSERTES(axiom_node_detach(m_pAxiomNode, m_pEnv), CDomNoItemException,
      "Error while node detaching");
    m_bOwner = true;
    return *this;
  }

  CDataObject& CDataObject::ReplaceNode(CDataObject& rNewNode)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL, "Not initialized");
    axiom_node_t* pParentNode = axiom_node_get_parent(m_pAxiomNode, m_pEnv);

    DetachNode();
    Free();

    Attach(rNewNode);

    if (pParentNode != NULL)
    {
      axiom_node_add_child(pParentNode, m_pEnv, rNewNode.m_pAxiomNode);
    }

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

  CDataObject& CDataObject::Clone(const CDataObject& rDataObject)
  {
    RISE_ASSERTS(rDataObject.m_pAxiomNode != NULL && rDataObject.m_pAxiomElement != NULL, "Not initialized");

    axiom_node_t* pNode = Clone(const_cast<CDataObject&>(rDataObject).m_pAxiomNode, NULL);
    Attach(pNode);
    return *this;
  }

  CDataObject CDataObject::Clone() const
  {
    CDataObject tdoClone;
    return tdoClone.Clone(*this);
  }

  CDataObject CDataObject::Parent()
  {
    RISE_ASSERTS(m_pAxiomNode, "Not initialized");
    return axiom_node_get_parent(m_pAxiomNode, m_pEnv);
  }

  CDataObject CDataObject::NextSibling()
  {
    axiom_node_t* pNode = NULL;
    while ((pNode = axiom_node_get_next_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT);
    return pNode;
  }

  CDataObject CDataObject::PreviousSibling()
  {
    axiom_node_t* pNode = NULL;
    while ((pNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT);
    return pNode;
  }

  void CDataObject::SetNextSibling()
  {
    while ((m_pAxiomNode = axiom_node_get_next_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(m_pAxiomNode, m_pEnv) != AXIOM_ELEMENT);

    m_pAxiomElement = m_pAxiomNode ?
          reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(m_pAxiomNode, m_pEnv)) :
          NULL;
  }

  void CDataObject::SetPreviousSibling()
  {
    while ((m_pAxiomNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(m_pAxiomNode, m_pEnv) != AXIOM_ELEMENT);

    m_pAxiomElement = m_pAxiomNode ?
          reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(m_pAxiomNode, m_pEnv)) :
          NULL;
  }

  //////////////////////////////////////////////////////////////////////////
  // child nodes management

  CDataObject CDataObject::FirstChild()
  {
    axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
    while (pNode != NULL && axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT)
    {
      pNode = axiom_node_get_next_sibling(m_pAxiomNode, m_pEnv);
    }

    return pNode;
  }

  const CDataObject CDataObject::FirstChild() const
  {
    axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
    while (pNode != NULL && axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT)
    {
      pNode = axiom_node_get_next_sibling(m_pAxiomNode, m_pEnv);
    }

    return pNode;
  }

  CDataObject CDataObject::LastChild()
  {
    axiom_node_t* pNode = axiom_node_get_last_child(m_pAxiomNode, m_pEnv);
    while (pNode != NULL && axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT)
    {
      pNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv);
    }

    return pNode;
  }

  const CDataObject CDataObject::LastChild() const
  {
    axiom_node_t* pNode = axiom_node_get_last_child(m_pAxiomNode, m_pEnv);
    while (pNode != NULL && axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT)
    {
      pNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv);
    }

    return pNode;
  }

  CDataObject CDataObject::CreateChild()
  {
    axiom_node_t* pAxiomNode = NULL;
    axiom_element_t* pAxiomElement = axiom_element_create(m_pEnv, NULL, "", NULL, &pAxiomNode);
    RISE_ASSERTES(pAxiomElement, CDomInternalException, "Failed to create axiom element");

    axiom_node_add_child(m_pAxiomNode, m_pEnv, pAxiomNode);

    return CDataObject(pAxiomNode, pAxiomElement);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName)
  {
    axiom_node_t* pAxiomNode = NULL;
    axiom_element_t* pAxiomElement = axiom_element_create(m_pEnv, m_pAxiomNode, szLocalName, NULL, &pAxiomNode);
    RISE_ASSERTES(pAxiomElement, CDomInternalException, "Failed to create axiom element");

    return CDataObject(pAxiomNode, pAxiomElement);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName)
  {
    return CreateChild(sLocalName.c_str());
  }

  CDataObject CDataObject::CreateChild(const CQName& rQName)
  {
    CDataObject tdoChild(rQName);
    return AppendChild(tdoChild);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }


  CDataObject CDataObject::CreateChild(const std::string& sLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }



  CDataObject CDataObject::CreateChild(const char* szLocalName, const char* szText)
  {
    RISE_ASSERTS(m_pAxiomNode, "Not initialized");

    axiom_node_t* pAxiomNode = NULL;
    axiom_element_t* pAxiomElement = axiom_element_create(m_pEnv, m_pAxiomNode, szLocalName, NULL, &pAxiomNode);
    RISE_ASSERTES(pAxiomElement, CDomInternalException, "Failed to create axiom element");

    axis2_status_t nResult =
        axiom_element_set_text(pAxiomElement, m_pEnv, szText, pAxiomNode);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "can't set text");

    return CDataObject(pAxiomNode, pAxiomElement);
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, const char* szText)
  {
    return CreateChild(sLocalName.c_str(), szText);
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, const std::string& sText)
  {
    return CreateChild(szLocalName, sText.c_str());
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, const std::string& sText)
  {
    return CreateChild(sLocalName.c_str(), sText.c_str());
  }

  CDataObject CDataObject::CreateChild(const char* szLocalName, const CValue& rValue)
  {
    return CreateChild(szLocalName, rValue.AsString().c_str());
  }

  CDataObject CDataObject::CreateChild(const std::string& sLocalName, const CValue& rValue)
  {
    return CreateChild(sLocalName.c_str(), rValue.AsString().c_str());
  }

  CDataObject CDataObject::CreateChildOnce(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElem = NULL;

    if (FindChildByLocalName(szLocalName, &pNode, &pElem))
    {
      return CDataObject(pNode, pElem);
    }

    return CreateChild(szLocalName);
  }

  CDataObject CDataObject::CreateChildOnce(const std::string& sLocalName)
  {
    return CreateChildOnce(sLocalName.c_str());
  }

  CDataObject CDataObject::AppendChild(CDataObject& rDataObject)
  {
    RISE_ASSERTS(m_pAxiomNode, "Not initialized");
    RISE_ASSERTS(rDataObject.m_pAxiomNode, "Child is not initialized");

    axiom_node_add_child(m_pAxiomNode, m_pEnv, rDataObject.m_pAxiomNode);
    rDataObject.m_bOwner = false;
    
    return rDataObject;
  }

  CDataObject CDataObject::AppendChild(const CDataObject& rDataObject)
  {
    RISE_ASSERTS(m_pAxiomNode, "Not initialized");
    RISE_ASSERTS(rDataObject.m_pAxiomNode, "Child is not initialized");

    axiom_node_add_child(m_pAxiomNode, m_pEnv, const_cast<CDataObject&>(rDataObject).m_pAxiomNode);
    const_cast<CDataObject&>(rDataObject).m_bOwner = false;

    return rDataObject;
  }

  CDataObject CDataObject::DetachChild(Iterator& itChild)
  {
    RISE_ASSERTES(itChild.m_pDataObject != NULL && *(itChild.m_pDataObject) == *this, 
      CDomNoItemException, "Iterator is not bound to current dataobject");
    RISE_ASSERTES(itChild.m_pAxiomNode != NULL, CDomNoItemException, "Iterator == End()");

    return CDataObject(axiom_node_detach(itChild.m_pAxiomNode, m_pEnv));
  }

  void CDataObject::RemoveChild(Iterator& itChild)
  {
    RISE_ASSERTES(itChild.m_pDataObject != NULL && *(itChild.m_pDataObject) == *this, 
      CDomNoItemException, "Iterator is not bound to current dataobject");
    RISE_ASSERTES(itChild.m_pAxiomNode != NULL, CDomNoItemException, "Iterator == End()");

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

  CDataObject::Iterator CDataObject::FindChildByQName(const CQName& rQName)
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    for (axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv))
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

  CDataObject::ConstIterator CDataObject::FindChildByQName(const CQName& rQName) const
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    for (axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv))
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

  CDataObject::Iterator CDataObject::FindChildByQName(const CQName& rQName, const Iterator& itStart)
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
      CDomNoItemException, "Iterator is not bound to current dataobject");

    for (axiom_node_t* pNode = itStart.m_pAxiomNode;
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv))
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

  CDataObject::ConstIterator CDataObject::FindChildByQName(const CQName& rQName, const ConstIterator& itStart) const
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
      CDomNoItemException, "Iterator is not bound to current dataobject");

    for (axiom_node_t* pNode = itStart.m_pAxiomNode;
      pNode != NULL;
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv))
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

  bool CDataObject::FindChildByLocalName(const char* szLocalName, axiom_node_t** ppNode, axiom_element_t** ppElement) const
  {
    RISE_ASSERTP(ppNode != NULL);

    axiom_element_t* pElem = NULL;
    const axis2_char_t* szCurrentLocalName = NULL;

    for (axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
      pNode; pNode = axiom_node_get_next_sibling(pNode, m_pEnv))
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_ELEMENT)
      {
        pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNode, m_pEnv));
        if (pElem)
        {
          szCurrentLocalName = axiom_element_get_localname(pElem, m_pEnv);
          if (szCurrentLocalName)
          {
            if (!axutil_strcmp(szCurrentLocalName, szLocalName))
            {
              *ppNode = pNode;
              if (ppElement)
              {
                *ppElement = pElem;
              }
              return true;
            }
          }
        }
      }
    }

    return false;
  }

  bool CDataObject::FindChildByLocalName(const char* szLocalName, axiom_node_t* pNodeStart,
                                         axiom_node_t** ppNode, axiom_element_t** ppElement) const
  {
    RISE_ASSERTP(ppNode != NULL);

    if (pNodeStart)
    {
      axiom_element_t* pElem = NULL;
      const axis2_char_t* szCurrentLocalName = NULL;

      while ((pNodeStart = axiom_node_get_next_sibling(pNodeStart, m_pEnv)))
      {
        if (axiom_node_get_node_type(pNodeStart, m_pEnv) == AXIOM_ELEMENT)
        {
          pElem = reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pNodeStart, m_pEnv));
          if (pElem)
          {
            szCurrentLocalName = axiom_element_get_localname(pElem, m_pEnv);
            if (szCurrentLocalName)
            {
              if (!axutil_strcmp(szCurrentLocalName, szLocalName))
              {
                *ppNode = pNodeStart;
                if (ppElement)
                {
                  *ppElement = pElem;
                }
                return true;
              }
            }
          }
        }
      }
    }

    return false;
  }

  CDataObject::Iterator CDataObject::FindChildByLocalName(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, &pNode);
    return Iterator(this, pNode);
  }

  CDataObject::Iterator CDataObject::FindChildByLocalName(const std::string& sLocalName)
  {
    return FindChildByLocalName(sLocalName.c_str());
  }

  CDataObject::ConstIterator CDataObject::FindChildByLocalName(const char* szLocalName) const
  {
    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, &pNode);
    return ConstIterator(this, pNode);
  }

  CDataObject::ConstIterator CDataObject::FindChildByLocalName(const std::string& sLocalName) const
  {
    return FindChildByLocalName(sLocalName.c_str());
  }

  CDataObject::Iterator CDataObject::FindChildByLocalName(const char* szLocalName, const Iterator& itStart)
  {
    RISE_ASSERTES(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this,
      CDomNoItemException, "Iterator is not bound to current dataobject");

    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, itStart.m_pAxiomNode, &pNode);
    return Iterator(this, pNode);
  }

  CDataObject::Iterator CDataObject::FindChildByLocalName(const std::string& sLocalName, const Iterator& itStart)
  {
    return FindChildByLocalName(sLocalName.c_str(), itStart);
  }

  CDataObject::ConstIterator CDataObject::FindChildByLocalName(const char* szLocalName,
                                                               const ConstIterator& itStart) const
  {
    RISE_ASSERTES(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this,
      CDomNoItemException, "Iterator is not bound to current dataobject");

    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, itStart.m_pAxiomNode, &pNode);
    return ConstIterator(this, pNode);
  }

  CDataObject::ConstIterator CDataObject::FindChildByLocalName(const std::string& sLocalName,
                                                               const ConstIterator& itStart) const
  {
    return FindChildByLocalName(sLocalName.c_str(), itStart);
  }

  CDataObject CDataObject::GetChildByQName(const CQName& stQName)
  {
    axiom_node_t* pNode = FindChildByQName(stQName).m_pAxiomNode;
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Child item \"" + stQName.ToString() + "\" is not found");
    return CDataObject(pNode);
  }

  const CDataObject CDataObject::GetChildByQName(const CQName& stQName) const
  {
    axiom_node_t* pNode = FindChildByQName(stQName).m_pAxiomNode;
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Child item \"" + stQName.ToString() + "\" is not found");
    return CDataObject(pNode);
  }

  CDataObject CDataObject::GetChildByLocalName(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    RISE_ASSERTES(FindChildByLocalName(szLocalName, &pNode, &pElement), CDomNoItemException,
                  "Child item \"" + std::string(szLocalName) + "\" is not found");
    return CDataObject(pNode, pElement);
  }

  CDataObject CDataObject::GetChildByLocalName(const std::string& sLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    RISE_ASSERTES(FindChildByLocalName(sLocalName.c_str(), &pNode, &pElement), CDomNoItemException,
                  "Child item \"" + sLocalName + "\" is not found");
    return CDataObject(pNode, pElement);
  }

  const CDataObject CDataObject::GetChildByLocalName(const char* szLocalName) const
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    RISE_ASSERTES(FindChildByLocalName(szLocalName, &pNode, &pElement), CDomNoItemException,
                  "Child item \"" + std::string(szLocalName) + "\" is not found");
    return CDataObject(pNode, pElement);
  }

  const CDataObject CDataObject::GetChildByLocalName(const std::string& sLocalName) const
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    RISE_ASSERTES(FindChildByLocalName(sLocalName.c_str(), &pNode, &pElement), CDomNoItemException,
                  "Child item \"" + sLocalName + "\" is not found");
    return CDataObject(pNode, pElement);
  }

  //////////////////////////////////////////////////////////////////////////
  // data manipulation

  // optimized set value functions
  void CDataObject::SetValue(bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void CDataObject::SetValue(const char* szText)
  {
    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, szText, m_pAxiomNode);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "can't set node value");
  }

  void CDataObject::SetValue(const std::string& sText)
  {
    SetValue(sText.c_str());
  }


  // optimized get value functions
  bool CDataObject::GetValue(bool& rbValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rbValue);
  }

  bool CDataObject::GetValue(byte& rbtValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rbtValue);
  }

  bool CDataObject::GetValue(int& rnValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rnValue);
  }

  bool CDataObject::GetValue(short& rshValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rshValue);
  }

  bool CDataObject::GetValue(long& rlValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rlValue);
  }

  bool CDataObject::GetValue(long long& rllValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rllValue);
  }

  bool CDataObject::GetValue(unsignedByte& rubtValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rubtValue);
  }

  bool CDataObject::GetValue(unsigned int& runValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, runValue);
  }

  bool CDataObject::GetValue(unsigned short& rushValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rushValue);
  }

  bool CDataObject::GetValue(unsigned long& rulValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rulValue);
  }

  bool CDataObject::GetValue(unsigned long long& rullValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rullValue);
  }

  bool CDataObject::GetValue(float& rfValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rfValue);
  }

  bool CDataObject::GetValue(double& rdValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    return FromCString(szText, rdValue);
  }

  void CDataObject::GetValue(std::string& sValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    RISE_ASSERTS(szText, "node value is NULL");
    sValue = szText;
  }

  CValue CDataObject::Value()
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    return CValue(this);
  }

  const CValue CDataObject::Value() const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    return CValue(const_cast<CDataObject*>(this));
  }

  CValue CDataObject::GetValue() const
  {
    return GetText();
  }

  void CDataObject::SetValue(const CValue& rValue)
  {
    SetText(rValue.AsString().c_str());
  }

  std::string CDataObject::GetText() const
  {
    axis2_char_t* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);

    return szText ? szText : "";
  }

  void CDataObject::GetText(std::string& sText) const
  {
    axis2_char_t* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);

    sText = szText ? szText : "";
  }

  void CDataObject::SetText(const char* szText)
  {
    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, szText, m_pAxiomNode);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "can't set text");
  }

  void CDataObject::SetText(const std::string& sText)
  {
    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, sText.c_str(), m_pAxiomNode);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "can't set text");
  }

  //////////////////////////////////////////////////////////////////////////
  // namespace management

  CNamespace CDataObject::GetDefaultNamespace() const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    CNamespace tNs(axiom_element_get_default_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode));
    return tNs;
  }

  void CDataObject::DeclareDefaultNamespace(const std::string& sUri)
  {
    RISE_ASSERT(m_pAxiomElement);
    axiom_element_declare_default_namespace(m_pAxiomElement, m_pEnv, const_cast<axis2_char_t*>(sUri.c_str()));
  }

  CNamespace CDataObject::GetNamespace() const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    CNamespace tNs(axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode));
    return tNs;
  }

  void CDataObject::DeclareNamespace(CNamespace& rNamespace)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_element_declare_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, rNamespace);
    rNamespace.SetDataObject(this);
  }

  void CDataObject::SetNamespace(CNamespace& rNamespace)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, rNamespace, m_pAxiomNode);
    rNamespace.SetDataObject(this);
  }

  CNamespace CDataObject::FindNamespace(const std::string& sUri)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    CNamespace tNs(axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, sUri.c_str(), NULL));
    return tNs;
  }

  CNamespace CDataObject::FindNamespace(const std::string& sUri, const std::string& sPrefix)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    CNamespace tNs(axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, sUri.c_str(), sPrefix.c_str()));
    return tNs;
  }

  CNamespace CDataObject::FindDeclaredNamespace(const std::string& sUri)
  {
    RISE_ASSERTS(m_pAxiomElement, "Not initialized");
    CNamespace tNs(axiom_element_find_declared_namespace(m_pAxiomElement, m_pEnv, sUri.c_str(), NULL));
    return tNs;
  }

  CNamespace CDataObject::FindDeclaredNamespace(const std::string& sUri, const std::string& sPrefix)
  {
    RISE_ASSERTS(m_pAxiomElement, "Not initialized");
    CNamespace tNs(axiom_element_find_declared_namespace(m_pAxiomElement, m_pEnv, sUri.c_str(), sPrefix.c_str()));
    return tNs;
  }

  CNamespace CDataObject::FindNamespaceUri(const std::string& sPrefix) const
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    CNamespace tNs(axiom_element_find_namespace_uri(m_pAxiomElement, m_pEnv,
      const_cast<axis2_char_t*>(sPrefix.c_str()), m_pAxiomNode));
    return tNs;
  }

  //////////////////////////////////////////////////////////////////////////
  // attribute management

  void CDataObject::CreateAttribute(const char* szAttrName, const char* szAttrText)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_attribute_t* pAxiomAttribute =
        axiom_attribute_create(m_pEnv, szAttrName, szAttrText, NULL);
    RISE_ASSERTS(pAxiomAttribute, "Failed to create axiom attribute");

    axis2_status_t nResult =
        axiom_element_add_attribute(m_pAxiomElement, m_pEnv, pAxiomAttribute, m_pAxiomNode);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "Failed to add axiom attribute");
  }

  void CDataObject::CreateAttribute(const std::string& sAttrName, const char* szAttrText)
  {
    CreateAttribute(sAttrName.c_str(), szAttrText);
  }

  void CDataObject::CreateAttribute(const char* szAttrName, const std::string& sAttrText)
  {
    CreateAttribute(szAttrName, sAttrText.c_str());
  }

  void CDataObject::CreateAttribute(const std::string& sAttrName, const std::string& sAttrText)
  {
    CreateAttribute(sAttrName.c_str(), sAttrText.c_str());
  }

  void CDataObject::CreateAttribute(const char* szAttrName, const CValue& rValue)
  {
    CreateAttribute(szAttrName, rValue.AsString().c_str());
  }

  void CDataObject::CreateAttribute(const std::string& sAttrName, const CValue& rValue)
  {
    CreateAttribute(sAttrName.c_str(), rValue.AsString().c_str());
  }

  void CDataObject::AppendAttribute(CAttribute& rAttribute)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axis2_status_t nResult =
        axiom_element_add_attribute(m_pAxiomElement, m_pEnv, rAttribute, m_pAxiomNode);
    RISE_ASSERTS(nResult == AXIS2_SUCCESS, "Failed to add axiom attribute");
  }

  void CDataObject::RemoveAttribute(AttributeIterator& itAttribute)
  {
    RISE_ASSERTS(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_element_remove_attribute(m_pAxiomElement, m_pEnv, *itAttribute);
  }

  void CDataObject::RemoveAllAttributes()
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash)
    {
      for (axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);
        pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
      {
        void* pHashValue = NULL;

        axutil_hash_this(pIndex, NULL, NULL, &pHashValue);
        if (pHashValue != NULL)
        {
          axiom_element_remove_attribute(m_pAxiomElement, m_pEnv,
                                         reinterpret_cast<axiom_attribute_t*>(pHashValue));
        }
      }
    }
  }

  CDataObject::AttributeIterator CDataObject::FindAttributeByQName(const CQName& stQName)
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return AttributeIterator(this, NULL);
    }

    for (axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);
      pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
    {
      void* pHashValue = NULL;

      axutil_hash_this(pIndex, NULL, NULL, &pHashValue);
      if (pHashValue != NULL)
      {
        axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
        if (stQName == axiom_attribute_get_qname(pAttr, m_pEnv))
        {
          return AttributeIterator(this, pIndex);
        }
      }
    }

    return AttributeIterator(this, NULL);
  }

  CDataObject::AttributeIterator CDataObject::FindAttributeByQName(const CQName& stQName,
                                                                   const AttributeIterator& itStart)
  {
    for (axutil_hash_index_t* pIndex = itStart.m_pAttributeIndex;
      pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
    {
      void* pHashValue = NULL;

      axutil_hash_this(pIndex, NULL, NULL, &pHashValue);
      if (pHashValue != NULL)
      {
        axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
        if (stQName == axiom_attribute_get_qname(pAttr, m_pEnv))
        {
          return AttributeIterator(this, pIndex);
        }
      }
    }

    return AttributeIterator(this, NULL);
  }

  CDataObject::AttributeIterator CDataObject::FindAttributeByLocalName(const std::string& sLocalName)
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return AttributeIterator(this, NULL);
    }

    for (axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);
      pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
    {
      void* pHashValue = NULL;

      axutil_hash_this(pIndex, NULL, NULL, &pHashValue);
      if (pHashValue != NULL)
      {
        axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
        if (sLocalName == axiom_attribute_get_localname(pAttr, m_pEnv))
        {
          return AttributeIterator(this, pIndex);
        }
      }
    }

    return AttributeIterator(this, NULL);
  }

  CDataObject::ConstAttributeIterator CDataObject::FindAttributeByQName(const CQName& stQName) const
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return ConstAttributeIterator(this, NULL);
    }

    for (axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);
      pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
    {
      void* pHashValue = NULL;

      axutil_hash_this(pIndex, NULL, NULL, &pHashValue);
      if (pHashValue != NULL)
      {
        axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
        if (stQName == axiom_attribute_get_qname(pAttr, m_pEnv))
        {
          return ConstAttributeIterator(this, pIndex);
        }
      }
    }

    return ConstAttributeIterator(this, NULL);
  }

  CDataObject::ConstAttributeIterator CDataObject::FindAttributeByQName(const CQName& stQName,
                                                                        const ConstAttributeIterator& itStart) const
  {
    for (axutil_hash_index_t* pIndex = itStart.m_pAttributeIndex;
      pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
    {
      void* pHashValue = NULL;

      axutil_hash_this(pIndex, NULL, NULL, &pHashValue);
      if (pHashValue != NULL)
      {
        axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
        if (stQName == axiom_attribute_get_qname(pAttr, m_pEnv))
        {
          return ConstAttributeIterator(this, pIndex);
        }
      }
    }

    return ConstAttributeIterator(this, NULL);
  }

  CDataObject::ConstAttributeIterator CDataObject::FindAttributeByLocalName(const std::string& sLocalName) const
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return ConstAttributeIterator(this, NULL);
    }

    for (axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);
      pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
    {
      void* pHashValue = NULL;

      axutil_hash_this(pIndex, NULL, NULL, &pHashValue);
      if (pHashValue != NULL)
      {
        axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
        if (sLocalName == axiom_attribute_get_localname(pAttr, m_pEnv))
        {
          return ConstAttributeIterator(this, pIndex);
        }
      }
    }

    return ConstAttributeIterator(this, NULL);
  }

  CAttribute CDataObject::GetAttributeByQName(const CQName& stQName)
  {
    CAttribute tAttr(this, axiom_element_get_attribute(m_pAxiomElement, m_pEnv, stQName));
    return tAttr;
  }

  const CAttribute CDataObject::GetAttributeByQName(const CQName& stQName) const
  {
    CAttribute tAttr(const_cast<CDataObject*>(this), axiom_element_get_attribute(m_pAxiomElement,
                                                                                 m_pEnv, stQName));
    return tAttr;
  }

  CValue CDataObject::GetAttributeValueByName(const char* szLocalName) const
  {
    const char* szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                                    const_cast<char*>(szLocalName));
    return CValue(szValue ? szValue : "");
  }

  CValue CDataObject::GetAttributeValueByName(const std::string& sLocalName) const
  {
    return GetAttributeValueByName(sLocalName.c_str());
  }

  std::string CDataObject::GetAttributeTextByName(const std::string& sLocalName) const
  {
    const char* szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                                    const_cast<char*>(sLocalName.c_str()));
    return szValue ? szValue : "";
  }

  void CDataObject::GetAttributeTextByName(const std::string& sLocalName, std::string& sText) const
  {
    GetAttributeTextByName(sLocalName.c_str(), sText);
  }

  void CDataObject::GetAttributeTextByName(const char* szLocalName, std::string& sText) const
  {
    const char* szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                                    const_cast<char*>(szLocalName));
    sText = szValue ? szValue : "";
  }

  //////////////////////////////////////////////////////////////////////////
  // iteration

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

  CDataObject::AttributeIterator CDataObject::AttributeEnd()
  {
    return AttributeIterator(this, NULL);
  }

  CDataObject::ConstAttributeIterator CDataObject::AttributeBegin() const
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return ConstAttributeIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);

    return ConstAttributeIterator(this, pIndex);
  }

  CDataObject::ConstAttributeIterator CDataObject::AttributeEnd() const
  {
    return ConstAttributeIterator(this, NULL);
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

  CDataObject::ConstNamespaceIterator CDataObject::NamespaceBegin() const
  {
    axutil_hash_t* pNsHash = axiom_element_get_namespaces(m_pAxiomElement, m_pEnv);
    if (pNsHash == NULL)
    {
      return ConstNamespaceIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pNsHash, m_pEnv);

    return ConstNamespaceIterator(this, pIndex);
  }

  CDataObject::ConstNamespaceIterator CDataObject::NamespaceEnd() const
  {
    return ConstNamespaceIterator(this, NULL);
  }


  //////////////////////////////////////////////////////////////////////////
  // support operators

  CDataObject& CDataObject::operator=(axiom_node_t* pAxiomNode)
  {
    Attach(pAxiomNode);
    return *this;
  }

  CDataObject& CDataObject::operator=(const CDataObject& rDataObject)
  {
    if (m_pAxiomNode != rDataObject.m_pAxiomNode)
    {
      Detach();
      m_bOwner = rDataObject.m_bOwner;
      const_cast<CDataObject&>(rDataObject).m_bOwner = false;
      m_pAxiomNode = const_cast<CDataObject&>(rDataObject).m_pAxiomNode;
      m_pAxiomElement = const_cast<CDataObject&>(rDataObject).m_pAxiomElement;
    }
    return *this;
  }

  bool CDataObject::operator==(const CDataObject& rDataObject) const
  {
    return m_pAxiomNode == rDataObject.m_pAxiomNode &&
      m_pAxiomElement == rDataObject.m_pAxiomElement;
  }

  bool CDataObject::operator!=(const CDataObject& rDataObject) const
  {
    return !operator==(rDataObject);
  }

  CDataObject& CDataObject::FromString(const char* szXml)
  {
    RISE_ASSERTS(szXml, "szXml is NULL");
    return FromString(szXml, strlen(szXml));
  }

  CDataObject& CDataObject::FromString(const char* szXml, int nSize)
  {
    axiom_xml_reader_t* pAxiomXmlReader = NULL;
    axiom_stax_builder_t* pStaxBuilder = NULL;
    axiom_node_t* pAxiomNode = NULL;

    pAxiomXmlReader = axiom_xml_reader_create_for_memory(
      m_pEnv,
      const_cast<char*>(szXml),
      nSize,
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

  CDataObject& CDataObject::FromString(const std::string& sXml)
  {
    return FromString(sXml.c_str(), static_cast<int>(sXml.size()));
  }

  std::string CDataObject::ToString() const
  {
    std::string sXml;
    ToString(sXml);
    return sXml;
  }

  void CDataObject::ToString(std::string& sResult) const
  {
    char* szXml = axiom_node_to_string(m_pAxiomNode, m_pEnv);
    RISE_ASSERTES(szXml != NULL, CDomFormatException, "cannot convert DataObject to string");
    sResult = szXml;
    AXIS2_FREE(m_pEnv->allocator, szXml);
  }

  CDataObject* CDataObject::operator->()
  {
    return this;
  }

  const CDataObject* CDataObject::operator->() const
  {
    return this;
  }

  //////////////////////////////////////////////////////////////////////////
  // deprecated functions

  CDataObject CDataObject::GetOrAdd(const std::string& sName)
  {
    return CreateChildOnce(sName);
  }

  void CDataObject::RemoveChildByLocalName(const std::string& sName)
  {
    Iterator itFind = FindChildByLocalName(sName);
    if (itFind != End())
      RemoveChild(itFind);
  }

  CDataObject::Iterator CDataObject::Find(const std::string& sLocalName)
  {
    return FindChildByLocalName(sLocalName);
  }

  CDataObject::ConstIterator CDataObject::Find(const std::string& sLocalName) const
  {
    return FindChildByLocalName(sLocalName);
  }

  const CDataObject CDataObject::operator()(const std::string& sLocalName) const
  {
    return const_cast<CDataObject*>(this)->GetChildByLocalName(sLocalName);
  }

  CDataObject CDataObject::operator()(const std::string& sLocalName)
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

  void CDataObject::SetName(const std::string& sName)
  {
    SetLocalName(sName);
  }

  CDataObject CDataObject::Add(const std::string& sName)
  {
    return CreateChild(sName);
  }

  CDataObject CDataObject::Add(const std::string& sName, const CValue& rValue)
  {
    return CreateChild(sName, rValue);
  }

  CDataObject CDataObject::Add(CDataObject rDataObject)
  {
    return AppendChild(rDataObject);
  }

  CValue CDataObject::operator[](const std::string& sName)
  {
    return GetChildByLocalName(sName).GetValue();
  }

  const CValue CDataObject::operator[](const std::string& sName) const
  {
    return GetChildByLocalName(sName).GetValue();
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
        
      while (pNode != NULL &&
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT))
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
    } else
    {
      do
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
      while (pNode != NULL && 
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));
    }
    
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Attempt to --begin()");
    m_pAxiomNode = pNode;
    return *this;
  }

  CDataObject::Iterator& CDataObject::Iterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAxiomNode != NULL, CDomNoItemException, "Attempt to ++end()");

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
  // ConstIterator

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
    
    axiom_node_t* pNode = m_pAxiomNode;

    if (pNode == NULL) // iterator == end()
    {
      pNode = axiom_node_get_last_child(m_pDataObject->m_pAxiomNode, m_pDataObject->m_pEnv);
        
      while (pNode != NULL &&
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT))
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
    } else
    {
      do
      {
        pNode = axiom_node_get_previous_sibling(pNode, m_pDataObject->m_pEnv);
      }
      while (pNode != NULL && 
        (axiom_node_get_node_type(pNode, m_pDataObject->m_pEnv) != AXIOM_ELEMENT));
    }
    
    RISE_ASSERTES(pNode != NULL, CDomNoItemException, "Attempt to --begin()");
    m_pAxiomNode = pNode;
    return *this;
  }

  CDataObject::ConstIterator& CDataObject::ConstIterator::operator++()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAxiomNode != NULL, CDomNoItemException, "Attempt to ++end()");

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

  CDataObject::ConstNamespaceIterator::ConstNamespaceIterator(const CDataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex):
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
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Attempt to ++end()");

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
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Can\'t get attribute");

    CAttribute tAttr(m_pDataObject, reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  CAttribute CDataObject::AttributeIterator::operator->()
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Can\'t get attribute");

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
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Attempt to ++end()");

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
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Can\'t get attribute");

    //!!! const HACK
    CAttribute tAttr(const_cast<CDataObject*>(m_pDataObject), reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  const CAttribute CDataObject::ConstAttributeIterator::operator->() const
  {
    RISE_ASSERTS(m_pDataObject != NULL, "Iterator is not iterable");
    RISE_ASSERTES(m_pAttributeIndex != NULL, CDomNoItemException, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    RISE_ASSERTES(pHashValue != NULL, CDomNoItemException, "Can\'t get attribute");

    CAttribute tAttr(const_cast<CDataObject*>(m_pDataObject), reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  CDataObject& operator<<(CDataObject& rDataObject, const CValue& rValue)
  {
    rDataObject.SetText(rValue.AsString().c_str());
    return rDataObject;
  }

  const CDataObject& operator>>(const CDataObject& rDataObject, CValue& rValue)
  {
    rDataObject.GetText(rValue);
    return rDataObject;
  }
}
