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

#include <iostream>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include "XMLNode.h"
#include "XMLException.h"

namespace rise 
{
  namespace xml
  {
    CXMLNode::CXMLNode(CXMLNode* pParentNode /*= NULL*/):
      m_pNodeParent(pParentNode), m_eNodeType(ENTGENERIC)
    {
    }


    CXMLNode::CXMLNode(const CString& sNodeText, CXMLNode* pParentNode /*= NULL*/, ENodeType eNodeType /*= ENTGENERIC*/): 
      m_pNodeParent(pParentNode), m_eNodeType(eNodeType)
    {
      if ( eNodeType == ENTGENERIC )
        SetNodeNsName(sNodeText);
      else
        m_sContent = sNodeText;
    }

    CXMLNode::CXMLNode(const CXMLNode& rNode)
    {
      *this = rNode;
    }

    CXMLNode::~CXMLNode() 
    {
    }

    CXMLNode& CXMLNode::operator=(const CXMLNode& rNode)
    {
      m_sName = rNode.m_sName;
      m_sNamespace = rNode.m_sNamespace;
      m_sContent = rNode.m_sContent;
      m_tAttr = rNode.m_tAttr;
      m_tNsList = rNode.m_tNsList;

      m_tSubNodes.clear();
      for (TXMLNodeList::const_iterator itNode = rNode.m_tSubNodes.begin();
           itNode != rNode.m_tSubNodes.end(); ++itNode)
      {
        CXMLNode& rChild = *m_tSubNodes.insert(NodeEnd(), CXMLNode());
        rChild = *itNode;
        rChild.m_pNodeParent = this;
      }

      m_eNodeType = rNode.m_eNodeType;

      return *this;
    }

    CXMLNode* CXMLNode::GetParent()
    {
      return m_pNodeParent;
    }

    const CXMLNode* CXMLNode::GetParent() const
    {
      return m_pNodeParent;
    }

    void CXMLNode::SetNodeNsName( const CString& sNsName )
    {
      TSize nPos = sNsName.find(':');
      if (nPos != CString::npos)
      {
        StrMid(sNsName, m_sName, nPos + 1);
        StrLeft(sNsName, m_sNamespace, nPos);
      }
      else
        m_sName = sNsName;
    }

    const CXMLValue& CXMLNode::Attribute( const CString& sName ) const
    {
      for ( TXMLAttrConstIterator itAttr = AttrBegin(); itAttr != AttrEnd(); ++itAttr)
        if (itAttr->sAttrName == sName)
          return itAttr->sAttrValue;

      RISE_THROWS(CLogicNoItemException, "Attribute [" + sName + "] is not found");
    }

    CXMLValue& CXMLNode::Attribute(const CString& sName)
    {
      for ( TXMLAttrIterator itAttr = AttrBegin(); itAttr != AttrEnd(); ++itAttr)
        if (itAttr->sAttrName == sName)
          return itAttr->sAttrValue;

      RISE_THROWS(CLogicNoItemException, "Attribute [" + sName + "] is not found");
    }

    CXMLNode::TXMLAttrConstIterator CXMLNode::FindAttribute(const CString& sName) const
    {
      TXMLAttrConstIterator itAttr = AttrBegin();
      for ( ; itAttr != AttrEnd() && itAttr->sAttrName != sName; ++itAttr);
      return itAttr;
    }

    CXMLNode::TXMLAttrIterator CXMLNode::FindAttribute(const CString& sName)
    {
      TXMLAttrIterator itAttr = AttrBegin();
      for ( ; itAttr != AttrEnd() && itAttr->sAttrName != sName; ++itAttr);
      return itAttr;
    }

    bool CXMLNode::IsAttributeExists(const CString& sName) const
    {
      for ( TXMLAttrConstIterator itAttr = AttrBegin(); itAttr != AttrEnd(); ++itAttr)
        if (itAttr->sAttrName == sName)
          return true;

      return false;
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::FindSubnode(const CString& sName) const
    {
      return FindSubnode(sName, NodeBegin());
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindSubnode(const CString& sName)
    {
      return FindSubnode(sName, NodeBegin());
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::FindSubnode(const CString& sName, TXMLNodeConstIterator itNode) const
    {
      for ( ; itNode != NodeEnd() && itNode->NodeName() != sName; ++itNode);

      return itNode;
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindSubnode(const CString& sName, TXMLNodeIterator itNode)
    {
      for ( ; itNode != NodeEnd() && itNode->NodeName() != sName; ++itNode);

      return itNode;
    }

    const CXMLNode& CXMLNode::Subnode(const CString& sName) const
    {
      TXMLNodeConstIterator itNode = FindSubnode(sName);
      RISE_ASSERTES(itNode != NodeEnd(), CLogicNoItemException, "Node: [" + sName + "] not found");
      return *itNode;
    }

    CXMLNode& CXMLNode::Subnode(const CString& sName)
    {
      TXMLNodeIterator itNode = FindSubnode(sName);
      RISE_ASSERTES(itNode != NodeEnd(), CLogicNoItemException, "Node: [" + sName + "] not found");
      return *itNode;
    }

    bool CXMLNode::IsSubnodeExists(const CString& sName) const
    {
      for ( TXMLNodeConstIterator itNode = NodeBegin(); itNode != NodeEnd(); ++itNode)
        if (itNode->NodeName() == sName)
          return true;

      return false;
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::NodeBegin() const
    {
      return m_tSubNodes.begin();
    }

    CXMLNode::TXMLNodeIterator CXMLNode::NodeBegin()
    {
      return m_tSubNodes.begin();
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::NodeEnd() const
    {
      return m_tSubNodes.end();
    }

    CXMLNode::TXMLNodeIterator CXMLNode::NodeEnd()
    {
      return m_tSubNodes.end();
    }

    CXMLNode::TXMLAttrConstIterator CXMLNode::AttrBegin() const
    {  
      return m_tAttr.begin();
    }

    CXMLNode::TXMLAttrIterator CXMLNode::AttrBegin()
    {  
      return m_tAttr.begin();
    }

    CXMLNode::TXMLAttrConstIterator CXMLNode::AttrEnd() const
    {
      return m_tAttr.end();
    }

    CXMLNode::TXMLAttrIterator CXMLNode::AttrEnd()
    {
      return m_tAttr.end();
    }

    const CString& CXMLNode::NodeName() const
    {
      return m_sName;
    }

    CString& CXMLNode::NodeName()
    {
      return m_sName;
    }

    const CXMLValue& CXMLNode::NodeContent() const
    {
      return m_sContent;
    }

    CXMLValue& CXMLNode::NodeContent()
    {
      return m_sContent;
    }

    const CXMLValue& CXMLNode::operator[](const CString& sName) const
    {
      return Subnode(sName).NodeContent();
    }

    CXMLValue& CXMLNode::operator[](const CString& sName)
    {
      return GetOrAddSubNode(sName).NodeContent();
    }

    CXMLNode& CXMLNode::GetOrAddSubNode(const CString& sNodeName)
    {
      TXMLNodeIterator itNode = FindSubnode(sNodeName);
      if ( itNode != NodeEnd()) 
        return *itNode;
      else
        return AddSubNode(sNodeName);
    }

    const CXMLNode::ENodeType& CXMLNode::NodeType() const
    {
      return m_eNodeType;
    }

    CXMLNode::ENodeType& CXMLNode::NodeType()
    {
      return m_eNodeType;
    }

    int CXMLNode::SubNodesCount() const
    {
      return (int)m_tSubNodes.size();
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute) const
    {
      return FindNodeMatch(sNodeName, stAttribute, FindSubnode(sNodeName));
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute)
    {
      return FindNodeMatch(sNodeName, stAttribute, FindSubnode(sNodeName));
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute,
      TXMLNodeConstIterator itNode) const
    {
      for(; itNode != NodeEnd(); itNode = FindSubnode(sNodeName, itNode))
      {
        TXMLAttrConstIterator itAttr = itNode->FindAttribute(stAttribute.sAttrName);
        if ((itAttr != itNode->AttrEnd()) && (*itAttr == stAttribute))
          return itNode;
        ++itNode;
      }

      return NodeEnd();
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute,
      TXMLNodeIterator itNode)
    {
      for(; itNode != NodeEnd(); itNode = FindSubnode(sNodeName, itNode))
      {
        TXMLAttrIterator itAttr = itNode->FindAttribute(stAttribute.sAttrName);
        if ((itAttr != itNode->AttrEnd()) && (*itAttr == stAttribute))
          return itNode;
        ++itNode;
      }

      return NodeEnd();
    }

    SXMLAttribute& CXMLNode::AddAttribute( const CString& sAttrName, const CXMLValue& sAttrValue )
    {
      return AddAttribute(SXMLAttribute(sAttrName, sAttrValue));
    }

    SXMLAttribute& CXMLNode::AddAttribute(const SXMLAttribute& stAttribute)
    {
      RISE_ASSERTE(!IsAttributeExists(stAttribute.sAttrName), CLogicAlreadyExistsException);

      return *m_tAttr.insert(AttrEnd(), stAttribute);
    }

    CXMLNode::TXMLAttrIterator CXMLNode::AddAttribute(const CString& sAttrName, const CXMLValue& sAttrValue, TXMLAttrIterator itAttr)
    {
      return AddAttribute(SXMLAttribute(sAttrName, sAttrValue), itAttr);
    }

    CXMLNode::TXMLAttrIterator CXMLNode::AddAttribute(const SXMLAttribute& stAttribute, TXMLAttrIterator itAttr)
    {
      RISE_ASSERTE(!IsAttributeExists(stAttribute.sAttrName), CLogicAlreadyExistsException);

      return m_tAttr.insert(itAttr, stAttribute);
    }

    void CXMLNode::DelAttribute(CXMLNode::TXMLAttrIterator itAttr)
    {
      RISE_ASSERTES(itAttr != AttrEnd(), CLogicNoItemException, "invalid iterator value");
      m_tAttr.erase(itAttr);
    }

    CXMLNode& CXMLNode::AddSubNode(const CString& sNodeText, CXMLNode::ENodeType eNodeType/* = ENTGENERIC*/)
    {
      CXMLNode& rChild = *m_tSubNodes.insert(NodeEnd(), CXMLNode(sNodeText, this, eNodeType));
      rChild.m_pNodeParent = this;
      return rChild;
    }

    CXMLNode& CXMLNode::AddSubNode(const CString& sNodeText, TXMLNodeIterator itNode, ENodeType eNodeType/* = ENTGENERIC*/)
    {
      CXMLNode& rChild = *m_tSubNodes.insert(itNode, CXMLNode(sNodeText, this, eNodeType));
      rChild.m_pNodeParent = this;
      return rChild;
    }

    void CXMLNode::DelSubNode(CXMLNode::TXMLNodeIterator itNode)
    {
      RISE_ASSERTES(itNode != NodeEnd(), CLogicNoItemException, "invalid iterator value");
      m_tSubNodes.erase(itNode);
    }

    void CXMLNode::Clear()
    {
      m_tSubNodes.clear();
      m_tAttr.clear();
    }

    const CXMLNode::TXMLNsList& CXMLNode::GetNsList() const
    {
      return m_tNsList;
    }

    CXMLNode::TXMLNsList& CXMLNode::GetNsList()
    {
      return m_tNsList;
    }

    CXMLNode* CXMLNode::FindNamespaceDecl( const CString& sNamespace )
    {
      for(CXMLNode* pNode = this; pNode != NULL; pNode = pNode->GetParent())
      {
        const TXMLNsList& rNsList = pNode->GetNsList();
        for (TXMLNsList::const_iterator itNs = rNsList.begin(); itNs != rNsList.end(); ++itNs)
          if (itNs->sNs == sNamespace)
            return pNode;
      }
      return NULL;
    }

    const CXMLNode* CXMLNode::FindNamespaceDecl( const CString& sNamespace ) const
    {
      for(const CXMLNode* pNode = this; pNode != NULL; pNode = pNode->GetParent())
      {
        const TXMLNsList& rNsList = pNode->GetNsList();
        for (TXMLNsList::const_iterator itNs = rNsList.begin(); itNs != rNsList.end(); ++itNs)
          if (itNs->sNs == sNamespace)
            return pNode;
      }
      return NULL;
    }

    const CString& CXMLNode::Namespace() const
    {
      return m_sNamespace;
    }

    CString& CXMLNode::Namespace()
    {
      return m_sNamespace;
    }

    CString CXMLNode::GetNodeNsName() const
    {
      return m_sNamespace == "" ? m_sName : m_sNamespace + ":" + m_sName;
    }

    // class CXMLNode /////////////////////////////////////////////////

    CXMLOStream& WriteNode(CXMLOStream& rStream, const CXMLNode& rXMLNode, int nLevel = 0 )
    {
      for ( int i = 0; i < nLevel; ++i )
        rStream << "  ";

      rStream << "<";
      switch ( rXMLNode.NodeType() )
      {
        case CXMLNode::ENTGENERIC:
        {
          rStream << rXMLNode.GetNodeNsName();

          // write NS
          for (CXMLNode::TXMLNsConstIterator itNs = rXMLNode.GetNsList().begin(); 
            itNs != rXMLNode.GetNsList().end(); ++itNs)
            rStream << *itNs;

          // write attrs
          for (CXMLNode::TXMLAttrConstIterator itAttr = rXMLNode.AttrBegin(); itAttr != rXMLNode.AttrEnd(); ++itAttr)
            rStream << *itAttr;

          // 
          if ( (rXMLNode.SubNodesCount() == 0) && (rXMLNode.NodeContent() == "") )
            rStream << "/>\n";
          else
          {
            rStream << ">";
            rStream << rXMLNode.NodeContent();
            if ( rXMLNode.SubNodesCount() != 0 )
            {
              rStream << "\n";
              // subnodes

              for ( CXMLNode::TXMLNodeConstIterator itNode = rXMLNode.NodeBegin(); itNode != rXMLNode.NodeEnd(); ++itNode )
                WriteNode(rStream, *itNode, nLevel + 1);

              for ( int i = 0; i < nLevel; ++i )
                rStream << "  ";
            }
            rStream << "</" << rXMLNode.GetNodeNsName() << ">\n";
          }
        } break;
        case CXMLNode::ENTCDATA:
        {
          rStream << "![CDATA[" << rXMLNode.NodeContent().AsString() << "]]>\n";
        } break;
        case CXMLNode::ENTCOMMENT:
        {
          rStream << "!--" << rXMLNode.NodeContent() << "-->\n"; // CHECK SPACE!
        } break;
        default:
          THROWXML(CXMLFormatException, "Invalid Node Type: " + ToStr(rXMLNode.NodeType()) + 
                                         " NodeNsName=" + rXMLNode.GetNodeNsName(), rXMLNode.NodeType(), NULL, 0);
      }
      
      return rStream;
    }

    CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLNode& rXMLNode)
    {
      return WriteNode(rStream, rXMLNode);
    }

    CXMLIStream& ReadSubNodes(CXMLIStream& rStream, CXMLNode& rXMLNode)
    {
      SXMLAttribute stAttribute;
      CString sTmp;

      for(;;)
      {
        rStream.SkipWhitespace();
        ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found", 
          rStream.GetFileName(), rStream.GetLine());

        if (rStream.Test("/"))
          return rStream;

        if (rStream.Test("?")) // skip xml parser instructions
        {
          rStream.ReadStringUntil(sTmp, "?>");
          rStream.SkipWhitespace();
          ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found",
            rStream.GetFileName(), rStream.GetLine());
        }
        else
        if (rStream.Test("!"))  // special
        {
          if (rStream.Test("[CDATA["))
          {
            rStream.ReadStringUntil(sTmp, "]]>");
            rXMLNode.AddSubNode(sTmp, CXMLNode::ENTCDATA);
          } else
          if (rStream.Test("--"))
          {
            rStream.ReadRawStringUntil(sTmp, "-->");
            rXMLNode.AddSubNode(sTmp, CXMLNode::ENTCOMMENT);
          } else
            THROWXML(CXMLFormatException, "Invalid Special id ", , rStream.GetFileName(), rStream.GetLine());
        } else
        {
          bool bIsEmptyTag = false;
          rStream.ReadId(sTmp);
          CXMLNode& rXMLNodeNew = rXMLNode.AddSubNode(sTmp);
          for(;;)  // reading attrs
          {
            rStream.SkipWhitespace();
            if(rStream.Test(">"))
              break;
            else
            if(rStream.Test("/>"))
            {
              bIsEmptyTag = true;
              break; // end reading node
            }
            
            rStream >> stAttribute;
            if (StrLeft(stAttribute.sAttrName, 6) == "xmlns:")
              rXMLNodeNew.GetNsList().push_back(SXMLNamespace(StrMid(const_cast<const SXMLAttribute&>(stAttribute).sAttrName, 6), 
                        const_cast<const SXMLAttribute&>(stAttribute).sAttrValue));
            else
            if (stAttribute.sAttrName == "xmlns")
              rXMLNodeNew.GetNsList().push_back(SXMLNamespace("", stAttribute.sAttrValue));
            else
              rXMLNodeNew.AddAttribute(const_cast<const SXMLAttribute&>(stAttribute));

          }  // while reading attrs

          if (rXMLNodeNew.Namespace() != "")
            if(rXMLNodeNew.FindNamespaceDecl(rXMLNodeNew.Namespace()) == NULL)
              LogDebug2() << "Could\'nt find xmlns declaration: \"" + rXMLNodeNew.Namespace() + "\"";

          if (!bIsEmptyTag)
          {
            rStream.AddContent(rXMLNodeNew.NodeContent());
            if (!rStream.Test("</"))
              ReadSubNodes(rStream, rXMLNodeNew); 

            rStream.ReadId(sTmp);
            ASSERTXMLS(sTmp == rXMLNodeNew.GetNodeNsName(), CXMLFormatException, 
              "Unmatched opening and closed tags: found </"
              + sTmp + "> expected: </" + rXMLNodeNew.GetNodeNsName() + ">", 
              rStream.GetFileName(), rStream.GetLine());
            rStream.SkipWhitespace();
            rStream.ReadStringUntil(sTmp, ">"); 
            ASSERTXMLS(sTmp == "", CXMLFormatException, "Unexpected text in closing tag:" + sTmp,
              rStream.GetFileName(), rStream.GetLine());
          }
        }
        rStream.AddContent(rXMLNode.NodeContent());
      }
    }

    CXMLIStream& operator>>(CXMLIStream& rStream, CXMLNode& rXMLNode)
    {
      CString sTmp;
      SXMLAttribute stAttribute;
     
      rStream.SkipWhitespace();
      ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found", 
        rStream.GetFileName(), rStream.GetLine());

      bool bFound = false;
      do
      {
        bFound = false;
        while (rStream.Test("!")) // skip xml parser instructions
        {
          if(rStream.Test("--")) // comment
          {
            rStream.ReadRawStringUntil(sTmp, "-->");
          }
          else
          {
            rStream.ReadStringUntil(sTmp, "]>");
          }
          rStream.SkipWhitespace();
          ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found",
            rStream.GetFileName(), rStream.GetLine());
          bFound = true;
        }

        while (rStream.Test("?")) // skip xml parser instructions
        {
          rStream.ReadStringUntil(sTmp, "?>");
          rStream.SkipWhitespace();
          ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found",
            rStream.GetFileName(), rStream.GetLine());
          bFound = true;
        }
      }
      while(bFound);

      rStream.ReadId(sTmp);
      rXMLNode.SetNodeNsName(sTmp);
      for(;;)
      {
        rStream.SkipWhitespace();
        if(rStream.Test(">"))
        {
          TChar chTmp = ' ';
          rStream.ReadStringUntil(rXMLNode.NodeContent(), "<");
          if(rXMLNode.NodeContent().AsString().size() > 0)
            chTmp = rXMLNode.NodeContent().AsString()[(int)(rXMLNode.NodeContent().AsString().size() - 1)];
          StrTrim(rXMLNode.NodeContent().AsString());
          rStream.UnGetChar(chTmp);
          ReadSubNodes(rStream, rXMLNode); //read subnodes

          rStream.ReadStringUntil(sTmp, ">");
          ASSERTXMLS(sTmp == rXMLNode.GetNodeNsName(), CXMLFormatException, 
            "Unmatched opening and closing tags: found </"
            + sTmp + "> expected: </" + rXMLNode.GetNodeNsName() + ">", 
            rStream.GetFileName(), rStream.GetLine() );
          break;
        } else
        if(rStream.Test("/>"))
          break;// end reading root node
        
        rStream >> stAttribute;
        if (StrLeft(stAttribute.sAttrName, 6) == "xmlns:")
          rXMLNode.GetNsList().push_back(SXMLNamespace(StrMid(const_cast<const SXMLAttribute&>(stAttribute).sAttrName, 6), 
          const_cast<const SXMLAttribute&>(stAttribute).sAttrValue));
        else
        if (stAttribute.sAttrName == "xmlns")
          rXMLNode.GetNsList().push_back(SXMLNamespace("", stAttribute.sAttrValue));
        else
          rXMLNode.AddAttribute(const_cast<const SXMLAttribute&>(stAttribute));
      }

      rStream.SkipWhitespace();
      rStream.ReadStringUntil(sTmp, "");
      ASSERTXMLS(sTmp == "", CXMLFormatException, "text after parsing", 
        rStream.GetFileName(), rStream.GetLine());
      
      return rStream;
    }

    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLNode& rXMLNode)
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLNode);
    }

    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLNode& rXMLNode)
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLNode);
    }

  } // namespace xml
}  // namespace rise
  
