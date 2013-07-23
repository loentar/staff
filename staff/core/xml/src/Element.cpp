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

#include <iostream>
#include "Attribute.h"
#include "Namespace.h"
#include "Element.h"
#include "Exception.h"

namespace staff 
{
namespace xml
{

  Element::Element(Element* pParent):
    Node(TypeElement, pParent),
    m_pFirstAttribute(NULL),
    m_pFirstNamespace(NULL),
    m_pFirstChild(NULL),
    m_pLastChild(NULL)
  {
  }

  Element::Element(const std::string& sName, Element* pParent):
    Node(TypeElement, pParent),
    m_pFirstAttribute(NULL),
    m_pFirstNamespace(NULL),
    m_pFirstChild(NULL),
    m_pLastChild(NULL)
  {
    SetName(sName);
  }

  Element::Element(const std::string& sName, const Value& rValue, Element* pParent):
    Node(TypeElement, pParent),
    m_pFirstAttribute(NULL),
    m_pFirstNamespace(NULL),
    m_pFirstChild(NULL),
    m_pLastChild(NULL)
  {
    SetName(sName);
    CreateChildText(rValue);
  }

  Element::Element(const Element& rElement):
    Node(TypeElement, NULL),
    m_pFirstAttribute(NULL),
    m_pFirstNamespace(NULL),
    m_pFirstChild(NULL),
    m_pLastChild(NULL)
  {
    CloneElement(rElement);
  }

  Element& Element::operator=(const Element& rElement)
  {
    Clear();
    return CloneElement(rElement);
  }

  Element::~Element()
  {
    Clear();
  }

  Element* Element::CloneElement(bool bRecursive /*= true*/) const
  {
    Element* pClone = NULL;
    switch (m_eType)
    {
    case TypeElement:
      pClone = new Element;
      break;
    default:
      STAFF_THROW_ASSERT("Can't create node clone");
    }

    try
    {
      pClone->CloneElement(*this, bRecursive);
    }
    catch (...)
    {
      delete pClone;
      throw;
    }

    return pClone;
  }

  Element& Element::CloneElement(const Element& rElement, bool bRecursive)
  {
    m_sName = rElement.m_sName;
    m_sPrefix = rElement.m_sPrefix;

    // clone attributes
    if (rElement.m_pFirstAttribute)
    {
      Attribute* pNewAttribute = new Attribute(*rElement.m_pFirstAttribute);
      m_pFirstAttribute = pNewAttribute;

      for (const Attribute* pAttribute = rElement.m_pFirstAttribute->m_pNextSibling;
           pAttribute; pAttribute = pAttribute->m_pNextSibling)
      {
        pNewAttribute->m_pNextSibling = new Attribute(*pAttribute);
        pNewAttribute = pNewAttribute->m_pNextSibling;
      }
    }

    // clone namespaces
    if (rElement.m_pFirstNamespace)
    {
      // default namespace
      Namespace* pNewNamespace = new Namespace(*rElement.m_pFirstNamespace);
      m_pFirstNamespace = pNewNamespace;

      for (const Namespace* pNamespace = rElement.m_pFirstNamespace->m_pNextSibling;
           pNamespace; pNamespace = pNamespace->m_pNextSibling)
      {
        pNewNamespace->m_pNextSibling = new Namespace(*pNamespace);
        pNewNamespace = pNewNamespace->m_pNextSibling;
      }

      m_sPrefix = rElement.m_sPrefix;
    }

    // clone childs
    if (bRecursive)
    {
      for (const Node* pChild = rElement.m_pFirstChild; pChild; pChild = pChild->m_pNextSibling)
      {
        AppendChild(pChild->Clone());
      }
    }

    return *this;
  }

  const std::string& Element::GetName() const
  {
    return m_sName;
  }

  void Element::SetName(const std::string& sName)
  {
    std::string::size_type nPos = sName.find_last_of(':');
    if (nPos == std::string::npos)
    {
      m_sName = sName;
      m_sPrefix.erase();
    }
    else
    {
      m_sName.assign(sName, nPos + 1, std::string::npos);
      m_sPrefix.assign(sName, 0, nPos);
    }
  }

  const std::string& Element::GetPrefix() const
  {
    return m_sPrefix;
  }

  std::string Element::GetPrefixName() const
  {
    return m_sPrefix.empty() ? m_sName : (m_sPrefix + ":" + m_sName);
  }

  std::string Element::GetChildsText() const
  {
    std::string sText;
    for (const Node* pChild = m_pFirstChild; pChild; pChild = pChild->m_pNextSibling)
    {
      if (pChild->GetType() == TypeText)
      {
        sText += pChild->GetValue().AsString();
      }
    }
    return sText;
  }

  const Value& Element::GetValue() const
  {
    static const Value tEmpty;

    for (const Node* pChild = m_pFirstChild; pChild; pChild = pChild->m_pNextSibling)
    {
      if (pChild->GetType() == TypeText)
      {
        return pChild->GetValue();
      }
    }

    return tEmpty;
  }

  const std::string& Element::GetTextValue() const
  {
    static const std::string sEmpty;

    for (const Node* pChild = m_pFirstChild; pChild; pChild = pChild->m_pNextSibling)
    {
      if (pChild->GetType() == TypeText)
      {
        return pChild->GetTextValue();
      }
    }

    return sEmpty;
  }

  void Element::SetValue(const Value& rValue)
  {
    Node* pChildText = NULL;
    {
      Node* pChildToRemove = NULL;
      for (Node* pChild = m_pFirstChild; pChild; pChild = pChild->m_pNextSibling)
      {
        if (pChildToRemove)
        {
          delete pChildToRemove;
          pChildToRemove = NULL;
        }

        if (pChild->GetType() == TypeText)
        {
          if (pChildText)
          {
            pChildToRemove = pChild;
          }
          else
          {
            pChildText = pChild;
          }
        }
      }
      if (pChildToRemove)
      {
        delete pChildToRemove;
      }
    }

    if (pChildText)
    {
      pChildText->SetValue(rValue);
    }
    else
    {
      CreateChildText(rValue);
    }
  }

  bool Element::IsTextNull() const
  {
    const Node* pChild = m_pFirstChild;
    for (; pChild && pChild->GetType() != TypeText; pChild = pChild->m_pNextSibling);
    return !pChild;
  }

  void Element::SetTextNull()
  {
    Node* pChildToRemove = NULL;
    for (Node* pChild = m_pFirstChild; pChild;)
    {
      if (pChild->GetType() == TypeText)
      {
        pChildToRemove = pChild;
        pChild = pChild->m_pNextSibling;
        delete pChildToRemove;
      }
      else
      {
        pChild = pChild->m_pNextSibling;
      }
    }
  }

  bool Element::IsEmpty() const
  {
    return !m_pFirstChild ||
        ((m_pFirstChild == m_pLastChild) && (m_pFirstChild->m_eType == TypeText)
         && m_pFirstChild->GetValue().AsString().empty());
  }

  bool Element::IsLeaf() const
  {
    return !m_pFirstChild ||
        ((m_pFirstChild == m_pLastChild) && (m_pFirstChild->m_eType == TypeText));
  }

  void Element::Clear()
  {
    if (m_pFirstAttribute)
    {
      // destroy attributes
      Attribute* pAttribute = m_pFirstAttribute;
      Attribute* pPrevAttribute = NULL;
      while (pAttribute)
      {
        pPrevAttribute = pAttribute;
        pAttribute = pAttribute->m_pNextSibling;
        delete pPrevAttribute;
      }
    }

    if (m_pFirstNamespace)
    {
      // destroy namespaces
      Namespace* pNamespace = m_pFirstNamespace;
      Namespace* pPrevNamespace = NULL;
      while (pNamespace)
      {
        pPrevNamespace = pNamespace;
        pNamespace = pNamespace->m_pNextSibling;
        delete pPrevNamespace;
      }
    }

    if (m_pFirstChild)
    {
      Node* pChildToFree = NULL;
      for (Node* pChild = m_pFirstChild; pChild;)
      {
        pChildToFree = pChild;
        pChild = pChild->m_pNextSibling;

        pChildToFree->m_pParent = NULL; // prevent unneeded detaching
        delete pChildToFree;
      }
      m_pFirstChild = NULL;
      m_pLastChild = NULL;
    }
  }

  // childs management

  Element& Element::CreateChildElement()
  {
    return AppendChild(new Element);
  }

  Element& Element::CreateChildElement(const std::string& sName)
  {
    return AppendChild(new Element(sName));
  }

  Element& Element::CreateChildElement(const std::string& sName, const Value& rValue)
  {
    return AppendChild(new Element(sName, rValue));
  }

  Element& Element::CreateChildElementOnce(const std::string& sName)
  {
    Element* pChild = FindChildElementByName(sName);
    if (pChild)
    {
      return *pChild;
    }

    return AppendChild(new Element(sName));
  }

  Comment& Element::CreateChildComment()
  {
    return AppendChild(new Comment);
  }

  Comment& Element::CreateChildComment(const Value& rValue)
  {
    return AppendChild(new Comment(rValue));
  }

  Text& Element::CreateChildText()
  {
    return AppendChild(new Text);
  }

  Text& Element::CreateChildText(const Value& rValue)
  {
    return AppendChild(new Text(rValue));
  }

  Cdata& Element::CreateChildCdata()
  {
    return AppendChild(new Cdata);
  }

  Cdata& Element::CreateChildCdata(const Value& rValue)
  {
    return AppendChild(new Cdata(rValue));
  }

  Node& Element::AppendChild(Node* pNode)
  {
    STAFF_ASSERT_PARAM(pNode);
    STAFF_ASSERT(pNode != this, "Can't append self as child");
    STAFF_ASSERT(!pNode->m_pParent && !pNode->m_pNextSibling && !pNode->m_pPreviousSibling,
                 "Can't append node that is already in tree");

    if (!m_pFirstChild || !m_pLastChild)
    {
      STAFF_DEBUG_ASSERT(!m_pFirstChild && !m_pLastChild, "Internal error");
      m_pFirstChild = pNode;
    }
    else
    {
      // append
      STAFF_DEBUG_ASSERT(!m_pLastChild->m_pNextSibling, "Internal error");

      m_pLastChild->m_pNextSibling = pNode;
      pNode->m_pPreviousSibling = m_pLastChild;
    }

    pNode->m_pParent = this;
    m_pLastChild = pNode;

    return *pNode;
  }

  Element& Element::AppendChild(Element* pNode)
  {
    return static_cast<Element&>(AppendChild(reinterpret_cast<Node*>(pNode)));
  }

  Comment& Element::AppendChild(Comment* pNode)
  {
    return static_cast<Comment&>(AppendChild(reinterpret_cast<Node*>(pNode)));
  }

  Text& Element::AppendChild(Text* pNode)
  {
    return static_cast<Text&>(AppendChild(reinterpret_cast<Node*>(pNode)));
  }

  Cdata& Element::AppendChild(Cdata* pNode)
  {
    return static_cast<Cdata&>(AppendChild(reinterpret_cast<Node*>(pNode)));
  }

  void Element::RemoveChild(Node* pNode)
  {
    STAFF_ASSERT_PARAM(pNode);
    STAFF_ASSERT(pNode->GetParent() == this, "This child node belong to the different parent");
    delete pNode;
  }

  const Node* Element::GetFirstChild() const
  {
    return m_pFirstChild;
  }

  Node* Element::GetFirstChild()
  {
    return m_pFirstChild;
  }

  const Node* Element::GetLastChild() const
  {
    return m_pLastChild;
  }

  Node* Element::GetLastChild()
  {
    return m_pLastChild;
  }

  const Element* Element::GetFirstChildElement() const
  {
    const Node* pNode = m_pFirstChild;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->m_pNextSibling);
    return reinterpret_cast<const Element*>(pNode);
  }

  Element* Element::GetFirstChildElement()
  {
    Node* pNode = m_pFirstChild;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->m_pNextSibling);
    return reinterpret_cast<Element*>(pNode);
  }

  const Element* Element::GetLastChildElement() const
  {
    const Node* pNode = m_pLastChild;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->GetPreviousSibling());
    return reinterpret_cast<const Element*>(pNode);
  }

  Element* Element::GetLastChildElement()
  {
    Node* pNode = m_pLastChild;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->GetPreviousSibling());
    return reinterpret_cast<Element*>(pNode);
  }

  const Element* Element::FindChildElementByName(const std::string& sName) const
  {
    const Element* pFirstChild = GetFirstChildElement();
    return pFirstChild ? FindChildElementByName(sName, pFirstChild) : NULL;
  }

  Element* Element::FindChildElementByName(const std::string& sName)
  {
    Element* pFirstChild = GetFirstChildElement();
    return pFirstChild ? FindChildElementByName(sName, pFirstChild) : NULL;
  }

  const Element* Element::FindChildElementByName(const std::string& sName, const Element* pBegin) const
  {
    if (pBegin)
    {
      STAFF_ASSERT(pBegin->GetParent() == this, "This child node belong to the different parent");

      for (; pBegin && pBegin->m_sName != sName; pBegin = pBegin->GetNextSiblingElement());
    }
    return pBegin;
  }

  Element* Element::FindChildElementByName(const std::string& sName, Element* pBegin)
  {
    if (pBegin)
    {
      STAFF_ASSERT(pBegin->GetParent() == this, "This child node belong to the different parent");

      for (; pBegin && pBegin->m_sName != sName; pBegin = pBegin->GetNextSiblingElement());
    }
    return pBegin;
  }

  const Element& Element::GetChildElementByName(const std::string& sName) const
  {
    const Element* pElement = FindChildElementByName(sName);
    STAFF_ASSERT(pElement, "Child element with name [" + sName + "] does not exists");
    return *pElement;
  }

  Element& Element::GetChildElementByName(const std::string& sName)
  {
    Element* pElement = FindChildElementByName(sName);
    STAFF_ASSERT(pElement, "Child element with name [" + sName + "] does not exists");
    return *pElement;
  }

  unsigned Element::GetChildCount() const
  {
    unsigned nResult = 0;
    for (Node* pNode = m_pFirstChild; pNode; pNode = pNode->m_pNextSibling, ++nResult);
    return nResult;
  }

  unsigned Element::GetChildElementCount() const
  {
    unsigned nResult = 0;
    for (Node* pNode = m_pFirstChild; pNode; pNode = pNode->m_pNextSibling)
    {
      if (pNode->GetType() == TypeElement)
      {
        ++nResult;
      }
    }
    return nResult;
  }


  // attributes management

  Attribute& Element::CreateAttribute(const std::string& sName, const std::string& sValue)
  {
    return AppendAttribute(new Attribute(sName, sValue));
  }

  Attribute& Element::CreateAttribute(const std::string& sName, const std::string& sValue,
                                      const std::string& sPrefix)
  {
    return AppendAttribute(new Attribute(sName, sValue, sPrefix));
  }

  Attribute& Element::AppendAttribute(const Attribute& rAttribute)
  {
    return AppendAttribute(new Attribute(rAttribute));
  }

  Attribute& Element::AppendAttribute(Attribute* pAttribute)
  {
    STAFF_ASSERT_PARAM(pAttribute);

    if (!m_pFirstAttribute)
    {
      // there is no attributes yet.
      m_pFirstAttribute = pAttribute;
    }
    else
    {
      // searching for last attribute
      Attribute* pLastAttribute = m_pFirstAttribute;
      for (; pLastAttribute->m_pNextSibling; pLastAttribute = pLastAttribute->m_pNextSibling);
      pLastAttribute->m_pNextSibling = pAttribute;
    }

    return *pAttribute;
  }

  const Attribute* Element::FindAttribute(const std::string& sName) const
  {
    const Attribute* pAttribute = m_pFirstAttribute;
    for (; pAttribute && pAttribute->GetName() != sName; pAttribute = pAttribute->m_pNextSibling);
    return pAttribute;
  }

  Attribute* Element::FindAttribute(const std::string& sName)
  {
    Attribute* pAttribute = m_pFirstAttribute;
    for (; pAttribute && pAttribute->GetName() != sName; pAttribute = pAttribute->m_pNextSibling);
    return pAttribute;
  }

  const Attribute& Element::GetAttribute(const std::string& sName) const
  {
    const Attribute* pAttribute = FindAttribute(sName);
    STAFF_ASSERT(pAttribute, "Attribute with name [" + sName + "] does not exists");
    return *pAttribute;
  }

  Attribute& Element::GetAttribute(const std::string& sName)
  {
    Attribute* pAttribute = FindAttribute(sName);
    STAFF_ASSERT(pAttribute, "Attribute with name [" + sName + "] does not exists");
    return *pAttribute;
  }

  const std::string& Element::GetAttributeValue(const std::string& sName) const
  {
    return GetAttribute(sName).GetValue();
  }

  void Element::SetAttributeValue(const std::string& sName, const std::string& sValue)
  {
    GetAttribute(sName).SetValue(sValue);
  }

  void Element::RemoveAttribute(Attribute* pAttribute)
  {
    STAFF_ASSERT(m_pFirstAttribute, "There is no attributes in element");

    if (m_pFirstAttribute == pAttribute)
    {
      m_pFirstAttribute = m_pFirstAttribute->m_pNextSibling;
    }
    else
    {
      // searching for attribute to remove
      Attribute* pPrevAttribute = m_pFirstAttribute;
      for (; pPrevAttribute->m_pNextSibling && pPrevAttribute->m_pNextSibling != pAttribute;
           pPrevAttribute = pPrevAttribute->m_pNextSibling);

      STAFF_ASSERT(pPrevAttribute->m_pNextSibling,
                   "Attribute with name [" + pAttribute->GetName() + "] does not exists");

      pPrevAttribute->m_pNextSibling = pAttribute->m_pNextSibling;
    }
    delete pAttribute;
  }

  void Element::RemoveAttributeByName(const std::string& sName)
  {
    STAFF_ASSERT(m_pFirstAttribute, "There is no attributes in element");

    Attribute* pAttributeToRemove = NULL;
    if (m_pFirstAttribute->GetName() == sName)
    {
      pAttributeToRemove = m_pFirstAttribute;
      m_pFirstAttribute = m_pFirstAttribute->m_pNextSibling;
    }
    else
    {
      // searching for attribute to remove
      Attribute* pPrevAttribute = m_pFirstAttribute;
      pAttributeToRemove = pPrevAttribute->m_pNextSibling;
      for (; pAttributeToRemove && pAttributeToRemove->GetName() != sName;
           pPrevAttribute = pAttributeToRemove, pAttributeToRemove = pAttributeToRemove->m_pNextSibling);

      pPrevAttribute->m_pNextSibling = pAttributeToRemove->m_pNextSibling;
    }

    STAFF_ASSERT(pAttributeToRemove, "Attribute with name [" + sName + "] does not exists");
    delete pAttributeToRemove;
  }

  bool Element::IsAttributeExists(const std::string& sName) const
  {
    return !!FindAttribute(sName);
  }


  const Attribute* Element::GetFirstAttribute() const
  {
    return m_pFirstAttribute;
  }

  Attribute* Element::GetFirstAttribute()
  {
    return m_pFirstAttribute;
  }



  // namespaces management

  Namespace& Element::DeclareDefaultNamespace(const std::string& sUri)
  {
    return DeclareNamespace(sUri, "");
  }

  Namespace& Element::DeclareNamespace(const std::string& sUri, const std::string& sPrefix)
  {
    Namespace* pPrevNamespace = NULL;
    Namespace* pNamespace = m_pFirstNamespace;
    for (; pNamespace && sPrefix != pNamespace->GetPrefix();
         pPrevNamespace = pNamespace, pNamespace = pNamespace->m_pNextSibling);
    if (pNamespace)
    {
      pNamespace->SetUri(sUri);
    }
    else
    {
      pNamespace = new Namespace(sPrefix, sUri);
      if (pPrevNamespace)
      {
        pPrevNamespace->m_pNextSibling = pNamespace;
      }
      else
      {
        m_pFirstNamespace = pNamespace;
      }
    }

    return *pNamespace;
  }

  Namespace& Element::DeclareNamespace(const Namespace& rNamespace)
  {
    return DeclareNamespace(rNamespace.m_sUri, rNamespace.m_sPrefix);
  }

  Namespace& Element::SetNamespace(const std::string& sUri, const std::string& sPrefix,
                                   bool bRecursive /*= true*/)
  {
    ReplacePrefix(sPrefix, bRecursive);
    return DeclareNamespace(sUri, sPrefix);
  }

  Namespace& Element::SetNamespace(const Namespace& rNamespace,
                                   bool bRecursive /*= true*/)
  {
    ReplacePrefix(rNamespace.m_sPrefix, bRecursive);
    return DeclareNamespace(rNamespace.m_sUri, rNamespace.m_sPrefix);
  }

  const Namespace* Element::GetNamespace() const
  {
    return FindNamespaceByPrefix(m_sPrefix);
  }

  Namespace* Element::GetNamespace()
  {
    return FindNamespaceByPrefix(m_sPrefix);
  }

  const std::string& Element::GetNamespacePrefixByUri(const std::string& sUri)
  {
    const Namespace* pNamespace = FindNamespaceByUri(sUri);
    STAFF_ASSERT(pNamespace, "Namespace with URI [" + sUri + "] is not declared in current element");
    return pNamespace->GetPrefix();
  }

  const std::string& Element::GetNamespaceUriByPrefix(const std::string& sPrefix)
  {
    const Namespace* pNamespace = FindNamespaceByPrefix(sPrefix);
    STAFF_ASSERT(pNamespace, "Namespace with prefix [" + sPrefix + "] is not declared in current element");
    return pNamespace->GetUri();
  }

  const Namespace* Element::FindNamespaceByUri(const std::string& sUri) const
  {
    const Namespace* pNamespace = m_pFirstNamespace;
    for (; pNamespace && sUri != pNamespace->GetUri(); pNamespace = pNamespace->m_pNextSibling);
    return pNamespace;
  }

  Namespace* Element::FindNamespaceByUri(const std::string& sUri)
  {
    Namespace* pNamespace = m_pFirstNamespace;
    for (; pNamespace && sUri != pNamespace->GetUri(); pNamespace = pNamespace->m_pNextSibling);
    return pNamespace;
  }

  const Namespace* Element::FindNamespaceByPrefix(const std::string& sPrefix) const
  {
    const Namespace* pNamespace = m_pFirstNamespace;
    for (; pNamespace && sPrefix != pNamespace->GetPrefix(); pNamespace = pNamespace->m_pNextSibling);
    return pNamespace;
  }

  Namespace* Element::FindNamespaceByPrefix(const std::string& sPrefix)
  {
    Namespace* pNamespace = m_pFirstNamespace;
    for (; pNamespace && sPrefix != pNamespace->GetPrefix(); pNamespace = pNamespace->m_pNextSibling);
    return pNamespace;
  }

  const Namespace* Element::GetFirstNamespace() const
  {
    return m_pFirstNamespace;
  }

  Namespace* Element::GetFirstNamespace()
  {
    return m_pFirstNamespace;
  }

  void Element::FindElementNamespaceDeclarationByUri(const std::string& sUri,
                                                     const Namespace** ppFoundNamespace,
                                                     const Element** ppFoundElement) const
  {
    const Element* pElement = this;
    const Namespace* pNamespace = NULL;

    for (; pElement; pElement = pElement->GetParent())
    {
      pNamespace = FindNamespaceByUri(sUri);
      if (pNamespace)
      {
        break;
      }
    }

    if (ppFoundNamespace)
    {
      *ppFoundNamespace = pNamespace;
    }
    if (ppFoundElement)
    {
      *ppFoundElement = pElement;
    }
  }

  void Element::FindElementNamespaceDeclarationByUri(const std::string& sUri,
                                                     Namespace** ppFoundNamespace,
                                                     Element** ppFoundElement)
  {
    Element* pElement = this;
    Namespace* pNamespace = NULL;

    for (; pElement; pElement = pElement->GetParent())
    {
      pNamespace = FindNamespaceByUri(sUri);
      if (pNamespace)
      {
        break;
      }
    }

    if (ppFoundNamespace)
    {
      *ppFoundNamespace = pNamespace;
    }
    if (ppFoundElement)
    {
      *ppFoundElement = pElement;
    }
  }


  void Element::FindElementNamespaceDeclarationByPrefix(const std::string& sPrefix,
                                                        const Namespace** ppFoundNamespace,
                                                        const Element** ppFoundElement) const
  {
    const Element* pElement = this;
    const Namespace* pNamespace = NULL;

    for (; pElement; pElement = pElement->GetParent())
    {
      pNamespace = pElement->FindNamespaceByPrefix(sPrefix);
      if (pNamespace)
      {
        break;
      }
    }

    if (ppFoundNamespace)
    {
      *ppFoundNamespace = pNamespace;
    }
    if (ppFoundElement)
    {
      *ppFoundElement = pElement;
    }
  }

  void Element::FindElementNamespaceDeclarationByPrefix(const std::string& sPrefix,
                                                        Namespace** ppFoundNamespace,
                                                        Element** ppFoundElement)
  {
    Element* pElement = this;
    Namespace* pNamespace = NULL;

    for (; pElement; pElement = pElement->GetParent())
    {
      pNamespace = FindNamespaceByPrefix(sPrefix);
      if (pNamespace)
      {
        break;
      }
    }

    if (ppFoundNamespace)
    {
      *ppFoundNamespace = pNamespace;
    }
    if (ppFoundElement)
    {
      *ppFoundElement = pElement;
    }
  }


  Namespace* Element::FindNamespaceDeclarationByUri(const std::string& sUri)
  {
    Namespace* pNamespace = NULL;
    FindElementNamespaceDeclarationByUri(sUri, &pNamespace, NULL);
    return pNamespace;
  }

  const Namespace* Element::FindNamespaceDeclarationByUri(const std::string& sUri) const
  {
    const Namespace* pNamespace = NULL;
    FindElementNamespaceDeclarationByUri(sUri, &pNamespace, NULL);
    return pNamespace;
  }

  Namespace* Element::FindNamespaceDeclarationByPrefix(const std::string& sPrefix)
  {
    Namespace* pNamespace = NULL;
    FindElementNamespaceDeclarationByPrefix(sPrefix, &pNamespace, NULL);
    return pNamespace;
  }

  const Namespace* Element::FindNamespaceDeclarationByPrefix(const std::string& sPrefix) const
  {
    const Namespace* pNamespace = NULL;
    FindElementNamespaceDeclarationByPrefix(sPrefix, &pNamespace, NULL);
    return pNamespace;
  }


  Element* Element::FindElementByNamespaceDeclarationUri(const std::string& sUri)
  {
    Element* pElement = NULL;
    FindElementNamespaceDeclarationByUri(sUri, NULL, &pElement);
    return pElement;
  }

  const Element* Element::FindElementByNamespaceDeclarationUri(const std::string& sUri) const
  {
    const Element* pElement = NULL;
    FindElementNamespaceDeclarationByUri(sUri, NULL, &pElement);
    return pElement;
  }

  Element* Element::FindElementByNamespaceDeclarationPrefix(const std::string& sPrefix)
  {
    Element* pElement = NULL;
    FindElementNamespaceDeclarationByPrefix(sPrefix, NULL, &pElement);
    return pElement;
  }

  const Element* Element::FindElementByNamespaceDeclarationPrefix(const std::string& sPrefix) const
  {
    const Element* pElement = NULL;
    FindElementNamespaceDeclarationByPrefix(sPrefix, NULL, &pElement);
    return pElement;
  }


  void Element::ReplacePrefix(const std::string& sNewPrefix, bool bRecursive)
  {
    if (bRecursive)
    {
      for (Element* pElement = GetFirstChildElement(); pElement;
           pElement = pElement->GetNextSiblingElement())
      {
        if (pElement->m_sPrefix == m_sPrefix)
        {
          pElement->ReplacePrefix(sNewPrefix, bRecursive);
        }
      }
    }

    m_sPrefix = sNewPrefix;
  }

} // namespace xml
} // namespace staff

