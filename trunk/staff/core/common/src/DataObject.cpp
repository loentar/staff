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

#include <axis2_const.h>
#include <axutil_utils.h>
#include <axiom.h>
#include <axiom_node.h>
#include <axiom_element.h>
#include <axiom_attribute.h>
#include <axiom_namespace.h>
#include <staff/utils/tocstring.h>
#include <staff/utils/fromcstring.h>
#include "Exception.h"
#include "Runtime.h"
#include "QName.h"
#include "Namespace.h"
#include "Attribute.h"
#include "Value.h"
#include "DataObject.h"

// detect axis2c-1.7.0 and above
#ifdef AXIS2_SVC_CLIENT_CLOSED
#define AXIS2_VERSION_1_7_0_AND_ABOVE
#endif

namespace staff
{
  enum { NUM_TO_STR_BUFF_SIZE = 32 };

  static const char* g_szXsiSchemaUrl = "http://www.w3.org/2001/XMLSchema-instance";

  DataObject::DataObject(axiom_node_t* pAxiomNode /*= NULL*/):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    if (pAxiomNode)
    {
      Attach(pAxiomNode);
    }
  }

  DataObject::DataObject(const char* szLocalName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName);
  }

  DataObject::DataObject(const std::string& sLocalName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str());
  }

  DataObject::DataObject(const QName& stQName):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(stQName);
  }

  DataObject::DataObject(const DataObject& rDataObject):
    m_pAxiomNode(rDataObject.m_pAxiomNode),
    m_pAxiomElement(rDataObject.m_pAxiomElement),
    m_bOwner(rDataObject.m_bOwner)
  {
    rDataObject.m_bOwner = false;
  }

  DataObject::DataObject(const char* szLocalName, bool bValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, byte btValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, int nValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, short shValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, long lValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, long long llValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, unsignedByte ubtValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, unsigned int unValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, unsigned short ushValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, unsigned long ulValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, unsigned long long ullValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, float fValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, double dValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, long double ldValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }


  DataObject::DataObject(const std::string& sLocalName, bool bValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, byte btValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, int nValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, short shValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, long lValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, long long llValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, unsignedByte ubtValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, unsigned int unValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, unsigned short ushValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, unsigned long ulValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, unsigned long long ullValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, float fValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, double dValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const std::string& sLocalName, long double ldValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  DataObject::DataObject(const char* szLocalName, const char* szText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName, szText);
  }

  DataObject::DataObject(const std::string& sLocalName, const char* szText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str(), szText);
  }

  DataObject::DataObject(const char* szLocalName, const std::string& sText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName, sText.c_str());
  }

  DataObject::DataObject(const std::string& sLocalName, const std::string& sText):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str(), sText.c_str());
  }

  DataObject::DataObject(const char* szLocalName, const Value& rValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(szLocalName, rValue.AsString().c_str());
  }

  DataObject::DataObject(const std::string& sLocalName, const Value& rValue):
    m_pAxiomNode(NULL),
    m_pAxiomElement(NULL),
    m_bOwner(false)
  {
    Create(sLocalName.c_str(), rValue.AsString().c_str());
  }

  DataObject::DataObject(axiom_node_t* pAxiomNode, axiom_element_t* pAxiomElement):
    m_pAxiomNode(pAxiomNode),
    m_pAxiomElement(pAxiomElement),
    m_bOwner(false)
  {
  }

  DataObject::~DataObject()
  {
    Detach();
  }

  void DataObject::Attach(axiom_node_t* pAxiomNode, bool bOwner /*= false*/)
  {
    Detach();

    STAFF_ASSERT_PARAM(pAxiomNode);
    STAFF_ASSERT_DOM(axiom_node_get_node_type(pAxiomNode, m_pEnv) == AXIOM_ELEMENT,
      "Given node is not a element");

    axiom_element_t* pAxiomElement = 
      reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pAxiomNode, m_pEnv));
    STAFF_ASSERT_DOM(pAxiomElement != NULL, "Can\'t get data element");

    m_pAxiomNode = pAxiomNode;
    m_pAxiomElement = pAxiomElement;
    m_bOwner = bOwner;
  }

  void DataObject::Detach()
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

  bool DataObject::IsOwner() const
  {
    return m_bOwner;
  }

  void DataObject::SetOwner(bool bOwner)
  {
    m_bOwner = bOwner;
  }

  DataObject::operator axiom_node_t*()
  {
    return m_pAxiomNode;
  }

  DataObject::operator axiom_element_t*()
  {
    return m_pAxiomElement;
  }

  bool DataObject::IsNull() const
  {
    return (m_pAxiomNode == NULL || m_pAxiomElement == NULL);
  }

  bool DataObject::IsInit() const
  {
    return (m_pAxiomNode != NULL && m_pAxiomElement != NULL);
  }

  //////////////////////////////////////////////////////////////////////////
  // Node properties

  QName DataObject::GetQName()
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT_DOM(pqName != NULL, "Can\'t get node's QName");

    QName tqName(pqName);
    return tqName;
  }

  void DataObject::SetQName(const QName& stQName) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_element_set_localname(m_pAxiomElement, m_pEnv, stQName.GetLocalPart().c_str());

    axiom_namespace_t* pNewNamespace = axiom_namespace_create(m_pEnv, stQName.GetNamespaceUri().c_str(), 
      stQName.GetPrefix().c_str());
    STAFF_ASSERT_DOM(pNewNamespace != NULL, "Can\'t create namespace");

    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNewNamespace, m_pAxiomNode);
  }

  void DataObject::GetLocalName(std::string& sLocalName) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    const axis2_char_t* szLocalName = axiom_element_get_localname(m_pAxiomElement, m_pEnv);

    sLocalName = szLocalName ? szLocalName : "";
  }

  std::string DataObject::GetLocalName() const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    const axis2_char_t* szLocalName = axiom_element_get_localname(m_pAxiomElement, m_pEnv);

    return szLocalName ? szLocalName : "";
  }

  void DataObject::SetLocalName(const char* szLocalName) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axis2_status_t nResult =
        axiom_element_set_localname(m_pAxiomElement, m_pEnv, szLocalName);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "Can't set local name");
  }

  void DataObject::SetLocalName(const std::string& sLocalName) const
  {
    SetLocalName(sLocalName.c_str());
  }

  void DataObject::GetPrefix(std::string& sPrefix) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName =
        axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT_DOM(pqName != NULL, "Can\'t get node's QName");

    const axis2_char_t* szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);

    sPrefix = szPrefix ? szPrefix : "";
  }

  std::string DataObject::GetPrefix() const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName = axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT_DOM(pqName != NULL, "Can\'t get node's QName");

    const axis2_char_t* szPrefix = axutil_qname_get_prefix(pqName, m_pEnv);

    return szPrefix == NULL ? "" : szPrefix;
  }

  void DataObject::SetPrefix(const char* szPrefix)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_namespace_t* pNewNamespace = NULL;
    axiom_namespace_t* pOldNamespace =
        axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    const char* szUri = NULL;

    if (pOldNamespace != NULL)
    {
      szUri = axiom_namespace_get_uri(pOldNamespace, m_pEnv);
    }

    pNewNamespace = axiom_namespace_create(m_pEnv, szUri != NULL ? szUri : "", szPrefix);
    STAFF_ASSERT_DOM(pNewNamespace != NULL, "Can\'t create namespace");

    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNewNamespace, m_pAxiomNode);
  }

  void DataObject::SetPrefix(const std::string& sPrefix)
  {
    SetPrefix(sPrefix.c_str());
  }

  void DataObject::GetNamespaceUri(std::string& sNamespaceUri) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName =
        axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT_DOM(pqName != NULL, "Can\'t get node's QName");

    const axis2_char_t* szUri = axutil_qname_get_uri(pqName, m_pEnv);

    sNamespaceUri = szUri ? szUri : "";
  }

  std::string DataObject::GetNamespaceUri() const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axutil_qname_t* pqName =
        axiom_element_get_qname(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT_DOM(pqName != NULL, "Can\'t get node's QName");

    const axis2_char_t* szUri = axutil_qname_get_uri(pqName, m_pEnv);

    return szUri != NULL ? szUri : "";
  }


  void DataObject::SetNamespaceUri(const char* szUri) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_namespace_t* pNamespace = axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    if (!pNamespace)
    {
      pNamespace = axiom_namespace_create(m_pEnv, szUri, "");
      STAFF_ASSERT_DOM(pNamespace, "Can't create axiom namespace");
      axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNamespace, m_pAxiomNode);
    }
    else
    {
      axutil_string_t* pStrUri = axutil_string_create(m_pEnv, szUri);
      STAFF_ASSERT_DOM(pStrUri, "Can't create axuil_string");

      axis2_status_t nResult = axiom_namespace_set_uri_str(pNamespace, m_pEnv, pStrUri);
      if (nResult != AXIS2_SUCCESS)
      {
        axutil_string_free(pStrUri, m_pEnv);
        STAFF_THROW_ASSERT("Failed to axiom_namespace_set_uri_str");
      }

      axutil_string_free(pStrUri, m_pEnv);
    }
  }

  void DataObject::SetNamespaceUri(const std::string& sUri) const
  {
    SetNamespaceUri(sUri.c_str());
  }

  void DataObject::SetNamespaceUriGenPrefix(const char* szUri, std::string* psPrefix,
                                            bool bSetNamespace /*= true*/)
  {
    static const axis2_ssize_t uDefPrefixSize = 2;
    static const axis2_char_t szDefPrefix[uDefPrefixSize + 1] = "ns";

    STAFF_ASSERT(m_pAxiomNode, "Not initialized");

    // find top element
    axiom_node_t* pTopNode = m_pAxiomNode;
    axiom_node_t* pCurrNode = NULL;

    while (!!(pCurrNode = axiom_node_get_parent(pTopNode, m_pEnv)))
    {
      pTopNode = pCurrNode;
    }

    STAFF_ASSERT(axiom_node_get_node_type(pTopNode, m_pEnv) == AXIOM_ELEMENT,
                 "Top node is not an element!");

    axiom_element_t* pTopElement =
      reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pTopNode, m_pEnv));

    STAFF_ASSERT_DOM(pTopElement, "Can't get data element");

    axiom_namespace_t* pNamespace =
        axiom_element_find_declared_namespace(pTopElement, m_pEnv, szUri, NULL);

    if (pNamespace)
    { // namespace found
      if (psPrefix)
      {
        const char* szPrefix = axiom_namespace_get_prefix(pNamespace, m_pEnv);
        *psPrefix = szPrefix ? szPrefix : "";
      }
    }
    else
    {
      // namespace is not found, creating one
      int nLastUsedIndex = -1;

      axutil_hash_t* pNsHash = axiom_element_get_namespaces(pTopElement, m_pEnv);
      if (pNsHash)
      {
        int nLastIndex = 0;
        axis2_char_t* szCurrPrefix = NULL;
        axiom_namespace_t* pCurrNs = NULL;
        for (axutil_hash_index_t* pIndex = axutil_hash_first(pNsHash, m_pEnv);
             pIndex; pIndex = axutil_hash_next(m_pEnv, pIndex))
        {
          axutil_hash_this(pIndex, NULL, NULL, reinterpret_cast<void**>(&pCurrNs));
          if (pCurrNs)
          {
            szCurrPrefix = axiom_namespace_get_prefix(pCurrNs, m_pEnv);
            if (!axutil_strncmp(szCurrPrefix, szDefPrefix, uDefPrefixSize))
            {
              szCurrPrefix += uDefPrefixSize;
              // get greatest number of ns
              if (staff::FromCString(szCurrPrefix, nLastIndex) &&
                  nLastIndex > nLastUsedIndex)
              {
                nLastUsedIndex = nLastIndex;
              }
            }
          }
        }
      }

      ++nLastUsedIndex;

      static const axis2_ssize_t nPrefixSize = uDefPrefixSize + NUM_TO_STR_BUFF_SIZE;
      axis2_char_t szPrefix[nPrefixSize];
      STAFF_ASSERT(static_cast<axis2_ssize_t>(
                    staff_snprintf(szPrefix, nPrefixSize, "%s%d", szDefPrefix, nLastUsedIndex)) <
                   nPrefixSize, "Buffer overrun");

      pNamespace = axiom_namespace_create(m_pEnv, szUri, szPrefix);
      STAFF_ASSERT_DOM(pNamespace, "Failed to create namespace");

      // adding new namespace without finding in declarations
      axiom_element_declare_namespace_assume_param_ownership(pTopElement, m_pEnv, pNamespace);

      if (psPrefix)
      {
        *psPrefix = szPrefix;
      }
    }

    if (bSetNamespace)
    {
      axiom_element_set_namespace_assume_param_ownership(m_pAxiomElement, m_pEnv, pNamespace);
    }
  }

  void DataObject::SetNamespaceUriGenPrefix(const std::string& sUri,
                                            std::string* psPrefix /*= NULL*/,
                                            bool bSetNamespace /*= true*/)
  {
    SetNamespaceUriGenPrefix(sUri.c_str(), psPrefix, bSetNamespace);
  }

  //////////////////////////////////////////////////////////////////////////
  // node management

  void DataObject::Create()
  {
    Detach();

    m_pAxiomElement = axiom_element_create(m_pEnv, NULL, "", NULL, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      STAFF_THROW_ASSERT("Failed to create axiom element");
    }

    m_bOwner = true;
  }

  void DataObject::Create(const char* szLocalName)
  {
    Detach();

    m_pAxiomElement = axiom_element_create(m_pEnv, NULL, szLocalName, NULL, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      STAFF_THROW_ASSERT("Failed to create axiom element");
    }

    m_bOwner = true;
  }

  void DataObject::Create(const std::string& sLocalName)
  {
    Create(sLocalName.c_str());
  }

  void DataObject::Create(const QName& rQName)
  {
    Detach();

    m_pAxiomElement = axiom_element_create_with_qname(m_pEnv, NULL, rQName, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      STAFF_THROW_ASSERT("Failed to create axiom element");
    }

    m_bOwner = true;
  }


  void DataObject::Create(const char* szLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, long double ldValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(szLocalName, szBuffer);
  }

  void DataObject::Create(const char* szLocalName, const QName& rstQName)
  {
    Create(szLocalName, rstQName.ToString().c_str());
  }


  void DataObject::Create(const std::string& sLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, long double ldValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    Create(sLocalName.c_str(), szBuffer);
  }

  void DataObject::Create(const std::string& sLocalName, const QName& rstQName)
  {
    Create(sLocalName.c_str(), rstQName.ToString().c_str());
  }


  void DataObject::Create(const char* szLocalName, const char* szText)
  {
    Detach();

    m_pAxiomElement = axiom_element_create(m_pEnv, NULL, szLocalName, NULL, &m_pAxiomNode);
    if (!m_pAxiomElement)
    {
      m_pAxiomNode = NULL;
      STAFF_THROW_ASSERT("Failed to create axiom element");
    }

    m_bOwner = true;

    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, szText, m_pAxiomNode);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "can't set text");
  }

  void DataObject::Create(const std::string& sLocalName, const char* szText)
  {
    Create(sLocalName.c_str(), szText);
  }

  void DataObject::Create(const char* szLocalName, const std::string& sText)
  {
    Create(szLocalName, sText.c_str());
  }

  void DataObject::Create(const std::string& sLocalName, const std::string& sText)
  {
    Create(sLocalName.c_str(), sText.c_str());
  }

  void DataObject::Create(const std::string& sLocalName, const Value& rValue)
  {
    Create(sLocalName.c_str(), rValue.AsString().c_str());
  }

  void DataObject::Free()
  {
    if (m_pAxiomNode != NULL)
    {
      axiom_node_free_tree(m_pAxiomNode, m_pEnv);
      m_pAxiomNode = NULL;
      m_pAxiomElement = NULL;
    }
  }

  DataObject& DataObject::DetachNode()
  {
    STAFF_ASSERT(m_pAxiomNode != NULL, "Not initialized");

    STAFF_ASSERT_DOM(axiom_node_detach(m_pAxiomNode, m_pEnv),
      "Error while node detaching");
    m_bOwner = true;
    return *this;
  }

  DataObject& DataObject::ReplaceNode(DataObject& rNewNode)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL, "Not initialized");
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

  axiom_node_t* DataObject::Clone(axiom_node_t* pNodeIn, axiom_node_t* pNodeOutParent)
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

  DataObject& DataObject::Clone(const DataObject& rDataObject)
  {
    STAFF_ASSERT(rDataObject.m_pAxiomNode != NULL && rDataObject.m_pAxiomElement != NULL, "Not initialized");

    axiom_node_t* pNode = Clone(const_cast<DataObject&>(rDataObject).m_pAxiomNode, NULL);
    Attach(pNode);
    return *this;
  }

  DataObject DataObject::Clone() const
  {
    DataObject tdoClone;
    return tdoClone.Clone(*this);
  }

  DataObject DataObject::Copy() const
  {
    DataObject tdoResult;
    tdoResult.m_pAxiomNode = m_pAxiomNode;
    tdoResult.m_pAxiomElement = m_pAxiomElement;
    return *this;
  }

  DataObject DataObject::Parent() const
  {
    STAFF_ASSERT(m_pAxiomNode, "Not initialized");
    return axiom_node_get_parent(m_pAxiomNode, m_pEnv);
  }

  DataObject DataObject::NextSibling() const
  {
    axiom_node_t* pNode = NULL;
    while ((pNode = axiom_node_get_next_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT);
    return pNode;
  }

  DataObject DataObject::PreviousSibling() const
  {
    axiom_node_t* pNode = NULL;
    while ((pNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT);
    return pNode;
  }

  void DataObject::SetNextSibling()
  {
    while ((m_pAxiomNode = axiom_node_get_next_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(m_pAxiomNode, m_pEnv) != AXIOM_ELEMENT);

    m_pAxiomElement = m_pAxiomNode ?
          reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(m_pAxiomNode, m_pEnv)) :
          NULL;
  }

  void DataObject::SetPreviousSibling()
  {
    while ((m_pAxiomNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv)) != NULL &&
           axiom_node_get_node_type(m_pAxiomNode, m_pEnv) != AXIOM_ELEMENT);

    m_pAxiomElement = m_pAxiomNode ?
          reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(m_pAxiomNode, m_pEnv)) :
                NULL;
  }

  //////////////////////////////////////////////////////////////////////////
  // child nodes management

  DataObject DataObject::FirstChild()
  {
    return axiom_node_get_first_element(m_pAxiomNode, m_pEnv);
  }

  const DataObject DataObject::FirstChild() const
  {
    return axiom_node_get_first_element(m_pAxiomNode, m_pEnv);
  }

  DataObject DataObject::LastChild()
  {
    axiom_node_t* pNode = axiom_node_get_last_child(m_pAxiomNode, m_pEnv);
    while (pNode != NULL && axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT)
    {
      pNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv);
    }

    return pNode;
  }

  const DataObject DataObject::LastChild() const
  {
    axiom_node_t* pNode = axiom_node_get_last_child(m_pAxiomNode, m_pEnv);
    while (pNode != NULL && axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT)
    {
      pNode = axiom_node_get_previous_sibling(m_pAxiomNode, m_pEnv);
    }

    return pNode;
  }

  DataObject DataObject::CreateChild()
  {
    axiom_node_t* pAxiomNode = NULL;
    axiom_element_t* pAxiomElement = axiom_element_create(m_pEnv, NULL, "", NULL, &pAxiomNode);
    STAFF_ASSERT_DOM(pAxiomElement, "Failed to create axiom element");

    axiom_node_add_child(m_pAxiomNode, m_pEnv, pAxiomNode);

    return DataObject(pAxiomNode, pAxiomElement);
  }

  DataObject DataObject::CreateChild(const char* szLocalName)
  {
    axiom_node_t* pAxiomNode = NULL;
    axiom_element_t* pAxiomElement = axiom_element_create(m_pEnv, m_pAxiomNode, szLocalName, NULL, &pAxiomNode);
    STAFF_ASSERT_DOM(pAxiomElement, "Failed to create axiom element");

    return DataObject(pAxiomNode, pAxiomElement);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName)
  {
    return CreateChild(sLocalName.c_str());
  }

  DataObject DataObject::CreateChild(const QName& rQName)
  {
    DataObject tdoChild(rQName);
    return AppendChild(tdoChild);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, long double ldValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(szLocalName, szBuffer);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, const QName& rstQName)
  {
    return CreateChild(szLocalName, rstQName.ToString().c_str());
  }


  DataObject DataObject::CreateChild(const std::string& sLocalName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, long double ldValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateChild(sLocalName.c_str(), szBuffer);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, const QName& rstQName)
  {
    return CreateChild(sLocalName.c_str(), rstQName.ToString().c_str());
  }



  DataObject DataObject::CreateChild(const char* szLocalName, const char* szText)
  {
    STAFF_ASSERT(m_pAxiomNode, "Not initialized");

    axiom_node_t* pAxiomNode = NULL;
    axiom_element_t* pAxiomElement = axiom_element_create(m_pEnv, m_pAxiomNode, szLocalName, NULL, &pAxiomNode);
    STAFF_ASSERT_DOM(pAxiomElement, "Failed to create axiom element");

    axis2_status_t nResult =
        axiom_element_set_text(pAxiomElement, m_pEnv, szText, pAxiomNode);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "can't set text");

    return DataObject(pAxiomNode, pAxiomElement);
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, const char* szText)
  {
    return CreateChild(sLocalName.c_str(), szText);
  }

  DataObject DataObject::CreateChild(const char* szLocalName, const std::string& sText)
  {
    return CreateChild(szLocalName, sText.c_str());
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, const std::string& sText)
  {
    return CreateChild(sLocalName.c_str(), sText.c_str());
  }

  DataObject DataObject::CreateChild(const char* szLocalName, const Value& rValue)
  {
    return CreateChild(szLocalName, rValue.AsString().c_str());
  }

  DataObject DataObject::CreateChild(const std::string& sLocalName, const Value& rValue)
  {
    return CreateChild(sLocalName.c_str(), rValue.AsString().c_str());
  }

  DataObject DataObject::CreateChildOnce(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElem = NULL;

    if (FindChildByLocalName(szLocalName, &pNode, &pElem))
    {
      return DataObject(pNode, pElem);
    }

    return CreateChild(szLocalName);
  }

  DataObject DataObject::CreateChildOnce(const std::string& sLocalName)
  {
    return CreateChildOnce(sLocalName.c_str());
  }

  DataObject DataObject::AppendChild(DataObject& rDataObject)
  {
    STAFF_ASSERT(m_pAxiomNode, "Not initialized");
    STAFF_ASSERT(rDataObject.m_pAxiomNode, "Child is not initialized");

    axiom_node_add_child(m_pAxiomNode, m_pEnv, rDataObject.m_pAxiomNode);
    rDataObject.m_bOwner = false;
    
    return rDataObject;
  }

  DataObject DataObject::AppendChild(const DataObject& rDataObject)
  {
    STAFF_ASSERT(m_pAxiomNode, "Not initialized");
    STAFF_ASSERT(rDataObject.m_pAxiomNode, "Child is not initialized");

    axiom_node_add_child(m_pAxiomNode, m_pEnv, const_cast<DataObject&>(rDataObject).m_pAxiomNode);
    const_cast<DataObject&>(rDataObject).m_bOwner = false;

    return rDataObject;
  }

  DataObject DataObject::DetachChild(Iterator& itChild)
  {
    STAFF_ASSERT_DOM(itChild.m_pDataObject != NULL && *(itChild.m_pDataObject) == *this,
      "Iterator is not bound to current dataobject");
    STAFF_ASSERT_DOM(itChild.m_pAxiomNode != NULL, "Iterator == End()");

    return DataObject(axiom_node_detach(itChild.m_pAxiomNode, m_pEnv));
  }

  void DataObject::RemoveChild(Iterator& itChild)
  {
    STAFF_ASSERT_DOM(itChild.m_pDataObject != NULL && *(itChild.m_pDataObject) == *this,
      "Iterator is not bound to current dataobject");
    STAFF_ASSERT_DOM(itChild.m_pAxiomNode != NULL, "Iterator == End()");

    axiom_node_free_tree(itChild.m_pAxiomNode, m_pEnv);
  }

  void DataObject::RemoveChildByLocalName(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    if (FindChildByLocalName(szLocalName, &pNode))
    {
      axiom_node_free_tree(pNode, m_pEnv);
    }
  }

  void DataObject::RemoveChildByLocalName(const std::string& sLocalName)
  {
    RemoveChildByLocalName(sLocalName.c_str());
  }

  void DataObject::RemoveAllChildren()
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

  DataObject::Iterator DataObject::FindChildByQName(const QName& rQName)
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

  DataObject::ConstIterator DataObject::FindChildByQName(const QName& rQName) const
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

  DataObject::Iterator DataObject::FindChildByQName(const QName& rQName, const Iterator& itStart)
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    STAFF_ASSERT_DOM(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this,
      "Iterator is not bound to current dataobject");

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

  DataObject::ConstIterator DataObject::FindChildByQName(const QName& rQName, const ConstIterator& itStart) const
  {
    axiom_element_t* pElem = NULL;
    axutil_qname_t* pqName = NULL;
    const axis2_char_t* szLocalName = NULL;
    const axis2_char_t* szPrefix = NULL;
    const axis2_char_t* szNamespaceUri = NULL;

    const std::string& sLocalPart = rQName.GetLocalPart();
    const std::string& sPrefix = rQName.GetPrefix();
    const std::string& sNamespaceUri = rQName.GetNamespaceUri();

    STAFF_ASSERT_DOM(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this,
      "Iterator is not bound to current dataobject");

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

  bool DataObject::FindChildByLocalName(const char* szLocalName, axiom_node_t** ppNode, axiom_element_t** ppElement) const
  {
    STAFF_ASSERT_PARAM(ppNode != NULL);

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

  bool DataObject::FindChildByLocalName(const char* szLocalName, axiom_node_t* pNodeStart,
                                         axiom_node_t** ppNode, axiom_element_t** ppElement) const
  {
    STAFF_ASSERT_PARAM(ppNode != NULL);

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

  DataObject::Iterator DataObject::FindChildByLocalName(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, &pNode);
    return Iterator(this, pNode);
  }

  DataObject::Iterator DataObject::FindChildByLocalName(const std::string& sLocalName)
  {
    return FindChildByLocalName(sLocalName.c_str());
  }

  DataObject::ConstIterator DataObject::FindChildByLocalName(const char* szLocalName) const
  {
    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, &pNode);
    return ConstIterator(this, pNode);
  }

  DataObject::ConstIterator DataObject::FindChildByLocalName(const std::string& sLocalName) const
  {
    return FindChildByLocalName(sLocalName.c_str());
  }

  DataObject::Iterator DataObject::FindChildByLocalName(const char* szLocalName, const Iterator& itStart)
  {
    STAFF_ASSERT_DOM(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this,
      "Iterator is not bound to current dataobject");

    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, itStart.m_pAxiomNode, &pNode);
    return Iterator(this, pNode);
  }

  DataObject::Iterator DataObject::FindChildByLocalName(const std::string& sLocalName, const Iterator& itStart)
  {
    return FindChildByLocalName(sLocalName.c_str(), itStart);
  }

  DataObject::ConstIterator DataObject::FindChildByLocalName(const char* szLocalName,
                                                               const ConstIterator& itStart) const
  {
    STAFF_ASSERT_DOM(itStart.m_pDataObject != NULL && *(itStart.m_pDataObject) == *this,
      "Iterator is not bound to current dataobject");

    axiom_node_t* pNode = NULL;
    FindChildByLocalName(szLocalName, itStart.m_pAxiomNode, &pNode);
    return ConstIterator(this, pNode);
  }

  DataObject::ConstIterator DataObject::FindChildByLocalName(const std::string& sLocalName,
                                                               const ConstIterator& itStart) const
  {
    return FindChildByLocalName(sLocalName.c_str(), itStart);
  }

  DataObject DataObject::GetChildByQName(const QName& stQName)
  {
    axiom_node_t* pNode = FindChildByQName(stQName).m_pAxiomNode;
    STAFF_ASSERT_DOM(pNode != NULL, "Child item \"" + stQName.ToString() + "\" is not found");
    return DataObject(pNode);
  }

  const DataObject DataObject::GetChildByQName(const QName& stQName) const
  {
    axiom_node_t* pNode = FindChildByQName(stQName).m_pAxiomNode;
    STAFF_ASSERT_DOM(pNode != NULL, "Child item \"" + stQName.ToString() + "\" is not found");
    return DataObject(pNode);
  }

  DataObject DataObject::GetChildByLocalName(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    STAFF_ASSERT_DOM(FindChildByLocalName(szLocalName, &pNode, &pElement),
                  "Child item \"" + std::string(szLocalName) + "\" is not found");
    return DataObject(pNode, pElement);
  }

  DataObject DataObject::GetChildByLocalName(const std::string& sLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    STAFF_ASSERT_DOM(FindChildByLocalName(sLocalName.c_str(), &pNode, &pElement),
                  "Child item \"" + sLocalName + "\" is not found");
    return DataObject(pNode, pElement);
  }

  const DataObject DataObject::GetChildByLocalName(const char* szLocalName) const
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    STAFF_ASSERT_DOM(FindChildByLocalName(szLocalName, &pNode, &pElement),
                  "Child item \"" + std::string(szLocalName) + "\" is not found");
    return DataObject(pNode, pElement);
  }

  const DataObject DataObject::GetChildByLocalName(const std::string& sLocalName) const
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    STAFF_ASSERT_DOM(FindChildByLocalName(sLocalName.c_str(), &pNode, &pElement),
                  "Child item \"" + sLocalName + "\" is not found");
    return DataObject(pNode, pElement);
  }

  DataObject DataObject::GetChildByLocalNameOpt(const char* szLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    FindChildByLocalName(szLocalName, &pNode, &pElement);
    return DataObject(pNode, pElement);
  }

  DataObject DataObject::GetChildByLocalNameOpt(const std::string& sLocalName)
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    FindChildByLocalName(sLocalName.c_str(), &pNode, &pElement);
    return DataObject(pNode, pElement);
  }

  const DataObject DataObject::GetChildByLocalNameOpt(const char* szLocalName) const
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    FindChildByLocalName(szLocalName, &pNode, &pElement);
    return DataObject(pNode, pElement);
  }

  const DataObject DataObject::GetChildByLocalNameOpt(const std::string& sLocalName) const
  {
    axiom_node_t* pNode = NULL;
    axiom_element_t* pElement = NULL;
    FindChildByLocalName(sLocalName.c_str(), &pNode, &pElement);
    return DataObject(pNode, pElement);
  }

  //////////////////////////////////////////////////////////////////////////
  // data manipulation

  // optimized set value functions
  void DataObject::SetValue(bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(long double ldValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    SetValue(szBuffer);
  }

  void DataObject::SetValue(const char* szText)
  {
    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, szText, m_pAxiomNode);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "can't set node value");
  }

  void DataObject::SetValue(const std::string& sText)
  {
    SetValue(sText.c_str());
  }

  void DataObject::SetValue(const QName& rstQName)
  {
    SetValue(rstQName.ToString().c_str());
  }


  // optimized get value functions
  bool DataObject::GetValue(bool& rbValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rbValue);
  }

  bool DataObject::GetValue(byte& rbtValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rbtValue);
  }

  bool DataObject::GetValue(int& rnValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rnValue);
  }

  bool DataObject::GetValue(short& rshValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rshValue);
  }

  bool DataObject::GetValue(long& rlValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rlValue);
  }

  bool DataObject::GetValue(long long& rllValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rllValue);
  }

  bool DataObject::GetValue(unsignedByte& rubtValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rubtValue);
  }

  bool DataObject::GetValue(unsigned int& runValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, runValue);
  }

  bool DataObject::GetValue(unsigned short& rushValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rushValue);
  }

  bool DataObject::GetValue(unsigned long& rulValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rulValue);
  }

  bool DataObject::GetValue(unsigned long long& rullValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rullValue);
  }

  bool DataObject::GetValue(float& rfValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rfValue);
  }

  bool DataObject::GetValue(double& rdValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rdValue);
  }

  bool DataObject::GetValue(long double& rldValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rldValue);
  }

  void DataObject::GetValue(std::string& sValue) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);

    // szText == NULL in case "<node></node>"
    sValue = szText ? szText : "";
  }

  void DataObject::GetValue(QName& rstQName) const
  {
    char* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);

    rstQName.FromString(szText ? szText : "");
  }

  Value DataObject::GetValue() const
  {
    return GetText();
  }

  void DataObject::SetValue(const Value& rValue)
  {
    SetText(rValue.AsString().c_str());
  }

  std::string DataObject::GetText() const
  {
    axis2_char_t* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);

    return szText ? szText : "";
  }

  void DataObject::GetText(std::string& sText) const
  {
    axis2_char_t* szText = axiom_element_get_text(m_pAxiomElement, m_pEnv, m_pAxiomNode);

    if (!szText)
    {
      sText.clear();
    }
    else
    {
      sText = szText;
    }
  }

  void DataObject::SetText(const char* szText)
  {
    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, szText, m_pAxiomNode);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "can't set text");
  }

  void DataObject::SetText(const std::string& sText)
  {
    axis2_status_t nResult =
        axiom_element_set_text(m_pAxiomElement, m_pEnv, sText.c_str(), m_pAxiomNode);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "can't set text");
  }

  bool DataObject::IsTextNull()
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    bool bResult = true;

    for (axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
         pNode; pNode = axiom_node_get_next_sibling(pNode, m_pEnv))
    {
      if (axiom_node_get_node_type(pNode, m_pEnv) == AXIOM_TEXT)
      {
        bResult = false;
        break;
      }
    }

    return bResult;
  }

  void DataObject::SetTextNull()
  {
    STAFF_ASSERT(m_pAxiomNode != NULL, "Not initialized");

    axiom_node_t* pTempNode = NULL;
    axiom_node_t* pNextNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);
    while (pNextNode)
    {
      pTempNode = pNextNode;
      pNextNode = axiom_node_get_next_sibling(pTempNode, m_pEnv);
      if (axiom_node_get_node_type(pTempNode, m_pEnv) == AXIOM_TEXT)
      {
        axiom_node_free_tree(pTempNode, m_pEnv);
      }
    }
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, bool& rbValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rbValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, byte& rbtValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rbtValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, int& rnValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rnValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, short& rshValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rshValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, long& rlValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rlValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, long long& rllValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rllValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, unsignedByte& rubtValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rubtValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, unsigned int& runValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, runValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, unsigned short& rushValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rushValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, unsigned long& rulValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rulValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, unsigned long long& rullValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rullValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, float& rfValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rfValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, double& rdValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rdValue);
  }

  bool DataObject::GetChildValueByLocalName(const char* szLocalName, long double& rldValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);
    STAFF_ASSERT(szText, "value of node [" + GetLocalName() + "] is NULL");
    return FromCString(szText, rldValue);
  }

  void DataObject::GetChildValueByLocalName(const char* szLocalName, std::string& sValue) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);

    // szText == NULL in case "<node></node>"
    sValue = szText ? szText : "";
  }

  void DataObject::GetChildValueByLocalName(const char* szLocalName, QName& rstQName) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);

    rstQName.FromString(szText ? szText : "");
  }

  std::string DataObject::GetChildTextByLocalName(const char* szLocalName) const
  {
    const char* szText = GetChildTextByLocalNameC(szLocalName);

    // szText == NULL in case "<node></node>"
    return szText ? szText : "";
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, bool& rbValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rbValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, byte& rbtValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rbtValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, int& rnValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rnValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, short& rshValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rshValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, long& rlValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rlValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, long long& rllValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rllValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, unsignedByte& rubtValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rubtValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, unsigned int& runValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), runValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, unsigned short& rushValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rushValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, unsigned long& rulValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rulValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, unsigned long long& rullValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rullValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, float& rfValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rfValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, double& rdValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rdValue);
  }

  bool DataObject::GetChildValueByLocalName(const std::string& sLocalName, long double& rldValue) const
  {
    return GetChildValueByLocalName(sLocalName.c_str(), rldValue);
  }

  void DataObject::GetChildValueByLocalName(const std::string& sLocalName, std::string& sValue) const
  {
    const char* szText = GetChildTextByLocalNameC(sLocalName.c_str());
    sValue = szText ? szText : "";
  }

  void DataObject::GetChildValueByLocalName(const std::string& sLocalName, QName& rstQName) const
  {
    const char* szText = GetChildTextByLocalNameC(sLocalName.c_str());
    rstQName.FromString(szText ? szText : "");
  }

  std::string DataObject::GetChildTextByLocalName(const std::string& sLocalName) const
  {
    const char* szText = GetChildTextByLocalNameC(sLocalName.c_str());
    return szText ? szText : "";
  }

  const char* DataObject::GetChildTextByLocalNameC(const char* szLocalName) const
  {
    STAFF_ASSERT_PARAM(szLocalName);
    axiom_node_t* pChildNode = NULL;
    axiom_element_t* pChildElement = NULL;
    STAFF_ASSERT(FindChildByLocalName(szLocalName, &pChildNode, &pChildElement),
                 "Child element [" + std::string(szLocalName) + "] does not exists");
    return axiom_element_get_text(pChildElement, m_pEnv, pChildNode);
  }

  //////////////////////////////////////////////////////////////////////////
  // dynamic types support

  void DataObject::SetInstanceType(const char* szInstanceType)
  {
    DeclareNamespace(g_szXsiSchemaUrl, "xsi");
    CreateAttribute("xsi:type", szInstanceType);
  }

  void DataObject::SetInstanceType(const std::string& sInstanceType)
  {
    DeclareNamespace(g_szXsiSchemaUrl, "xsi");
    CreateAttribute("xsi:type", sInstanceType);
  }

  void DataObject::SetCppInstanceType(const char* szCppInstanceType, const char* szUri)
  {
    std::string sType;
    SetNamespaceUriGenPrefix(szUri, &sType);

    if (!sType.empty())
    {
      SetInstanceType(sType.append(":").append(szCppInstanceType));
    }
    else
    {
      SetInstanceType(szCppInstanceType);
    }
  }

  void DataObject::SetCppInstanceType(const std::string& sCppInstanceType, const std::string& sUri)
  {
    SetCppInstanceType(sCppInstanceType, sUri.c_str());
  }

  std::string DataObject::GetInstanceType() const
  {
    const std::string& sXsiPrefix = GetNamespacePrefixByUri(g_szXsiSchemaUrl);
    return GetAttributeTextByName(sXsiPrefix.empty() ? "type" : sXsiPrefix + ":type");
  }

  void DataObject::GetCppInstanceType(std::string& sCppInstanceType, std::string& sUri) const
  {
    const std::string& sInstanceType = GetInstanceType();

    std::string::size_type nPos = sInstanceType.find_last_of(':');
    std::string sPrefix;
    if (nPos != std::string::npos)
    {
      sPrefix = sInstanceType.substr(0, nPos);
      sCppInstanceType = sInstanceType.substr(nPos + 1);
    }
    else
    {
      sCppInstanceType = sInstanceType;
    }

    sUri = GetNamespaceUriByPrefix(sPrefix);
  }

  bool DataObject::GetInstanceTypeOpt(std::string& sInstanceType) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_namespace_t* pNs = axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode,
                                                          g_szXsiSchemaUrl, NULL);
    if (!pNs)
    {
      return false;
    }

    const char* szPrefix = static_cast<const char*>(axiom_namespace_get_prefix(pNs, m_pEnv));
    const std::string& sXsiPrefix(szPrefix ? szPrefix : "");

    axiom_attribute_t* pAttr =
        GetAxiomAttributeByLocalName((sXsiPrefix.empty() ? "type" : sXsiPrefix + ":type").c_str());
    if (!pAttr)
    {
      return false;
    }

    const char* szValue = axiom_attribute_get_value(pAttr, m_pEnv);
    sInstanceType = szValue ? szValue : "";
    return true;
  }

  void DataObject::RemoveInstanceType()
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    const std::string& sXsiPrefix = GetNamespacePrefixByUri(g_szXsiSchemaUrl);
    axiom_attribute_t* pAttr =
        GetAxiomAttributeByLocalName((sXsiPrefix.empty() ? "type" : sXsiPrefix + ":type").c_str());
    axiom_element_remove_attribute(m_pAxiomElement, m_pEnv, pAttr);
  }

  //////////////////////////////////////////////////////////////////////////
  // xsd:nillable support

  void DataObject::SetNil(bool bIsNil /*= true*/)
  {
    DeclareNamespace(g_szXsiSchemaUrl, "xsi");
    CreateAttribute("xsi:nil", bIsNil ? "true" : "false");
  }

  bool DataObject::IsNil() const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_namespace_t* pNs = axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode,
                                                          g_szXsiSchemaUrl, NULL);
    if (!pNs)
    {
      return false;
    }

    const char* szValue = NULL;
    const char* szPrefix = static_cast<const char*>(axiom_namespace_get_prefix(pNs, m_pEnv));

    if (!szPrefix || !szPrefix[0])
    {
      szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                          const_cast<char*>("nil"));
    }
    else
    {
      char* szAttrName = axutil_stracat(m_pEnv, szPrefix, ":nil");
      szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv, szAttrName);
      AXIS2_FREE(m_pEnv->allocator, szAttrName);
    }

    if (!szValue)
    {
      return false;
    }

    return !axutil_strcmp(szValue, "true") || !axutil_strcmp(szValue, "1");
  }


  //////////////////////////////////////////////////////////////////////////
  // namespace management

  Namespace DataObject::GetDefaultNamespace() const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
#ifndef AXIS2_VERSION_1_7_0_AND_ABOVE
    Namespace tNs(axiom_element_get_default_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode));
#else
    // just return current namespace
    Namespace tNs(axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode));
#endif
    return tNs;
  }

  void DataObject::DeclareDefaultNamespace(const char* szUri)
  {
    STAFF_ASSERT_PARAM(m_pAxiomElement);
#ifndef AXIS2_VERSION_1_7_0_AND_ABOVE
    axiom_element_declare_default_namespace(m_pAxiomElement, m_pEnv, const_cast<axis2_char_t*>(szUri));
#else
    axiom_namespace_t* pAxiomNamespace = axiom_namespace_create(m_pEnv, szUri, "");
    STAFF_ASSERT(pAxiomNamespace, "Failed to create axiom_namespace");
    axiom_element_declare_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, pAxiomNamespace);
#endif
  }

  void DataObject::DeclareDefaultNamespace(const std::string& sUri)
  {
    DeclareDefaultNamespace(sUri.c_str());
  }

  void DataObject::RedeclareParentNamespace()
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_node_t* pParentNode = axiom_node_get_parent(m_pAxiomNode, m_pEnv);
    if (pParentNode)
    {
      axiom_element_t* pParentElement =
          reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pParentNode, m_pEnv));
      if (pParentElement)
      {
        axiom_namespace_t* pParentNamespace = axiom_element_get_namespace(pParentElement, m_pEnv, pParentNode);
        if (pParentNamespace)
        {
          axiom_element_set_namespace_assume_param_ownership(m_pAxiomElement, m_pEnv, pParentNamespace);
        }
      }
    }
  }

  Namespace DataObject::GetNamespace() const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    Namespace tNs(axiom_element_get_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode));
    return tNs;
  }

  void DataObject::DeclareNamespace(Namespace& rNamespace)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_element_declare_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, rNamespace);
    rNamespace.SetDataObject(this);
  }

  void DataObject::DeclareNamespace(const char* szUri, const char* szPrefix /*= ""*/)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_namespace_t* pNamespace = axiom_namespace_create(m_pEnv, szUri, szPrefix);
    STAFF_ASSERT(pNamespace, "Failed to create namespace");
    axiom_element_declare_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, pNamespace);
  }

  void DataObject::DeclareNamespace(const std::string& sUri, const std::string& sPrefix /*= ""*/)
  {
    DeclareNamespace(sUri.c_str(), sPrefix.c_str());
  }

  void DataObject::SetNamespace(Namespace& rNamespace)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, rNamespace, m_pAxiomNode);
    rNamespace.SetDataObject(this);
  }

  void DataObject::SetNamespace(const char* szUri, const char* szPrefix /*= ""*/)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_namespace_t* pNamespace = axiom_namespace_create(m_pEnv, szUri, szPrefix);
    STAFF_ASSERT(pNamespace, "Failed to create namespace");
    axiom_element_set_namespace(m_pAxiomElement, m_pEnv, pNamespace, m_pAxiomNode);
  }

  void DataObject::SetNamespace(const std::string& sUri, const std::string& sPrefix /*= ""*/)
  {
    DeclareNamespace(sUri.c_str(), sPrefix.c_str());
  }

  std::string DataObject::GetNamespacePrefixByUri(const char* szUri) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_namespace_t* pNs = axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, szUri, NULL);
    STAFF_ASSERT(pNs, "Can't find prefix for uri: [" + std::string(szUri) + "]");
    const char* pPrefix = static_cast<const char*>(axiom_namespace_get_prefix(pNs, m_pEnv));
    return pPrefix ? pPrefix : "";
  }

  std::string DataObject::GetNamespacePrefixByUri(const std::string& sUri) const
  {
    return GetNamespacePrefixByUri(sUri.c_str());
  }

  std::string DataObject::GetNamespaceUriByPrefix(const char* szPrefix) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_namespace_t* pNs = axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, NULL, szPrefix);
    STAFF_ASSERT(pNs, "Can't find uri for prefix: [" + std::string(szPrefix) + "]");
    const char* szUri = static_cast<const char*>(axiom_namespace_get_uri(pNs, m_pEnv));
    return szUri ? szUri : "";
  }

  std::string DataObject::GetNamespaceUriByPrefix(const std::string& sPrefix) const
  {
    return GetNamespaceUriByPrefix(sPrefix.c_str());
  }

  Namespace DataObject::FindNamespace(const std::string& sUri)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    Namespace tNs(axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, sUri.c_str(), NULL));
    return tNs;
  }

  Namespace DataObject::FindNamespace(const std::string& sUri, const std::string& sPrefix)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    Namespace tNs(axiom_element_find_namespace(m_pAxiomElement, m_pEnv, m_pAxiomNode, sUri.c_str(), sPrefix.c_str()));
    return tNs;
  }

  Namespace DataObject::FindDeclaredNamespace(const std::string& sUri)
  {
    STAFF_ASSERT(m_pAxiomElement, "Not initialized");
    Namespace tNs(axiom_element_find_declared_namespace(m_pAxiomElement, m_pEnv, sUri.c_str(), NULL));
    return tNs;
  }

  Namespace DataObject::FindDeclaredNamespace(const std::string& sUri, const std::string& sPrefix)
  {
    STAFF_ASSERT(m_pAxiomElement, "Not initialized");
    Namespace tNs(axiom_element_find_declared_namespace(m_pAxiomElement, m_pEnv, sUri.c_str(), sPrefix.c_str()));
    return tNs;
  }

  Namespace DataObject::FindNamespaceUri(const std::string& sPrefix) const
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    Namespace tNs(axiom_element_find_namespace_uri(m_pAxiomElement, m_pEnv,
      const_cast<axis2_char_t*>(sPrefix.c_str()), m_pAxiomNode));
    return tNs;
  }

  //////////////////////////////////////////////////////////////////////////
  // attribute management

  void DataObject::CreateAttribute(const char* szAttrName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, long double ldValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(szAttrName, szBuffer);
  }

  void DataObject::CreateAttribute(const char* szAttrName, const QName& rstQName)
  {
    return CreateAttribute(szAttrName, rstQName.ToString().c_str());
  }


  void DataObject::CreateAttribute(const std::string& sAttrName, bool bValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(bValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, byte btValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(btValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, int nValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(nValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, short shValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(shValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, long lValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(lValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, long long llValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(llValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, unsignedByte ubtValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ubtValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, unsigned int unValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(unValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, unsigned short ushValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ushValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, unsigned long ulValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ulValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, unsigned long long ullValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ullValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, float fValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(fValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, double dValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(dValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, long double ldValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(ldValue, szBuffer, NUM_TO_STR_BUFF_SIZE);
    return CreateAttribute(sAttrName.c_str(), szBuffer);
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, const QName& rstQName)
  {
    return CreateAttribute(sAttrName.c_str(), rstQName.ToString().c_str());
  }


  void DataObject::CreateAttribute(const char* szAttrName, const char* szAttrText)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axiom_attribute_t* pAxiomAttribute =
        axiom_attribute_create(m_pEnv, szAttrName, szAttrText, NULL);
    STAFF_ASSERT(pAxiomAttribute, "Failed to create axiom attribute");

    axis2_status_t nResult =
        axiom_element_add_attribute(m_pAxiomElement, m_pEnv, pAxiomAttribute, m_pAxiomNode);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "Failed to add axiom attribute");
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, const char* szAttrText)
  {
    CreateAttribute(sAttrName.c_str(), szAttrText);
  }

  void DataObject::CreateAttribute(const char* szAttrName, const std::string& sAttrText)
  {
    CreateAttribute(szAttrName, sAttrText.c_str());
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, const std::string& sAttrText)
  {
    CreateAttribute(sAttrName.c_str(), sAttrText.c_str());
  }

  void DataObject::CreateAttribute(const char* szAttrName, const Value& rValue)
  {
    CreateAttribute(szAttrName, rValue.AsString().c_str());
  }

  void DataObject::CreateAttribute(const std::string& sAttrName, const Value& rValue)
  {
    CreateAttribute(sAttrName.c_str(), rValue.AsString().c_str());
  }

  void DataObject::AppendAttribute(Attribute& rAttribute)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");

    axis2_status_t nResult =
        axiom_element_add_attribute(m_pAxiomElement, m_pEnv, rAttribute, m_pAxiomNode);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "Failed to add axiom attribute");
    rAttribute.SetOwner(false);
  }

  void DataObject::RemoveAttribute(AttributeIterator& itAttribute)
  {
    STAFF_ASSERT(m_pAxiomNode != NULL && m_pAxiomElement != NULL, "Not initialized");
    axiom_element_remove_attribute(m_pAxiomElement, m_pEnv, *itAttribute);
  }

  void DataObject::RemoveAllAttributes()
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

  DataObject::AttributeIterator DataObject::FindAttributeByQName(const QName& stQName)
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

  DataObject::AttributeIterator DataObject::FindAttributeByQName(const QName& stQName,
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

  DataObject::AttributeIterator DataObject::FindAttributeByLocalName(const std::string& sLocalName)
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

  DataObject::ConstAttributeIterator DataObject::FindAttributeByQName(const QName& stQName) const
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

  DataObject::ConstAttributeIterator DataObject::FindAttributeByQName(const QName& stQName,
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

  DataObject::ConstAttributeIterator DataObject::FindAttributeByLocalName(const std::string& sLocalName) const
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

  Attribute DataObject::GetAttributeByQName(const QName& stQName)
  {
    Attribute tAttr(this, axiom_element_get_attribute(m_pAxiomElement, m_pEnv, stQName));
    return tAttr;
  }

  const Attribute DataObject::GetAttributeByQName(const QName& stQName) const
  {
    Attribute tAttr(const_cast<DataObject*>(this), axiom_element_get_attribute(m_pAxiomElement,
                                                                                 m_pEnv, stQName));
    return tAttr;
  }

  Attribute DataObject::GetAttributeByLocalName(const char* szLocalName)
  {
    axiom_attribute_t* pAttr = GetAxiomAttributeByLocalName(szLocalName);
    STAFF_ASSERT(pAttr, "Attribute [" + std::string(szLocalName) + "] is not found");
    return Attribute(this, pAttr);
  }

  Attribute DataObject::GetAttributeByLocalName(const std::string& sLocalName)
  {
    axiom_attribute_t* pAttr = GetAxiomAttributeByLocalName(sLocalName.c_str());
    STAFF_ASSERT(pAttr, "Attribute [" + sLocalName + "] is not found");
    return Attribute(this, pAttr);
  }

  const Attribute DataObject::GetAttributeByLocalName(const char* szLocalName) const
  {
    axiom_attribute_t* pAttr = GetAxiomAttributeByLocalName(szLocalName);
    STAFF_ASSERT(pAttr, "Attribute [" + std::string(szLocalName) + "] is not found");
    return Attribute(const_cast<DataObject*>(this), pAttr);
  }

  const Attribute DataObject::GetAttributeByLocalName(const std::string& sLocalName) const
  {
    axiom_attribute_t* pAttr = GetAxiomAttributeByLocalName(sLocalName.c_str());
    STAFF_ASSERT(pAttr, "Attribute [" + sLocalName + "] is not found");
    return Attribute(const_cast<DataObject*>(this), pAttr);
  }

  Attribute DataObject::GetAttributeByLocalNameOpt(const char* szLocalName)
  {
    return Attribute(this, GetAxiomAttributeByLocalName(szLocalName));
  }

  Attribute DataObject::GetAttributeByLocalNameOpt(const std::string& sLocalName)
  {
    return Attribute(this, GetAxiomAttributeByLocalName(sLocalName.c_str()));
  }

  const Attribute DataObject::GetAttributeByLocalNameOpt(const char* szLocalName) const
  {
    return Attribute(const_cast<DataObject*>(this), GetAxiomAttributeByLocalName(szLocalName));
  }

  const Attribute DataObject::GetAttributeByLocalNameOpt(const std::string& sLocalName) const
  {
    return Attribute(const_cast<DataObject*>(this), GetAxiomAttributeByLocalName(sLocalName.c_str()));
  }

  axiom_attribute_t* DataObject::GetAxiomAttributeByLocalName(const char *szLocalName) const
  {
    STAFF_ASSERT(m_pAxiomElement, "Not initialized");

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
          axiom_attribute_t* pAttr = reinterpret_cast<axiom_attribute_t*>(pHashValue);
          if (!axutil_strcmp(szLocalName, axiom_attribute_get_localname(pAttr, m_pEnv)))
          {
            return pAttr;
          }
        }
      }
    }

    return NULL;
  }

  Value DataObject::GetAttributeValueByName(const char* szLocalName) const
  {
    const char* szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                                    const_cast<char*>(szLocalName));
    return Value(szValue ? szValue : "");
  }

  Value DataObject::GetAttributeValueByName(const std::string& sLocalName) const
  {
    return GetAttributeValueByName(sLocalName.c_str());
  }

  std::string DataObject::GetAttributeTextByName(const std::string& sLocalName) const
  {
    const char* szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                                    const_cast<char*>(sLocalName.c_str()));
    return szValue ? szValue : "";
  }

  void DataObject::GetAttributeTextByName(const std::string& sLocalName, std::string& sText) const
  {
    GetAttributeTextByName(sLocalName.c_str(), sText);
  }

  void DataObject::GetAttributeTextByName(const char* szLocalName, std::string& sText) const
  {
    const char* szValue = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                                    const_cast<char*>(szLocalName));
    sText = szValue ? szValue : "";
  }

  // optimized get value functions
  bool DataObject::GetAttributeValueByName(const char* szLocalName, bool& rbValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rbValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, byte& rbtValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rbtValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, int& rnValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rnValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, short& rshValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rshValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, long& rlValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rlValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, long long& rllValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rllValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, unsignedByte& rubtValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rubtValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, unsigned int& runValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, runValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, unsigned short& rushValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rushValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, unsigned long& rulValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rulValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, unsigned long long& rullValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rullValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, float& rfValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rfValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, double& rdValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rdValue);
  }

  bool DataObject::GetAttributeValueByName(const char* szLocalName, long double& rldValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));
    STAFF_ASSERT(szText, "attribute [" + std::string(szLocalName) + "] is not found or NULL");
    return FromCString(szText, rldValue);
  }

  void DataObject::GetAttributeValueByName(const char* szLocalName, std::string& sValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));

    sValue = szText ? szText : "";
  }

  void DataObject::GetAttributeValueByName(const char* szLocalName, QName& sValue) const
  {
    char* szText = axiom_element_get_attribute_value_by_name(m_pAxiomElement, m_pEnv,
                                                             const_cast<char*>(szLocalName));

    sValue.FromString(szText ? szText : "");
  }

  // optimized get value functions
  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, bool& rbValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rbValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, byte& rbtValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rbtValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, int& rnValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rnValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, short& rshValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rshValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, long& rlValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rlValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, long long& rllValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rllValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, unsignedByte& rubtValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rubtValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, unsigned int& runValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), runValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, unsigned short& rushValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rushValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, unsigned long& rulValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rulValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, unsigned long long& rullValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rullValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, float& rfValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rfValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, double& rdValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rdValue);
  }

  bool DataObject::GetAttributeValueByName(const std::string& sLocalName, long double& rldValue) const
  {
    return GetAttributeValueByName(sLocalName.c_str(), rldValue);
  }

  void DataObject::GetAttributeValueByName(const std::string& sLocalName, std::string& sValue) const
  {
    GetAttributeValueByName(sLocalName.c_str(), sValue);
  }

  void DataObject::GetAttributeValueByName(const std::string& sLocalName, QName& sValue) const
  {
    GetAttributeValueByName(sLocalName.c_str(), sValue);
  }


  //////////////////////////////////////////////////////////////////////////
  // iteration

  DataObject::Iterator DataObject::Begin()
  {
    axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);

    while (pNode != NULL && 
      (axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT))
    {
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv);
    }

    return Iterator(this, pNode);
  }

  DataObject::ConstIterator DataObject::Begin() const
  {
    axiom_node_t* pNode = axiom_node_get_first_child(m_pAxiomNode, m_pEnv);

    while (pNode != NULL && 
      (axiom_node_get_node_type(pNode, m_pEnv) != AXIOM_ELEMENT))
    {
      pNode = axiom_node_get_next_sibling(pNode, m_pEnv);
    }

    return ConstIterator(this, pNode);
  }

  DataObject::Iterator DataObject::End()
  {
    return Iterator(this, NULL);
  }

  DataObject::ConstIterator DataObject::End() const
  {
    return ConstIterator(this, NULL);
  }

  DataObject::AttributeIterator DataObject::AttributeBegin()
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return AttributeIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);

    return AttributeIterator(this, pIndex);
  }

  DataObject::AttributeIterator DataObject::AttributeEnd()
  {
    return AttributeIterator(this, NULL);
  }

  DataObject::ConstAttributeIterator DataObject::AttributeBegin() const
  {
    axutil_hash_t* pAttrHash = axiom_element_get_all_attributes(m_pAxiomElement, m_pEnv);
    if (pAttrHash == NULL)
    {
      return ConstAttributeIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pAttrHash, m_pEnv);

    return ConstAttributeIterator(this, pIndex);
  }

  DataObject::ConstAttributeIterator DataObject::AttributeEnd() const
  {
    return ConstAttributeIterator(this, NULL);
  }

  DataObject::NamespaceIterator DataObject::NamespaceBegin()
  {
    axutil_hash_t* pNsHash = axiom_element_get_namespaces(m_pAxiomElement, m_pEnv);
    if (pNsHash == NULL)
    {
      return NamespaceIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pNsHash, m_pEnv);

    return NamespaceIterator(this, pIndex);
  }

  DataObject::NamespaceIterator DataObject::NamespaceEnd()
  {
    return NamespaceIterator(this, NULL);
  }

  DataObject::ConstNamespaceIterator DataObject::NamespaceBegin() const
  {
    axutil_hash_t* pNsHash = axiom_element_get_namespaces(m_pAxiomElement, m_pEnv);
    if (pNsHash == NULL)
    {
      return ConstNamespaceIterator(this, NULL); // end
    }

    axutil_hash_index_t* pIndex = axutil_hash_first(pNsHash, m_pEnv);

    return ConstNamespaceIterator(this, pIndex);
  }

  DataObject::ConstNamespaceIterator DataObject::NamespaceEnd() const
  {
    return ConstNamespaceIterator(this, NULL);
  }


  //////////////////////////////////////////////////////////////////////////
  // support operators

  DataObject& DataObject::operator=(axiom_node_t* pAxiomNode)
  {
    Attach(pAxiomNode);
    return *this;
  }

  DataObject& DataObject::operator=(const DataObject& rDataObject)
  {
    if (m_pAxiomNode != rDataObject.m_pAxiomNode)
    {
      Detach();
      m_bOwner = rDataObject.m_bOwner;
      rDataObject.m_bOwner = false;
      m_pAxiomNode = rDataObject.m_pAxiomNode;
      m_pAxiomElement = rDataObject.m_pAxiomElement;
    }
    return *this;
  }

  bool DataObject::operator==(const DataObject& rDataObject) const
  {
    return m_pAxiomNode == rDataObject.m_pAxiomNode &&
      m_pAxiomElement == rDataObject.m_pAxiomElement;
  }

  bool DataObject::operator!=(const DataObject& rDataObject) const
  {
    return !operator==(rDataObject);
  }

  DataObject& DataObject::FromString(const char* szXml)
  {
    STAFF_ASSERT(szXml, "szXml is NULL");
    return FromString(szXml, strlen(szXml));
  }

  DataObject& DataObject::FromString(const char* szXml, int nSize)
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
    STAFF_ASSERT_DOM(pAxiomXmlReader != NULL, "No Axis XML Reader");

    try
    {
      pStaxBuilder = axiom_stax_builder_create(m_pEnv, pAxiomXmlReader);
      STAFF_ASSERT_DOM(pStaxBuilder != NULL, "No Stax Builder");
    }
    catch (...)
    {
      axiom_xml_reader_free(pAxiomXmlReader, m_pEnv);
      throw;
    }

    try
    {
      axiom_document_t* pAxiomDoc = axiom_stax_builder_get_document(pStaxBuilder, m_pEnv);
      STAFF_ASSERT_DOM(pAxiomDoc != NULL, "No Axis Document");

#ifdef AXIS2_VERSION_1_7_0_AND_ABOVE
      // on Axis2/C-1.7.0 we should call axiom_document_get_root_element
      // else axiom_document_build_all will return NULL
      axiom_document_get_root_element(pAxiomDoc, m_pEnv);
#endif
      pAxiomNode = axiom_document_build_all(pAxiomDoc, m_pEnv);
      STAFF_ASSERT_DOM(pAxiomNode != NULL, "No Root Element in the pAxiomDoc");

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
      axiom_stax_builder_free_self(pStaxBuilder, m_pEnv);
      throw;
    }

    axiom_stax_builder_free_self(pStaxBuilder, m_pEnv);
    return *this;
  }

  DataObject& DataObject::FromString(const std::string& sXml)
  {
    return FromString(sXml.c_str(), static_cast<int>(sXml.size()));
  }

  std::string DataObject::ToString() const
  {
    std::string sXml;
    ToString(sXml);
    return sXml;
  }

  void DataObject::ToString(std::string& sResult) const
  {
    char* szXml = axiom_node_to_string(m_pAxiomNode, m_pEnv);
    STAFF_ASSERT_DOM(szXml != NULL, "cannot convert DataObject to string");
    sResult = szXml;
    AXIS2_FREE(m_pEnv->allocator, szXml);
  }

  DataObject* DataObject::operator->()
  {
    return this;
  }

  const DataObject* DataObject::operator->() const
  {
    return this;
  }

  axutil_env_t* DataObject::m_pEnv = Runtime::Inst().GetAxis2Env();


  //////////////////////////////////////////////////////////////////////////
  // Iterator

  DataObject::Iterator::Iterator():
    m_pDataObject(NULL),
    m_pAxiomNode(NULL)
  {
  }

  DataObject::Iterator::Iterator(DataObject* pDataObject, axiom_node_t* pAxiomNode):
    m_pDataObject(pDataObject),
    m_pAxiomNode(pAxiomNode)
  {
  }

  DataObject::Iterator::Iterator(const Iterator& rIter)
  {
    operator=(rIter);
  }

  DataObject::Iterator::~Iterator()
  {
  }


  DataObject::Iterator& DataObject::Iterator::operator=(const Iterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAxiomNode = rIter.m_pAxiomNode;
    return *this;
  }

  DataObject::Iterator& DataObject::Iterator::operator--()
  {
    STAFF_ASSERT_DOM(m_pDataObject, "Can't --DataObject.End()");
    
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
    
    STAFF_ASSERT_DOM(pNode != NULL, "Attempt to --begin()");
    m_pAxiomNode = pNode;
    return *this;
  }

  DataObject::Iterator& DataObject::Iterator::operator++()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAxiomNode != NULL, "Attempt to ++end()");

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

  DataObject::Iterator DataObject::Iterator::operator--(int)
  {
    Iterator itRes = *this;
    operator--();
    return itRes;
  }

  DataObject::Iterator DataObject::Iterator::operator++(int)
  {
    Iterator itRes = *this;
    operator++();
    return itRes;
  }

  bool DataObject::Iterator::operator==(const Iterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject && 
           m_pAxiomNode == rIter.m_pAxiomNode;
  }

  bool DataObject::Iterator::operator!=(const Iterator& rIter) const
  {
    return !operator==(rIter);
  }

  DataObject DataObject::Iterator::operator*()
  {
    return DataObject(m_pAxiomNode);
  }

  const DataObject DataObject::Iterator::operator*() const
  {
    return DataObject(m_pAxiomNode);
  }

  DataObject DataObject::Iterator::operator->()
  {
    return DataObject(m_pAxiomNode);
  }

  const DataObject DataObject::Iterator::operator->() const
  {
    return DataObject(m_pAxiomNode);
  }

  //////////////////////////////////////////////////////////////////////////
  // ConstIterator

  DataObject::ConstIterator::ConstIterator():
    m_pDataObject(NULL),
    m_pAxiomNode(NULL)
  {
  }

  DataObject::ConstIterator::ConstIterator(const DataObject* pDataObject, axiom_node_t* pAxiomNode):
    m_pDataObject(pDataObject),
    m_pAxiomNode(pAxiomNode)
  {
  }

  DataObject::ConstIterator::ConstIterator(const ConstIterator& rIter)
  {
    operator=(rIter);
  }

  DataObject::ConstIterator::~ConstIterator()
  {
  }


  DataObject::ConstIterator& DataObject::ConstIterator::operator=(const ConstIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAxiomNode = rIter.m_pAxiomNode;
    return *this;
  }

  DataObject::ConstIterator& DataObject::ConstIterator::operator--()
  {
    STAFF_ASSERT_DOM(m_pDataObject, "Can't --DataObject.End()");

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
    
    STAFF_ASSERT_DOM(pNode != NULL, "Attempt to --begin()");
    m_pAxiomNode = pNode;
    return *this;
  }

  DataObject::ConstIterator& DataObject::ConstIterator::operator++()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAxiomNode != NULL, "Attempt to ++end()");

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

  DataObject::ConstIterator DataObject::ConstIterator::operator--(int)
  {
    ConstIterator itRes = *this;
    operator--();
    return itRes;
  }

  DataObject::ConstIterator DataObject::ConstIterator::operator++(int)
  {
    ConstIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool DataObject::ConstIterator::operator==(const ConstIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject && 
           m_pAxiomNode == rIter.m_pAxiomNode;
  }

  bool DataObject::ConstIterator::operator!=(const ConstIterator& rIter) const
  {
    return !operator==(rIter);
  }

  const DataObject DataObject::ConstIterator::operator*() const
  {
    return m_pAxiomNode;
  }

  const DataObject DataObject::ConstIterator::operator->() const
  {
    return m_pAxiomNode;
  }

  //////////////////////////////////////////////////////////////////////////
  // NamespaceIterator

  DataObject::NamespaceIterator::NamespaceIterator():
    m_pDataObject(NULL),
    m_pNamespaceIndex(NULL)
  {
  }

  DataObject::NamespaceIterator::NamespaceIterator(DataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex):
    m_pDataObject(pDataObject),
    m_pNamespaceIndex(pNamespaceIndex)
  {
  }

  DataObject::NamespaceIterator::NamespaceIterator(const DataObject::NamespaceIterator& rIter)
  {
    operator=(rIter);
  }

  DataObject::NamespaceIterator::~NamespaceIterator()
  {
  }

  DataObject::NamespaceIterator& DataObject::NamespaceIterator::operator=(const DataObject::NamespaceIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pNamespaceIndex = rIter.m_pNamespaceIndex;
    return *this;
  }

  DataObject::NamespaceIterator& DataObject::NamespaceIterator::operator++()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pNamespaceIndex != NULL, "++ operator after end");

    m_pNamespaceIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pNamespaceIndex);
    return *this;
  }

  DataObject::NamespaceIterator DataObject::NamespaceIterator::operator++(int)
  {
    NamespaceIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool DataObject::NamespaceIterator::operator==(const DataObject::NamespaceIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pNamespaceIndex == rIter.m_pNamespaceIndex;
  }

  bool DataObject::NamespaceIterator::operator!=(const DataObject::NamespaceIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  Namespace DataObject::NamespaceIterator::operator*()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pNamespaceIndex != NULL, "* operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Cannot get namespace");

    Namespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), m_pDataObject);
    return tNs;
  }

  Namespace DataObject::NamespaceIterator::operator->()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pNamespaceIndex != NULL, "-> operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Cannot get namespace");

    Namespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), m_pDataObject);
    return tNs;
  }

  //////////////////////////////////////////////////////////////////////////
  // ConstNamespaceIterator

  DataObject::ConstNamespaceIterator::ConstNamespaceIterator():
    m_pDataObject(NULL),
    m_pNamespaceIndex(NULL)
  {
  }

  DataObject::ConstNamespaceIterator::ConstNamespaceIterator(const DataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex):
    m_pDataObject(pDataObject),
    m_pNamespaceIndex(pNamespaceIndex)
  {
  }

  DataObject::ConstNamespaceIterator::ConstNamespaceIterator(const DataObject::ConstNamespaceIterator& rIter)
  {
    operator=(rIter);
  }

  DataObject::ConstNamespaceIterator::~ConstNamespaceIterator()
  {
  }

  DataObject::ConstNamespaceIterator& DataObject::ConstNamespaceIterator::operator=(const DataObject::ConstNamespaceIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pNamespaceIndex = rIter.m_pNamespaceIndex;
    return *this;
  }

  DataObject::ConstNamespaceIterator& DataObject::ConstNamespaceIterator::operator++()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pNamespaceIndex != NULL, "++ operator after end");

    m_pNamespaceIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pNamespaceIndex);
    return *this;
  }

  DataObject::ConstNamespaceIterator DataObject::ConstNamespaceIterator::operator++(int)
  {
    ConstNamespaceIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool DataObject::ConstNamespaceIterator::operator==(const DataObject::ConstNamespaceIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pNamespaceIndex == rIter.m_pNamespaceIndex;
  }

  bool DataObject::ConstNamespaceIterator::operator!=(const DataObject::ConstNamespaceIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  const Namespace DataObject::ConstNamespaceIterator::operator*() const
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pNamespaceIndex != NULL, "* operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Cannot get namespace");

    Namespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), const_cast<DataObject*>(m_pDataObject));
    return tNs;
  }

  const Namespace DataObject::ConstNamespaceIterator::operator->() const
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pNamespaceIndex != NULL, "-> operator after end");

    void* pHashValue = NULL;

    axutil_hash_this(m_pNamespaceIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Cannot get namespace");

    Namespace tNs(reinterpret_cast<axiom_namespace_t*>(pHashValue), const_cast<DataObject*>(m_pDataObject));
    return tNs;
  }


  //////////////////////////////////////////////////////////////////////////
  // AttributeIterator

  DataObject::AttributeIterator::AttributeIterator():
    m_pDataObject(NULL),
    m_pAttributeIndex(NULL)
  {
  }

  DataObject::AttributeIterator::AttributeIterator(DataObject* pDataObject, axutil_hash_index_t* pAttributeIndex):
    m_pDataObject(pDataObject),
    m_pAttributeIndex(pAttributeIndex)
  {
  }

  DataObject::AttributeIterator::AttributeIterator(const DataObject::AttributeIterator& rIter)
  {
    operator=(rIter);
  }

  DataObject::AttributeIterator::~AttributeIterator()
  {
  }

  DataObject::AttributeIterator& DataObject::AttributeIterator::operator=(const DataObject::AttributeIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAttributeIndex = rIter.m_pAttributeIndex;
    return *this;
  }

  DataObject::AttributeIterator& DataObject::AttributeIterator::operator++()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAttributeIndex != NULL, "Attempt to ++end()");

    m_pAttributeIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pAttributeIndex);
    return *this;
  }

  DataObject::AttributeIterator DataObject::AttributeIterator::operator++(int)
  {
    AttributeIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool DataObject::AttributeIterator::operator==(const DataObject::AttributeIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pAttributeIndex == rIter.m_pAttributeIndex;
  }

  bool DataObject::AttributeIterator::operator!=(const DataObject::AttributeIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  Attribute DataObject::AttributeIterator::operator*()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAttributeIndex != NULL, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Can\'t get attribute");

    Attribute tAttr(m_pDataObject, reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  Attribute DataObject::AttributeIterator::operator->()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAttributeIndex != NULL, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Can\'t get attribute");

    Attribute tAttr(m_pDataObject, reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  //////////////////////////////////////////////////////////////////////////
  // ConstAttributeIterator

  DataObject::ConstAttributeIterator::ConstAttributeIterator():
    m_pDataObject(NULL),
    m_pAttributeIndex(NULL)
  {
  }

  DataObject::ConstAttributeIterator::ConstAttributeIterator(const DataObject* pDataObject, axutil_hash_index_t* pAttributeIndex):
    m_pDataObject(pDataObject),
    m_pAttributeIndex(pAttributeIndex)
  {
  }

  DataObject::ConstAttributeIterator::ConstAttributeIterator(const DataObject::ConstAttributeIterator& rIter)
  {
    operator=(rIter);
  }

  DataObject::ConstAttributeIterator::~ConstAttributeIterator()
  {
  }

  DataObject::ConstAttributeIterator& DataObject::ConstAttributeIterator::operator=(const DataObject::ConstAttributeIterator& rIter)
  {
    m_pDataObject = rIter.m_pDataObject;
    m_pAttributeIndex = rIter.m_pAttributeIndex;
    return *this;
  }

  DataObject::ConstAttributeIterator& DataObject::ConstAttributeIterator::operator++()
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAttributeIndex != NULL, "Attempt to ++end()");

    m_pAttributeIndex = axutil_hash_next(m_pDataObject->m_pEnv, m_pAttributeIndex);
    return *this;
  }

  DataObject::ConstAttributeIterator DataObject::ConstAttributeIterator::operator++(int)
  {
    ConstAttributeIterator itRes = *this;
    operator++();
    return itRes;
  }

  bool DataObject::ConstAttributeIterator::operator==(const DataObject::ConstAttributeIterator& rIter) const
  {
    return m_pDataObject == rIter.m_pDataObject &&
           m_pAttributeIndex == rIter.m_pAttributeIndex;
  }

  bool DataObject::ConstAttributeIterator::operator!=(const DataObject::ConstAttributeIterator& rIter) const
  {
    return !(operator==(rIter));
  }

  const Attribute DataObject::ConstAttributeIterator::operator*() const
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAttributeIndex != NULL, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Can\'t get attribute");

    //!!! const HACK
    Attribute tAttr(const_cast<DataObject*>(m_pDataObject), reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  const Attribute DataObject::ConstAttributeIterator::operator->() const
  {
    STAFF_ASSERT(m_pDataObject != NULL, "Iterator is not iterable");
    STAFF_ASSERT_DOM(m_pAttributeIndex != NULL, "Attempt to ++end()");

    void* pHashValue = NULL;

    axutil_hash_this(m_pAttributeIndex, NULL, NULL, &pHashValue);
    STAFF_ASSERT_DOM(pHashValue != NULL, "Can\'t get attribute");

    Attribute tAttr(const_cast<DataObject*>(m_pDataObject), reinterpret_cast<axiom_attribute_t*>(pHashValue));
    return tAttr;
  }

  DataObject& operator<<(DataObject& rDataObject, const Value& rValue)
  {
    rDataObject.SetText(rValue.AsString().c_str());
    return rDataObject;
  }

  const DataObject& operator>>(const DataObject& rDataObject, Value& rValue)
  {
    rDataObject.GetText(rValue);
    return rDataObject;
  }

}
