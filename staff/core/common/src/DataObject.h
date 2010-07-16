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

#ifndef _DATAOBJECT_H_
#define _DATAOBJECT_H_

#include <string>
#include "staffcommonexport.h"

// AXIS2/C typedefs
typedef struct axiom_node axiom_node_t;
typedef struct axiom_element axiom_element_t;
typedef struct axiom_attribute axiom_attribute_t;
typedef struct axutil_hash_index_t axutil_hash_index_t;
typedef struct axutil_env axutil_env_t;

// namespace
namespace staff
{
  class CQName;
  class CNamespace;
  class CAttribute;
  class CValue;

  //!  DataObject
  class STAFF_COMMON_EXPORT CDataObject
  {
  public:
    //! DataObject iterator
    class STAFF_COMMON_EXPORT Iterator;
    //! DataObject const iterator
    class STAFF_COMMON_EXPORT ConstIterator;
    //! DataObject namespace iterator
    class STAFF_COMMON_EXPORT NamespaceIterator;
    //! DataObject namespace const iterator
    class STAFF_COMMON_EXPORT ConstNamespaceIterator;
    //! DataObject attribute iterator
    class STAFF_COMMON_EXPORT AttributeIterator;
    //! DataObject attribute const iterator
    class STAFF_COMMON_EXPORT ConstAttributeIterator;

  public:
    //!        default constructor
    CDataObject(axiom_node_t* pAxiomNode = NULL);

    //!        constructor with ownership transfer
    /*! \sa Clone */
    CDataObject(const CDataObject& rDataObject);

    //!         initializing constructor
    /*! \param  sLocalName - local name for creating node
    */
    CDataObject(const std::string& sLocalName);

    //!         initializing constructor
    /*! \param  sLocalName - local name for creating node
        \param  sText - node value(string)
    */
    explicit CDataObject(const std::string& sLocalName, const std::string& sText);

    //!         initializing constructor
    /*! \param  sLocalName - local name for creating node
        \param  rValue - node value
    */
    CDataObject(const std::string& sLocalName, const CValue& rValue);

    //!         initializing constructor
    /*! \param  stQName - node qname
    */
    CDataObject(const CQName& stQName);

    //!        destructor
    ~CDataObject();

    //!         attach AxiOM node to DataObject
    /*! to attach child node to node tree use AttachNode
        \sa Detach
        \param  pAxiomNode - AxiOM Node
        \param  bOwner - true - set ownership flag
        */
    void Attach(axiom_node_t* pAxiomNode, bool bOwner = false);

    //!         detach DataObject from AxiOM node
    /*! if this DataObject ownerships AxiOM node, AxiOM node will be freed
        to detach nodes from tree use DetachNode 
        \sa DetachNode */
    void Detach();

    //!         get ownership flag
    /*! \return true - if ownership flag set
        */
    bool IsOwner();

    //!         set ownership flag
    /*! \param  bOwner - ownership flag
        */
    void SetOwner(bool bOwner);

    //!         type cast operator to AxiOM node
    /*! \return AxiOM node
    */
    operator axiom_node_t*();

    //!         type cast operator to AxiOM element
    /*! \return AxiOM element
    */
    operator axiom_element_t*();

    //!         tests if the DataObject is initialized or not
    /*! \return true if the DataObject is initialized
    */
    bool IsInit() const;

    //////////////////////////////////////////////////////////////////////////
    // Node properties

    //!         get QName
    /*! \return QName
    */
    CQName GetQName();
    
    //!         set QName
    /*! \param  stQName - QName
        */
    void SetQName(const CQName& stQName) const;

    //!         get local name
    /*! \return local name
    */
    std::string GetLocalName() const;
    
    //!         set local name
    /*! \param  sLocalName - local name
        */
    void SetLocalName(const std::string& sLocalName) const;

    //!         get prefix
    /*! \return prefix
    */
    std::string GetPrefix() const;
    
    //!         set prefix
    /*! \param  sPrefix - prefix
        */
    void SetPrefix(const std::string& sPrefix) const;

    //!         get default namespace URI
    /*! \return namespace URI
    */
    std::string GetNamespaceUri() const;
    
    //!         set namespace URI
    /*! \param  sUri - set namespace URI
        */
    void SetNamespaceUri(const std::string& sUri) const;

    //////////////////////////////////////////////////////////////////////////
    // node management

    //!         create unnamed node and set ownership flag
    void Create();

    //!         create named node and set ownership flag
    /*! \param  sLocalName - node name
        */
    void Create(const std::string& sLocalName);

    //!         create node with QName and set ownership flag
    /*! \param  rstQName - QName
        */
    void Create(const CQName& rstQName);

    //!         create named node, set ownership flag, set node value(string)
    /*! \param  sLocalName - node name
        \param  sText - node value(string)
    */
    void Create(const std::string& sLocalName, const std::string& sText);

    //!         create named node, set ownership flag, set node value
    /*! \param  sLocalName - node name
        \param  rValue - node value
    */
    void Create(const std::string& sLocalName, const CValue& rValue);

    //!         free node tree
    /*!         forced node tree freeing, even if owner flag is not set */
    void Free();

    //!         detach current node from existing node tree and set owner to true
    /*! \return reference to current node
    */
    CDataObject& DetachNode();

    //!         replace current node with given    
    CDataObject& ReplaceNode(CDataObject& rNewNode);

    //!         create tree node copy from given
    /*! sets ownership flag for newly created tree
        \param  rDataObject - source DataObject
        \return reference to current node
        */
    CDataObject& Clone(const CDataObject& rDataObject);

    //!         create tree node copy from given
    /*! sets ownership flag for newly created tree
        \return cloned DataObject
        */
    CDataObject Clone() const;

    //////////////////////////////////////////////////////////////////////////
    // child nodes management
    
    //!         create unnamed child node
    /*! \return created child
        */
    CDataObject CreateChild();

    //!         create child node with QName
    /*! \param  rstQName - QName
        \return created child
        */
    CDataObject CreateChild(const CQName& rstQName);

    //!         create child node with local name
    /*! \param  sLocalName - child's local name
        \return created child
        */
    CDataObject CreateChild(const std::string& sLocalName);

    //!         create child node with local name and value
    /*! \param  sLocalName - child's local name
        \param  rValue - child's value
        \return created child
        */
    CDataObject CreateChild(const std::string& sLocalName, const CValue& rValue);

    //!         create child with given local name if no child nodes with given name exists
    /*! \param  sLocalName - child's local name
        \return created or already existing child
        */
    CDataObject CreateChildOnce(const std::string& sLocalName);

    //!         add given tree as child node
    /*! resets rDataObject's ownership flag
        \param  rDataObject - tree node to add
        \return appended child
        */
    CDataObject AppendChild(CDataObject& rDataObject);
    
    //!         add given tree as child node
    /*! resets rDataObject's ownership flag
        \param  rDataObject - tree node to add
        \return appended child
        */
    CDataObject AppendChild(const CDataObject& rDataObject);
    
    //!         detach child tree
    /*! \param  itChild - child iterator
        \return detached child
        */
    CDataObject DetachChild(Iterator& itChild);

    //!         detach and free child tree
    /*! \param  itChild - child iterator
        */
    void RemoveChild(Iterator& itChild);

    //!         detach and free child tree
    /*! \param  sName - child iterator
        */
    void RemoveChildByLocalName(const std::string& sName);

    //!         remove all child nodes
    void RemoveAllChildren();

    //!         find child node by QName
    /*! \param  stQName - QName
        \return iterator to found child node or End()
        */
    Iterator FindChildByQName(const CQName& stQName);

    //!         find child node by QName
    /*! \param  stQName - QName
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByQName(const CQName& stQName) const;

    //!         find child node by QName
    /*! \param  stQName - QName
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    Iterator FindChildByQName(const CQName& stQName, const Iterator& itStart);

    //!         find child node by QName
    /*! \param  stQName - QName
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByQName(const CQName& stQName, const ConstIterator& itStart) const;

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \return iterator to found child node or End()
        */
    Iterator FindChildByLocalName(const std::string& sLocalName);

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByLocalName(const std::string& sLocalName) const;

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    Iterator FindChildByLocalName(const std::string& sLocalName, const Iterator& itStart);

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByLocalName(const std::string& sLocalName, const ConstIterator& itStart) const;

    //!         get child node by QName
    /*! \param  stQName - QName
        \return child node
        */
    CDataObject GetChildByQName(const CQName& stQName);

    //!         get child node by QName
    /*! \param  stQName - QName
        \return child node
        */
    const CDataObject GetChildByQName(const CQName& stQName) const;

    //!         get child node by local name
    /*! \param  sLocalName - local name
        \return child node
        */
    CDataObject GetChildByLocalName(const std::string& sLocalName);

    //!         get child node by local name
    /*! \param  sLocalName - local name
        \return child node
        */
    const CDataObject GetChildByLocalName(const std::string& sLocalName) const;

    //////////////////////////////////////////////////////////////////////////
    // data manipulation

    //!         get/set node value
    /*! example: 
        int nValue = tDataObject.Value(); // reading
        tDataObject.Value() = 2.3; // writing

        \return node value
    */
    CValue Value();

    //!         get node value(copy)
    /*! \return node value(copy)
        */
    CValue GetValue() const;

    //!         set node value
    /*! \param  rValue - node value
    */
    void SetValue(const CValue& rValue);

    //!         get node value(string)
    /*! \return node value(string)
    */
    std::string GetText() const;
    
    //!         set node value(string)
    /*! \param  sText - node value(string)
        */
    void SetText(const std::string& sText);

    //!         get/set child's node value
    /*! example: 
        int nValue = tDataObject["Param1"]; // reading
        tDataObject["Param1"] = 2.3; // writing

        \param  sName - child's local name
        \return child's node value
    */
    CValue operator[](const std::string& sName);

    //!         get child's node value
    /*! example: 
        int nValue = tDataObject["Param1"]; // reading
    
        \param  sName - child's local name
        \return child's node value
    */
    const CValue operator[](const std::string& sName) const;

    //////////////////////////////////////////////////////////////////////////
    // namespace management

    //!         get the namespace
    /*! \return namespace
        */
    CNamespace GetDefaultNamespace() const;

    //!         declare default namespace for element
    /*! \param  sUri - default namespace
        */
    void DeclareDefaultNamespace(const std::string& sUri);

    //!         get the namespace
    /*! \return namespace
        */
    CNamespace GetNamespace() const;

    //!         declare new namespace in element scope
    /*! \param  rNamespace - namespace
        */
    void DeclareNamespace(CNamespace& rNamespace);

    //!         set namespace for element
    /*! \param  rNamespace - namespace
        */
    void SetNamespace(CNamespace& rNamespace);

    //!         find a namespace in the scope of the document.
    /*!         Start to find from the given node and go up the hierarchy.
        \param  sUri - namespace URI
        \return found namespace.
        */
    CNamespace FindNamespace(const std::string& sUri);

    //!         find a namespace in the scope of the document.
    /*! \param  sUri - namespace URI
        \param  sPrefix - prefix
        \return found namespace.
        */
    CNamespace FindNamespace(const std::string& sUri, const std::string& sPrefix);
    
    //!         finds a namespace in current element's scope, by uri
    /*! \param  sUri - namespace URI
        \return found namespace.
        */
    CNamespace FindDeclaredNamespace(const std::string& sUri);

    //!         finds a namespace in current element's scope, by uri and prefix
    /*! \param  sUri - namespace URI
        \param  sPrefix - prefix
        \return found namespace.
        */
    CNamespace FindDeclaredNamespace(const std::string& sUri, const std::string& sPrefix);

    //!         Find namespace URI in current element's scope by prefix
    /*! \param  sPrefix - prefix
        \return found namespace
        */
    CNamespace FindNamespaceUri(const std::string& sPrefix) const;


    //////////////////////////////////////////////////////////////////////////
    // attribute management

    //!         add attribute
    /*! resets rAttribute's owner flag
        \param  rAttribute - attribute
        */
    void AppendAttribute(CAttribute& rAttribute);
    
    //!         remove attribute
    /*! \param  itAttribute - attribute's iterator
        */
    void RemoveAttribute(AttributeIterator& itAttribute);

    //!         remove all attributes
    void RemoveAllAttributes();

    //!         find attribute by QName
    /*! \param  stQName - QName
        \return iterator to found attribute or AttributeEnd()
        */
    AttributeIterator FindAttributeByQName(const CQName& stQName);

    //!         find attribute by QName
    /*! \param  stQName - QName
        \param  itStart - iterator to attribute to start with
        \return iterator to found attribute or AttributeEnd()
        */
    AttributeIterator FindAttributeByQName(const CQName& stQName, const AttributeIterator& itStart);

    //!         find attribute by local name
    /*! \param  sLocalName - attribute's local name
        \return iterator to found attribute or AttributeEnd()
        */
    AttributeIterator FindAttributeByLocalName(const std::string& sLocalName);

    //!         get attribute by QName
    /*! \param  stQName - attribute's QName
        \return attribute
        */
    CAttribute GetAttributeByQName(const CQName& stQName);

    //!         get attribute value by name
    /*! \param  sLocalName - attribute's local name
        \return attribute
        */
    CValue GetAttributeValueByName(const std::string& sLocalName) const;

    //!         get attribute text by name
    /*! \param  sLocalName - attribute's local name
        \return attribute
        */
    std::string GetAttributeTextByName(const std::string& sLocalName) const;


    //////////////////////////////////////////////////////////////////////////
    // iteration

    //!         get iterator to the first child node
    /*! \return iterator to the first child node
    */
    Iterator Begin();

    //!         get iterator to the first child node
    /*! \return iterator to the first child node
    */
    ConstIterator Begin() const;

    //!         get iterator to the next by last child node
    /*! \return iterator to the next by last child node
    */
    Iterator End();

    //!         get iterator to the next by last child node
    /*! \return iterator to the next by last child node
    */
    ConstIterator End() const;

    //!         get iterator to the first node's attribute
    /*! \return iterator to the first node's attribute
    */
    AttributeIterator AttributeBegin();

    //!         get iterator to the next by last node's attribute
    /*! \return iterator to the next by last node's attribute
    */
    AttributeIterator AttributeEnd();

    //!         get iterator to the first node's namespace
    /*! \return iterator to the first node's namespace
    */
    NamespaceIterator NamespaceBegin();

    //!         get iterator to the next by last node's namespace
    /*! \return iterator to the next by last node's namespace
    */
    NamespaceIterator NamespaceEnd();

    //////////////////////////////////////////////////////////////////////////
    // support operators

    //!         transfer ownership operator
    /*! for tree copying use Clone
        \param  rDataObject - source DataObject
        \return current DataObject
        \sa Clone
        */
    CDataObject& operator=(const CDataObject& rDataObject);
    
    //!         tests two DataObjects to equality
    /*! \param  rDataObject - other DataObject
        \return true - if both DataObjects points to the same AxiOM node
        */
    bool operator==(const CDataObject& rDataObject) const;
    
    //!         tests two DataObjects to inequality
    /*! \param  rDataObject - other DataObject
        \return true - if DataObjects points to the different AxiOM nodes
        */
    bool operator!=(const CDataObject& rDataObject) const;

    //!         returns DataObject representation in XML
    /*! \return DataObject representation in XML
    */
    std::string ToString() const;

    //!         create DataObject from an XML
    /*! \param  sXml - XML-string
        \return reference to current DataObject
        */
    CDataObject& FromString(const std::string& sXml);

    //!         member access operator
    /*! \return this
    */
    CDataObject* operator->();

    //!         member access operator
    /*! \return this
    */
    const CDataObject* operator->() const;

#ifndef STAFF_NO_DEPRECATED
    //////////////////////////////////////////////////////////////////////////
    // deprecated functions

    //!         deprecated function
    /*! \sa CreateChildOnce
        */
    STAFF_DEPRECATED(CreateChildOnce) CDataObject GetOrAdd(const std::string& sName);

    //!         deprecated function
    /*! \sa FindChildByLocalName
        */
    STAFF_DEPRECATED(FindChildByLocalName) Iterator Find(const std::string& sLocalName);

    //!         deprecated function
    /*! \sa FindChildByLocalName
        */
    STAFF_DEPRECATED(FindChildByLocalName) ConstIterator Find(const std::string& sLocalName) const;

    //!         deprecated function
    /*! \sa GetChildByLocalName
        */
    STAFF_DEPRECATED(GetChildByLocalName) const CDataObject operator()(const std::string& sLocalName) const;

    //!         deprecated function
    /*! \sa GetChildByLocalName
        */
    STAFF_DEPRECATED(GetChildByLocalName) CDataObject operator()(const std::string& sLocalName);

    //!         deprecated function
    /*! \sa SetNamespaceUri
        */
    STAFF_DEPRECATED(SetNamespaceUri) void SetNamespace(const std::string& sNamespace);

    //!         deprecated function
    /*! \sa GetLocalName*/
    STAFF_DEPRECATED(GetLocalName) const std::string Name() const;

    //!         deprecated function
    /*! \sa SetLocalName */
    STAFF_DEPRECATED(SetLocalName)void SetName(const std::string& sName);

    //!         deprecated function
    /*! \sa CreateChild
        */
    STAFF_DEPRECATED(CreateChild) CDataObject Add(const std::string& sName);

    //!         deprecated function
    /*! \sa CreateChild
        */
    STAFF_DEPRECATED(CreateChild) CDataObject Add(const std::string& sName, const CValue& rValue);

    //!         deprecated function
    /*! \sa AppendChild
        */
    STAFF_DEPRECATED(AppendChild) CDataObject Add(CDataObject rDataObject);

    //!         deprecated function
    /*! \sa GetValue
    */
    STAFF_DEPRECATED(GetValue) const CValue Value() const;

#endif

  private:
    //!         internal clone function
    /*! \param  pNodeIn - source node
        \param  pNodeOutParent - parent node
        \return copied result
        */
    axiom_node_t* Clone(axiom_node_t* pNodeIn, axiom_node_t* pNodeOutParent);

  private:
    axiom_node_t* m_pAxiomNode;         //!<  DataObject's AxiOM node
    axiom_element_t* m_pAxiomElement;   //!<  DataObject's AxiOM node element
    static axutil_env_t* m_pEnv;        //!<  Axis2/C environment
    bool m_bOwner;                      //!<  ownership flag
    
    friend class Iterator;
    friend class ConstIterator;
    friend class NamespaceIterator;
    friend class ConstNamespaceIterator;
    friend class AttributeIterator;
    friend class ConstAttributeIterator;
  };

  //! DataObject child iterator
  class CDataObject::Iterator
  {
  public:
    //!         constructor
    Iterator();
    
    //!         initializing constructor
    /*! \param  pDataObject - iterator's DataObject
        \param  pAxiomNode - current AxiOM node
        */
    Iterator(CDataObject* pDataObject, axiom_node_t* pAxiomNode);
    
    //!         copy constructor
    /*! \param  rIter - other iterator
        */
    Iterator(const Iterator& rIter);
    
    //!         destructor
    ~Iterator();

    //!         copy operator
    /*! \param  rIter - other iterator
        \return current iterator
        */
    Iterator& operator=(const Iterator& rIter);
    
    //!         predecrement operator
    /*! \return new value
    */
    Iterator& operator--();

    //!         preincrement operator
    /*! \return new value
    */
    Iterator& operator++();

    //!         postdecrement operator
    /*! \return new value
    */
    Iterator operator--(int);

    //!         postincrement operator
    /*! \return new value
    */
    Iterator operator++(int);
    
    //!         test target iterator for equality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are the same
        */
    bool operator==(const Iterator& rIter) const;

    //!         test target iterator for inequality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are different
        */
    bool operator!=(const Iterator& rIter) const;

    //!         object access operator
    /*! \return DataObject
    */
    CDataObject operator*();

    //!         object access operator
    /*! \return DataObject
    */
    const CDataObject operator*() const;

    //!         object members access operator
    /*! \return DataObject
    */
    CDataObject operator->();

    //!         object members access operator
    /*! \return DataObject
    */
    const CDataObject operator->() const;

  private:
    CDataObject* m_pDataObject; //!<  bound DataObject
    axiom_node_t* m_pAxiomNode; //!<  current child node
    friend class CDataObject;
  };

  //! DataObject child const iterator
  class CDataObject::ConstIterator
  {
  public:
    //!         constructor
    ConstIterator();
    
    //!         initializing constructor
    /*! \param  pDataObject - iterator's DataObject
        \param  pAxiomNode - current AxiOM node
        */
    ConstIterator(const CDataObject* pDataObject, axiom_node_t* pAxiomNode);
    
    //!         copy constructor
    /*! \param  rIter - other iterator
        */
    ConstIterator(const ConstIterator& rIter);
    
    //!         destructor
    ~ConstIterator();

    //!         copy operator
    /*! \param  rIter - other iterator
        \return current iterator
        */
    ConstIterator& operator=(const ConstIterator& rIter);
    
    //!         predecrement operator
    /*! \return new value
    */
    ConstIterator& operator--();

    //!         preincrement operator
    /*! \return new value
    */
    ConstIterator& operator++();

    //!         postdecrement operator
    /*! \return new value
    */
    ConstIterator operator--(int);

    //!         postincrement operator
    /*! \return new value
    */
    ConstIterator operator++(int);

    //!         test target iterator for equality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are the same
        */
    bool operator==(const ConstIterator& rIter) const;
    
    //!         test target iterator for inequality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are different
        */
    bool operator!=(const ConstIterator& rIter) const;

    //!         object access operator
    /*! \return DataObject
    */
    const CDataObject operator*() const;

    //!         object members access operator
    /*! \return DataObject
    */
    const CDataObject operator->() const;

  private:
    const CDataObject* m_pDataObject; //!<  bound DataObject
    axiom_node_t* m_pAxiomNode;       //!<  current child node
    friend class CDataObject;
  };

  //! Namespace iterator
  class CDataObject::NamespaceIterator
  {
  public:
    //!         constructor
    NamespaceIterator();

    //!         initializing constructor
    /*! \param  pDataObject - DataObject
        \param  pNamespaceIndex - AxiOM namespace index
        */
    NamespaceIterator(CDataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex);

    //!         copy constructor
    /*! \param  rIter - iterator
    */
    NamespaceIterator(const NamespaceIterator& rIter);

    //!         destructor
    ~NamespaceIterator();

    //!         copy operator
    /*! \param  rIter - source iterator
        \return ref to current iterator
        */
    NamespaceIterator& operator=(const NamespaceIterator& rIter);

    //!         preincrement
    /*! \return ref to current iterator
        */
    NamespaceIterator& operator++();

    //!         postincrement
    /*! \return copy of old iterator
        */
    NamespaceIterator operator++(int);

    //!         test target iterator for equality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are the same
        */
    bool operator==(const NamespaceIterator& rIter) const;

    //!         test target iterator for inequality with specified iterator
    /*! \param  rIter - iterator
        \return true if the iterators are different
        */
    bool operator!=(const NamespaceIterator& rIter) const;

    //!         namespace access operator
    /*! \return namespace
    */
    CNamespace operator*();

    //!         namespace members access operator
    /*! \return namespace
    */
    CNamespace operator->();

  private:
    CDataObject* m_pDataObject;             //!<  bound DataObject
    axutil_hash_index_t* m_pNamespaceIndex; //!<  namespace index
    friend class CDataObject;
  };

  //! Namespace const iterator
  class CDataObject::ConstNamespaceIterator
  {
  public:
    //!         constructor
    ConstNamespaceIterator();

    //!         initializing constructor
    /*! \param  pDataObject - dataobject
        \param  pNamespaceIndex - AxiOM namespace index
        */
    ConstNamespaceIterator(CDataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex);

    //!         copy constructor
    /*! \param  rIter - iterator
    */
    ConstNamespaceIterator(const ConstNamespaceIterator& rIter);

    //!         destructor
    ~ConstNamespaceIterator();

    //!         copy operator
    /*! \param  rIter - source iterator
        \return ref to current iterator
        */
    ConstNamespaceIterator& operator=(const ConstNamespaceIterator& rIter);

    //!         preincrement
    /*! \return ref to current iterator
        */
    ConstNamespaceIterator& operator++();

    //!         postincrement
    /*! \return copy of old iterator
        */
    ConstNamespaceIterator operator++(int);

    //!         test target iterator for equality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are the same
        */
    bool operator==(const ConstNamespaceIterator& rIter) const;

    //!         test target iterator for inequality with specified iterator
    /*! \param  rIter - iterator
        \return true if the iterators are different
        */
    bool operator!=(const ConstNamespaceIterator& rIter) const;

    //!         namespace access operator
    /*! \return namespace
    */
    const CNamespace operator*() const;

    //!         namespace members access operator
    /*! \return namespace
    */
    const CNamespace operator->() const;

  private:
    const CDataObject* m_pDataObject;       //!<  bound DataObject
    axutil_hash_index_t* m_pNamespaceIndex; //!<  namespace index
    friend class CDataObject;
  };


  //! attributes iterator
  class CDataObject::AttributeIterator
  {
  public:
    //!         constructor
    AttributeIterator();

    //!         initializing constructor
    /*! \param  pDataObject - DataObject
        \param  pAttributeIndex - AxiOM attribute index
        */
    AttributeIterator(CDataObject* pDataObject, axutil_hash_index_t* pAttributeIndex);

    //!         copy constructor
    /*! \param  rIter - iterator
    */
    AttributeIterator(const AttributeIterator& rIter);

    //!         destructor
    ~AttributeIterator();

    //!         copy operator
    /*! \param  rIter - source iterator
        \return ref to current iterator
        */
    AttributeIterator& operator=(const AttributeIterator& rIter);

    //!         preincrement
    /*! \return ref to current iterator
        */
    AttributeIterator& operator++();

    //!         postincrement
    /*! \return copy of old iterator
    */
    AttributeIterator operator++(int);

    //!         test target iterator for equality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are the same
        */
    bool operator==(const AttributeIterator& rIter) const;

    //!         test target iterator for inequality with specified iterator
    /*! \param  rIter - iterator
        \return true if the iterators are different
        */
    bool operator!=(const AttributeIterator& rIter) const;

    //!         attribute access operator
    /*! \return attribute
    */
    CAttribute operator*();

    //!         attribute members access operator
    /*! \return attribute
    */
    CAttribute operator->();

  private:
    CDataObject* m_pDataObject;             //!<  bound DataObject
    axutil_hash_index_t* m_pAttributeIndex; //!<  attribute index
    friend class CDataObject;
  };

  //! attributes const iterator
  class CDataObject::ConstAttributeIterator
  {
  public:
    //!         constructor
    ConstAttributeIterator();

    //!         initializing constructor
    /*! \param  pDataObject - DataObject
        \param  pAttributeIndex - AxiOM attribute index
        */
    ConstAttributeIterator(const CDataObject* pDataObject, axutil_hash_index_t* pAttributeIndex);

    //!         copy constructor
    /*! \param  rIter - iterator
    */
    ConstAttributeIterator(const ConstAttributeIterator& rIter);

    //!         destructor
    ~ConstAttributeIterator();

    //!         copy operator
    /*! \param  rIter - source iterator
        \return ref to current iterator
        */
    ConstAttributeIterator& operator=(const ConstAttributeIterator& rIter);

    //!         preincrement
    /*! \return ref to current iterator
    */
    ConstAttributeIterator& operator++();

    //!         postincrement
    /*! \return copy of old iterator
    */
    ConstAttributeIterator operator++(int);

    //!         test target iterator for equality with specified iterator
    /*! \param  rIter - other iterator
        \return true if the iterators are the same
        */
    bool operator==(const ConstAttributeIterator& rIter) const;

    //!         test target iterator for inequality with specified iterator
    /*! \param  rIter - iterator
        \return true if the iterators are different
        */
    bool operator!=(const ConstAttributeIterator& rIter) const;

    //!         attribute access operator
    /*! \return attribute
    */
    const CAttribute operator*() const;

    //!         attribute members access operator
    /*! \return attribute
    */
    const CAttribute operator->() const;

  private:
    const CDataObject* m_pDataObject;       //!<  bound DataObject
    axutil_hash_index_t* m_pAttributeIndex; //!<  attribute index
    friend class CDataObject;
  };

  //!         put value to DataObject's value
  /*! \param  rDataObject - DataObject
      \param  rValue - value
      \return DataObject
      */
  STAFF_COMMON_EXPORT CDataObject& operator<<(CDataObject& rDataObject, const CValue& rValue);

  //!         get value from DataObject
  /*! \param  rDataObject - DataObject
      \param  rValue - value
      \return DataObject
      */
  STAFF_COMMON_EXPORT const CDataObject& operator>>(const CDataObject& rDataObject, CValue& rValue);
}

#endif // _DATAOBJECT_H_
