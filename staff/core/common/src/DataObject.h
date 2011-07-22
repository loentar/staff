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
  // wsdl compat
  typedef char byte;
  typedef unsigned char unsignedByte;

  class QName;
  class Namespace;
  class Attribute;
  class Value;

  //!  DataObject
  class STAFF_COMMON_EXPORT DataObject
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
    DataObject(axiom_node_t* pAxiomNode = NULL);

    //!        constructor with ownership transfer
    /*! \sa Clone */
    DataObject(const DataObject& rDataObject);

    //!         initializing constructor
    /*! \param  szLocalName - local name for creating node (C-string)
    */
    explicit DataObject(const char* szLocalName);

    //!         initializing constructor
    /*! \param  sLocalName - local name for creating node
    */
    explicit DataObject(const std::string& sLocalName);

    // optimized constructors
    DataObject(const char* szLocalName, bool bValue);
    DataObject(const char* szLocalName, byte btValue);
    DataObject(const char* szLocalName, int nValue);
    DataObject(const char* szLocalName, short shValue);
    DataObject(const char* szLocalName, long lValue);
    DataObject(const char* szLocalName, long long llValue);
    DataObject(const char* szLocalName, unsignedByte ubtValue);
    DataObject(const char* szLocalName, unsigned int unValue);
    DataObject(const char* szLocalName, unsigned short ushValue);
    DataObject(const char* szLocalName, unsigned long ulValue);
    DataObject(const char* szLocalName, unsigned long long ullValue);
    DataObject(const char* szLocalName, float fValue);
    DataObject(const char* szLocalName, double dValue);

    DataObject(const std::string& sLocalName, bool bValue);
    DataObject(const std::string& sLocalName, byte btValue);
    DataObject(const std::string& sLocalName, int nValue);
    DataObject(const std::string& sLocalName, short shValue);
    DataObject(const std::string& sLocalName, long lValue);
    DataObject(const std::string& sLocalName, long long llValue);
    DataObject(const std::string& sLocalName, unsignedByte ubtValue);
    DataObject(const std::string& sLocalName, unsigned int unValue);
    DataObject(const std::string& sLocalName, unsigned short ushValue);
    DataObject(const std::string& sLocalName, unsigned long ulValue);
    DataObject(const std::string& sLocalName, unsigned long long ullValue);
    DataObject(const std::string& sLocalName, float fValue);
    DataObject(const std::string& sLocalName, double dValue);

    //!         initializing constructor
    /*! \param  szLocalName - local name for creating node(C-string)
        \param  szText - node value(C-string)
    */
    explicit DataObject(const char* szLocalName, const char* szText);

    //!         initializing constructor
    /*! \param  sLocalName - local name for creating node
        \param  szText - node value(C-string)
    */
    explicit DataObject(const std::string& sLocalName, const char* szText);

    //!         initializing constructor
    /*! \param  szLocalName - local name for creating node(C-string)
        \param  sText - node value
    */
    explicit DataObject(const char* szLocalName, const std::string& sText);

    //!         initializing constructor
    /*! \param  sLocalName - local name for creating node
        \param  sText - node value(string)
    */
    explicit DataObject(const std::string& sLocalName, const std::string& sText);

    //!         initializing constructor
    /*! \param  szLocalName - local name for creating node(C-string)
        \param  rValue - node value
    */
    DataObject(const char* szLocalName, const Value& rValue);

    //!         initializing constructor
    /*! \param  sLocalName - local name for creating node
        \param  rValue - node value
    */
    DataObject(const std::string& sLocalName, const Value& rValue);

    //!         initializing constructor
    /*! \param  rQName - node qname
    */
    DataObject(const QName& rQName);

    //!        destructor
    ~DataObject();

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
    bool IsOwner() const;

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

    //!         tests if the DataObject is NULL or not
    /*! \return true if the DataObject is NULL
    */
    bool IsNull() const;

    //!         tests if the DataObject is initialized or not
    /*! \return true if the DataObject is initialized
    */
    bool IsInit() const;

    //////////////////////////////////////////////////////////////////////////
    // Node properties

    //!         get QName
    /*! \return QName
    */
    QName GetQName();

    
    //!         set QName
    /*! \param  rQName - QName
        */
    void SetQName(const QName& rQName) const;

    //!         get local name
    /*! \return local name
    */
    std::string GetLocalName() const;

    //!         get local name
    /*! \param  sLocalName - resulting local name
    */
    void GetLocalName(std::string& sLocalName) const;

    //!         set local name
    /*! \param  szLocalName - local name(C-string)
        */
    void SetLocalName(const char* szLocalName) const;

    //!         set local name
    /*! \param  sLocalName - local name
        */
    void SetLocalName(const std::string& sLocalName) const;

    //!         get prefix
    /*! \param  sPrefix - prefix
    */
    void GetPrefix(std::string& sPrefix) const;

    //!         get prefix
    /*! \return prefix
    */
    std::string GetPrefix() const;
    
    //!         set prefix
    /*! \param  szPrefix - prefix(C-string)
        */
    void SetPrefix(const char* szPrefix);

    //!         set prefix
    /*! \param  sPrefix - prefix
        */
    void SetPrefix(const std::string& sPrefix);

    //!         get default namespace URI
    /*! \param sNamespaceUri - resulting namespace URI
    */
    void GetNamespaceUri(std::string& sNamespaceUri) const;

    //!         get default namespace URI
    /*! \return namespace URI
    */
    std::string GetNamespaceUri() const;

    //!         set namespace URI
    /*! \param  szUri - namespace URI(C-string)
        */
    void SetNamespaceUri(const char* szUri) const;

    //!         set namespace URI
    /*! \param  sUri - namespace URI
        */
    void SetNamespaceUri(const std::string& sUri) const;

    //////////////////////////////////////////////////////////////////////////
    // node management

    //!         create unnamed node and set ownership flag
    void Create();

    //!         create named node and set ownership flag
    /*! \param  szLocalName - node name(C-string)
        */
    void Create(const char* szLocalName);

    //!         create named node and set ownership flag
    /*! \param  sLocalName - node name
        */
    void Create(const std::string& sLocalName);

    //!         create node with QName and set ownership flag
    /*! \param  rQName - QName
        */
    void Create(const QName& rQName);

    // optimized create functions
    void Create(const char* szLocalName, bool bValue);
    void Create(const char* szLocalName, byte btValue);
    void Create(const char* szLocalName, int nValue);
    void Create(const char* szLocalName, short shValue);
    void Create(const char* szLocalName, long lValue);
    void Create(const char* szLocalName, long long llValue);
    void Create(const char* szLocalName, unsignedByte ubtValue);
    void Create(const char* szLocalName, unsigned int unValue);
    void Create(const char* szLocalName, unsigned short ushValue);
    void Create(const char* szLocalName, unsigned long ulValue);
    void Create(const char* szLocalName, unsigned long long ullValue);
    void Create(const char* szLocalName, float fValue);
    void Create(const char* szLocalName, double dValue);

    void Create(const std::string& sLocalName, bool bValue);
    void Create(const std::string& sLocalName, byte btValue);
    void Create(const std::string& sLocalName, int nValue);
    void Create(const std::string& sLocalName, short shValue);
    void Create(const std::string& sLocalName, long lValue);
    void Create(const std::string& sLocalName, long long llValue);
    void Create(const std::string& sLocalName, unsignedByte ubtValue);
    void Create(const std::string& sLocalName, unsigned int unValue);
    void Create(const std::string& sLocalName, unsigned short ushValue);
    void Create(const std::string& sLocalName, unsigned long ulValue);
    void Create(const std::string& sLocalName, unsigned long long ullValue);
    void Create(const std::string& sLocalName, float fValue);
    void Create(const std::string& sLocalName, double dValue);

    //!         create named node, set ownership flag, set node value(string)
    /*! \param  sLocalName - node name
        \param  szText - node value(C-string)
    */
    void Create(const char* szLocalName, const char* szText);

    //!         create named node, set ownership flag, set node value(string)
    /*! \param  sLocalName - node name
        \param  szText - node value(C-string)
    */
    void Create(const std::string& sLocalName, const char* szText);

    //!         create named node, set ownership flag, set node value(string)
    /*! \param  sLocalName - node name
        \param  sText - node value(string)
    */
    void Create(const char* szLocalName, const std::string& sText);

    //!         create named node, set ownership flag, set node value(string)
    /*! \param  sLocalName - node name
        \param  sText - node value(string)
    */
    void Create(const std::string& sLocalName, const std::string& sText);

    //!         create named node, set ownership flag, set node value
    /*! \param  sLocalName - node name
        \param  rValue - node value
    */
    void Create(const std::string& sLocalName, const Value& rValue);

    //!         free node tree
    /*!         forced node tree freeing, even if owner flag is not set */
    void Free();

    //!         detach current node from existing node tree and set owner to true
    /*! \return reference to current node
    */
    DataObject& DetachNode();

    //!         replace current node with given    
    DataObject& ReplaceNode(DataObject& rNewNode);

    //!         create deep copy of the axiom tree
    /*! sets ownership flag for newly created tree
        \param  rDataObject - source DataObject
        \return reference to current node
        */
    DataObject& Clone(const DataObject& rDataObject);

    //!         create deep copy of the axiom tree
    /*! sets ownership flag for newly created tree
        \return cloned DataObject
        */
    DataObject Clone() const;

    //!         get parent element
    /*! \return parent element
      */
    DataObject Parent();

    //!         get next sibling element
    /*! \return next sibling element
      */
    DataObject NextSibling();

    //!         get previous sibling element
    /*! \return previous sibling element
      */
    DataObject PreviousSibling();

    //!         move to the next sibling element
    /*! to check for DataObject for validaty please use IsInit() */
    void SetNextSibling();

    //!         move to the previous sibling element
    /*! to check for DataObject for validaty please use IsInit() */
    void SetPreviousSibling();


    //////////////////////////////////////////////////////////////////////////
    // child nodes management

    //!         get first child element
    /*! \return first child element
      */
    DataObject FirstChild();

    //!         get first child elemen (const)t
    /*! \return first child element
      */
    const DataObject FirstChild() const;

    //!         get last child element
    /*! \return last child element
      */
    DataObject LastChild();

    //!         get last child element (const)
    /*! \return last child element
      */
    const DataObject LastChild() const;

    //!         create unnamed child node
    /*! \return created child
        */
    DataObject CreateChild();

    //!         create child node with QName
    /*! \param  rQName - QName
        \return created child
        */
    DataObject CreateChild(const QName& rQName);

    //!         create child node with local name
    /*! \param  szLocalName - child's local name(C-string)
        \return created child
        */
    DataObject CreateChild(const char* szLocalName);

    //!         create child node with local name
    /*! \param  sLocalName - child's local name
        \return created child
        */
    DataObject CreateChild(const std::string& sLocalName);

    // optimized create child functions
    DataObject CreateChild(const char* szLocalName, bool bValue);
    DataObject CreateChild(const char* szLocalName, byte btValue);
    DataObject CreateChild(const char* szLocalName, int nValue);
    DataObject CreateChild(const char* szLocalName, short shValue);
    DataObject CreateChild(const char* szLocalName, long lValue);
    DataObject CreateChild(const char* szLocalName, long long llValue);
    DataObject CreateChild(const char* szLocalName, unsignedByte ubtValue);
    DataObject CreateChild(const char* szLocalName, unsigned int unValue);
    DataObject CreateChild(const char* szLocalName, unsigned short ushValue);
    DataObject CreateChild(const char* szLocalName, unsigned long ulValue);
    DataObject CreateChild(const char* szLocalName, unsigned long long ullValue);
    DataObject CreateChild(const char* szLocalName, float fValue);
    DataObject CreateChild(const char* szLocalName, double dValue);

    DataObject CreateChild(const std::string& sLocalName, bool bValue);
    DataObject CreateChild(const std::string& sLocalName, byte btValue);
    DataObject CreateChild(const std::string& sLocalName, int nValue);
    DataObject CreateChild(const std::string& sLocalName, short shValue);
    DataObject CreateChild(const std::string& sLocalName, long lValue);
    DataObject CreateChild(const std::string& sLocalName, long long llValue);
    DataObject CreateChild(const std::string& sLocalName, unsignedByte ubtValue);
    DataObject CreateChild(const std::string& sLocalName, unsigned int unValue);
    DataObject CreateChild(const std::string& sLocalName, unsigned short ushValue);
    DataObject CreateChild(const std::string& sLocalName, unsigned long ulValue);
    DataObject CreateChild(const std::string& sLocalName, unsigned long long ullValue);
    DataObject CreateChild(const std::string& sLocalName, float fValue);
    DataObject CreateChild(const std::string& sLocalName, double dValue);

    //!         create child node with local name and value
    /*! \param  szLocalName - child's local name(C-string)
        \param  szText - child's text value(C-string)
        \return created child
        */
    DataObject CreateChild(const char* szLocalName, const char* szText);

    //!         create child node with local name and value
    /*! \param  sLocalName - child's local name
        \param  szText - child's text value(C-string)
        \return created child
        */
    DataObject CreateChild(const std::string& sLocalName, const char* szText);

    //!         create child node with local name and value
    /*! \param  szLocalName - child's local name(C-string)
        \param  sText - child's text value(string)
        \return created child
        */
    DataObject CreateChild(const char* szLocalName, const std::string& sText);

    //!         create child node with local name and value
    /*! \param  sLocalName - child's local name
        \param  sText - child's text value(string)
        \return created child
        */
    DataObject CreateChild(const std::string& sLocalName, const std::string& sText);

    //!         create child node with local name and value
    /*! \param  szLocalName - child's local name(C-string)
        \param  sValue - child's value
        \return created child
        */
    DataObject CreateChild(const char* szLocalName, const Value& rValue);

    //!         create child node with local name and value
    /*! \param  sLocalName - child's local name
        \param  sValue - child's value
        \return created child
        */
    DataObject CreateChild(const std::string& sLocalName, const Value& rValue);

    //!         create child with given local name if no child nodes with given name exists
    /*! \param  sLocalName - child's local name
        \return created or already existing child
        */
    DataObject CreateChildOnce(const char* szLocalName);

    //!         create child with given local name if no child nodes with given name exists
    /*! \param  sLocalName - child's local name
        \return created or already existing child
        */
    DataObject CreateChildOnce(const std::string& sLocalName);

    //!         add given tree as child node
    /*! resets rDataObject's ownership flag
        \param  rDataObject - tree node to add
        \return appended child
        */
    DataObject AppendChild(DataObject& rDataObject);
    
    //!         add given tree as child node
    /*! resets rDataObject's ownership flag
        \param  rDataObject - tree node to add
        \return appended child
        */
    DataObject AppendChild(const DataObject& rDataObject);
    
    //!         detach child tree
    /*! \param  itChild - child iterator
        \return detached child
        */
    DataObject DetachChild(Iterator& itChild);

    //!         detach and free child tree
    /*! \param  itChild - child iterator
        */
    void RemoveChild(Iterator& itChild);

    //!         detach and free child tree
    /*! \param  szLocalName - child name(C-string)
        */
    void RemoveChildByLocalName(const char* szLocalName);

    //!         detach and free child tree
    /*! \param  sLocalName - child name
        */
    void RemoveChildByLocalName(const std::string& sLocalName);

    //!         remove all child nodes
    void RemoveAllChildren();

    //!         find child node by QName
    /*! \param  rQName - QName
        \return iterator to found child node or End()
        */
    Iterator FindChildByQName(const QName& rQName);

    //!         find child node by QName
    /*! \param  rQName - QName
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByQName(const QName& rQName) const;

    //!         find child node by QName
    /*! \param  rQName - QName
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    Iterator FindChildByQName(const QName& rQName, const Iterator& itStart);

    //!         find child node by QName
    /*! \param  rQName - QName
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByQName(const QName& rQName, const ConstIterator& itStart) const;

    //!         find child node by local name
    /*! \param  szLocalName - local name(C-string)
        \param  ppNode - resulting pointer to node
        \param  ppElement - resulting pointer to element
        \return true if node is found
        */
    bool FindChildByLocalName(const char* szLocalName,
                              axiom_node_t** ppNode, axiom_element_t** ppElement = NULL) const;

    //!         find child node by local name
    /*! \param  szLocalName - local name(C-string)
        \param  pNodeStart - node to search from
        \param  ppNode - resulting pointer to node
        \param  ppElement - optional resulting pointer to element
        \return true if node is found
        */
    bool FindChildByLocalName(const char* szLocalName, axiom_node_t* pNodeStart,
                              axiom_node_t** ppNode, axiom_element_t** ppElement = NULL) const;

    //!         find child node by local name
    /*! \param  szLocalName - local name(C-string)
        \return iterator to found child node or End()
        */
    Iterator FindChildByLocalName(const char* szLocalName);

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \return iterator to found child node or End()
        */
    Iterator FindChildByLocalName(const std::string& sLocalName);

    //!         find child node by local name
    /*! \param  szLocalName - local name(C-string)
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByLocalName(const char* szLocalName) const;

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByLocalName(const std::string& sLocalName) const;

    //!         find child node by local name
    /*! \param  szLocalName - local name(C-string)
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    Iterator FindChildByLocalName(const char* szLocalName, const Iterator& itStart);

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    Iterator FindChildByLocalName(const std::string& sLocalName, const Iterator& itStart);

    //!         find child node by local name
    /*! \param  szLocalName - local name(C-string)
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByLocalName(const char* szLocalName, const ConstIterator& itStart) const;

    //!         find child node by local name
    /*! \param  sLocalName - local name
        \param  itStart - child's iterator to start with
        \return iterator to found child node or End()
        */
    ConstIterator FindChildByLocalName(const std::string& sLocalName, const ConstIterator& itStart) const;

    //!         get child node by QName
    /*! \param  rQName - QName
        \return child node
        */
    DataObject GetChildByQName(const QName& rQName);

    //!         get child node by QName
    /*! \param  rQName - QName
        \return child node
        */
    const DataObject GetChildByQName(const QName& rQName) const;

    //!         get child node by local name
    /*! \param  szLocalName - local name(C-string)
        \return child node
        */
    DataObject GetChildByLocalName(const char* szLocalName);

    //!         get child node by local name
    /*! \param  sLocalName - local name
        \return child node
        */
    DataObject GetChildByLocalName(const std::string& sLocalName);

    //!         get child node by local name
    /*! \param  szLocalName - local name(C-string)
        \return child node
        */
    const DataObject GetChildByLocalName(const char* szLocalName) const;

    //!         get child node by local name
    /*! \param  sLocalName - local name
        \return child node
        */
    const DataObject GetChildByLocalName(const std::string& sLocalName) const;

    //!         get child node by local name
    /*! does not throws an exception if child does not exists
        to check is returned dataobject is valid please use !DataObject.IsNull()
        \param  szLocalName - local name(C-string)
        \return child node
        */
    DataObject GetChildByLocalNameOpt(const char* szLocalName);

    //!         get child node by local name
    /*! does not throws an exception if child does not exists
        to check is returned dataobject is valid please use !DataObject.IsNull()
        \param  sLocalName - local name
        \return child node
        */
    DataObject GetChildByLocalNameOpt(const std::string& sLocalName);

    //!         get child node by local name
    /*! does not throws an exception if child does not exists
        to check is returned dataobject is valid please use !DataObject.IsNull()
        \param  szLocalName - local name(C-string)
        \return child node
        */
    const DataObject GetChildByLocalNameOpt(const char* szLocalName) const;

    //!         get child node by local name
    /*! does not throws an exception if child does not exists
        to check is returned dataobject is valid please use !DataObject.IsNull()
        \param  sLocalName - local name
        \return child node
        */
    const DataObject GetChildByLocalNameOpt(const std::string& sLocalName) const;

    //////////////////////////////////////////////////////////////////////////
    // data manipulation

    // optimized set value functions
    void SetValue(bool bValue);
    void SetValue(byte btValue);
    void SetValue(int nValue);
    void SetValue(short shValue);
    void SetValue(long lValue);
    void SetValue(long long llValue);
    void SetValue(unsignedByte ubtValue);
    void SetValue(unsigned int unValue);
    void SetValue(unsigned short ushValue);
    void SetValue(unsigned long ulValue);
    void SetValue(unsigned long long ullValue);
    void SetValue(float fValue);
    void SetValue(double dValue);
    void SetValue(const char* szText);
    void SetValue(const std::string& sText);


    // optimized get value functions
    // returns true, if conversion to type was successed
    bool GetValue(bool& rbValue) const;
    bool GetValue(byte& rbtValue) const;
    bool GetValue(int& rnValue) const;
    bool GetValue(short& rshValue) const;
    bool GetValue(long& rlValue) const;
    bool GetValue(long long& rllValue) const;
    bool GetValue(unsignedByte& rubtValue) const;
    bool GetValue(unsigned int& runValue) const;
    bool GetValue(unsigned short& rushValue) const;
    bool GetValue(unsigned long& rulValue) const;
    bool GetValue(unsigned long long& rullValue) const;
    bool GetValue(float& rfValue) const;
    bool GetValue(double& rdValue) const;
    void GetValue(std::string& sValue) const;

    //!         get node value(copy)
    /*! \return node value(copy)
        */
    Value GetValue() const;

    //!         set node value
    /*! \param  rValue - node value
    */
    void SetValue(const Value& rValue);

    //!         get node value(string)
    /*! \return node value(string)
    */
    std::string GetText() const;
    
    //!         get node value(string)
    /*! alias for GetValue(std::string)
        \sa GetValue
        \param sText - resulting node value(string)
    */
    void GetText(std::string& sText) const;

    //!         set node value(string)
    /*! alias for SetValue(char*)
        \sa SetValue
        \param  sText - node value(string)
        */
    void SetText(const char* szText);

    //!         set node value(string)
    /*! alias for SetValue(std::string)
        \sa SetValue
        \param  sText - node value(string)
        */
    void SetText(const std::string& sText);

    //////////////////////////////////////////////////////////////////////////
    // namespace management

    //!         get the namespace
    /*! \return namespace
        */
    Namespace GetDefaultNamespace() const;

    //!         declare default namespace for element
    /*! \param  szUri - default namespace(C-string)
        */
    void DeclareDefaultNamespace(const char* szUri);

    //!         declare default namespace for element
    /*! \param  sUri - default namespace
        */
    void DeclareDefaultNamespace(const std::string& sUri);

    //!         get the namespace
    /*! \return namespace
        */
    Namespace GetNamespace() const;

    //!         declare new namespace in element scope
    /*! \param  rNamespace - namespace
        */
    void DeclareNamespace(Namespace& rNamespace);

    //!         set namespace for element
    /*! \param  rNamespace - namespace
        */
    void SetNamespace(Namespace& rNamespace);

    //!         find a namespace in the scope of the document.
    /*!         Start to find from the given node and go up the hierarchy.
        \param  sUri - namespace URI
        \return found namespace.
        */
    Namespace FindNamespace(const std::string& sUri);

    //!         find a namespace in the scope of the document.
    /*! \param  sUri - namespace URI
        \param  sPrefix - prefix
        \return found namespace.
        */
    Namespace FindNamespace(const std::string& sUri, const std::string& sPrefix);

    
    //!         finds a namespace in current element's scope, by uri
    /*! \param  sUri - namespace URI
        \return found namespace.
        */
    Namespace FindDeclaredNamespace(const std::string& sUri);

    //!         finds a namespace in current element's scope, by uri and prefix
    /*! \param  sUri - namespace URI
        \param  sPrefix - prefix
        \return found namespace.
        */
    Namespace FindDeclaredNamespace(const std::string& sUri, const std::string& sPrefix);

    //!         Find namespace URI in current element's scope by prefix
    /*! \param  sPrefix - prefix
        \return found namespace
        */
    Namespace FindNamespaceUri(const std::string& sPrefix) const;

    //////////////////////////////////////////////////////////////////////////
    // attribute management

    //!         create and add attribute
    /*! \param  szAttrName - attribute name (C-string)
        \param  szAttrText - attribute text (C-string)
        */
    void CreateAttribute(const char* szAttrName, const char* szAttrText);

    //!         create and add attribute
    /*! \param  sAttrName - attribute name
        \param  szAttrText - attribute text (C-string)
        */
    void CreateAttribute(const std::string& sAttrName, const char* szAttrText);

    //!         create and add attribute
    /*! \param  szAttrName - attribute name (C-string)
        \param  sAttrText - attribute text
        */
    void CreateAttribute(const char* szAttrName, const std::string& sAttrText);

    //!         create and add attribute
    /*! \param  sAttrName - attribute name
        \param  sAttrText - attribute text
        */
    void CreateAttribute(const std::string& sAttrName, const std::string& sAttrText);

    //!         create and add attribute
    /*! \param  szAttrName - attribute name (C-string)
        \param  rValue - attribute value
        */
    void CreateAttribute(const char* szAttrName, const Value& rValue);

    //!         create and add attribute
    /*! \param  sAttrName - attribute name
        \param  rValue - attribute value
        */
    void CreateAttribute(const std::string& sAttrName, const Value& rValue);

    //!         add attribute
    /*! resets rAttribute's owner flag
        \param  rAttribute - attribute
        */
    void AppendAttribute(Attribute& rAttribute);

    
    //!         remove attribute
    /*! \param  itAttribute - attribute's iterator
        */
    void RemoveAttribute(AttributeIterator& itAttribute);

    //!         remove all attributes
    void RemoveAllAttributes();

    //!         find attribute by QName
    /*! \param  rQName - QName
        \return iterator to found attribute or AttributeEnd()
        */
    AttributeIterator FindAttributeByQName(const QName& rQName);

    //!         find attribute by QName
    /*! \param  rQName - QName
        \param  itStart - iterator to attribute to start with
        \return iterator to found attribute or AttributeEnd()
        */
    AttributeIterator FindAttributeByQName(const QName& rQName, const AttributeIterator& itStart);

    //!         find attribute by local name
    /*! \param  sLocalName - attribute's local name
        \return iterator to found attribute or AttributeEnd()
        */
    AttributeIterator FindAttributeByLocalName(const std::string& sLocalName);

    //!         find attribute by QName
    /*! \param  rQName - QName
        \return iterator to found attribute or AttributeEnd()
        */
    ConstAttributeIterator FindAttributeByQName(const QName& rQName) const;

    //!         find attribute by QName
    /*! \param  rQName - QName
        \param  itStart - iterator to attribute to start with
        \return iterator to found attribute or AttributeEnd()
        */
    ConstAttributeIterator FindAttributeByQName(const QName& rQName, const ConstAttributeIterator& itStart) const;

    //!         find attribute by local name
    /*! \param  sLocalName - attribute's local name
        \return iterator to found attribute or AttributeEnd()
        */
    ConstAttributeIterator FindAttributeByLocalName(const std::string& sLocalName) const;

    //!         get attribute by QName
    /*! \param  rQName - attribute's QName
        \return attribute
        */
    Attribute GetAttributeByQName(const QName& rQName);

    //!         get attribute by QName
    /*! \param  rQName - attribute's QName
        \return attribute
        */
    const Attribute GetAttributeByQName(const QName& rQName) const;

    //!         get attribute value by name
    /*! \param  szLocalName - attribute's local name(C-string)
        \return attribute's value
        */
    Value GetAttributeValueByName(const char* szLocalName) const;

    //!         get attribute value by name
    /*! \param  sLocalName - attribute's local name
        \return attribute's value
        */
    Value GetAttributeValueByName(const std::string& sLocalName) const;

    //!         get attribute text by name
    /*! \param  sLocalName - attribute's local name
        \return attribute's text
        */
    std::string GetAttributeTextByName(const std::string& sLocalName) const;

    //!         get attribute text by name
    /*! \param  sLocalName - attribute's local name
        \param  sText - resulting attribute's text
        */
    void GetAttributeTextByName(const std::string& sLocalName, std::string& sText) const;

    //!         get attribute text by name
    /*! \param  szLocalName - attribute's local name(C-string)
        \param  sText - resulting attribute's text
        */
    void GetAttributeTextByName(const char* szLocalName, std::string& sText) const;


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

    //!         get iterator to the first node's attribute
    /*! \return iterator to the first node's attribute
    */
    ConstAttributeIterator AttributeBegin() const;

    //!         get iterator to the next by last node's attribute
    /*! \return iterator to the next by last node's attribute
    */
    ConstAttributeIterator AttributeEnd() const;

    //!         get iterator to the first node's namespace
    /*! \return iterator to the first node's namespace
    */
    NamespaceIterator NamespaceBegin();

    //!         get iterator to the next by last node's namespace
    /*! \return iterator to the next by last node's namespace
    */
    NamespaceIterator NamespaceEnd();

    //!         get iterator to the first node's namespace
    /*! \return iterator to the first node's namespace
    */
    ConstNamespaceIterator NamespaceBegin() const;

    //!         get iterator to the next by last node's namespace
    /*! \return iterator to the next by last node's namespace
    */
    ConstNamespaceIterator NamespaceEnd() const;

    //////////////////////////////////////////////////////////////////////////
    // support operators

    //!         initializing operator
    /*! ownership flag sets to false
        \param  pAxiomNode - axiom node
        \return current DataObject
        */
    DataObject& operator=(axiom_node_t* pAxiomNode);

    //!         transfer ownership operator
    /*! for tree copying use Clone
        \param  rDataObject - source DataObject
        \return current DataObject
        \sa Clone
        */
    DataObject& operator=(const DataObject& rDataObject);
    
    //!         tests two DataObjects to equality
    /*! \param  rDataObject - other DataObject
        \return true - if both DataObjects points to the same AxiOM node
        */
    bool operator==(const DataObject& rDataObject) const;
    
    //!         tests two DataObjects to inequality
    /*! \param  rDataObject - other DataObject
        \return true - if DataObjects points to the different AxiOM nodes
        */
    bool operator!=(const DataObject& rDataObject) const;

    //!         returns DataObject representation in XML
    /*! \return DataObject representation in XML
    */
    std::string ToString() const;

    //!         get DataObject representation in XML
    /*! \param  sResult - resulting DataObject representation in XML
    */
    void ToString(std::string& sResult) const;

    //!         create DataObject from an XML
    /*! \param  szXml - XML-string (C-string)
        \return reference to current DataObject
        */
    DataObject& FromString(const char* szXml);

    //!         create DataObject from an XML
    /*! \param  szXml - XML-string (C-string)
        \param  nSize - XML-string size
        \return reference to current DataObject
        */
    DataObject& FromString(const char* szXml, int nSize);

    //!         create DataObject from an XML
    /*! \param  sXml - XML-string
        \return reference to current DataObject
        */
    DataObject& FromString(const std::string& sXml);

    //!         member access operator
    /*! \return this
    */
    DataObject* operator->();

    //!         member access operator
    /*! \return this
    */
    const DataObject* operator->() const;

  private:
    //!         internal clone function
    /*! \param  pNodeIn - source node
        \param  pNodeOutParent - parent node
        \return copied result
        */
    axiom_node_t* Clone(axiom_node_t* pNodeIn, axiom_node_t* pNodeOutParent);

    //! internal constructor for childs
    DataObject(axiom_node_t* pAxiomNode, axiom_element_t* pAxiomElement);

  private:
    axiom_node_t* m_pAxiomNode;         //!<  DataObject's AxiOM node
    axiom_element_t* m_pAxiomElement;   //!<  DataObject's AxiOM node element
    bool m_bOwner;                      //!<  ownership flag
    static axutil_env_t* m_pEnv;        //!<  Axis2/C environment

    friend class Iterator;
    friend class ConstIterator;
    friend class NamespaceIterator;
    friend class ConstNamespaceIterator;
    friend class AttributeIterator;
    friend class ConstAttributeIterator;
  };

  //! DataObject child iterator
  class DataObject::Iterator
  {
  public:
    //!         constructor
    Iterator();
    
    //!         initializing constructor
    /*! \param  pDataObject - iterator's DataObject
        \param  pAxiomNode - current AxiOM node
        */
    Iterator(DataObject* pDataObject, axiom_node_t* pAxiomNode);
    
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
    DataObject operator*();

    //!         object access operator
    /*! \return DataObject
    */
    const DataObject operator*() const;

    //!         object members access operator
    /*! \return DataObject
    */
    DataObject operator->();

    //!         object members access operator
    /*! \return DataObject
    */
    const DataObject operator->() const;

  private:
    DataObject* m_pDataObject; //!<  bound DataObject
    axiom_node_t* m_pAxiomNode; //!<  current child node
    friend class DataObject;
  };

  //! DataObject child const iterator
  class DataObject::ConstIterator
  {
  public:
    //!         constructor
    ConstIterator();
    
    //!         initializing constructor
    /*! \param  pDataObject - iterator's DataObject
        \param  pAxiomNode - current AxiOM node
        */
    ConstIterator(const DataObject* pDataObject, axiom_node_t* pAxiomNode);
    
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
    const DataObject operator*() const;

    //!         object members access operator
    /*! \return DataObject
    */
    const DataObject operator->() const;

  private:
    const DataObject* m_pDataObject; //!<  bound DataObject
    axiom_node_t* m_pAxiomNode;       //!<  current child node
    friend class DataObject;
  };

  //! Namespace iterator
  class DataObject::NamespaceIterator
  {
  public:
    //!         constructor
    NamespaceIterator();

    //!         initializing constructor
    /*! \param  pDataObject - DataObject
        \param  pNamespaceIndex - AxiOM namespace index
        */
    NamespaceIterator(DataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex);

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
    Namespace operator*();

    //!         namespace members access operator
    /*! \return namespace
    */
    Namespace operator->();

  private:
    DataObject* m_pDataObject;             //!<  bound DataObject
    axutil_hash_index_t* m_pNamespaceIndex; //!<  namespace index
    friend class DataObject;
  };

  //! Namespace const iterator
  class DataObject::ConstNamespaceIterator
  {
  public:
    //!         constructor
    ConstNamespaceIterator();

    //!         initializing constructor
    /*! \param  pDataObject - dataobject
        \param  pNamespaceIndex - AxiOM namespace index
        */
    ConstNamespaceIterator(const DataObject* pDataObject, axutil_hash_index_t* pNamespaceIndex);

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
    const Namespace operator*() const;

    //!         namespace members access operator
    /*! \return namespace
    */
    const Namespace operator->() const;

  private:
    const DataObject* m_pDataObject;       //!<  bound DataObject
    axutil_hash_index_t* m_pNamespaceIndex; //!<  namespace index
    friend class DataObject;
  };


  //! attributes iterator
  class DataObject::AttributeIterator
  {
  public:
    //!         constructor
    AttributeIterator();

    //!         initializing constructor
    /*! \param  pDataObject - DataObject
        \param  pAttributeIndex - AxiOM attribute index
        */
    AttributeIterator(DataObject* pDataObject, axutil_hash_index_t* pAttributeIndex);

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
    Attribute operator*();

    //!         attribute members access operator
    /*! \return attribute
    */
    Attribute operator->();

  private:
    DataObject* m_pDataObject;             //!<  bound DataObject
    axutil_hash_index_t* m_pAttributeIndex; //!<  attribute index
    friend class DataObject;
  };

  //! attributes const iterator
  class DataObject::ConstAttributeIterator
  {
  public:
    //!         constructor
    ConstAttributeIterator();

    //!         initializing constructor
    /*! \param  pDataObject - DataObject
        \param  pAttributeIndex - AxiOM attribute index
        */
    ConstAttributeIterator(const DataObject* pDataObject, axutil_hash_index_t* pAttributeIndex);

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
    const Attribute operator*() const;

    //!         attribute members access operator
    /*! \return attribute
    */
    const Attribute operator->() const;

  private:
    const DataObject* m_pDataObject;       //!<  bound DataObject
    axutil_hash_index_t* m_pAttributeIndex; //!<  attribute index
    friend class DataObject;
  };

  //!         put value to DataObject's value
  /*! \param  rDataObject - DataObject
      \param  rValue - value
      \return DataObject
      */
  STAFF_DEPRECATED(DataObject.SetValue) STAFF_COMMON_EXPORT DataObject& operator<<(DataObject& rDataObject, const Value& rValue);

  //!         get value from DataObject
  /*! \param  rDataObject - DataObject
      \param  rValue - value
      \return DataObject
      */
  STAFF_DEPRECATED(DataObject.GetValue) STAFF_COMMON_EXPORT const DataObject& operator>>(const DataObject& rDataObject, Value& rValue);


#ifndef STAFF_NO_DEPRECATED
//  typedef DataObject CDataObject STAFF_DEPRECATED(DataObject);
// many implementation can forward CDataObject declaration like
// namespace staff { class CDataObject; }
// so we create inherited class to support deprecated api

  class STAFF_DEPRECATED(DataObject) STAFF_COMMON_EXPORT CDataObject: public DataObject
  {
  public:
    CDataObject(axiom_node_t* pAxiomNode = NULL);
    CDataObject(const DataObject& rDataObject);
    CDataObject(const std::string& sLocalName);
    CDataObject(const std::string& sLocalName, const Value& rValue);
    CDataObject(const QName& rQName);
  };
#endif
}

#endif // _DATAOBJECT_H_
