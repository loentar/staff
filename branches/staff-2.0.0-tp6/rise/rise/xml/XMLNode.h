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

#ifndef _XMLNODE_H_
#define _XMLNODE_H_

#include <string>
#include <list>
#include <utility>
#include <rise/os/osdllexport.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <rise/xml/XMLValue.h>
#include <rise/xml/XMLAttribute.h>
#include <rise/xml/XMLNamespace.h>

namespace rise
{
  namespace xml
  {
    //!        xml node
    class RISE_EXPORT CXMLNode
    {
    public:
      enum ENodeType //! node type
      { 
        ENTGENERIC,  //!< generic node
        ENTCDATA,    //!< CDATA section
        ENTCOMMENT   //!< comment
      };

      //! xmlnodes list
      typedef std::list<CXMLNode> TXMLNodeList;
      //! attributes list
      typedef std::list<SXMLAttribute> TXMLAttrList;
      //! namespaces list
      typedef std::list<SXMLNamespace> TXMLNsList;

      //! nodes iterator
      typedef TXMLNodeList::iterator TXMLNodeIterator;
      //! attributes iterator
      typedef TXMLAttrList::iterator TXMLAttrIterator;
      //! namespaces iterator
      typedef TXMLNsList::iterator TXMLNsIterator;
      
      //! nodes const iterator
      typedef TXMLNodeList::const_iterator TXMLNodeConstIterator;
      //! attributes const iterator
      typedef TXMLAttrList::const_iterator TXMLAttrConstIterator;
      //! namespaces const iterator
      typedef TXMLNsList::const_iterator TXMLNsConstIterator;

    public:
      //!         constructor
      /*! \param  pParentNode - parent node
          */
      CXMLNode(CXMLNode* pParentNode = NULL);

      //!         initializing constructor
      /*! \param  sNodeText - node name, if eNodeType = ENTGENERIC, else node content
          \param  pParentNode - parent node
          \param  eNodeType - node type
          */
      CXMLNode(const CString& sNodeText, CXMLNode* pParentNode = NULL, ENodeType eNodeType = ENTGENERIC); 

      //!         copying constructor
      /*! \param  rNode - existing node
          */
      CXMLNode(const CXMLNode& rNode);

      //!         destructor
      virtual ~CXMLNode();

      //!         copy operator
      /*! \param  rNode - source node
          \return *this
          */
      CXMLNode& operator=(const CXMLNode& rNode);

      //!         get parent node
      /*! \return pointer to parent node or NULL
          */
      CXMLNode* GetParent();

      //!         get parent node
      /*! \return pointer to parent node or NULL
          */
      const CXMLNode* GetParent() const;

      //!         set node prefix and name
      /*! \param  sNsName - prefix:name
          */
      void SetNodeNsName(const CString& sNsName);

      //!         get node prefix and name
      /*! \return sNsName - prefix:name
      */
      CString GetNodeNsName() const;

      //!        get attribute
      /*! \param  sName - attribute name
          \return attribute value
      */
      const CXMLValue& Attribute(const CString& sName) const;

      //!        get attribute
      /*! \param  sName - attribute name
          \return attribute value
      */
      CXMLValue& Attribute(const CString& sName);

      //!        is attribute exists
      /*
         \param  sName - attribute name
         \return true, if attribute exists
      */
      bool IsAttributeExists(const CString& sName) const;

      //!        get child node
      /*! \param  sName - child node name
          \return ref to child node
      */
      const CXMLNode& Subnode(const CString& sName) const;

      //!        get child node
      /*! \param  sName - child node name
          \return ref to child node
      */
      CXMLNode& Subnode(const CString& sName);

      //!        is child node exists
      /* \param  sName - child node name
         \return true, if child node exists
      */
      bool IsSubnodeExists(const CString& sName) const;

      //!         get iterator to first child
      /*! \return iterator to first child
      */
      TXMLNodeConstIterator NodeBegin() const;

      //!         get iterator to first child
      /*! \return iterator to first child
      */
      TXMLNodeIterator NodeBegin();

      //!         get iterator to next by last child
      /*! \return iterator to next by last child
      */
      TXMLNodeConstIterator NodeEnd() const;

      //!         get iterator to next by last child
      /*! \return iterator to next by last child
      */
      TXMLNodeIterator NodeEnd();

      //!         get iterator to first attribute
      /*! \return iterator to first attribute
      */
      TXMLAttrConstIterator AttrBegin() const;

      //!         get iterator to first attribute
      /*! \return iterator to first attribute
      */
      TXMLAttrIterator AttrBegin();

      //!         get iterator to next by last attribute
      /*! \return iterator to next by last attribute
      */
      TXMLAttrConstIterator AttrEnd() const;

      //!         get iterator to next by last attribute
      /*! \return iterator to next by last attribute
      */
      TXMLAttrIterator AttrEnd();

      //!         get node name
      /*! \return const reference to node name
      */
      const CString& NodeName() const;

      //!         get and set node name
      /*! \return reference to node name
      */
      CString& NodeName();

      //!         get node value
      /*! \return const reference to node value
      */
      const CXMLValue& NodeContent() const;

      //!         get and set node value
      /*! \return reference to node value
      */
      CXMLValue& NodeContent();

      //!         get child node value
      /*! \param  sName - child node name
          \return child node value
          */
      const CXMLValue& operator[](const CString& sName) const;

      //!         get and set child node value
      /*! \param  sName - child node name
          \return child node value
          */
      CXMLValue& operator[](const CString& sName);

      //!         find child node with given name, add, if no child node found
      /*! \param  sNodeName - node name
          \return reference to child node
          */
      CXMLNode& GetOrAddSubNode(const CString& sNodeName);

      //!         get node type
      /*! \return reference to node type
      */
      const ENodeType& NodeType() const;

      //!         get/set node type
      /*! \return reference to node type
      */
      ENodeType& NodeType();

      //!         get child count
      /*! \return child nodes counts
      */
      int SubNodesCount() const;

      //!         add attribute
      /*! \param  sAttrName - attribute name
          \param  sAttrValue - attribute value
          \return added attribute
          */
      SXMLAttribute& AddAttribute(const CString& sAttrName, const CXMLValue& sAttrValue);

      //!         add attribute
      /*! \param  stAttribute - attribute
          \return added attribute
          */
      SXMLAttribute& AddAttribute(const SXMLAttribute& stAttribute);

      //!         add attribute
      /*! \param  sAttrName - attribute name
          \param  sAttrValue - attribute value
          \param  itAttr - attribute iterator
          \return added attribute
          */
      TXMLAttrIterator AddAttribute(const CString& sAttrName, const CXMLValue& sAttrValue, TXMLAttrIterator itAttr);

      //!         add attribute
      /*! \param  stAttribute - attribute
          \param  itAttr - attribute iterator
          \return added attribute
          */
      TXMLAttrIterator AddAttribute(const SXMLAttribute& stAttribute, TXMLAttrIterator itAttr);

      //!        remove attribute
      /*! \param itAttr - attribute iterator
      */
      void DelAttribute(TXMLAttrIterator itAttr);



      //!         add child node
      /*! \param  sNodeText - node name, if eNodeType = ENTGENERIC, else nodecontent
          \param  eNodeType - node type
          \return reference to added child node
      */
      CXMLNode& AddSubNode(const CString& sNodeText, ENodeType eNodeType = ENTGENERIC);

      //!         add child node
      /*! \param  sNodeText - node name, if eNodeType = ENTGENERIC, else nodecontent
          \param  itNode - node iterator
          \param  eNodeType - node type
          \return reference to added child node
      */
      CXMLNode& AddSubNode(const CString& sNodeText, TXMLNodeIterator itNode, ENodeType eNodeType = ENTGENERIC);

      //!         remove child node
      /*! \param  itNode - iterator to child node
          */
      void DelSubNode(TXMLNodeIterator itNode);

      //!        remove all childs and attributes
      void Clear();

      //!        find attribute by name
      /*! \param  sName - attribute name
          \return iterator to found attribute or AttrEnd()
      */
      TXMLAttrConstIterator FindAttribute(const CString& sName) const;

      //!        find attribute by name
      /*! \param  sName - attribute name
          \return iterator to found attribute or AttrEnd()
      */
      TXMLAttrIterator FindAttribute(const CString& sName);

      //!        find attribute, using unary predicate
      /*! \param  pFindFunction - unary predicate
          \return iterator to found attribute or AttrEnd()
      */
      template<typename UNARY_PREDICATE>
      TXMLAttrConstIterator FindAttributeIf(UNARY_PREDICATE pFindFunction) const;

      //!        find attribute, using unary predicate
      /*! \param  pFindFunction - unary predicate
          \return iterator to found attribute or AttrEnd()
      */
      template<typename UNARY_PREDICATE>
      TXMLAttrIterator FindAttributeIf(UNARY_PREDICATE pFindFunction);


      //!         find child node by name
      /*! \param  sName - child node name
          \return iterator to found child node or NodeEnd() if no child node found
          */
      TXMLNodeConstIterator FindSubnode(const CString& sName) const;

      //!         find child node by name
      /*! \param  sName - child node name
          \return iterator to found child node or NodeEnd() if no child node found
          */
      TXMLNodeIterator FindSubnode(const CString& sName);

      //!         find child node by name
      /*! \param  sName - child node name
          \param  itNode - iterator
          \return iterator to found child node or NodeEnd() if no child node found
          */
      TXMLNodeConstIterator FindSubnode(const CString& sName, TXMLNodeConstIterator itNode) const;

      //!         find child node by name
      /*! \param  sName - child node name
          \param  itNode - iterator
          \return iterator to found child node or NodeEnd() if no child node found
          */
      TXMLNodeIterator FindSubnode(const CString& sName, TXMLNodeIterator itNode);

      //!         find child node with given node name and attribute
      /*! \param  sNodeName - node name
          \param  stAttribute - attribute
          \return iterator to found node
      */
      TXMLNodeConstIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute) const;

      //!         find child node with given node name and attribute
      /*! \param  sNodeName - node name
          \param  stAttribute - attribute
          \return iterator to found node
      */
      TXMLNodeIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute);

      //!         find child node with given node name and attribute
      /*! \param  sNodeName - node name
          \param  stAttribute - attribute
          \param  itNode - iterator to child node
          \return iterator to found node
      */
      TXMLNodeConstIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute,
        TXMLNodeConstIterator itNode) const;

      //!         find child node with given node name and attribute
      /*! \param  sNodeName - node name
          \param  stAttribute - attribute
          \param  itNode - iterator to child node
          \return iterator to found node
      */
      TXMLNodeIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute,
        TXMLNodeIterator itNode);

      //!         find child node by predicate
      /*! \param  pFindFunction - find node predicate
          \return iterator to found node
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeConstIterator FindNodeIf(UNARY_PREDICATE pFindFunction) const;

      //!         find child node by predicate
      /*! \param  pFindFunction - find node predicate
          \return iterator to found node
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeIterator FindNodeIf(UNARY_PREDICATE pFindFunction);

      //!         find child node by predicate
      /*! \param  pFindFunction - find node predicate
          \param  itNode - child node iterator
          \return iterator to found node
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeConstIterator FindNodeIf(UNARY_PREDICATE pFindFunction, TXMLNodeConstIterator itNode) const;

      //!         find child node by predicate
      /*! \param  pFindFunction - find node predicate
          \param  itNode - child node iterator
          \return iterator to found node
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeIterator FindNodeIf(UNARY_PREDICATE pFindFunction, TXMLNodeIterator itNode);

      //!        sort attributes by binary predicate
      /*! \param  pCompareFunction - binary predicate
      */
      template<typename BINARY_PREDICATE>
      void SortAttrs(BINARY_PREDICATE pCompareFunction);

      //!        sort nodes by binary predicate
      /*! \param  pCompareFunction - binary predicate
      */
      template<typename BINARY_PREDICATE>
      void SortNodes(BINARY_PREDICATE pCompareFunction);

      //!         get default namespace
      /*! \return namespace
      */
      const CString& Namespace() const;

      //!         get/set default namespace
      /*! \return namespace
      */
      CString& Namespace();

      //!         get namespaces list
      /*! \return namespaces list
          */
      const TXMLNsList& GetNsList() const;

      //!         get namespaces list
      /*! \return namespaces list
      */
      TXMLNsList& GetNsList();

      //!         find namespace declaration
      /*! \param  sNamespace - prefix
          \return node with namespace declaration
          */
      CXMLNode* FindNamespaceDecl(const CString& sNamespace);

      //!         find namespace declaration
      /*! \param  sNamespace - prefix
          \return node with namespace declaration
          */
      const CXMLNode* FindNamespaceDecl(const CString& sNamespace) const;

    private:
      CXMLNode*        m_pNodeParent;     //!<  parent node
      TXMLAttrList     m_tAttr;           //!<  attributes list
      TXMLNodeList     m_tSubNodes;       //!<  subnode list
      CString          m_sName;           //!<  node name
      CXMLValue        m_sContent;        //!<  node value
      ENodeType        m_eNodeType;       //!<  node type
      CString          m_sNamespace;      //!<  default namespace
      TXMLNsList       m_tNsList;         //!<  node namespaces

    }; // class CXMLNode //////////////////////////////////////////////////

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLNode - reference to node
        \return  serializing stream
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLNode& rXMLNode);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLNode - reference to node
        \return  deserializing stream
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLNode& rXMLNode);

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLNode - reference to node
        \return  serializing stream
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLNode& rXMLNode);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLNode - reference to node
        \return  deserializing stream
    */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLNode& rXMLNode);

  } // namespace xml
} // namespace rise

#include "XMLNode.hpp"

#endif // #ifndef _XMLNODE_H_
