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

#ifndef _XMLNODE_HPP_
#define _XMLNODE_HPP_

namespace rise
{
  namespace xml
  {

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       FindAttribute
    //    DESCRIPTION:    поиск артибутов по предикату
    //    PARAMETRS:      pFindFunction - унарный предикат поиска
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    template<typename UNARY_PREDICATE>
    CXMLNode::TXMLAttrConstIterator CXMLNode::FindAttributeIf(UNARY_PREDICATE pFindFunction) const
    {
      TXMLAttrConstIterator itAttr = AttrBegin();
      for ( ; itAttr != AttrEnd() && !pFindFunction(*itAttr); ++itAttr);
      return itAttr;
    }

    template<typename UNARY_PREDICATE>
    CXMLNode::TXMLAttrIterator CXMLNode::FindAttributeIf(UNARY_PREDICATE pFindFunction)
    {
      TXMLAttrIterator itAttr = AttrBegin();
      for ( ; itAttr != AttrEnd() && !pFindFunction(*itAttr); ++itAttr);
      return itAttr;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       FindNode
    //    DESCRIPTION:    поиск подузлов по предикату
    //    PARAMETRS:      pFindFunction - унарный предикат поиска
    //                    itNode - итератор на узел, начиная с которого будет производиться поиск
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    template<typename UNARY_PREDICATE>
    CXMLNode::TXMLNodeConstIterator CXMLNode::FindNodeIf(UNARY_PREDICATE pFindFunction) const
    {
      return FindNodeIf(pFindFunction, NodeBegin());
    }

    template<typename UNARY_PREDICATE>
    CXMLNode::TXMLNodeIterator CXMLNode::FindNodeIf(UNARY_PREDICATE pFindFunction)
    {
      return FindNodeIf(pFindFunction, NodeBegin());
    }

    template<typename UNARY_PREDICATE>
    CXMLNode::TXMLNodeConstIterator CXMLNode::FindNodeIf(UNARY_PREDICATE pFindFunction, TXMLNodeConstIterator itNode) const
    {
      for ( ; itNode != NodeEnd() && !pFindFunction(*itNode); ++itNode);
      return itNode;
    }

    template<typename UNARY_PREDICATE>
    CXMLNode::TXMLNodeIterator CXMLNode::FindNodeIf(UNARY_PREDICATE pFindFunction, TXMLNodeIterator itNode)
    {
      for ( ; itNode != NodeEnd() && !pFindFunction(*itNode); ++itNode);
      return itNode;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       SortAttrs
    //    DESCRIPTION:    сортировка атрибутов по заданному условию
    //    PARAMETRS:      pCompareFunction - бинарный предикат сортировки
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    template<typename BINARY_PREDICATE>
    void CXMLNode::SortAttrs(BINARY_PREDICATE pCompareFunction)
    {
      m_tAttr.sort(pCompareFunction);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       SortNodes
    //    DESCRIPTION:    сортировка подузлов по заданному условию
    //    PARAMETRS:      pCompareFunction - бинарный предикат сортировки
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    template<typename BINARY_PREDICATE>
    void CXMLNode::SortNodes(BINARY_PREDICATE pCompareFunction)
    {
      m_tSubNodes.sort(pCompareFunction);
    }

  } // namespace xml
} // namespace rise

#endif // _XMLNODE_HPP_
