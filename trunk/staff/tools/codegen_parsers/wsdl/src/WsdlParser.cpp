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

#include <set>
#include <rise/common/Log.h>
#include <rise/xml/XMLDocument.h>
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLAttribute.h>
#include <rise/xml/XMLNamespace.h>
#include <rise/plugin/PluginExport.h>
#include <staff/codegen/tools.h>
#include "HttpClient.h"
#include "WsdlParser.h"

RISE_DECLARE_PLUGIN(staff::codegen::WsdlParser)

namespace staff
{
namespace codegen
{
  struct NamespacesCache
  {
    typedef std::map<std::string, rise::xml::CXMLNode::TXMLNsList> NamespacesCacheMap;
    static NamespacesCacheMap mCache;
  };

  NamespacesCache::NamespacesCacheMap NamespacesCache::mCache;

  struct SimpleType;
  struct ComplexType;

  struct QName
  {
    std::string sName;
    std::string sPrefix;
    std::string sNamespace;
    std::string sDescr;
    std::string sDetail;

    QName()
    {
    }

    QName(const std::string& sNameIn, const std::string& sPrefixIn, const std::string& sNamespaceIn):
      sName(sNameIn), sPrefix(sPrefixIn), sNamespace(sNamespaceIn)
    {
    }

    QName(const std::string& sNameIn, const std::string& sPrefixIn, const std::string& sNamespaceIn,
           const std::string& sDescrIn):
      sName(sNameIn), sPrefix(sPrefixIn), sNamespace(sNamespaceIn), sDescr(sDescrIn)
    {
    }

    std::string GetNsName() const
    {
      return sPrefix.empty() ? sName : (sPrefix + ":" + sName);
    }

    void FromType(const rise::xml::CXMLNode& rElement, const std::string& sType);
  };

  std::ostream& operator<<(std::ostream& rOut, const QName& rstQName)
  {
    return rOut << rstQName.GetNsName() << " = " << rstQName.sNamespace << " [" << rstQName.sDescr << "]";
  }

  //////////////////////////////////////////////////////////////////////////
  struct Attribute: public QName
  {
    QName stType;
    std::string sDefault;
    bool bIsRef;
    bool bIsOptional;

    Attribute();
    Attribute& Parse(const rise::xml::CXMLNode& rNodeAttr);
  };

  //////////////////////////////////////////////////////////////////////////
  struct AttributeGroup: public QName
  {
    bool bIsRef;
    std::list<Attribute> lsAttributes;

    AttributeGroup();
    AttributeGroup& Parse(const rise::xml::CXMLNode& rNodeAttributeGroup);
  };

  //////////////////////////////////////////////////////////////////////////
  struct Element: public QName
  {
    QName stType;
    bool bIsArray;
    bool bIsExtern;
    bool bIsRef;
    bool bIsOptional;
    bool bIsNillable;
    std::string sDefault;
    std::string sChoiceId;

    std::list<SimpleType> lsSimpleTypes;
    std::list<ComplexType> lsComplexTypes;

    Element();
    Element& Parse(const rise::xml::CXMLNode& rNodeElement);

  private:
    bool GetType(const rise::xml::CXMLNode& rElement, std::string& sCppTypeName);
  };

  //////////////////////////////////////////////////////////////////////////
  struct SimpleType: public QName
  {
    typedef std::pair<std::string, std::string> StringPair;

    bool bIsExtern;
    QName stBaseType;
    StringList lsEnumValues;
    std::list< StringPair > lsRestrictions;

    SimpleType();
    SimpleType& Parse(const rise::xml::CXMLNode& rNodeSimpleType);
  };

  //////////////////////////////////////////////////////////////////////////
  struct ComplexType: public QName
  {
    std::string sElementName;
    std::list<Element> lsElements;
    std::list<Attribute> lsAttributes;
    std::list<AttributeGroup> lsAttributeGroups;
    std::string sParentName;
    std::string sParentNs;
    bool bIsExtern;
    bool bIsMessagePart;
    bool bIsSimpleContent;
    bool bHasAnyAttribute;

    ComplexType();
    ComplexType& Parse(const rise::xml::CXMLNode& rNodeComplexType);

  private:
    unsigned nLastChoiceId;

    void ParseSequence(const rise::xml::CXMLNode& rNodeSequence, const std::string& sChoiceId = "");

    void ParseComplexAttr(const rise::xml::CXMLNode& rNodeAttr);
    void ParseComplexContent(const rise::xml::CXMLNode& rNodeComplexContent, bool bIsSimple);
  };


  //////////////////////////////////////////////////////////////////////////
  std::string FindNamespaceUri( const rise::xml::CXMLNode& rNode, const std::string& sPrefix )
  {
    for (const rise::xml::CXMLNode* pNode = &rNode; pNode != NULL; pNode = pNode->GetParent())
    {
      const rise::xml::CXMLNode::TXMLNsList& rNsList = pNode->GetNsList();
      for (rise::xml::CXMLNode::TXMLNsList::const_iterator itNs = rNsList.begin();
        itNs != rNsList.end(); ++itNs)
      {
        if (itNs->sNs == sPrefix)
        {
          return itNs->sUri;
        }
      }
    }

    if (sPrefix.empty())
    {
      return "http://www.w3.org/2001/XMLSchema";
    }

    rise::LogWarning() << "Can't find prefix declaration [" << sPrefix
                       << "] for node [" << rNode.GetNodeNsName() << "]";
    return "";
  }

  void GetTns(const rise::xml::CXMLNode& rNode, std::string& sNamespaceUri, std::string& sPrefix)
  {
    sNamespaceUri.erase();
    sPrefix.erase();

    for (const rise::xml::CXMLNode* pNode = &rNode; pNode != NULL; pNode = pNode->GetParent())
    {
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = pNode->FindAttribute("targetNamespace");
      if (itAttr != pNode->AttrEnd())
      {
        if (sNamespaceUri.empty()) // take only current targetNamespace
        {
          sNamespaceUri = itAttr->sAttrValue.AsString();
        }

        // find prefix everywhere in parent nodes
        const rise::xml::CXMLNode::TXMLNsList& rNsList = pNode->GetNsList();
        for (rise::xml::CXMLNode::TXMLNsList::const_iterator itNs = rNsList.begin();
          itNs != rNsList.end(); ++itNs)
        {
          if (itNs->sUri == sNamespaceUri)
          {
            sPrefix = itNs->sNs;
            return;
          }
        }
      }
    }

    RISE_ASSERTS(!sNamespaceUri.empty(), "Can't find target namespace for node: " + rise::ToStr(rNode));
  }


  void GetNsPrefix(const rise::xml::CXMLNode& rNode, const std::string& sNamespaceUri, std::string& sPrefix)
  {
    for (const rise::xml::CXMLNode* pNode = &rNode; pNode != NULL; pNode = pNode->GetParent())
    {
      const rise::xml::CXMLNode::TXMLNsList& rNsList = pNode->GetNsList();
      for (rise::xml::CXMLNode::TXMLNsList::const_iterator itNs = rNsList.begin();
        itNs != rNsList.end(); ++itNs)
      {
        if (itNs->sUri == sNamespaceUri)
        {
          sPrefix = itNs->sNs;
          return;
        }
      }
    }

//    RISE_THROWS(rise::CLogicNoItemException, "Can't find target namespace with nsuri: [" + sNamespaceUri
//                + "] prefix: [" + sPrefix + "] for: " + rise::ToStr(rNode));
    sPrefix.erase();
  }

  const std::string& GetTns(const rise::xml::CXMLNode& rNode)
  {
    for (const rise::xml::CXMLNode* pNode = &rNode; pNode != NULL; pNode = pNode->GetParent())
    {
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = pNode->FindAttribute("targetNamespace");
      if (itAttr != pNode->AttrEnd())
      {
        return itAttr->sAttrValue.AsString();
      }
    }

    RISE_THROWS(rise::CLogicNoItemException, "Can't find target namespace for: " + rise::ToStr(rNode));
  }

  std::string StripPrefix(const std::string& sSymbol)
  {
    std::string::size_type nPos = sSymbol.find_last_of(':');
    if (nPos != std::string::npos)
    {
      return sSymbol.substr(nPos + 1);
    }

    return sSymbol;
  }

  std::string GetPrefix(const std::string& sSymbol)
  {
    std::string::size_type nPos = sSymbol.find_last_of(':');
    if (nPos != std::string::npos)
    {
      return sSymbol.substr(0, nPos);
    }

    return "";
  }

  void NormalizeString(std::string& sString)
  {
    std::string::size_type nPosBegin = 0;
    std::string::size_type nPosEnd = 0;
    while ((nPosBegin = sString.find_first_of(" \t", nPosEnd)) != std::string::npos)
    {
      if (sString[nPosBegin] == '\t')
      {
        sString[nPosBegin] = ' ';
      }

      ++nPosBegin;
      nPosEnd = sString.find_first_not_of(" \t", nPosBegin);
      if (nPosEnd == std::string::npos)
      {
        break;
      }

      if (nPosEnd != nPosBegin)
      {
        sString.erase(nPosBegin, nPosEnd - nPosBegin);
      }

      nPosEnd = nPosBegin;
    }
  }

  bool FixId(std::string& sId)
  {
    static const char* szIdChars = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    bool bChanged = false;
    if (sId.empty())
    {
      return false;
    }

    std::string::size_type nPos = 0;
    while ((nPos = sId.find_first_not_of(szIdChars)) != std::string::npos)
    {
      sId[nPos] = '_';
      bChanged = true;
    }

    const char chFirst = sId[0];
    if (chFirst >= '0' && chFirst <= '9')
    {
      sId.insert(0, "_", 1);
      bChanged = true;
    }

    return bChanged;
  }

  bool ReadDoc(const rise::xml::CXMLNode& rNode, std::string& sDescr, std::string& sDetail)
  {
    bool bReaded = false;
    rise::xml::CXMLNode::TXMLNodeConstIterator itDoc = rNode.FindSubnode("documentation");
    if (itDoc == rNode.NodeEnd())
    {
      itDoc = rNode.FindSubnode("annotation");
      if (itDoc != rNode.NodeEnd())
      {
        itDoc = itDoc->FindSubnode("documentation");
      }
    }

    if (itDoc != rNode.NodeEnd())
    {
      bReaded = true;
      const std::string& sDoc = itDoc->NodeContent().AsString();
      sDescr = sDoc;
#ifdef __linux__
      rise::StrReplace(sDescr, "\r", "", true);
#endif

      std::string::size_type nPos = 0;
      while ((nPos = sDescr.find(".", nPos)) != std::string::npos)
      {
        if (nPos < sDescr.size() &&
            std::string(" \t\n").find(sDescr[nPos + 1]) != std::string::npos)
        {
          sDetail = sDescr.substr(nPos + 1);
          sDescr.erase(nPos + 1);
          rise::StrTrimLeft(sDetail);
          rise::StrTrimLeft(sDescr);
          break;
        }
        ++nPos;
      }

      rise::StrReplace(sDescr, "\n", " ", true);
      NormalizeString(sDescr);
    }
    return bReaded;
  }

  const rise::xml::CXMLNode& GetGroupDeclaration(const rise::xml::CXMLNode& rGroup)
  {
    const std::string& sGroupName = rGroup.Attribute("ref").AsString();
    const rise::xml::CXMLNode* pNodeSchema = rGroup.GetParent();
    for (; pNodeSchema != NULL && pNodeSchema->NodeName() != "schema";
      pNodeSchema = pNodeSchema->GetParent());
    RISE_ASSERTS(pNodeSchema, "Can't find schema declaration");
    rise::xml::CXMLNode::TXMLNodeConstIterator itGroupDecl =
      pNodeSchema->FindNodeMatch("group", rise::xml::SXMLAttribute("name", sGroupName));
    RISE_ASSERTS(itGroupDecl != pNodeSchema->NodeEnd(),
                 "Can't find group \"" + sGroupName + "\" declaration");
    return *itGroupDecl;
  }


  void MakeOptional(DataType& rDataType)
  {
    if (rDataType.sName == "Optional")
    {
      std::string sType = rDataType.lsParams.empty()
          ? "<unknown>"
          : rDataType.lsParams.front().sNamespace + rDataType.lsParams.front().sName;
      rise::LogWarning() << "Type " << sType << " already marked as optional";
      return;
    }
    rDataType.lsParams.push_front(rDataType);
    rDataType.lsParams.resize(1);
    rDataType.sName = "Optional";
    rDataType.sNamespace = "staff::";
    rDataType.eType = DataType::TypeTemplate;
    rDataType.sUsedName.erase();
  }

  //////////////////////////////////////////////////////////////////////////
  void QName::FromType(const rise::xml::CXMLNode& rElement, const std::string& sType)
  {
    sName = StripPrefix(sType);
    sPrefix = GetPrefix(sType);
    sNamespace = FindNamespaceUri(rElement, sPrefix);
  }

  //////////////////////////////////////////////////////////////////////////
  Attribute::Attribute():
    bIsRef(false), bIsOptional(true)
  {
  }

  Attribute& Attribute::Parse(const rise::xml::CXMLNode& rNodeAttr)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeAttr.FindAttribute("name");

    itAttr = rNodeAttr.FindAttribute("name");
    if (itAttr != rNodeAttr.AttrEnd())
    {
      sName = itAttr->sAttrValue.AsString();
    }

    itAttr = rNodeAttr.FindAttribute("use");
    if (itAttr != rNodeAttr.AttrEnd())
    {
      const std::string& sUse = itAttr->sAttrValue.AsString();
      if (sUse == "required")
      {
        bIsOptional = false;
      }
      else
      if (sUse == "optional")
      {
        bIsOptional = true;
      }
      else
      {
        rise::LogWarning() << "attribute use [" << sUse << "] not supported";
      }
    }

    itAttr = rNodeAttr.FindAttribute("default");
    if (itAttr != rNodeAttr.AttrEnd())
    {
      sDefault = itAttr->sAttrValue.AsString();
    }

    GetTns(rNodeAttr, sNamespace, sPrefix);

    ReadDoc(rNodeAttr, sDescr, sDetail);
    //  if type exists, attr is simple
    itAttr = rNodeAttr.FindAttribute("type");
    if (itAttr != rNodeAttr.AttrEnd())
    { // attr is simple
      stType.FromType(rNodeAttr, itAttr->sAttrValue.AsString());
    }
    else
    { // reference to another type
      itAttr = rNodeAttr.FindAttribute("ref");
      if (itAttr != rNodeAttr.AttrEnd())
      {
        bIsRef = true;
        stType.FromType(rNodeAttr, itAttr->sAttrValue.AsString());
      }
    }

    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  Element::Element():
    bIsArray(false), bIsExtern(false), bIsRef(false), bIsOptional(false), bIsNillable(false)
  {
  }

  Element& Element::Parse( const rise::xml::CXMLNode& rNodeElement )
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeElement.FindAttribute("name");

    sName = (itAttr != rNodeElement.AttrEnd()) ? itAttr->sAttrValue.AsString() : "";

    GetTns(rNodeElement, sNamespace, sPrefix);

    ReadDoc(rNodeElement, sDescr, sDetail);

    rise::xml::CXMLNode::TXMLAttrConstIterator itNodeAttr = rNodeElement.FindAttribute("default");
    if (itNodeAttr != rNodeElement.AttrEnd())
    {
      sDefault = itNodeAttr->sAttrValue.AsString();
    }

    itAttr = rNodeElement.FindAttribute("nillable");
    bIsNillable = (itAttr != rNodeElement.AttrEnd()) && (itAttr->sAttrValue.AsString() == "true");

    itAttr = rNodeElement.FindAttribute("maxOccurs");
    bIsArray = (itAttr != rNodeElement.AttrEnd()) && (itAttr->sAttrValue.AsString() == "unbounded");

    if (!bIsArray)
    {
      itAttr = rNodeElement.FindAttribute("minOccurs");
      bIsOptional = (itAttr != rNodeElement.AttrEnd()) && (itAttr->sAttrValue.AsString() == "0");
    }

    //  if type exists, element is simple
    itAttr = rNodeElement.FindAttribute("type");
    if (itAttr != rNodeElement.AttrEnd())
    { // element is simple
      stType.FromType(rNodeElement, itAttr->sAttrValue.AsString());
    }
    else
    { // element is complex
      // reference to another type
      itAttr = rNodeElement.FindAttribute("ref");
      if (itAttr != rNodeElement.AttrEnd())
      { // find element
        bIsRef = true;
        stType.FromType(rNodeElement, itAttr->sAttrValue.AsString());
      }
      else
      {
        // has subtypes
        for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = rNodeElement.NodeBegin();
          itChild != rNodeElement.NodeEnd(); ++itChild)
        {
          if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
          {
            const std::string& sNodeName = itChild->NodeName();
            if (sNodeName == "complexType")
            {
              lsComplexTypes.push_back(ComplexType());
              lsComplexTypes.back().sElementName = sName;
              lsComplexTypes.back().Parse(*itChild);
            }
            else
            if (sNodeName == "simpleType")
            {
              lsSimpleTypes.push_back(SimpleType());
              lsSimpleTypes.back().Parse(*itChild);
            }
          }
        }//for
      }
    }

    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  SimpleType::SimpleType():
    bIsExtern(false)
  {
  }

  SimpleType& SimpleType::Parse(const rise::xml::CXMLNode& rNodeSimpleType)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeSimpleType.FindAttribute("name");

    sName = (itAttr != rNodeSimpleType.AttrEnd()) ? itAttr->sAttrValue.AsString() : "";

    GetTns(rNodeSimpleType, sNamespace, sPrefix);

    ReadDoc(rNodeSimpleType, sDescr, sDetail);

    for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = rNodeSimpleType.NodeBegin();
      itChild != rNodeSimpleType.NodeEnd(); ++itChild)
    {
      if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sNodeName = itChild->NodeName();
        if (sNodeName == "restriction")
        {
          const std::string& sBaseType = itChild->Attribute("base").AsString();
          stBaseType.sPrefix = GetPrefix(sBaseType);
          stBaseType.sName = StripPrefix(sBaseType);
          stBaseType.sNamespace = FindNamespaceUri(rNodeSimpleType, stBaseType.sPrefix);

          // process restriction
          for (rise::xml::CXMLNode::TXMLNodeConstIterator itSubChild = itChild->NodeBegin();
            itSubChild != itChild->NodeEnd(); ++itSubChild)
          {
            if (itSubChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
            {
              if (itSubChild->NodeName() == "enumeration")
              {
                lsEnumValues.push_back(itSubChild->Attribute("value").AsString());
              }
              else
              {
                lsRestrictions.push_back(StringPair(itSubChild->NodeName(),
                                                  itSubChild->Attribute("value").AsString()));
              }
            }
          }
        }
      }
    }

    return *this;
  }


  //////////////////////////////////////////////////////////////////////////
  ComplexType::ComplexType():
    bIsExtern(false), bIsMessagePart(false), bIsSimpleContent(false),
    bHasAnyAttribute(false), nLastChoiceId(0)
  {
  }

  ComplexType& ComplexType::Parse( const rise::xml::CXMLNode& rNodeComplexType )
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttrName = rNodeComplexType.FindAttribute("name");

    if (itAttrName != rNodeComplexType.AttrEnd())
    {
      sName = itAttrName->sAttrValue.AsString();
    }
    else
    {
      sName = sElementName;
    }

    GetTns(rNodeComplexType, sNamespace, sPrefix);

    ReadDoc(rNodeComplexType, sDescr, sDetail);

    for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = rNodeComplexType.NodeBegin();
      itChild != rNodeComplexType.NodeEnd(); ++itChild)
    {
      if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sNodeName = itChild->NodeName();
        if (sNodeName == "sequence" || sNodeName == "all")
        {
          ParseSequence(*itChild);
        }
        else
        if (sNodeName == "complexContent")
        {
          ParseComplexContent(*itChild, false);
        }
        else
        if (sNodeName == "simpleContent")
        {
          ParseComplexContent(*itChild, true);
        }
        else
        if (sNodeName == "choice")
        {
          ParseSequence(*itChild, rise::ToStr(nLastChoiceId++));
        }
        else
        if (sNodeName == "group")
        {
          ParseSequence(GetGroupDeclaration(*itChild));
        }
        else
        if (sNodeName == "attribute")
        {
          lsAttributes.push_back(Attribute());
          lsAttributes.back().Parse(*itChild);
        }
        else
        if (sNodeName == "anyAttribute")
        {
          bHasAnyAttribute = true;
        }
        else
        if (sNodeName == "attributeGroup")
        {
          lsAttributeGroups.insert(lsAttributeGroups.end(), AttributeGroup())->Parse(*itChild);
        }
        else
        if (sNodeName != "annotation" && sNodeName != "documentation") // already parsed
        {
          rise::LogWarning() << "Unsupported complexType with \"" << sNodeName << "\"";
        }
      }
    }

    return *this;
  }

  void ComplexType::ParseSequence(const rise::xml::CXMLNode& rNodeSequence, const std::string& sChoiceId /*= ""*/)
  {
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = rNodeSequence.NodeBegin();
      itChild != rNodeSequence.NodeEnd(); ++itChild)
    {
      if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sNodeName = itChild->NodeName();
        if (sNodeName == "element")
        {
          Element& rElement = *lsElements.insert(lsElements.end(), Element());
          rElement.Parse(*itChild);
          rElement.sChoiceId = sChoiceId;
          if (rElement.stType.sName == "anyType")
          {
            rElement.stType.sName = "DataObject";
          }
        }
        else
        if (sNodeName == "any")
        {
          Element& rElement = *lsElements.insert(lsElements.end(), Element());
          rise::xml::CXMLNode::TXMLAttrConstIterator itAttrNs = itChild->FindAttribute("namespace");
          rElement.Parse(*itChild);
          rElement.stType.sName = "DataObject";
          rElement.sChoiceId = sChoiceId;
          if (itAttrNs != itChild->AttrEnd())
          {
            rElement.stType.sNamespace = itAttrNs->sAttrValue.AsString();
          }
        }
        else
        if (sNodeName == "sequence")
        {
          ParseSequence(*itChild, sChoiceId);
        }
        else
        if (sNodeName == "choice")
        {
          ParseSequence(*itChild, rise::ToStr(nLastChoiceId++));
        }
        else
        if (sNodeName == "group")
        {
          ParseSequence(GetGroupDeclaration(*itChild), sChoiceId);
        }
        else
        if (sNodeName != "annotation" && sNodeName != "documentation") // already parsed
        {
          rise::LogWarning() << "Unsupported sequence with \"" << sNodeName << "\"";
        }
      }
    }
  }


  void ComplexType::ParseComplexAttr(const rise::xml::CXMLNode& rNodeAttr)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttrRef = rNodeAttr.FindAttribute("ref");
    if (itAttrRef != rNodeAttr.AttrEnd())
    {
      std::string sRef = StripPrefix(itAttrRef->sAttrValue.AsString());

      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrRefType = rNodeAttr.AttrBegin();
      for (; itAttrRefType != rNodeAttr.AttrEnd();
          ++itAttrRefType)
      {
        std::string sAttrName = StripPrefix(itAttrRefType->sAttrName);

        if (sAttrName == sRef)
        {
          break;
        }
      }

      if (itAttrRefType != rNodeAttr.AttrEnd())
      {
        std::string sType = itAttrRefType->sAttrValue.AsString();

        if (sType.size() != 0)
        {
          Element stElemType;
          stElemType.bIsArray = (sType.substr(sType.size() - 2) == "[]");
          if (stElemType.bIsArray)
          {
            sType.erase(sType.size() - 2);
          }

          stElemType.stType.sName = sType;
          stElemType.sName = "elem" + stElemType.stType.sName;
          GetTns(rNodeAttr, stElemType.sNamespace, stElemType.sPrefix);

          lsElements.push_back(stElemType);
        }
      }
    }
    else
    {
      lsAttributes.insert(lsAttributes.end(), Attribute())->Parse(rNodeAttr);
    }
  }

  void ComplexType::ParseComplexContent( const rise::xml::CXMLNode& rNodeComplexContent, bool bIsSimple )
  {
    rise::xml::CXMLNode::TXMLNodeConstIterator itNodeExtension = rNodeComplexContent.FindSubnode("extension");

    this->bIsSimpleContent = bIsSimple;

    if (itNodeExtension != rNodeComplexContent.NodeEnd())
    {
      sParentName = itNodeExtension->Attribute("base").AsString();
      sParentNs = FindNamespaceUri(rNodeComplexContent, GetPrefix(sParentName));

      for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = itNodeExtension->NodeBegin();
        itChild != itNodeExtension->NodeEnd(); ++itChild)
      {
        if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
        {
          const std::string& sNodeName = itChild->NodeName();
          if (sNodeName == "sequence" || sNodeName == "all")
          {
            ParseSequence(*itChild);
          }
          else
          if (sNodeName == "attribute")
          {
            ParseComplexAttr(*itChild);
          }
        }
      }
    }
    else
    {
      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeRestriction = rNodeComplexContent.FindSubnode("restriction");

      if (itNodeRestriction != rNodeComplexContent.NodeEnd())
      {
        sParentName = itNodeRestriction->Attribute("base").AsString();

        for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeAttr = itNodeRestriction->FindSubnode("attribute");
            itNodeAttr != itNodeRestriction->NodeEnd();
            itNodeAttr = itNodeRestriction->FindSubnode("attribute", ++itNodeAttr))
        {
          ParseComplexAttr(*itNodeAttr);
        }
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////
  AttributeGroup::AttributeGroup():
    bIsRef(false)
  {
  }

  AttributeGroup& AttributeGroup::Parse(const rise::xml::CXMLNode& rNodeAttributeGroup)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeAttributeGroup.FindAttribute("ref");
    if (itAttr != rNodeAttributeGroup.AttrEnd())
    {
      bIsRef = true;
      FromType(rNodeAttributeGroup, itAttr->sAttrValue.AsString());
    }
    else
    {
      itAttr = rNodeAttributeGroup.FindAttribute("name");
      if (itAttr != rNodeAttributeGroup.AttrEnd())
      {
        sName = itAttr->sAttrValue.AsString();
      }

      GetTns(rNodeAttributeGroup, sNamespace, sPrefix);
      ReadDoc(rNodeAttributeGroup, sDescr, sDetail);

      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeAttr = rNodeAttributeGroup.NodeBegin();
          itNodeAttr != rNodeAttributeGroup.NodeEnd(); ++itNodeAttr)
      {
        if (itNodeAttr->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
        {
          if (itNodeAttr->NodeName() != "attribute")
          {
            if (itNodeAttr->NodeName() != "annotation" && itNodeAttr->NodeName() != "documentation")
            {
              rise::LogWarning() << "Unsupported [" << itNodeAttr->NodeName() << "] within attributeGroup";
            }
          }
          else
          {
            lsAttributes.insert(lsAttributes.end(), Attribute())->Parse(*itNodeAttr);
          }
        }
      }
    }

    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  typedef std::map<std::string, Element> ElementMap;
  typedef std::map<std::string, ComplexType> ComplexTypeMap;
  typedef std::map<std::string, SimpleType> SimpleTypeMap;
  typedef std::map<std::string, Attribute> AttributeMap;
  typedef std::map<std::string, AttributeGroup> AttributeGroupMap;

  class WsdlParserImpl;

  struct WsdlTypes
  {
    ElementMap mElements;
    ComplexTypeMap mComplexTypes;
    SimpleTypeMap mSimpleTypes;
    AttributeMap mAttributes;
    AttributeGroupMap mAttributeGroups;
    std::set<std::string> m_setNsAliases;
    WsdlParserImpl* m_pParser;

    WsdlTypes(WsdlParserImpl* pParser);
    void Parse(rise::xml::CXMLNode& rNodeWsdl, Project& rProject, Interface& rInterface);
    void ParseSchema(const rise::xml::CXMLNode& rSchema);
    void ImportStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst);
    void Import(rise::xml::CXMLNode& rNodeImport, Project& rProject, Interface& rInterface);
    void IncludeAll(rise::xml::CXMLNode& rNode);
    void ImportAll(rise::xml::CXMLNode& rNode, Project& rProject, Interface& rInterface);
    void IncludeSchema(rise::xml::CXMLNode &rNodeParent, rise::xml::CXMLNode::TXMLNodeIterator &itNodeInclude);
  };


  //////////////////////////////////////////////////////////////////////////
  // wsdl parser impl

  class WsdlParserImpl
  {
  public:
    WsdlParserImpl(const ParseSettings& rParseSettings):
      m_stWsdlTypes(this), m_rParseSettings(rParseSettings)
    {
      m_sRootNamespace = "::";
      StringMap::const_iterator itRootNs = rParseSettings.mEnv.find("rootns");
      if (itRootNs != rParseSettings.mEnv.end() && !itRootNs->second.empty())
      {
        m_sRootNamespace = "::" + itRootNs->second + "::";
        rise::StrReplace(m_sRootNamespace, ".", "::", true);
      }
    }

    bool ParseTypeAny(const Element& rElement, DataType& rDataType)
    {
      if (m_rParseSettings.mEnv.count("use_schema_for_any") != 0)
      {
        const Element* pSchemaElem = NULL;
        const std::string& sNamespace = rElement.stType.sNamespace;
        // find DataSource declaration
        for (ElementMap::const_iterator itElem = m_stWsdlTypes.mElements.begin();
          itElem != m_stWsdlTypes.mElements.end() && !pSchemaElem; ++itElem)
        {
          const Element& rChildElem = itElem->second;
          if (rChildElem.sNamespace == sNamespace && !rChildElem.lsComplexTypes.empty())
          {
            pSchemaElem = &rChildElem;
            break;
          }
        }

        if (!pSchemaElem)
        {
          return false;
        }

        rDataType = ComplexTypeToData(pSchemaElem->lsComplexTypes.front(), m_stWsdlTypes);
        return true;
      }
      return false;
    }

    void ParsePartElement(const Element& rElement, Member& rMember, bool bIsResponse, int nRecursionLevel = 0)
    {
      // parse element operation parameters
      // declared as complex type
      if (!rElement.lsComplexTypes.empty())
      {
        // inline complex type
        for (std::list<ComplexType>::const_iterator itComplexType = rElement.lsComplexTypes.begin();
          itComplexType != rElement.lsComplexTypes.end(); ++itComplexType)
        {
          const ComplexType& rComplexType = *itComplexType;
          if (bIsResponse && rComplexType.lsElements.empty())
          {
            rMember.stReturn.stDataType.sName = "void";
          }
          else
          {
            for (std::list<Element>::const_iterator itParamElement = rComplexType.lsElements.begin();
              itParamElement != rComplexType.lsElements.end(); ++itParamElement)
            {
              const Element& rChildElement = *itParamElement;
              if (!rChildElement.stType.sName.empty()) // type is set: processing elem as struct
              {
                Param stParam;

                DataType stDataType;

                // trying to parse attached schema to "any" using namespace
                if (rChildElement.stType.sName != "DataObject" || rChildElement.sNamespace.empty()
                    || !ParseTypeAny(rChildElement, stDataType))
                {
                  GetCppType(rChildElement.stType, stDataType);
                }
                if (rChildElement.bIsArray)
                {
                  stParam.sName = rElement.sName;
                  stParam.stDataType.lsParams.push_back(stDataType);
                  stParam.stDataType.eType = DataType::TypeTemplate;
                  stParam.stDataType.sName = "list";
                  stParam.stDataType.sNamespace = "std::";
                }
                else
                {
                  stParam.sName = rChildElement.sName;
                  stDataType.sUsedName = stDataType.sNamespace + stDataType.sName;
                  OptimizeCppNs(stDataType.sUsedName, m_stInterface.sNamespace);
                  stParam.stDataType = stDataType;
                }
                stParam.stDataType.sNodeName = rChildElement.sName;

                if (!rComplexType.lsAttributes.empty() || rComplexType.bHasAnyAttribute)
                {
                  // create structure-wrapper for element with attributes
                  Struct stStruct;
                  stStruct.bForward = false;
                  stStruct.sName = rElement.sName;
                  stStruct.sNamespace = TnsToCppNs(rElement.sNamespace);
                  stStruct.sDescr = rElement.sDescr;
                  stStruct.sDetail = rElement.sDetail;

                  stParam.mOptions["useParentElement"] = "true";
                  stStruct.lsMembers.push_back(stParam);

                  for (std::list<Attribute>::const_iterator itAttr = rComplexType.lsAttributes.begin();
                    itAttr != rComplexType.lsAttributes.end(); ++itAttr)
                  {
                    Param stMember;
                    stMember.mOptions["isAttribute"] = "true";
                    if (!itAttr->sDefault.empty())
                    {
                      stMember.mOptions["defaultValue"] = itAttr->sDefault;
                    }
                    stMember.sName = itAttr->sName;
                    if (itAttr->stType.sName.empty()) // use string as default attribute value type
                    {
                      stMember.stDataType.sName = "string";
                      stMember.stDataType.sNamespace = "std::";
                      stMember.stDataType.sUsedName = "std::string";
                      stMember.stDataType.eType = DataType::TypeString;
                    }
                    else
                    {
                      GetCppType(itAttr->stType, stMember.stDataType);
                      stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
                      OptimizeCppNs(stMember.stDataType.sUsedName, stStruct.sNamespace);
                    }
                    if (itAttr->bIsOptional)
                    {
                      MakeOptional(stMember.stDataType);
                    }
                    stMember.sDescr = itAttr->sDescr;
                    stMember.sDetail = itAttr->sDetail;
                    stStruct.lsMembers.push_back(stMember);
                  }

                  if (rComplexType.bHasAnyAttribute)
                  {
                    Param stMember;
                    stMember.sName = "lsAnyAttributes";
                    stMember.stDataType.sName = "anyAttribute";
                    stMember.stDataType.sNamespace = "staff::";
                    stMember.stDataType.sUsedName = "staff::anyAttribute";
                    stMember.stDataType.eType = DataType::TypeGeneric;
                    stStruct.lsMembers.push_back(stMember);
                  }

                  m_stInterface.lsStructs.push_back(stStruct);

                  stParam.stDataType.eType = DataType::TypeStruct;
                  stParam.stDataType.sName = stStruct.sName;
                  stParam.stDataType.sNamespace = stStruct.sNamespace;
                  stParam.stDataType.sUsedName = stStruct.sNamespace + stParam.stDataType.sName;
                  stParam.stDataType.lsParams.clear();
                }

                OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);

                if (!bIsResponse)
                { // request
                  if (rElement.bIsOptional || rChildElement.bIsOptional)
                  {
                    MakeOptional(stParam.stDataType);
                  }

                  FixParamDataType(stParam.stDataType);

                  rMember.lsParams.push_back(stParam);
                  if (nRecursionLevel == 0 && rElement.sName != rMember.sName)
                  {
                    rMember.mOptions["requestElement"] = rElement.sName;
                  }

                }
                else
                { // response
                  if (nRecursionLevel == 0)
                  {
                    if (rElement.sName != (rMember.sName + "Result"))
                    {
                      rMember.mOptions["responseElement"] = rElement.sName;
                    }
                    if (!rChildElement.sName.empty() && !rChildElement.bIsArray)
                    {
                      rMember.mOptions["resultElement"] = rChildElement.sName;
                    }
                    if (rElement.bIsNillable)
                    {
                      MakeOptional(stParam.stDataType);
                    }
                  }
                  else
                  if (nRecursionLevel == 1)
                  {
                    rMember.mOptions["resultElement"] = rElement.sName;
                    if (rChildElement.bIsOptional)
                    {
                      MakeOptional(stParam.stDataType);
                    }
                  }
                  stParam.stDataType.sNodeName = rElement.sName;
                  rMember.stReturn = stParam;
                }
              }
              else // type is not set: processing as inline complex or simple type
              {
                if (nRecursionLevel == 0)
                {
                  if (bIsResponse)
                  {
                    if (rElement.sName != (rMember.sName + "Result"))
                    {
                      rMember.mOptions["responseElement"] = rElement.sName;
                    }
                  }
                  else
                  {
                    if (rElement.sName != rMember.sName)
                    {
                      rMember.mOptions["requestElement"] = rElement.sName;
                    }
                  }
                }
                ParsePartElement(rChildElement, rMember, bIsResponse, nRecursionLevel + 1);
              }
            }
          }
        }
      }
      else
      if (!rElement.lsSimpleTypes.empty())
      { // simple type
        Param stParam;
        stParam.sName = rElement.sName;
        stParam.stDataType = SimpleTypeToData(rElement.lsSimpleTypes.front(), m_stWsdlTypes);
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;

        if (bIsResponse)
        {
          rMember.stReturn = stParam;
        }
        else
        {
          FixParamDataType(stParam.stDataType);
          rMember.lsParams.push_back(stParam);
        }
      }
      else
      { // reference to type
        Param stParam;

        stParam.sName = rElement.sName;

        {// search in complex types
          ComplexTypeMap::iterator itComplexType =
              m_stWsdlTypes.mComplexTypes.find(rElement.stType.GetNsName());
          if (itComplexType != m_stWsdlTypes.mComplexTypes.end())
          {
            ComplexType& rComplexType = itComplexType->second;
            rComplexType.bIsMessagePart = true;

            if (rComplexType.lsElements.empty())
            { // assume void type
              stParam.stDataType.sName = "void";
            }
            else
            if (rComplexType.lsElements.size() == 1 && !rComplexType.lsElements.front().bIsArray) // inline complex type with element name
            {
              const Element& rElem = rComplexType.lsElements.front();

              GetCppType(rElem.stType, stParam.stDataType);

              if (stParam.stDataType.eType == DataType::TypeUnknown)
              {
                DataTypeFromName(rElem.stType.GetNsName(), stParam.stDataType, m_stWsdlTypes);
              }

              stParam.sName = rElem.sName;
            }
            else
            {
              stParam.stDataType = ComplexTypeToData(rComplexType, m_stWsdlTypes);
            }
          }
          else // search in simple types
          {
            SimpleTypeMap::const_iterator itSimpleType =
                m_stWsdlTypes.mSimpleTypes.find(rElement.stType.GetNsName());
            if (itSimpleType != m_stWsdlTypes.mSimpleTypes.end())
            {
              stParam.stDataType = SimpleTypeToData(itSimpleType->second, m_stWsdlTypes);
            }
            else // type is generic or unknown
            {
              GetCppType(rElement.stType, stParam.stDataType);
              if (stParam.stDataType.eType == DataType::TypeUnknown)
              {
                rise::LogError() << "Cannot detect type of " << rElement.stType.GetNsName()
                     << " interface name: " << m_stInterface.sName;
              }
            }
          }
        }


        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;
        OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);

        if (!bIsResponse)
        {  // set node name for request
          stParam.stDataType.sNodeName = rElement.sName;

          if (rElement.bIsOptional || (!nRecursionLevel && rElement.bIsNillable))
          {
            MakeOptional(stParam.stDataType);
          }

          FixParamDataType(stParam.stDataType);

          if (!nRecursionLevel && rElement.sName != rMember.sName)
          {
            rMember.mOptions["requestElement"] = rElement.sName;
          }

          rMember.mOptions["inlineRequestElement"] = "true";

          rMember.lsParams.push_back(stParam);
        }
        else
        {
          if (!nRecursionLevel && rElement.sName != (rMember.sName + "Result"))
          {
            rMember.mOptions["responseElement"] = rElement.sName;
          }

          if (rElement.bIsOptional || (!nRecursionLevel && rElement.bIsNillable))
          {
            MakeOptional(stParam.stDataType);
          }

          rMember.stReturn = stParam;
          rMember.stReturn.stDataType.sNodeName = rElement.sName;
        }
      }

    }

    // part is an params list or result
    void ParsePart(Member& rMember, const rise::xml::CXMLNode& rPart,
                   const WsdlTypes& rWsdlTypes, bool bIsResponse)
    {
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrType = rPart.FindAttribute("element");
      if (itAttrType != rPart.AttrEnd())
      { // reference to element type
        const std::string& sElementNsName = itAttrType->sAttrValue.AsString();

        // finding element of part
        ElementMap::const_iterator itElement = rWsdlTypes.mElements.find(sElementNsName);
        RISE_ASSERTES(itElement != rWsdlTypes.mElements.end(), rise::CLogicNoItemException,
          "Element " + itAttrType->sAttrValue.AsString() + " is not found, while parsing part");

        ParsePartElement(itElement->second, rMember, bIsResponse);
      }
      else
      { // inline part declaration
        const std::string& sPartName = rPart.Attribute("name").AsString();
        rise::xml::CXMLNode::TXMLAttrConstIterator itType = rPart.FindAttribute("type");
        RISE_ASSERTES(itType != rPart.AttrEnd(), rise::CLogicNoItemException,
          "Unknown part type: " + sPartName);

        const std::string& sType = itType->sAttrValue.AsString();

        Param stParam;
        stParam.sName = StripPrefix(sPartName);
        stParam.stDataType.sNodeName = stParam.sName;

        const std::string& sName = StripPrefix(sType);
        const std::string& sPrefix = GetPrefix(sType);
        const std::string& sNamespace = FindNamespaceUri(rPart, sPrefix);

        GetCppType(QName(sName, sPrefix, sNamespace), stParam.stDataType);
        RISE_ASSERTES(!stParam.stDataType.sName.empty(), rise::CLogicNoItemException, "Unknown part type");
        stParam.stDataType.sNodeName = stParam.stDataType.sName;
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;
        OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);

        FixParamDataType(stParam.stDataType);

        if (!bIsResponse)
        {
          rMember.lsParams.push_back(stParam);
        }
        else
        {
          rMember.stReturn = stParam;
        }
      }
    }


    void ParseRequest(Member& rMember, const rise::xml::CXMLNode& rMessage, const WsdlTypes& rWsdlTypes)
    {
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodePart = rMessage.FindSubnode("part");
          itNodePart != rMessage.NodeEnd();
          itNodePart = rMessage.FindSubnode("part", ++itNodePart))
      {
        ParsePart(rMember, *itNodePart, rWsdlTypes, false);
      }
    }

    void ParseResponse(Member& rMember, const rise::xml::CXMLNode& rMessage, const WsdlTypes& rWsdlTypes)
    {
      ParsePart(rMember, rMessage.Subnode("part"), rWsdlTypes, true);
    }


    void ParseOperation(Member& rMember, const rise::xml::CXMLNode& rOperation, const rise::xml::CXMLNode& rDefs, const WsdlTypes& rWsdlTypes)
    {
      bool bHasInput = false;
      bool bHasOutput = false;
      rMember.sName = rOperation.Attribute("name").AsString();

      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeInput =
        rOperation.FindSubnode("input");
      if (itNodeInput != rOperation.NodeEnd())
      {
        // request
        std::string sRequestName = StripPrefix(itNodeInput->Attribute("message").AsString());

        rise::xml::CXMLNode::TXMLNodeConstIterator itMessage =
          rDefs.FindNodeMatch("message", rise::xml::SXMLAttribute("name", sRequestName));

        RISE_ASSERTES(itMessage != rDefs.NodeEnd(), rise::CLogicNoItemException,
          "Can't find message definition(input) for: " + sRequestName);

        ParseRequest(rMember, *itMessage, rWsdlTypes);
        bHasInput = true;
      } // else notification message

      // response
      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeOutput =
        rOperation.FindSubnode("output");
      if (itNodeOutput != rOperation.NodeEnd())
      {
        std::string sResponseName = StripPrefix(itNodeOutput->Attribute("message").AsString());

        rise::xml::CXMLNode::TXMLNodeConstIterator itMessage =
          rDefs.FindNodeMatch("message", rise::xml::SXMLAttribute("name", sResponseName));

        RISE_ASSERTES(itMessage != rDefs.NodeEnd(), rise::CLogicNoItemException,
          "Can't find message definition(output) for: " + sResponseName);

        ParseResponse(rMember, *itMessage, rWsdlTypes);
        bHasOutput = true;
      }
      else
      { // one way message
        rMember.stReturn.stDataType.sName = "void";
      }

      if (bHasInput && !bHasOutput)
      {
        rMember.mOptions["mep"] = "in-only";
      }

      ReadDoc(rOperation, rMember.sDescr, rMember.sDetail);
    }

    class FindNodeBinding
    {
    public:
      FindNodeBinding(const std::string& sTypeName):
        m_sTypeName(StripPrefix(sTypeName))
      {}

      bool operator()(const rise::xml::CXMLNode& rFindNode)
      {
        if (rFindNode.NodeName() != "binding")
        {
          return false;
        }

        rise::xml::CXMLNode::TXMLAttrConstIterator itType = rFindNode.FindAttribute("type");
        if (itType == rFindNode.AttrEnd())
        {
          return false;
        }

        // filter soap transport
        rise::xml::CXMLNode::TXMLNodeConstIterator itBindingTransport = rFindNode.FindSubnode("binding");
        if (itBindingTransport == rFindNode.NodeEnd())
        {
          return false;
        }

        rise::xml::CXMLNode::TXMLAttrConstIterator itAttrTransport = itBindingTransport->FindAttribute("transport");
        if (itAttrTransport == itBindingTransport->AttrEnd())
        {
          return false;
        }

        if (itAttrTransport->sAttrValue != "http://schemas.xmlsoap.org/soap/http")
        { // not a soap over http transport
          return false;
        }

        // checking soap version, must be 1.1
        const std::string& sTransportUri = FindNamespaceUri(*itBindingTransport, itBindingTransport->Namespace());
        if (sTransportUri.empty())
        {
          std::string sName = "<noname>";
          rise::xml::CXMLNode::TXMLAttrConstIterator itBindingName = rFindNode.FindAttribute("name");
          if (itBindingName == rFindNode.AttrEnd())
          {
            sName = itBindingName->sAttrValue.AsString();
          }

          rise::LogWarning() << "can't find namespace declaration for binding with name: " << sName;
          return false;
        }

        if (sTransportUri != "http://schemas.xmlsoap.org/wsdl/soap/")
        { // not a soap 1.1
          return false;
        }

        return StripPrefix(itType->sAttrValue.AsString()) == m_sTypeName;
      }

    private:
      std::string m_sTypeName;
    };

    void ParseSoapAction(const std::string& sPortTypeName, const std::string& sOperationName, const rise::xml::CXMLNode& rDefs, std::string& sSoapAction)
    {
      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeBinding =
        rDefs.FindNodeIf(FindNodeBinding(sPortTypeName));

      if (itNodeBinding != rDefs.NodeEnd())
      {
        rise::xml::CXMLNode::TXMLNodeConstIterator itNodeOperationName =
          itNodeBinding->FindNodeMatch("operation", rise::xml::SXMLAttribute("name", sOperationName));

        if (itNodeOperationName != itNodeBinding->NodeEnd())
        {
          rise::xml::CXMLNode::TXMLNodeConstIterator itNodeOperation =
            itNodeOperationName->FindSubnode("operation");

          if (itNodeOperation != itNodeOperationName->NodeEnd())
          {
            rise::xml::CXMLNode::TXMLAttrConstIterator itAttrSoapAction =
              itNodeOperation->FindAttribute("soapAction");
            if (itAttrSoapAction != itNodeOperation->AttrEnd())
            {
              sSoapAction = itAttrSoapAction->sAttrValue.AsString();
            }
          }
        }
      }
    }

    void ParseService(Interface& rInterface, const rise::xml::CXMLNode& rDefs, const WsdlTypes& rWsdlTypes,
                      const std::string& sTargetNamespace)
    {
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeService = rDefs.FindSubnode("service");
            itNodeService != rDefs.NodeEnd(); itNodeService = rDefs.FindSubnode("service", ++itNodeService))
      {
        rInterface.lsClasses.push_back(Class());
        Class& rClass = rInterface.lsClasses.back();
        const rise::xml::CXMLNode& rService = *itNodeService;
        rClass.sName = rService.Attribute("name").AsString();
        rClass.mOptions["targetNamespace"] = sTargetNamespace;


        rise::xml::CXMLNode::TXMLNodeConstIterator itNodePort = rService.FindSubnode("port");
        if (itNodePort != rService.NodeEnd())
        {
          rise::xml::CXMLNode::TXMLNodeConstIterator itNodeAddress = itNodePort->FindSubnode("address");
          if (itNodeAddress != itNodePort->NodeEnd())
          {
            rClass.mOptions["serviceUri"] = itNodeAddress->Attribute("location").AsString();
          }

          const std::string& sBindingName = StripPrefix(itNodePort->Attribute("binding").AsString());

          rise::xml::CXMLNode::TXMLNodeConstIterator itBinding =
              rDefs.FindNodeMatch("binding", rise::xml::SXMLAttribute("name", sBindingName));
          RISE_ASSERTS(itBinding != rDefs.NodeEnd(), "Can't find service binding: " + sBindingName);

          const std::string& sPortTypeName = StripPrefix(itBinding->Attribute("type").AsString());

          rise::xml::CXMLNode::TXMLNodeConstIterator itPortType =
              rDefs.FindNodeMatch("portType", rise::xml::SXMLAttribute("name", sPortTypeName));

          RISE_ASSERTS(itPortType != rDefs.NodeEnd(), "Can't find portType: " + sPortTypeName);

          const rise::xml::CXMLNode& rPortType = *itPortType;

          for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeOp = rPortType.NodeBegin();
            itNodeOp != rPortType.NodeEnd(); ++itNodeOp)
          {
            if (itNodeOp->NodeType() == rise::xml::CXMLNode::ENTGENERIC &&
              itNodeOp->NodeName() == "operation")
            {
              Member tOperationMember;
              ParseOperation(tOperationMember, *itNodeOp, rDefs, rWsdlTypes);
              std::string sSoapAction;
              ParseSoapAction(sPortTypeName, tOperationMember.sName, rDefs, sSoapAction);
              if (!sSoapAction.empty())
              {
                tOperationMember.mOptions["soapAction"] = sSoapAction;
              }
              rClass.lsMembers.push_back(tOperationMember);
            }
          }

        }

        ReadDoc(rService, rClass.sDescr, rClass.sDetail);

        rClass.sNamespace = TnsToCppNs(GetTns(rDefs));
      }
    }

//    void WriteInlineTypes(const Element& rElement, const WsdlTypes& rWsdlTypes)
//    {
//      for (std::list<ComplexType>::const_iterator itComplex = rElement.lsComplexTypes.begin();
//        itComplex != rElement.lsComplexTypes.end(); ++itComplex)
//      {
//        for (std::list<Element>::const_iterator itElement = itComplex->lsElements.begin();
//          itElement != itComplex->lsElements.end(); ++itElement)
//        {
//          SDataType stDt;
//          ElementToData(*itElement, stDt, rWsdlTypes);
//          WriteInlineTypes(*itElement, rWsdlTypes);
//        }
//      }
//    }

//    static bool SortStructByParent(const Struct& rStruct1, const Struct& rStruct2)
//    {
//      return rStruct1.sName == rStruct2.sParentName;
//    }

    void ParseInterface(rise::xml::CXMLNode& rRootNode, Project& rProject)
    {
      bool bSchema = rRootNode.NodeName() == "schema";
      RISE_ASSERTES(bSchema || rRootNode.NodeName() == "definitions", rise::CLogicFormatException,
        "Invalid wsdl/xsd root node name: [" + rRootNode.NodeName() + "]");

      if (!bSchema)
      {
        m_stWsdlTypes.Parse(rRootNode, rProject, m_stInterface);
      }
      else
      {
        // import all before parsing schema
        // import xsd in definitions
        m_stWsdlTypes.ImportAll(rRootNode, rProject, m_stInterface);
        m_stWsdlTypes.ParseSchema(rRootNode);
      }

      for (ComplexTypeMap::const_iterator itComplex = m_stWsdlTypes.mComplexTypes.begin();
            itComplex != m_stWsdlTypes.mComplexTypes.end(); ++itComplex)
      {
        if (!itComplex->second.bIsMessagePart)
        {
          ComplexTypeToData(itComplex->second, m_stWsdlTypes);
        }
      }

      for (SimpleTypeMap::const_iterator itSimple = m_stWsdlTypes.mSimpleTypes.begin();
        itSimple != m_stWsdlTypes.mSimpleTypes.end(); ++itSimple)
      {
        SimpleTypeToData(itSimple->second, m_stWsdlTypes);
      }

//      for (TElementMap::const_iterator itElement = m_stWsdlTypes.mElements.begin();
//        itElement != m_stWsdlTypes.mElements.end(); ++itElement)
//      {
//        WriteInlineTypes(itElement->second, m_stWsdlTypes);
//      }

//      m_stInterface.lsStructs.sort(SortStructByParent);

      if (!bSchema)
      {
        ParseService(m_stInterface, rRootNode, m_stWsdlTypes, m_stInterface.sTargetNs);
      }
    }

    Interface& Parse( const std::string& sFileUri, Project& rProject )
    {
      std::string::size_type nPos = sFileUri.find_last_of("/\\");
      bool bRemote = sFileUri.find("://") != std::string::npos;
      const std::string& sInterfaceFileName = (nPos != std::string::npos) ?
                                              sFileUri.substr(nPos + 1) : sFileUri;
      const std::string& sInterfaceFilePath = (nPos != std::string::npos && !bRemote) ?
                                              sFileUri.substr(0, nPos + 1) : "";

      for (std::list<Interface>::iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        if (itInterface->sFileName == sInterfaceFileName &&
            itInterface->sFilePath == sInterfaceFilePath)
        {
          return *itInterface; // already parsed
        }
      }

      // parse new interface
      {
        rise::xml::CXMLDocument tWsdlDoc;
        rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();

        if (bRemote)
        {
          std::string sFileData;
          rise::LogDebug() << "Downloading the " << sFileUri;
          if (!HttpClient::Get(sFileUri, sFileData))
          {
            rise::LogError() << "Can't download file " << sFileUri;
          }
          std::istringstream issData(sFileData);
          tWsdlDoc.LoadFromStream(issData);
        }
        else
        {
          tWsdlDoc.LoadFromFile(sFileUri);
        }

        rise::xml::CXMLNode::TXMLAttrConstIterator itTns = rDefs.FindAttribute("targetNamespace");
        if (itTns != rDefs.AttrEnd())
        {
          m_stInterface.sTargetNs = itTns->sAttrValue.AsString();
        }
        else
        {
          m_stInterface.sTargetNs = sFileUri;
          if (m_stInterface.sTargetNs.find(':') == std::string::npos)
          {
            m_stInterface.sTargetNs = "http://tempui.org/" + m_stInterface.sTargetNs;
          }
          rise::LogDebug() << "Generating tns: for " << sFileUri << " [" << m_stInterface.sTargetNs << "]";
          rDefs.AddAttribute("targetNamespace", m_stInterface.sTargetNs);
        }

        // fill in interface name
        m_stInterface.sFileName = sInterfaceFileName;
        m_stInterface.sFilePath = sInterfaceFilePath;
        m_stInterface.sNamespace = TnsToCppNs(m_stInterface.sTargetNs);
        m_stInterface.sName = sInterfaceFileName;
        nPos = m_stInterface.sName.find_last_of('.');
        if (nPos != std::string::npos)
        {
          m_stInterface.sName.erase(nPos);
        }

        // strip namespace and version number
        while ((nPos = m_stInterface.sName.find_last_of(".-")) != std::string::npos)
        {
          const std::string& sLastToken = m_stInterface.sName.substr(nPos + 1);
          if (sLastToken.find_first_not_of("0123456789") == std::string::npos)
          { // version number
            m_stInterface.sName.erase(nPos);
          }
          else
          {
            if (m_stInterface.sName[nPos] == '.')
            {
              m_stInterface.sName.erase(0, nPos + 1);
            }
            break;
          }
        }

        rProject.lsInterfaces.push_back(m_stInterface);
        Interface& rProjectThisInterface = rProject.lsInterfaces.back();

        ParseInterface(rDefs, rProject);

        rProjectThisInterface = m_stInterface;

        // put namespaces into cache
        NamespacesCache::mCache[sFileUri] = rDefs.GetNsList();
      }

      return m_stInterface;
    }

    const WsdlTypes& GetWsdlTypes()
    {
      return m_stWsdlTypes;
    }

    const ParseSettings& GetParseSettings() const
    {
      return m_rParseSettings;
    }

  private:
    void DataTypeFromName(const std::string& sDataType, DataType& rDataType, const WsdlTypes& rWsdlTypes)
    {
      // struct/typedef
      SimpleTypeMap::const_iterator itSimpleType = rWsdlTypes.mSimpleTypes.find(sDataType);
      if (itSimpleType != rWsdlTypes.mSimpleTypes.end())
      {
        rDataType = SimpleTypeToData(itSimpleType->second, rWsdlTypes);
      }
      else
      {
        ComplexTypeMap::const_iterator itComplexType = rWsdlTypes.mComplexTypes.find(sDataType);
        if (itComplexType != rWsdlTypes.mComplexTypes.end())
        {
          rDataType = ComplexTypeToData(itComplexType->second, rWsdlTypes);
        }
        else
        {
          rDataType.eType = DataType::TypeUnknown;
        }
      }
    }

    DataType SimpleTypeToData(const SimpleType& rSimpleType, const WsdlTypes& rWsdlTypes)
    {
      DataType stDataType;

      // write enum
      if (!rSimpleType.lsEnumValues.empty() && rSimpleType.stBaseType.sName == "string")
      {
        Enum stEnum;

        stEnum.sName = rSimpleType.sName;
        if (stEnum.sName.empty())
        {
          static int nUnnamedEnumId = 0;
          stEnum.sName = "UnnamedEnum" + rise::ToStr(nUnnamedEnumId++);
        }
        stEnum.sNamespace = TnsToCppNs(rSimpleType.sNamespace);
        rise::StrReplace(stEnum.sName, ".", "::", true);

        stDataType.eType = DataType::TypeEnum;
        stDataType.sName = stEnum.sName;
        stDataType.sNamespace = stEnum.sNamespace;

        const BaseType* pEnum = GetBaseType(stEnum.sNamespace + stEnum.sName, m_stInterface, BaseType::TypeEnum);
        if (!pEnum)
        {
          stEnum.bForward = false;
          stEnum.sDescr = rSimpleType.sDescr;
          stEnum.sDetail = rSimpleType.sDetail;
          stEnum.mOptions["baseType"] = rSimpleType.stBaseType.sName;

          for (StringList::const_iterator itValue = rSimpleType.lsEnumValues.begin();
            itValue != rSimpleType.lsEnumValues.end(); ++itValue)
          {
            stEnum.lsMembers.push_back(Enum::EnumMember());
            Enum::EnumMember& rMember = stEnum.lsMembers.back();
            rMember.sName = *itValue;
            if (FixId(rMember.sName))
            {
              rMember.sValue = *itValue;
            }
          }

          std::string::size_type nPos = stEnum.sName.rfind("::");
          if (nPos != std::string::npos)
          {
            const std::string& sOwnerName = stEnum.sName.substr(0, nPos);
            // has a owner
            Struct* pstOwner = const_cast<Struct*>(GetStruct(sOwnerName, m_stInterface));
            if (pstOwner)
            {
              stEnum.sName.erase(0, nPos + 2);
              stEnum.sOwnerName = sOwnerName;
              pstOwner->lsEnums.push_back(stEnum);
            }
            else
            {
              rise::LogError() << "Can't find owner struct: " << stEnum.sName.substr(0, nPos);
            }
          }
          else
          {
            m_stInterface.lsEnums.push_back(stEnum);
          }
        }
      }
      // write typedef
      else
      {
        std::list<Typedef>::const_iterator itTypedef = m_stInterface.lsTypedefs.begin();
        for (; itTypedef != m_stInterface.lsTypedefs.end(); ++itTypedef)
        {
          if (itTypedef->sName == rSimpleType.sName)
          {
            break;
          }
        }

        if (itTypedef != m_stInterface.lsTypedefs.end())
        {
          stDataType.eType = DataType::TypeTypedef;
          stDataType.sName = itTypedef->sName;
          stDataType.sNamespace = itTypedef->sNamespace;
        }
        else
        if (!rSimpleType.sName.empty())
        {
          Typedef stTypedef;
          stTypedef.sName = rSimpleType.sName;
          stTypedef.sNamespace = TnsToCppNs(rSimpleType.sNamespace);
          stTypedef.sDescr = rSimpleType.sDescr;
          stTypedef.sDetail = rSimpleType.sDetail;

          m_stInterface.lsTypedefs.push_back(stTypedef);

          Typedef& rstTypedef = m_stInterface.lsTypedefs.back();

          std::string sEnumValues;
          for (StringList::const_iterator itValue = rSimpleType.lsEnumValues.begin();
            itValue != rSimpleType.lsEnumValues.end(); ++itValue)
          {
            if (!sEnumValues.empty())
            {
              sEnumValues += ",";
            }
            sEnumValues += *itValue;
          }
          if (!sEnumValues.empty())
          {
            rstTypedef.mOptions["enumValues"] = sEnumValues;
          }

          DataType stTypedefDataType;
          GetCppType(rSimpleType.stBaseType, stTypedefDataType);
          if (stTypedefDataType.eType == DataType::TypeUnknown)
          {
            DataTypeFromName(rSimpleType.stBaseType.sName, stTypedefDataType, rWsdlTypes);
          }
          rstTypedef.stDataType = stTypedefDataType;

          stDataType.eType = DataType::TypeTypedef;
          stDataType.sName = rstTypedef.sName;
          stDataType.sNamespace = rstTypedef.sNamespace;

          if (!rSimpleType.lsRestrictions.empty())
          {
            for (std::list< SimpleType::StringPair >::const_iterator itRestr = rSimpleType.lsRestrictions.begin();
                 itRestr != rSimpleType.lsRestrictions.end(); ++itRestr)
            {
              rstTypedef.mOptions["restriction-" + itRestr->first] = itRestr->second;
            }
          }
        }
        else
        {
          GetCppType(rSimpleType.stBaseType, stDataType);
        }
      }

      return stDataType;
    }

    void WriteAttributesToStruct(const std::list<Attribute>& rlsAttrs, Struct& rstStruct,
                                 const std::string& sGroupName = "")
    {
      for (std::list<Attribute>::const_iterator itAttr = rlsAttrs.begin(); itAttr != rlsAttrs.end(); ++itAttr)
      {
        const Attribute* pAttr = &*itAttr;
        while (pAttr->bIsRef)
        {
          AttributeMap::const_iterator itTargetElem = m_stWsdlTypes.mAttributes.find(pAttr->stType.GetNsName());

          RISE_ASSERTS(itTargetElem != m_stWsdlTypes.mAttributes.end(),
                       "Can't find attribute declaration for: " + pAttr->stType.GetNsName());
          pAttr = &itTargetElem->second;
        }

        Param stMember;
        stMember.mOptions["isAttribute"] = "true";
        if (!sGroupName.empty())
        {
          stMember.mOptions["attributeGroupName"] = sGroupName;
        }
        stMember.sName = pAttr->sName;
        if (pAttr->stType.sName.empty()) // use string as default attribute value type
        {
          stMember.stDataType.sName = "string";
          stMember.stDataType.sNamespace = "std::";
          stMember.stDataType.sUsedName = "std::string";
          stMember.stDataType.eType = DataType::TypeString;
        }
        else
        {
          GetCppType(pAttr->stType, stMember.stDataType);
          stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
          OptimizeCppNs(stMember.stDataType.sUsedName, rstStruct.sNamespace);
        }
        if (!pAttr->sDefault.empty())
        {
          stMember.mOptions["defaultValue"] = pAttr->sDefault;
        }
        if (pAttr->bIsOptional)
        {
          MakeOptional(stMember.stDataType);
        }
        stMember.sDescr = itAttr->sDescr;
        stMember.sDetail = itAttr->sDetail;
        rstStruct.lsMembers.push_back(stMember);
      }
    }

    DataType ComplexTypeToData(const ComplexType& rComplexType, const WsdlTypes& rWsdlTypes,
                                const std::string& sForceParentName = "", const std::string& sForceParentNs = "")
    {
      DataType stDataType;

      GetCppType(rComplexType, stDataType);

      if (stDataType.eType == DataType::TypeTypedef ||
          stDataType.eType == DataType::TypeStruct)
      {
        stDataType.sNodeName = stDataType.sName;
        return stDataType;
      }

      // single type array
      if (rComplexType.lsElements.size() == 1)
      {
        const Element& rElem = rComplexType.lsElements.front();
        if (rElem.bIsArray)
        {
          Typedef stTypedef;
          stTypedef.bExtern = rComplexType.bIsExtern;
          stTypedef.sName = stDataType.sName;
          stTypedef.sNamespace = stDataType.sNamespace;
          stTypedef.sDescr = rComplexType.sDescr;
          stTypedef.sDetail = rComplexType.sDetail;

          ElementToData(rElem, stTypedef.stDataType, rWsdlTypes);

          if (!rElem.sName.empty())
          {
            stTypedef.mOptions["elementName"] = rElem.sName;
          }

          m_stInterface.lsTypedefs.push_back(stTypedef);

          DataType stResDataType;
          stResDataType.eType = DataType::TypeTypedef;
          stResDataType.sName = stTypedef.sName;
          stResDataType.sNamespace = stTypedef.sNamespace;

          return stResDataType;
        }
      }

      if (rComplexType.bIsSimpleContent)
      {
        Typedef stTypedef;
        stTypedef.bExtern = rComplexType.bIsExtern;
        stTypedef.sName = stDataType.sName;
        stTypedef.sNamespace = stDataType.sNamespace;
        stTypedef.sDescr = rComplexType.sDescr;
        stTypedef.sDetail = rComplexType.sDetail;

        stTypedef.stDataType = stDataType;

        GetCppType(rComplexType.sParentName, rComplexType.sParentNs, stTypedef.stDataType);

        if (stTypedef.stDataType.eType == DataType::TypeUnknown)
        {
          DataTypeFromName(rComplexType.sParentName, stTypedef.stDataType, m_stWsdlTypes);
        }

        m_stInterface.lsTypedefs.push_back(stTypedef);

        DataType stResDataType;
        stResDataType.eType = DataType::TypeTypedef;
        stResDataType.sName = stTypedef.sName;
        stResDataType.sNamespace = stTypedef.sNamespace;

        return stResDataType;
      }

      if (!rComplexType.sName.empty())
      {
        Struct* pstStruct = NULL;

        {
          Struct stStruct;

          stStruct.bExtern = rComplexType.bIsExtern;
          stStruct.sName = stDataType.sName;
          stStruct.sNamespace = stDataType.sNamespace;
          stStruct.sDescr = rComplexType.sDescr;
          stStruct.sDetail = rComplexType.sDetail;

          // inheritance
          if (!sForceParentName.empty())
          {
            stStruct.sParentName = sForceParentName;
          }
          else
          {
            stStruct.sParentName = StripPrefix(rComplexType.sParentName);
          }

          if (!stStruct.sParentName.empty())
          {
            if (!sForceParentNs.empty())
            {
              stStruct.sParentNamespace = sForceParentNs;
            }
            else
            {
              DataType stParentType;
              GetCppType(rComplexType.sParentName, rComplexType.sParentNs, stParentType);

              stStruct.sParentNamespace = stParentType.sNamespace;
              stStruct.sParentName = stStruct.sParentNamespace + stParentType.sName;
              OptimizeCppNs(stStruct.sParentName, m_stInterface.sNamespace);
            }
          }

          stDataType.eType = DataType::TypeStruct;

          std::string::size_type nPos = stStruct.sName.rfind("::");
          if (nPos != std::string::npos)
          {
            const std::string& sOwnerName = stStruct.sName.substr(0, nPos);
            // has a owner
            Struct* pstOwner = const_cast<Struct*>(GetStruct(sOwnerName, m_stInterface));
            if (pstOwner)
            {
              stStruct.sName.erase(0, nPos + 2);
              stStruct.sOwnerName = sOwnerName;
              pstOwner->lsStructs.push_back(stStruct);
              pstStruct = &pstOwner->lsStructs.back();
            }
            else
            {
              rise::LogError() << "Can't find owner struct: " << stStruct.sName.substr(0, nPos);
            }
          }
          else
          {
            m_stInterface.lsStructs.push_back(stStruct);
            pstStruct = &m_stInterface.lsStructs.back();
          }
        }

        RISE_ASSERT(pstStruct);

        int nUnnamedElemNo = 0;

        pstStruct->bForward = false;

        for (std::list<Element>::const_iterator itElement = rComplexType.lsElements.begin();
          itElement != rComplexType.lsElements.end(); ++itElement)
        {
          const std::string& sChoiceId = itElement->sChoiceId;
          const Element* pElement = &*itElement;
          while (pElement->bIsRef)
          {
            ElementMap::const_iterator itTargetElem = m_stWsdlTypes.mElements.find(pElement->stType.GetNsName());

            RISE_ASSERTS(itTargetElem != m_stWsdlTypes.mElements.end(),
                         "Can't find element declaration for: " + pElement->stType.GetNsName());
            pElement = &itTargetElem->second;
          }

          Param stMember;
          stMember.sName = StripPrefix(pElement->sName);
          stMember.sDescr = pElement->sDescr;
          stMember.sDetail = pElement->sDetail;

          ElementToData(*pElement, stMember.stDataType, rWsdlTypes);

          if (stMember.sName.empty())
          {
            stMember.sName = "tUnnamed" + rise::ToStr(nUnnamedElemNo++);
            stMember.mOptions["useParentElement"] = "true";
          }

          // optimize template argument type
          if (stMember.stDataType.eType == DataType::TypeTemplate)
          {
            const std::string& sOwnerName =
                pstStruct->sNamespace +
                (pstStruct->sOwnerName.empty() ? "" : (pstStruct->sOwnerName + "::")) +
                pstStruct->sName + "::";

            RISE_ASSERTS(!stMember.stDataType.lsParams.empty(), "type of " + stMember.sName +
                        " is template, but no template arg is defined");
            DataType& rDataType = stMember.stDataType.lsParams.front();

            if (rDataType.eType == DataType::TypeStruct ||
                rDataType.eType == DataType::TypeTypedef ||
                rDataType.eType == DataType::TypeEnum)
            {
              rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
              OptimizeCppNs(rDataType.sUsedName, sOwnerName);
            }
          }
          else
          if (stMember.stDataType.eType == DataType::TypeStruct ||
              stMember.stDataType.eType == DataType::TypeTypedef ||
              stMember.stDataType.eType == DataType::TypeEnum)
          {
            // do not optimize namespace if member name equals data type name
            bool bDoNotOptimizeNs = stMember.stDataType.sName == stMember.sName;
            if (bDoNotOptimizeNs && stMember.stDataType.sNamespace.empty())
            {
              stMember.stDataType.sNamespace = "::";
            }

            const std::string& sOwnerName =
                pstStruct->sNamespace +
                (pstStruct->sOwnerName.empty() ? "" : (pstStruct->sOwnerName + "::")) +
                pstStruct->sName + "::";
            stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;

            if (!bDoNotOptimizeNs)
            {
              OptimizeCppNs(stMember.stDataType.sUsedName, sOwnerName);
            }
          }

          if (!pElement->sDefault.empty())
          {
            stMember.mOptions["defaultValue"] = pElement->sDefault;
          }

          if (!sChoiceId.empty())
          {
            stMember.mOptions["choiceId"] = sChoiceId;
            MakeOptional(stMember.stDataType);
          }

          pstStruct->lsMembers.push_back(stMember);
        }

        if (rComplexType.bHasAnyAttribute)
        {
          Param stMember;
          stMember.sName = "lsAnyAttributes";
          stMember.stDataType.sName = "anyAttribute";
          stMember.stDataType.sNamespace = "staff::";
          stMember.stDataType.sUsedName = "staff::anyAttribute";
          stMember.stDataType.eType = DataType::TypeGeneric;
          pstStruct->lsMembers.push_back(stMember);
        }

        WriteAttributesToStruct(rComplexType.lsAttributes, *pstStruct);

        for (std::list<AttributeGroup>::const_iterator itAttrGroup = rComplexType.lsAttributeGroups.begin();
          itAttrGroup != rComplexType.lsAttributeGroups.end(); ++itAttrGroup)
        {
          const AttributeGroup* pAttrGroup = &*itAttrGroup;
          while (pAttrGroup->bIsRef)
          {
            AttributeGroupMap::const_iterator itTargetElem =
                m_stWsdlTypes.mAttributeGroups.find(pAttrGroup->GetNsName());

            RISE_ASSERTS(itTargetElem != m_stWsdlTypes.mAttributeGroups.end(),
                         "Can't find attribute declaration for: " + pAttrGroup->GetNsName());
            pAttrGroup = &itTargetElem->second;
          }

          WriteAttributesToStruct(pAttrGroup->lsAttributes, *pstStruct, pAttrGroup->sName);
        }

      }

      return stDataType;
    }

    void ElementToData(const Element& rElement, DataType& rDataType, const WsdlTypes& rWsdlTypes,
                       const std::string& sForceParentName = "", const std::string& sForceParentNs = "")
    {
      const Element* pElement = &rElement;

      rDataType.eType = DataType::TypeUnknown;

      while (pElement->bIsRef)
      {
        ElementMap::const_iterator itTargetElem = m_stWsdlTypes.mElements.find(pElement->stType.GetNsName());

        RISE_ASSERTS(itTargetElem != m_stWsdlTypes.mElements.end(),
                     "Can't find element declaration for: " + pElement->stType.GetNsName());
        pElement = &itTargetElem->second;
      }

      if (!pElement->stType.sName.empty())
      {
        // inline complex/simple type declaration
        if (!pElement->lsSimpleTypes.empty())
        {
          for (std::list<SimpleType>::const_iterator itSimpleSubtype = pElement->lsSimpleTypes.begin();
            itSimpleSubtype != pElement->lsSimpleTypes.end(); ++itSimpleSubtype)
          {
            rDataType = SimpleTypeToData(*itSimpleSubtype, rWsdlTypes);
          }
        }
        else
        if (!pElement->lsComplexTypes.empty())
        {
          for (std::list<ComplexType>::const_iterator itComplexSubtype = pElement->lsComplexTypes.begin();
            itComplexSubtype != pElement->lsComplexTypes.end(); ++itComplexSubtype)
          {
            rDataType = ComplexTypeToData(*itComplexSubtype, rWsdlTypes, sForceParentName, sForceParentNs);
          }
        }
        else // reference to type
        {
          ComplexTypeMap::const_iterator itComplexType = m_stWsdlTypes.mComplexTypes.find(pElement->stType.GetNsName());
          if (itComplexType != m_stWsdlTypes.mComplexTypes.end())
          {
            rDataType = ComplexTypeToData(itComplexType->second, m_stWsdlTypes);
          }
          else
          {
            SimpleTypeMap::const_iterator itSimpleType = m_stWsdlTypes.mSimpleTypes.find(pElement->stType.GetNsName());
            if (itSimpleType != m_stWsdlTypes.mSimpleTypes.end())
            {
              rDataType = SimpleTypeToData(itSimpleType->second, m_stWsdlTypes);
            }
          }
        }
      }
      else // element is an inline complex or simple type
      {
        if (!pElement->lsSimpleTypes.empty())
        {
          rDataType = SimpleTypeToData(pElement->lsSimpleTypes.front(), rWsdlTypes);
        }
        else
        if (!pElement->lsComplexTypes.empty())
        {
          rDataType = ComplexTypeToData(pElement->lsComplexTypes.front(), rWsdlTypes,
                                        sForceParentName, sForceParentNs);
        }
        else
        {
          rise::LogWarning() << "Untyped element: " << pElement->sName;
        }
      }

      if (rDataType.eType != DataType::TypeUnknown)
      {
        if (pElement->bIsArray) // wrap in array
        {
          rDataType.sNodeName = pElement->sName;
          rDataType.lsParams.push_back(rDataType);
          rDataType.sName = "list";
          rDataType.sNamespace = "std::";
          rDataType.eType = DataType::TypeTemplate;
          rDataType.sUsedName.erase();
        }
      }
      else
      {
        if (pElement->bIsArray)
        {
          DataType stTempl;
          GetCppType(pElement->stType, stTempl);
          stTempl.sNodeName = pElement->sName;
          stTempl.sUsedName = stTempl.sNamespace + stTempl.sName;
          OptimizeCppNs(stTempl.sUsedName, m_stInterface.sNamespace);

          rDataType.sName = "list";
          rDataType.sNamespace = "std::";
          rDataType.eType = DataType::TypeTemplate;
          rDataType.sNodeName = pElement->sName;
          rDataType.lsParams.push_back(stTempl);
        }
        else
        {
          GetCppType(pElement->stType, rDataType);
          rDataType.sNodeName = pElement->sName;
          rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
          OptimizeCppNs(rDataType.sUsedName, m_stInterface.sNamespace);
        }
      }

      if (pElement->bIsOptional)
      {
        MakeOptional(rDataType);
      }
    }

    void GetCppType(const QName& stQName, DataType& rDataType)
    {
      bool bIsXmlType = stQName.sNamespace == "http://www.w3.org/2001/XMLSchema" ||
                        stQName.sNamespace == "http://schemas.xmlsoap.org/wsdl/";

      rDataType.sNamespace.erase();
      if (bIsXmlType)
      {
        if (stQName.sName == "string")
        {
          rDataType.sName = "string";
          rDataType.sNamespace = "std::";
          rDataType.eType = DataType::TypeString;
        }
        else
        if (stQName.sName == "DataObject")
        {
          rDataType.sName = "CDataObject";
          rDataType.sNamespace = "staff::";
          rDataType.eType = DataType::TypeDataObject;
        }
        else
        {
          if (stQName.sName == "boolean")
          {
            rDataType.sName = "bool";
          }
          else
          if (stQName.sName == "integer")
          {
            rDataType.sName = "int";
          }
          else
          if (stQName.sName == "unsignedLong")
          {
            rDataType.sName = "unsigned long";
          }
          else
          if (stQName.sName == "unsignedInt")
          {
            rDataType.sName = "unsigned int";
          }
          else
          if (stQName.sName == "unsignedShort")
          {
            rDataType.sName = "unsigned short";
          }
          else
          if (stQName.sName == "decimal" ||
            stQName.sName == "duration" ||
            stQName.sName == "dateTime" ||
            stQName.sName == "time" ||
            stQName.sName == "date" ||
            stQName.sName == "gYearMonth" ||
            stQName.sName == "gYear" ||
            stQName.sName == "gMonthDay" ||
            stQName.sName == "gDay" ||
            stQName.sName == "gMonth" ||
            stQName.sName == "hexBinary" ||
            stQName.sName == "base64Binary" ||
            stQName.sName == "anyURI" ||
            stQName.sName == "QName" ||
            stQName.sName == "NOTATION" ||
            stQName.sName == "normalizedString" ||
            stQName.sName == "token" ||
            stQName.sName == "language" ||
            stQName.sName == "IDREFS" ||
            stQName.sName == "ENTITIES" ||
            stQName.sName == "NMTOKEN" ||
            stQName.sName == "NMTOKENS" ||
            stQName.sName == "Name" ||
            stQName.sName == "NCName" ||
            stQName.sName == "ID" ||
            stQName.sName == "IDREF" ||
            stQName.sName == "ENTITY" ||
            stQName.sName == "nonPositiveInteger" ||
            stQName.sName == "negativeInteger" ||
            stQName.sName == "nonNegativeInteger" ||
            stQName.sName == "byte" ||
            stQName.sName == "unsignedByte" ||
            stQName.sName == "positiveInteger" ||
            stQName.sName == "anySimpleType")
          {
            rDataType.sName = stQName.sName;
            rDataType.sNamespace = "staff::";
          }
          else
          {
            rDataType.sName = stQName.sName;
          }
          rDataType.eType = DataType::TypeGeneric;
        }
        rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
      }
      else
      if (stQName.sName == "DataObject") // non xsd:any, may have additional schema
      {
        rDataType.sName = "CDataObject";
        rDataType.sNamespace = "staff::";
        rDataType.eType = DataType::TypeDataObject;
      }
      else
      if (stQName.sName == "anyType") // non xsd:anyType
      {
        rDataType.sName = stQName.sName;
        rDataType.sNamespace = "staff::";
        rDataType.eType = DataType::TypeGeneric;
      }
      else
      { // not an wsdl type, some typedef or struct
        rDataType.sName = stQName.sName;
        rDataType.sNamespace = TnsToCppNs(stQName.sNamespace);

        for (std::list<Typedef>::const_iterator itTypedef = m_stInterface.lsTypedefs.begin();
            itTypedef != m_stInterface.lsTypedefs.end(); ++itTypedef)
        {
          if (itTypedef->sName == rDataType.sName &&
              itTypedef->sNamespace == rDataType.sNamespace)
          {
            rDataType.eType = DataType::TypeTypedef;
            return;
          }
        }

        rise::StrReplace(rDataType.sName, ".", "::", true);

        const Struct* pstStruct = GetStruct(rDataType.sNamespace + rDataType.sName, m_stInterface);
        if (pstStruct)
        {
          rDataType.eType = DataType::TypeStruct;
          return;
        }

        rDataType.eType = DataType::TypeUnknown;
      }
    }

    void GetCppType(const std::string& sPrefixName, const std::string& sNamespace, DataType& rDataType)
    {
      std::string::size_type nPos = sPrefixName.find_last_of(':');
      const std::string& sPrefix = (nPos != std::string::npos) ? sPrefixName.substr(0, nPos) : "";
      const std::string& sName = (nPos != std::string::npos) ? sPrefixName.substr(nPos + 1) : sPrefixName;

      GetCppType(QName(sName, sPrefix, sNamespace), rDataType);
    }

    std::string TnsToCppNs(const std::string& sNamespace)
    {
      if (sNamespace == "http://www.w3.org/2001/XMLSchema")
      {
        return "";
      }

      // TODO: parse "urn:" ns form

      std::string sCppNamespace;

      // skip protocol
      std::string::size_type nPosBegin = sNamespace.find("://");
      if (nPosBegin == std::string::npos)
      {
        return "";
      }

      nPosBegin += 3;
      // skip address
      nPosBegin = sNamespace.find("/", nPosBegin);
      if (nPosBegin == std::string::npos)
      {
        return "";
      }

      std::string::size_type nPosNewBegin = sNamespace.find_last_of("/");
      if (nPosNewBegin == std::string::npos || nPosNewBegin < nPosBegin)
      {
        return "";
      }
      nPosBegin = nPosNewBegin + 1;

      std::string::size_type nPosEnd = sNamespace.find_last_of('.');
      if (nPosEnd == std::string::npos || nPosEnd == (sNamespace.size() - 1) || nPosEnd < nPosBegin)
      {
        return "";
      }

      // check namespace is lowercase and service name begins with uppercase letter
      char chNamespace = sNamespace[nPosBegin];
      if (tolower(chNamespace) != chNamespace)
      {
        return "";
      }

      // exclude service/interface name
      sCppNamespace = sNamespace.substr(nPosBegin, nPosEnd - nPosBegin);

      rise::StrReplace(sCppNamespace, ".", "::", true);

      return m_sRootNamespace + sCppNamespace + "::";
    }

    // optimize param as const ref
    void FixParamDataType(DataType& rDataType)
    {
      if ((rDataType.eType == DataType::TypeString) ||
          (rDataType.eType == DataType::TypeDataObject) ||
          (rDataType.eType == DataType::TypeTemplate) ||
          (rDataType.eType == DataType::TypeTypedef) ||
          (rDataType.eType == DataType::TypeStruct))
      {
        rDataType.bIsConst = true;
        rDataType.bIsRef = true;
      }
    }

  private:
    Interface m_stInterface;
    WsdlTypes m_stWsdlTypes;
    std::string m_sRootNamespace;
    const ParseSettings& m_rParseSettings;
  };

  WsdlTypes::WsdlTypes(WsdlParserImpl* pParser):
    m_pParser(pParser)
  {
  }

  void WsdlTypes::Parse(rise::xml::CXMLNode& rNodeWsdl, Project& rProject, Interface& rInterface)
  {
    // import all before parsing schema
    // import xsd in definitions
    ImportAll(rNodeWsdl, rProject, rInterface);

    // import xsd in types
    for(rise::xml::CXMLNode::TXMLNodeIterator itTypes = rNodeWsdl.FindSubnode("types");
        itTypes != rNodeWsdl.NodeEnd(); itTypes = rNodeWsdl.FindSubnode("types", ++itTypes))
    {
      ImportAll(*itTypes, rProject, rInterface);
      for(rise::xml::CXMLNode::TXMLNodeIterator itNodeChild = itTypes->FindSubnode("schema");
        itNodeChild != itTypes->NodeEnd(); itNodeChild = itTypes->FindSubnode("schema", ++itNodeChild))
      {
        ImportAll(*itNodeChild, rProject, rInterface);
      }
    }

    for(rise::xml::CXMLNode::TXMLNodeIterator itTypes = rNodeWsdl.FindSubnode("types");
        itTypes != rNodeWsdl.NodeEnd(); itTypes = rNodeWsdl.FindSubnode("types", ++itTypes))
    {
      for(rise::xml::CXMLNode::TXMLNodeIterator itNodeChild = itTypes->FindSubnode("schema");
        itNodeChild != itTypes->NodeEnd(); itNodeChild = itTypes->FindSubnode("schema", ++itNodeChild))
      {
        ParseSchema(*itNodeChild);
      }
    }
  }

  void WsdlTypes::ParseSchema(const rise::xml::CXMLNode& rSchema)
  {
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rSchema.NodeBegin();
      itType != rSchema.NodeEnd(); ++itType)
    {
      if (itType->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sType = itType->NodeName();
        if (sType == "element")
        {
          Element stElement;
          stElement.Parse(*itType);
          mElements[stElement.GetNsName()] = stElement;
        }
        else
        if (sType == "complexType")
        {
          ComplexType stComplexType;
          stComplexType.Parse(*itType);
          mComplexTypes[stComplexType.GetNsName()] = stComplexType;
        }
        else
        if (sType == "simpleType")
        {
          SimpleType stSimpleType;
          stSimpleType.Parse(*itType);
          mSimpleTypes[stSimpleType.GetNsName()] = stSimpleType;
        }
        else
        if (sType == "attribute")
        {
          Attribute stAttr;
          stAttr.Parse(*itType);
          mAttributes[stAttr.GetNsName()] = stAttr;
        }
        else
        if (sType == "attributeGroup")
        {
          AttributeGroup stAttrGroup;
          stAttrGroup.Parse(*itType);
          mAttributeGroups[stAttrGroup.GetNsName()] = stAttrGroup;
        }
        else
        if (sType == "group")
        { // already parsed if needed
        }
        else
        if (sType == "import")
        { // already imported
        }
        else
        if (sType == "annotation")
        { // documentation
        }
        else
        {
          rise::LogWarning() << "Unsupported schema node type: " << sType;
        }
      }
    }
  }

  void ReplacePrefix(Element& rElement, const std::string& sNamespace, const std::string& sPrefix)
  {
    rElement.bIsExtern = true;
    if (rElement.sNamespace == sNamespace)
    {
      rElement.sPrefix = sPrefix;
    }

    for (std::list<SimpleType>::iterator itSimpleType = rElement.lsSimpleTypes.begin();
        itSimpleType != rElement.lsSimpleTypes.end(); ++itSimpleType)
    {
      itSimpleType->bIsExtern = true;
      if (itSimpleType->sNamespace == sNamespace)
      {
        itSimpleType->sPrefix = sPrefix;
      }
    }

    for (std::list<ComplexType>::iterator itComplexType = rElement.lsComplexTypes.begin();
        itComplexType != rElement.lsComplexTypes.end(); ++itComplexType)
    {
      ComplexType& rComplexType = *itComplexType;
      rComplexType.bIsExtern = true;

      if (rComplexType.sNamespace == sNamespace)
      {
        rComplexType.sPrefix = sPrefix;
      }

      for (std::list<Element>::iterator itElement = rComplexType.lsElements.begin();
          itElement != rComplexType.lsElements.end(); ++itElement)
      {
        ReplacePrefix(*itElement, sNamespace, sPrefix);
      }
    }
  }

  void ImportEnums(const std::list<Enum>& rlsSrc, std::list<Enum>& rlsDst)
  {
    for (std::list<Enum>::const_iterator itEnum = rlsSrc.begin();
        itEnum != rlsSrc.end(); ++itEnum)
    {
      rlsDst.push_back(*itEnum);
      rlsDst.back().bExtern = true;
    }
  }

  void WsdlTypes::ImportStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst)
  {
    for (std::list<Struct>::const_iterator itStruct = rlsSrc.begin();
        itStruct != rlsSrc.end(); ++itStruct)
    {
      rlsDst.push_back(Struct());
      Struct& rstStruct = rlsDst.back();
      rstStruct.sName = itStruct->sName;
      rstStruct.sNamespace = itStruct->sNamespace;
      rstStruct.sParentName = itStruct->sParentName;
      rstStruct.sDescr = itStruct->sDescr;
      rstStruct.sDetail = itStruct->sDetail;
      rstStruct.bExtern = true;
      rstStruct.sOwnerName = itStruct->sOwnerName;
      ImportEnums(itStruct->lsEnums, rstStruct.lsEnums);
      ImportStruct(itStruct->lsStructs, rstStruct.lsStructs);
    }
  }

  void WsdlTypes::IncludeSchema(rise::xml::CXMLNode& rNodeParent,
                                rise::xml::CXMLNode::TXMLNodeIterator& itNodeInclude)
  {
    std::string sSchemaLocation;
    rise::xml::CXMLNode::TXMLAttrConstIterator itLocation = itNodeInclude->FindAttribute("location");
    if (itLocation != itNodeInclude->AttrEnd())
    {
      sSchemaLocation = itLocation->sAttrValue.AsString();
    }
    else
    {
      itLocation = itNodeInclude->FindAttribute("schemaLocation");
      if (itLocation == itNodeInclude->AttrEnd())
      {
        rise::LogWarning() << "schemaLocation is not in include instruction";
        return;
      }
      sSchemaLocation = itLocation->sAttrValue.AsString();
    }

    rise::xml::CXMLDocument tWsdlDoc;
    rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();

    bool bRemote = sSchemaLocation.find("://") != std::string::npos;
    if (bRemote)
    {
      std::string sFileData;
      rise::LogDebug() << "Downloading the " << sSchemaLocation;
      if (!HttpClient::Get(sSchemaLocation, sFileData))
      {
        rise::LogError() << "Can't download file " << sSchemaLocation;
      }
      std::istringstream issData(sFileData);
      tWsdlDoc.LoadFromStream(issData);
    }
    else
    {
      tWsdlDoc.LoadFromFile(sSchemaLocation);
    }

    IncludeAll(rDefs); // recursive include all

    // replace <include schemaLocation="..." /> to included content
    if (rDefs.NodeName() != "schema")
    {
      rise::LogWarning() << "Invalid root node name for XSD: expected: \"schema\" found \""
                         << rDefs.NodeName() << "\"";
    }

    rise::xml::CXMLNode::TXMLNodeConstIterator itNodeSchema = rDefs.NodeBegin();
    if (itNodeSchema != rDefs.NodeEnd())
    {
      *itNodeInclude = *itNodeSchema;
      ++itNodeSchema;
    }
    for (; itNodeSchema != rDefs.NodeEnd(); ++itNodeSchema)
    {
      rNodeParent.AddSubNode("", itNodeInclude) = *itNodeSchema;
    }
  }

  void WsdlTypes::Import(rise::xml::CXMLNode& rNodeImport, Project& rProject, Interface& rInterface)
  {
    std::string sImportNs;
    rise::xml::CXMLNode::TXMLAttrConstIterator itNamespace = rNodeImport.FindAttribute("namespace");
    if (itNamespace != rNodeImport.AttrEnd())
    {
      sImportNs = itNamespace->sAttrValue.AsString();
    }
    else
    { // import into default namespace
      sImportNs = FindNamespaceUri(rNodeImport, rNodeImport.Namespace());
    }

    std::string sSchemaLocation;
    rise::xml::CXMLNode::TXMLAttrConstIterator itLocation = rNodeImport.FindAttribute("location");
    if (itLocation != rNodeImport.AttrEnd())
    {
      sSchemaLocation = itLocation->sAttrValue.AsString();
    }
    else
    {
      itLocation = rNodeImport.FindAttribute("schemaLocation");
      if (itLocation == rNodeImport.AttrEnd())
      {
        // says to import given namespace into current namespace
        m_setNsAliases.insert(sImportNs);
        return;
      }
      sSchemaLocation = itLocation->sAttrValue.AsString();
    }

    RISE_ASSERTP(m_pParser);

    if (m_setNsAliases.count(sImportNs) != 0)
    { // import into current namespace
      static int nUnnamedNsNum = 0;
      rise::LogDebug2() << "Namespace: " << sImportNs << " is alias for current";

      const std::string& sPrefix = "ns" + rise::ToStr(++nUnnamedNsNum);

      // insert imported namespaces into current definitions
      rise::xml::CXMLNode* pNodeDefinitions = &rNodeImport;
      for (; pNodeDefinitions != NULL; pNodeDefinitions = pNodeDefinitions->GetParent())
      {
        if (pNodeDefinitions->NodeName() == "definitions")
        {
          break;
        }
      }
      RISE_ASSERTS(pNodeDefinitions, "Can't find definitions node");

      rise::xml::CXMLNode::TXMLNsList& rlsNamespaces = pNodeDefinitions->GetNsList();
      rlsNamespaces.push_back(rise::xml::SXMLNamespace(sPrefix, sImportNs));

/*      try
      {
        rise::xml::CXMLDocument tWsdlDoc;
        rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();

        try
        {
          tWsdlDoc.LoadFromFile(sSchemaLocation);
        }
        catch(rise::CException&)
        { // retry with include option
          tWsdlDoc.LoadFromFile(m_pParser->GetParseSettings().sInDir + '/' + sSchemaLocation);
        }

        m_pParser->ParseInterface(rDefs, rProject);
      }
      catch (rise::CException& rException)
      {
        rise::LogWarning() << "Cannot import xsd schema \"" << sSchemaLocation
            << "\": " << rException.GetDescr() << ". Service's interface may be incomplete.";
      }*/
    }

    std::string sImportNsPrefix;
    GetNsPrefix(rNodeImport, sImportNs, sImportNsPrefix);

    rise::LogDebug2() << "Importing: " << sSchemaLocation << " into namespace: " << sImportNs;

    WsdlParserImpl tWsdlParser(m_pParser->GetParseSettings());
    try
    {
      Interface& rNewInterface = tWsdlParser.Parse(m_pParser->GetParseSettings().sInDir
                                                        + sSchemaLocation, rProject);

      // import operations
      const WsdlTypes& rImportedWsdlTypes = tWsdlParser.GetWsdlTypes();
      for (ElementMap::const_iterator itElement = rImportedWsdlTypes.mElements.begin();
          itElement != rImportedWsdlTypes.mElements.end(); ++itElement)
      {
        Element& rElement = mElements[sImportNsPrefix + ":" + itElement->second.sName];
        rElement = itElement->second;
        ReplacePrefix(rElement, sImportNs, sImportNsPrefix);
      }

      // import extern attributes
      const AttributeMap& rmImpAttrs = tWsdlParser.GetWsdlTypes().mAttributes;
      for (AttributeMap::const_iterator itAttr = rmImpAttrs.begin(); itAttr != rmImpAttrs.end(); ++itAttr)
      {
        mAttributes[sImportNsPrefix + ":" + itAttr->second.sName] = itAttr->second;
      }

      ImportEnums(rNewInterface.lsEnums, rInterface.lsEnums);
      ImportStruct(rNewInterface.lsStructs, rInterface.lsStructs);

      // use extern typedefs
      for (std::list<Typedef>::const_iterator itTypedef = rNewInterface.lsTypedefs.begin();
          itTypedef != rNewInterface.lsTypedefs.end(); ++itTypedef)
      {
        Typedef stTypedef = *itTypedef;
        stTypedef.sName = itTypedef->sName;
        stTypedef.sNamespace = itTypedef->sNamespace;
        stTypedef.sDescr = itTypedef->sDescr;
        stTypedef.sDetail= itTypedef->sDetail;
        stTypedef.bExtern = true;
        rInterface.lsTypedefs.push_back(stTypedef);
      }

      Include stInclude;
      stInclude.sInterfaceName = rNewInterface.sName;
      stInclude.sNamespace = rNewInterface.sNamespace;
      stInclude.sFileName = rNewInterface.sFileName;
      stInclude.sFilePath = rNewInterface.sFilePath;
      stInclude.sTargetNs = rNewInterface.sTargetNs;
      rInterface.lsIncludes.push_back(stInclude);

      // insert imported namespaces into current definitions or xsd's schema element
      rise::xml::CXMLNode* pNodeDefinitions = &rNodeImport;
      for (; pNodeDefinitions != NULL; pNodeDefinitions = pNodeDefinitions->GetParent())
      {
        if (pNodeDefinitions->NodeName() == "definitions" ||
            (pNodeDefinitions->NodeName() == "schema" && pNodeDefinitions->GetParent() == NULL))
        {
          break;
        }
      }
      RISE_ASSERTS(pNodeDefinitions, "Can't find definitions node");

      const rise::xml::CXMLNode::TXMLNsList& lsImportedNamespaces = NamespacesCache::mCache[sSchemaLocation];
      rise::xml::CXMLNode::TXMLNsList& lsNamespaces = pNodeDefinitions->GetNsList();
      for (rise::xml::CXMLNode::TXMLNsList::const_iterator itImpNs = lsImportedNamespaces.begin();
          itImpNs != lsImportedNamespaces.end(); ++itImpNs)
      {
        bool bFound = false;
        for (rise::xml::CXMLNode::TXMLNsList::const_iterator itNs = lsNamespaces.begin();
            itNs != lsNamespaces.end(); ++itNs)
        {
          if (itNs->sNs == itImpNs->sNs)
          {
            bFound = true;
            break;
          }
        }

        if (!bFound)
        {
          lsNamespaces.push_back(*itImpNs);
        }
      }

      rise::LogDebug2() << "Importing: " << sSchemaLocation << " is complete";
    }
    catch (rise::CException& rException)
    {
      rise::LogWarning() << "Cannot import xsd schema \"" << sSchemaLocation
          << "\": " << rException.GetDescr() << ". Service's interface may be incomplete.";
    }
  }

  void WsdlTypes::IncludeAll(rise::xml::CXMLNode& rNode)
  {
    for(rise::xml::CXMLNode::TXMLNodeIterator itNodeChild = rNode.FindSubnode("include");
      itNodeChild != rNode.NodeEnd(); itNodeChild = rNode.FindSubnode("include", ++itNodeChild))
    {
      IncludeSchema(rNode, itNodeChild);
    }
  }


  void WsdlTypes::ImportAll(rise::xml::CXMLNode& rNode, Project& rProject, Interface& rInterface)
  {
    IncludeAll(rNode);
    for(rise::xml::CXMLNode::TXMLNodeIterator itNodeChild = rNode.FindSubnode("import");
      itNodeChild != rNode.NodeEnd(); itNodeChild = rNode.FindSubnode("import", ++itNodeChild))
    {
      Import(*itNodeChild, rProject, rInterface);
    }
  }

  // wsdl parser

  WsdlParser::WsdlParser()
  {
  }

  WsdlParser::~WsdlParser()
  {
  }


  const std::string& WsdlParser::GetId()
  {
    return m_sId;
  }

  void WsdlParser::Process(const ParseSettings& rParseSettings, Project& rProject)
  {
    for (StringList::const_iterator itFileName = rParseSettings.lsFiles.begin();
          itFileName != rParseSettings.lsFiles.end(); ++itFileName)
    {
      WsdlParserImpl tWsdlParser(rParseSettings);
      std::string sFileName = rParseSettings.sInDir + *itFileName;

      rise::LogDebug() << "Processing wsdl: " << *itFileName;

      tWsdlParser.Parse(sFileName, rProject);
    }
  }

  const std::string WsdlParser::m_sId = "wsdl";
}
}
