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

#include "Exception.h"
#include "Element.h"
#include "Node.h"

namespace staff
{
namespace xml
{

  Node::Node(Type eType, Element* pParent):
    m_eType(eType),
    m_pParent(pParent),
    m_pNextSibling(NULL),
    m_pPreviousSibling(NULL)
  {
    if (m_pParent)
    {
      m_pParent->AppendChild(this);
    }
  }

  Node::Node(Type eType, const Value& rValue, Element* pParent):
    m_eType(eType),
    m_tValue(rValue),
    m_pParent(pParent),
    m_pNextSibling(NULL),
    m_pPreviousSibling(NULL)
  {
    if (m_pParent)
    {
      m_pParent->AppendChild(this);
    }
  }

  Node::~Node()
  {
    // detach only if parent set
    if (m_pParent)
    {
      Detach();
    }
  }


  Node::Type Node::GetType() const
  {
    return m_eType;
  }

  const Value& Node::GetValue() const
  {
    return m_tValue;
  }

  const std::string& Node::GetTextValue() const
  {
    return m_tValue.AsString();
  }

  void Node::SetValue(const Value& rValue)
  {
    m_tValue = rValue;
  }

  const Element* Node::GetParent() const
  {
    return m_pParent;
  }

  Element* Node::GetParent()
  {
    return m_pParent;
  }

  Node* Node::Clone() const
  {
    Node* pClone = NULL;
    switch (m_eType)
    {
    case TypeComment:
      pClone = new Comment(m_tValue);
      break;
    case TypeText:
      pClone = new Text(m_tValue);
      break;
    case TypeCdata:
      pClone = new Cdata(m_tValue);
      break;
    case TypeElement:
      pClone = reinterpret_cast<const Element*>(this)->CloneElement();
      break;
    default:
      STAFF_THROW_ASSERT("Can't create node clone");
    }

    return pClone;
  }

  const Element& Node::GetElement() const
  {
    STAFF_ASSERT(m_eType == TypeElement, "This node is not an Element");
    return static_cast<const Element&>(*this);
  }

  Element& Node::GetElement()
  {
    STAFF_ASSERT(m_eType == TypeElement, "This node is not an Element");
    return static_cast<Element&>(*this);
  }


  const Node* Node::GetNextSibling() const
  {
    return m_pNextSibling;
  }

  Node* Node::GetNextSibling()
  {
    return m_pNextSibling;
  }

  const Node* Node::GetPreviousSibling() const
  {
    return m_pPreviousSibling;
  }

  Node* Node::GetPreviousSibling()
  {
    return m_pPreviousSibling;
  }

  const Element* Node::GetNextSiblingElement() const
  {
    const Node* pNode = m_pNextSibling;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->m_pNextSibling);
    return reinterpret_cast<const Element*>(pNode);
  }

  Element* Node::GetNextSiblingElement()
  {
    Node* pNode = m_pNextSibling;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->m_pNextSibling);
    return reinterpret_cast<Element*>(pNode);
  }

  const Element* Node::GetPreviousSiblingElement() const
  {
    const Node* pNode = m_pPreviousSibling;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->m_pPreviousSibling);
    return reinterpret_cast<const Element*>(pNode);
  }

  Element* Node::GetPreviousSiblingElement()
  {
    Node* pNode = m_pPreviousSibling;
    for (; pNode && pNode->GetType() != TypeElement; pNode = pNode->m_pPreviousSibling);
    return reinterpret_cast<Element*>(pNode);
  }

  Node* Node::Detach()
  {
    if (m_pParent)
    {
      if (m_pParent->m_pFirstChild == this)
      {
        m_pParent->m_pFirstChild = m_pNextSibling;
      }
      if (m_pParent->m_pLastChild == this)
      {
        m_pParent->m_pLastChild = m_pPreviousSibling;
      }
      m_pParent = NULL;
    }
    if (m_pNextSibling)
    {
      m_pNextSibling->m_pPreviousSibling = m_pPreviousSibling;
      m_pNextSibling = NULL;
    }
    if (m_pPreviousSibling)
    {
      m_pPreviousSibling->m_pNextSibling = m_pNextSibling;
      m_pPreviousSibling = NULL;
    }

    return this;
  }


  Comment::Comment(Element* pParent):
    Node(TypeComment, pParent)
  {
  }

  Comment::Comment(const std::string& sValue, Element* pParent):
    Node(TypeComment, sValue, pParent)
  {
  }


  Text::Text(Element* pParent):
    Node(TypeText, pParent)
  {
  }

  Text::Text(const std::string& sValue, Element* pParent):
    Node(TypeText, sValue, pParent)
  {
  }


  Cdata::Cdata(Element* pParent):
    Node(TypeCdata, pParent)
  {
  }

  Cdata::Cdata(const std::string& sValue, Element* pParent):
    Node(TypeCdata, sValue, pParent)
  {
  }

} // namespace xml
} // namespace staff
