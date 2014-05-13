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

#ifndef _STAFF_XML_ELEMENT_H_
#define _STAFF_XML_ELEMENT_H_

#include <string>
#include <list>
#include "Node.h"
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  class STAFF_XML_EXPORT Attribute;
  class STAFF_XML_EXPORT Namespace;

  //! XML Element
  class STAFF_XML_EXPORT Element: public Node
  {
  public:
    //! initializing constructor
    /*! \param  pParent - parent node
        */
    Element(Element* pParent = NULL);

    //! initializing constructor
    /*! \param  sName - element name
        \param  pParent - parent node
        */
    Element(const std::string& sName, Element* pParent = NULL);

    //! initializing constructor
    /*! \param  sName - element name
        \param  rValue - element's text node value
        \param  pParent - parent node
        */
    Element(const std::string& sName, const Value& rValue, Element* pParent = NULL);

    //! cloning constructor
    /*! \param  rElement - source element
      */
    Element(const Element& rElement);

    //! cloning operator
    /*! \param  rElement - source element
        \return *this
      */
    Element& operator=(const Element& rElement);

    //! destructor
    virtual ~Element();


    //! clone xml tree
    /*! \param  bRecursive - true - clone whole xml tree, false - clone this node only
        \return cloned xml tree
      */
    Element* CloneElement(bool bRecursive = true) const;

    //! clone xml tree to this element from given element
    /*! \param  rElement - source element
        \param  bRecursive - true - clone whole xml tree, false - clone this node only
        \return *this
      */
    Element& CloneElement(const Element& rElement, bool bRecursive = true);

    //! get element name
    /*! \return const reference to element name
    */
    const std::string& GetName() const;

    //! set element name
    /*! \param sName - element name if form [prefix:]element_name
      */
    void SetName(const std::string& sName);

    //! get prefix
    /*! \return prefix
      */
    const std::string& GetPrefix() const;

    //! get node name with prefix
    /*! \return node name with prefix
      */
    std::string GetPrefixName() const;


    //! get concatenated text childs values
    /*! \return text
    */
    virtual std::string GetChildsText() const;

    //! get first Text child content node
    /*! simple method to access text child value
        \return first child text, or empty string if no child text exists
    */
    virtual const Value& GetValue() const;

    //! get node value
    /*! \return node value
    */
    virtual const std::string& GetTextValue() const;

    //! set node value
    /*! simple method to access text child value
        keep only one text child and set text value for it
        \param rValue - new node value
    */
    virtual void SetValue(const Value& rValue);

    //! is text null
    /*! \return true if no text childs exists
      */
    bool IsTextNull() const;

    //! remove all text childs
    void SetTextNull();

    //! is element empty
    /*! \return true if element have no child nodes
      */
    bool IsEmpty() const;

    //! is element have no child nodes or exactly one text child node
    /*! \return true if element is leaf
      */
    bool IsLeaf() const;

    //! clear element for reuse
    /*! remove all children, attributes, namespaces.
      */
    void Clear();

    // childs management

    //! create and append child element
    /*! \return created element
      */
    Element& CreateChildElement();

    //! create and append child element
    /*! \param sName - element name
        \return created element
      */
    Element& CreateChildElement(const std::string& sName);

    //! create and append child element
    /*! \param sName - element name
        \param rValue - node value
        \return created element
      */
    Element& CreateChildElement(const std::string& sName, const Value& rValue);

    //! create new child element if no child element with given name exists
    /*! \param sName - element name
        \return created or already existing element
      */
    Element& CreateChildElementOnce(const std::string& sName);

    //! create and append child Comment
    /*! \return created comment node
      */
    Comment& CreateChildComment();

    //! create and append child Comment
    /*! \param rValue - node value
        \return created comment node
      */
    Comment& CreateChildComment(const Value& rValue);

    //! create and append child Text
    /*! \return created text node
      */
    Text& CreateChildText();

    //! create and append child (Cdata, Comment or Text)
    /*! \param rValue - node value
        \return created text node
      */
    Text& CreateChildText(const Value& rValue);

    //! create and append child Cdata type
    /*! \return created CDATA node
      */
    Cdata& CreateChildCdata();

    //! create and append child Cdata type
    /*! \param rValue - node value
        \return created CDATA node
      */
    Cdata& CreateChildCdata(const Value& rValue);


    //! insert child node before element
    /*! \param pNode - node to insert
        \param pBefore - existing child node to insert before
        \return reference to appended child
      */
    Node& InsertChildBefore(Node* pNode, Node* pBefore);

    //! insert child node before element
    /*! \param pNode - node to insert
        \param pBefore - existing child node to insert before
        \return reference to appended child
      */
    Element& InsertChildBefore(Element* pNode, Node* pBefore);

    //! insert child node before element
    /*! \param pNode - node to insert
        \param pBefore - existing child node to insert before
        \return reference to appended child
      */
    Comment& InsertChildBefore(Comment* pNode, Node* pBefore);

    //! insert child node before element
    /*! \param pNode - node to insert
        \param pBefore - existing child node to insert before
        \return reference to appended child
      */
    Text& InsertChildBefore(Text* pNode, Node* pBefore);

    //! insert child node before element
    /*! \param pNode - node to insert
        \param pBefore - existing child node to insert before
        \return reference to appended child
      */
    Cdata& InsertChildBefore(Cdata* pNode, Node* pBefore);


    //! insert child node after element
    /*! \param pNode - node to insert
        \param pAfter - existing child node to insert after
        \return reference to appended child
      */
    Node& InsertChildAfter(Node* pNode, Node* pAfter);

    //! insert child node after element
    /*! \param pNode - node to insert
        \param pAfter - existing child node to insert after
        \return reference to appended child
      */
    Element& InsertChildAfter(Element* pNode, Node* pAfter);

    //! insert child node after element
    /*! \param pNode - node to insert
        \param pAfter - existing child node to insert after
        \return reference to appended child
      */
    Comment& InsertChildAfter(Comment* pNode, Node* pAfter);

    //! insert child node after element
    /*! \param pNode - node to insert
        \param pAfter - existing child node to insert after
        \return reference to appended child
      */
    Text& InsertChildAfter(Text* pNode, Node* pAfter);

    //! insert child node after element
    /*! \param pNode - node to insert
        \param pAfter - existing child node to insert after
        \return reference to appended child
      */
    Cdata& InsertChildAfter(Cdata* pNode, Node* pAfter);


    //! append child node
    /*! \param pNode - child node
        \return reference to appended child
      */
    Node& AppendChild(Node* pNode);

    //! append child element
    /*! \param pNode - child element
        \return reference to appended child
      */
    Element& AppendChild(Element* pNode);

    //! append child node
    /*! \param pNode - child node
        \return reference to appended child
      */
    Comment& AppendChild(Comment* pNode);

    //! append child node
    /*! \param pNode - child node
        \return reference to appended child
      */
    Text& AppendChild(Text* pNode);

    //! append child node
    /*! \param pNode - child node
        \return reference to appended child
      */
    Cdata& AppendChild(Cdata* pNode);


    //! remove and free child node
    /*! this function is eqivalent to "delete pNode"
        \param pNode - child node to remove
      */
    void RemoveChild(Node* pNode);


    //! get first child
    /*! \return first child or NULL if there is no childs
      */
    const Node* GetFirstChild() const;

    //! get first child
    /*! \return first child or NULL if there is no childs
      */
    Node* GetFirstChild();

    //! get last child
    /*! \return last child or NULL if there is no childs
      */
    const Node* GetLastChild() const;

    //! get last child
    /*! \return last child or NULL if there is no childs
      */
    Node* GetLastChild();


    //! get first child element
    /*! \return first child element or NULL if there is no child elements
      */
    const Element* GetFirstChildElement() const;

    //! get first child element
    /*! \return first child element or NULL if there is no child elements
      */
    Element* GetFirstChildElement();

    //! get last child element
    /*! \return last child element or NULL if there is no child elements
      */
    const Element* GetLastChildElement() const;

    //! get last child element
    /*! \return last child element or NULL if there is no child elements
      */
    Element* GetLastChildElement();


    //! find child element by name
    /*! \param  sName - child element name
        \return child element or NULL if no child found
    */
    const Element* FindChildElementByName(const std::string& sName) const;

    //! find child element by name
    /*! \param  sName - child element name
        \return child element or NULL if no child found
    */
    Element* FindChildElementByName(const std::string& sName);

    //! find child element by name
    /*! \param  sName - child element name
        \param  pBegin - begin search from
        \return child element or NULL if no child found
    */
    const Element* FindChildElementByName(const std::string& sName, const Element* pBegin) const;

    //! find child element by name
    /*! \param  sName - child element name
        \param  pBegin - begin search from
        \return child element or NULL if no child found
    */
    Element* FindChildElementByName(const std::string& sName, Element* pBegin);

    //! get child element by name
    /*! throws an exception if no child found
        \param  sName - child element name
        \return const reference to child element
    */
    const Element& GetChildElementByName(const std::string& sName) const;

    //! get child element by name
    /*! throws an exception if no child found
        \param  sName - child element name
        \return reference to child element
    */
    Element& GetChildElementByName(const std::string& sName);


    //! calculate child nodes count
    /*! \return child nodes counts
    */
    unsigned GetChildCount() const;

    //! calculate child elements count
    /*! \return child elements counts
    */
    unsigned GetChildElementCount() const;


    // attributes management

    //! create attribute with name and value
    /*! \param  sName - attribute name
        \param  sValue - attribute value
        */
    Attribute& CreateAttribute(const std::string& sName, const std::string& sValue);

    //! create attribute with name, value and prefix
    /*! \param  sName - attribute name
        \param  sValue - attribute value
        \param  sPrefix - attribute prefix
        */
    Attribute& CreateAttribute(const std::string& sName, const std::string& sValue,
                               const std::string& sPrefix);

    //! append attribute's copy
    /*! \param  rAttribute - attribute
        */
    Attribute& AppendAttribute(const Attribute& rAttribute);

    //! append attribute
    /*! \param  pAttribute - attribute
        */
    Attribute& AppendAttribute(Attribute* pAttribute);


    //! find attribute by name
    /*! \param  sName - attribute name
        \return const iterator to attribute
    */
    const Attribute* FindAttribute(const std::string& sName) const;

    //! find attribute by name
    /*! \param  sName - attribute name
        \return iterator to attribute
    */
    Attribute* FindAttribute(const std::string& sName);

    //! get attribute by name
    /*! throws an exception if no attribute found
        \param  sName - attribute name
        \return attribute value
    */
    const Attribute& GetAttribute(const std::string& sName) const;

    //! get attribute by name
    /*! throws an exception if no attribute found
        \param  sName - attribute name
        \return attribute value
    */
    Attribute& GetAttribute(const std::string& sName);

    //! get attribute value by name
    /*! throws an exception if no attribute found
        \param  sName - attribute name
        \return attribute value
    */
    const std::string& GetAttributeValue(const std::string& sName) const;

    //! get attribute value by name
    /*! throws an exception if no attribute found
        \param  sName - attribute name
        \param  sValue - attribute value
        \return attribute value
    */
    void SetAttributeValue(const std::string& sName, const std::string& sValue);


    //! remove attribute
    /*! \param pAttribute - attribute to remove
      */
    void RemoveAttribute(Attribute* pAttribute);

    //! remove attribute by name
    /*! \param sName - attribute name to remove
      */
    void RemoveAttributeByName(const std::string& sName);

    //! is attribute exists
    /*! \param  sName - attribute name
        \return true, if attribute exists
    */
    bool IsAttributeExists(const std::string& sName) const;


    //! get first attribute
    /*! \return pointer to first attribute
      */
    const Attribute* GetFirstAttribute() const;

    //! get first attribute
    /*! \return pointer to first attribute
      */
    Attribute* GetFirstAttribute();


    // namespaces management

    //! declare default namespace
    /*! \param  sUri - namespace uri
        */
    Namespace& DeclareDefaultNamespace(const std::string& sUri);

    //! declare new namespace / modify existing
    /*! \param  sUri - namespace uri
        \param  sPrefix - namespace prefix
        */
    Namespace& DeclareNamespace(const std::string& sUri, const std::string& sPrefix);

    //! declare new namespace (create clone) / modify existing
    /*! \param  rNamespace - namespace
        \return new namespace
        */
    Namespace& DeclareNamespace(const Namespace& rNamespace);

    //! declare and set new namespace
    /*! \param  sUri - namespace uri
        \param  sPrefix - namespace prefix
        \param  bRecursive - set namespace for childs too
        \return new namespace
        */
    Namespace& SetNamespace(const std::string& sUri, const std::string& sPrefix,
                            bool bRecursive = true);

    //! declare and set new namespace (copy)
    /*! \param  rNamespace - namespace
        \param  bRecursive - set namespace for childs too
        \return new namespace
        */
    Namespace& SetNamespace(const Namespace& rNamespace, bool bRecursive = true);


    //! get namespace of element
    /*! \return namespace of element
        */
    const Namespace* GetNamespace() const;

    //! get namespace of element
    /*! \return namespace of element
        */
    Namespace* GetNamespace();

    //! get namespace prefix by uri
    /*! \param sUri - uri
        \return namespace prefix
      */
    const std::string& GetNamespacePrefixByUri(const std::string& sUri);

    //! get namespace uri by prefix
    /*! \param sPrefix - prefix
        \return namespace uri
      */
    const std::string& GetNamespaceUriByPrefix(const std::string& sPrefix);

    //! find namespace by uri
    /*! \param sUri - namespace uri
        \return namespace or NULL, if no namespace found
      */
    const Namespace* FindNamespaceByUri(const std::string& sUri) const;

    //! find namespace by uri
    /*! \param sUri - namespace uri
        \return namespace or NULL, if no namespace found
      */
    Namespace* FindNamespaceByUri(const std::string& sUri);

    //! find namespace by prefix
    /*! \param sPrefix - prefix
        \return namespace or NULL, if no namespace found
      */
    const Namespace* FindNamespaceByPrefix(const std::string& sPrefix) const;

    //! find namespace by prefix
    /*! \param sPrefix - prefix
        \return namespace or NULL, if no namespace found
      */
    Namespace* FindNamespaceByPrefix(const std::string& sPrefix);

    //! get first namespace
    /*! \return pointer to first namespace
      */
    const Namespace* GetFirstNamespace() const;

    //! get first declared namespace
    /*! \return pointer to first declared namespace
      */
    Namespace* GetFirstNamespace();


    //! find element namespace declaration by uri
    /*! start to find from current element and go up the hierarchy
        \param sUri - uri
        \param ppFoundNamespace - (optional) found namespace or NULL
        \param ppFoundElement - (optional) found element or NULL
      */
    void FindElementNamespaceDeclarationByUri(const std::string& sUri,
                                              const Namespace** ppFoundNamespace,
                                              const Element** ppFoundElement) const;

    //! find element namespace declaration by uri
    /*! start to find from current element and go up the hierarchy
        \param sUri - uri
        \param ppFoundNamespace - (optional) found namespace or NULL
        \param ppFoundElement - (optional) found element or NULL
      */
    void FindElementNamespaceDeclarationByUri(const std::string& sUri,
                                              Namespace** ppFoundNamespace,
                                              Element** ppFoundElement);

    //! find element namespace declaration by prefix
    /*! start to find from current element and go up the hierarchy
        \param sPrefix - prefix
        \param ppFoundNamespace - (optional) found namespace or NULL
        \param ppFoundElement - (optional) found element or NULL
      */
    void FindElementNamespaceDeclarationByPrefix(const std::string& sPrefix,
                                                 const Namespace** ppFoundNamespace,
                                                 const Element** ppFoundElement) const;

    //! find element namespace declaration by prefix
    /*! start to find from current element and go up the hierarchy
        \param sPrefix - prefix
        \param ppFoundNamespace - (optional) found namespace or NULL
        \param ppFoundElement - (optional) found element or NULL
      */
    void FindElementNamespaceDeclarationByPrefix(const std::string& sPrefix,
                                                 Namespace** ppFoundNamespace,
                                                 Element** ppFoundElement);


    //! find namespace declaration
    /*! convinent wrapper for FindElementNamespaceDeclarationByUri
        \param  sUri - namespace uri to find
        \return found namespace or NULL
        */
    const Namespace* FindNamespaceDeclarationByUri(const std::string& sUri) const;

    //! find namespace declaration
    /*! convinent wrapper for FindElementNamespaceDeclarationByUri
        \param  sUri - namespace uri to find
        \return found namespace or NULL
        */
    Namespace* FindNamespaceDeclarationByUri(const std::string& sUri);

    //! find element where given namespace is declared
    /*! convinent wrapper for FindElementNamespaceDeclarationByPrefix
        \param  sPrefix - prefix
        \return element where this namespace is declared
        */
    const Namespace* FindNamespaceDeclarationByPrefix(const std::string& sPrefix) const;

    //! find element where given namespace is declared
    /*! convinent wrapper for FindElementNamespaceDeclarationByPrefix
        \param  sPrefix - prefix
        \return element where this namespace is declared
        */
    Namespace* FindNamespaceDeclarationByPrefix(const std::string& sPrefix);


    //! find element where given namespace is declared
    /*! convinent wrapper for FindElementNamespaceDeclarationByUri
        \param  sUri - uri
        \return element where this namespace is declared
        */
    const Element* FindElementByNamespaceDeclarationUri(const std::string& sUri) const;

    //! find element where given namespace is declared
    /*! convinent wrapper for FindElementNamespaceDeclarationByUri
        \param  sUri - uri
        \return element where this namespace is declared
        */
    Element* FindElementByNamespaceDeclarationUri(const std::string& sUri);

    //! find element where given namespace is declared
    /*! convinent wrapper for FindElementNamespaceDeclarationByPrefix
        \param  sPrefix - prefix
        \return element where this namespace is declared
        */
    const Element* FindElementByNamespaceDeclarationPrefix(const std::string& sPrefix) const;

    //! find element where given namespace is declared
    /*! convinent wrapper for FindElementNamespaceDeclarationByPrefix
        \param  sPrefix - prefix
        \return element where this namespace is declared
        */
    Element* FindElementByNamespaceDeclarationPrefix(const std::string& sPrefix);

  private:
    void ReplacePrefix(const std::string& sNewPrefix, bool bRecursive);

  private:
    std::string     m_sName;            //!< element name
    Attribute*      m_pFirstAttribute;  //!< first attribute
    Namespace*      m_pFirstNamespace;  //!< first namespace
    std::string     m_sPrefix;          //!< namespace prefix of element
    Node*           m_pFirstChild;      //!< first child
    Node*           m_pLastChild;       //!< last child
    friend class Node;
  };

} // namespace xml
} // namespace staff

#endif // #ifndef _STAFF_XML_ELEMENT_H_
