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

#ifndef _STAFF_XML_NODE_H_
#define _STAFF_XML_NODE_H_

#include <string>
#include "Value.h"
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  class Element;

  //! XML Node
  class STAFF_XML_EXPORT Node
  {
  public:
    enum Type //! node type
    {
      TypeUnknown,  //!< invalid/unknown type
      TypeElement,  //!< element
      TypeComment,  //!< comment
      TypeText,     //!< text
      TypeCdata     //!< CDATA section
    };

  public:
    //! destructor
    virtual ~Node();

    //! get node type
    /*! \return node type
    */
    Type GetType() const;

    //! get node value
    /*! \return node value
    */
    virtual const Value& GetValue() const;

    //! get node value
    /*! \return node value
    */
    virtual const std::string& GetTextValue() const;

    //! set node value
    /*! \param rValue - new node value
    */
    virtual void SetValue(const Value& rValue);

    //! get parent element
    /*! \return pointer to parent node or NULL
        */
    const Element* GetParent() const;

    //! get parent element
    /*! \return pointer to parent node or NULL
        */
    Element* GetParent();


    //! clone node
    /*! \return cloned node
      */
    Node* Clone() const;


    //! get element of this node (only if node type = Element)
    /*! \return element
      */
    const Element& GetElement() const;

    //! get element of this node (only if node type = Element)
    /*! \return element
      */
    Element& GetElement();


    //! get next sibling node
    /*! \return next sibling node or NULL if current node is last
      */
    const Node* GetNextSibling() const;

    //! get next sibling node
    /*! \return next sibling node or NULL if current node is last
      */
    Node* GetNextSibling();

    //! get previous sibling node
    /*! \return previous sibling node or NULL if current node is first
      */
    const Node* GetPreviousSibling() const;

    //! get previous sibling node
    /*! \return previous sibling node or NULL if current node is first
      */
    Node* GetPreviousSibling();


    //! get next sibling element
    /*! \return next sibling element or NULL if current element is last
      */
    const Element* GetNextSiblingElement() const;

    //! get next sibling element
    /*! \return next sibling element or NULL if current element is last
      */
    Element* GetNextSiblingElement();

    //! get previous sibling element
    /*! \return previous sibling element or NULL if current element is first
      */
    const Element* GetPreviousSiblingElement() const;

    //! get previous sibling element
    /*! \return previous sibling element or NULL if current element is first
      */
    Element* GetPreviousSiblingElement();


    //! detach node from tree
    Node* Detach();

  protected:
    Node(Type eType, Element* pParent);
    Node(Type eType, const Value& rValue, Element* pParent);

  private:
    Node(const Node& rNode);
    Node& operator=(const Node& rNode);

  private:
    Type m_eType;             //!< node type
    Value m_tValue;           //!< node value
    Element* m_pParent;       //!< parent element
    Node* m_pNextSibling;     //!< next sibling
    Node* m_pPreviousSibling; //!< previous sibling
    friend class Element;
  };


  //! Comment node
  class STAFF_XML_EXPORT Comment: public Node
  {
  public:
    //!  constructor
    /*! \param pParent - parent element
      */
    Comment(Element* pParent = NULL);

    //!  constructor
    /*! \param sValue - node value
        \param pParent - parent element
      */
    Comment(const std::string& sValue, Element* pParent = NULL);
  };


  //! Text node
  class STAFF_XML_EXPORT Text: public Node
  {
  public:
    //!  constructor
    /*! \param pParent - parent element
      */
    Text(Element* pParent = NULL);

    //!  constructor
    /*! \param sValue - node value
        \param pParent - parent element
      */
    Text(const std::string& sValue, Element* pParent = NULL);
  };


  //! CDATA node
  class STAFF_XML_EXPORT Cdata: public Node
  {
  public:
    //!  constructor
    /*! \param pParent - parent element
      */
    Cdata(Element* pParent = NULL);

    //!  constructor
    /*! \param sValue - node value
        \param pParent - parent element
      */
    Cdata(const std::string& sValue, Element* pParent = NULL);
  };

} // namespace xml
} // namespace staff

#endif // #ifndef _STAFF_XML_NODE_H_
