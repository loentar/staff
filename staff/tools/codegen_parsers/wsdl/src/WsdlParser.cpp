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
#include <iostream>
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
    bool bIsMessage;
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
    std::list<Enum::EnumMember> lsEnumMembers;
    std::list< StringPair > lsRestrictions;

    SimpleType();
    SimpleType& Parse(const rise::xml::CXMLNode& rNodeSimpleType);
  };

  //////////////////////////////////////////////////////////////////////////
  struct ComplexType: public QName
  {
    std::list<Element> lsElements;
    std::list<Attribute> lsAttributes;
    std::list<AttributeGroup> lsAttributeGroups;
    std::string sParentName;
    std::string sParentNs;
    std::string sAnyAttrDescr;
    bool bIsExtern;
    bool bIsMessagePart;
    bool bIsSimpleContent;
    bool bHasAnyAttribute;
    bool bIsAbstract;
    bool bIsInlineArray;

    ComplexType();
    ComplexType& Parse(const rise::xml::CXMLNode& rNodeComplexType);

  private:
    unsigned nLastChoiceId;

    void ParseSequence(const rise::xml::CXMLNode& rNodeSequence, const std::string& sChoiceId = "");

    void ParseComplexAttr(const rise::xml::CXMLNode& rNodeAttr);
    void ParseComplexContent(const rise::xml::CXMLNode& rNodeComplexContent, bool bIsSimple);
  };


  //////////////////////////////////////////////////////////////////////////

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

    static std::string sEmptyTns;
    return sEmptyTns;
  }

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

    if (!sPrefix.empty())
    {
      rise::LogWarning() << "Can't find prefix declaration [" << sPrefix
                         << "] for node [" << rNode.GetNodeNsName() << "]";
    }

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

//    RISE_ASSERTS(!sNamespaceUri.empty(), "Can't find target namespace for node: " + rise::ToStr(rNode));
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

  bool ReadDoc(const rise::xml::CXMLNode& rNode, std::string& sDoc, bool bSingleLine = true)
  {
    rise::xml::CXMLNode::TXMLNodeConstIterator itDoc = rNode.FindSubnode("documentation");
    if (itDoc != rNode.NodeEnd())
    {
      sDoc = itDoc->NodeContent().AsString();
    }
    else
    {
      itDoc = rNode.FindSubnode("annotation");
      if (itDoc != rNode.NodeEnd())
      {
        rise::xml::CXMLNode::TXMLNodeConstIterator itDocum = itDoc->FindSubnode("documentation");
        if (itDocum != itDoc->NodeEnd())
        {
          sDoc = itDocum->NodeContent().AsString();
        }
      }
    }

    if (!sDoc.empty())
    {
#ifdef __linux__
      rise::StrReplace(sDoc, "\r", "", true);
#endif
      if (bSingleLine)
      {
        rise::StrReplace(sDoc, "\n", " ", true);
      }
      return true;
    }

    return false;
  }

  bool ReadDoc(const rise::xml::CXMLNode& rNode, std::string& sDescr, std::string& sDetail)
  {
    bool bReaded = ReadDoc(rNode, sDescr, false);

    if (bReaded)
    {
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


  void WrapTypeInTemplate(DataType& rDataType, const std::string& sTemplateName,
                          const std::string& sTemplateNamespace = "staff::", bool bIgnoreWarn = false)
  {
    if (rDataType.sName == sTemplateName)
    {
      // choice < optional >
      if (!bIgnoreWarn)
      {
        std::string sType = rDataType.lsParams.empty()
            ? "<unknown>"
            : rDataType.lsParams.front().sNamespace + rDataType.lsParams.front().sName;
        rise::LogWarning() << "Type " << sType << " already marked as " << sTemplateName;
      }
      return;
    }
    rDataType.lsParams.push_front(rDataType);
    rDataType.lsParams.resize(1);
    rDataType.sName = sTemplateName;
    rDataType.sNamespace = sTemplateNamespace;
    rDataType.eType = DataType::TypeTemplate;
    rDataType.sPrefix.erase();
    rDataType.sOwnerName.erase();
//    rDataType.sUsedName.erase();
  }

  bool IsElementArray(const rise::xml::CXMLNode& rElement)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rElement.FindAttribute("maxOccurs");
    return (itAttr != rElement.AttrEnd()) &&
        (itAttr->sAttrValue.AsString() == "unbounded" || itAttr->sAttrValue.AsInt() > 1);
  }

  template <typename Type>
  const Type* FindQNameType(const std::string& sName, const std::string& sNamespace,
                            const std::list<Type>& rlsTypes)
  {
    for (typename std::list<Type>::const_iterator itType = rlsTypes.begin();
         itType != rlsTypes.end(); ++itType)
    {
      if (itType->sName == sName && itType->sNamespace == sNamespace)
      {
        return &*itType;
      }
    }

    return NULL;
  }

  template <typename Type>
  Type* FindQNameType(const std::string& sName, const std::string& sNamespace,
                      std::list<Type>& rlsTypes)
  {
    for (typename std::list<Type>::iterator itType = rlsTypes.begin();
         itType != rlsTypes.end(); ++itType)
    {
      if (itType->sName == sName && itType->sNamespace == sNamespace)
      {
        return &*itType;
      }
    }

    return NULL;
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
        const std::string& sValue = itAttr->sAttrValue.AsString();
        if (sValue.substr(0, 4) == "xml:")
        {
          const std::string& sName = StripPrefix(sValue);
          std::string sPrefix = rNodeAttr.Namespace();
          if (!sPrefix.empty())
          {
            sPrefix += ':';
          }

          if (sName == "base")
          {
            stType.FromType(rNodeAttr, sPrefix + "anyURI");
          }
          else
          if (sName == "lang")
          {
            stType.FromType(rNodeAttr, sPrefix + "lang");
          }
          else
          if (sName == "space")
          {
            stType.FromType(rNodeAttr, sPrefix + "NCName");
          }
          else
          if (sName == "id")
          {
            stType.FromType(rNodeAttr, sPrefix + "ID");
          }
          else
          {
            rise::LogError() << "Invalid xml ref: [" << sValue << "]";
          }
        }
        else
        {
          bIsRef = true;
          stType.FromType(rNodeAttr, sValue);
        }
      }
    }

    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  Element::Element():
    bIsArray(false), bIsExtern(false), bIsRef(false), bIsOptional(false), bIsNillable(false),
    bIsMessage(false)
  {
  }

  Element& Element::Parse(const rise::xml::CXMLNode& rNodeElement)
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

    bIsArray = IsElementArray(rNodeElement);

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
              const rise::xml::CXMLNode& rNodeComplexType = *itChild;
              lsComplexTypes.push_back(ComplexType());
              ComplexType& rComplexType = lsComplexTypes.back();
              rComplexType.Parse(*itChild);

              if (!rNodeComplexType.IsAttributeExists("name"))
              {
                rComplexType.sName = sName;
                rComplexType.sDescr = sDescr;
                rComplexType.sDetail = sDetail;
              }
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
            const rise::xml::CXMLNode& rSubChild = *itSubChild;
            if (rSubChild.NodeType() == rise::xml::CXMLNode::ENTGENERIC)
            {
              const std::string& sSubChildNodeName = rSubChild.NodeName();

              if (sSubChildNodeName == "enumeration")
              {
                Enum::EnumMember stMember;
                const std::string& sValue = rSubChild.Attribute("value").AsString();
                rise::xml::CXMLNode::TXMLAttrConstIterator itCppEnumAttr = rSubChild.FindAttribute("cpp:enum");
                if (itCppEnumAttr != rSubChild.AttrEnd())
                {
                  stMember.sName = itCppEnumAttr->sAttrValue.AsString();
                  if (FixId(stMember.sName))
                  {
                    rise::LogWarning() << "Cpp enum name was renamed ["
                                       << itCppEnumAttr->sAttrValue.AsString() << "] => ["
                                       << stMember.sName << "]";
                  }
                  stMember.sValue = sValue;
                }
                else
                {
                  stMember.sName = sValue;
                  if (FixId(stMember.sName))
                  {
                    stMember.sValue = sValue;
                  }
                }
                ReadDoc(rSubChild, stMember.sDescr);

                lsEnumMembers.push_back(stMember);
              }
              else
              if (sSubChildNodeName != "annotation" && sSubChildNodeName != "documentation")
              {
                lsRestrictions.push_back(StringPair(sSubChildNodeName,
                                                  rSubChild.Attribute("value").AsString()));
              }
            }
          }
        }
        else
        if (sNodeName == "union")
        { // represent union as string
          lsRestrictions.push_back(StringPair(sNodeName, itChild->Attribute("memberTypes").AsString()));

          stBaseType.sName = "string";
          stBaseType.sNamespace = "http://www.w3.org/2001/XMLSchema";
        }
        else
        if (sNodeName == "list")
        { // represent union as string
          lsRestrictions.push_back(StringPair(sNodeName, itChild->Attribute("itemType").AsString()));

          stBaseType.sName = "string";
          stBaseType.sNamespace = "http://www.w3.org/2001/XMLSchema";
        }
        else
        if (sNodeName != "annotation" && sNodeName != "documentation")
        {
          rise::LogWarning() << "Unsupported [" << sNodeName << "] in simple type";
        }
      }
    }

    return *this;
  }


  //////////////////////////////////////////////////////////////////////////
  ComplexType::ComplexType():
    bIsExtern(false), bIsMessagePart(false), bIsSimpleContent(false),
    bHasAnyAttribute(false), bIsAbstract(false), bIsInlineArray(false), nLastChoiceId(0)
  {
  }

  ComplexType& ComplexType::Parse( const rise::xml::CXMLNode& rNodeComplexType )
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeComplexType.FindAttribute("name");
    if (itAttr != rNodeComplexType.AttrEnd())
    {
      sName = itAttr->sAttrValue.AsString();
    }

    itAttr = rNodeComplexType.FindAttribute("abstract");
    if (itAttr != rNodeComplexType.AttrEnd())
    {
      bIsAbstract = itAttr->sAttrValue.AsString() == "true";
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
          bIsInlineArray = IsElementArray(*itChild);
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
          ReadDoc(*itChild, sAnyAttrDescr);
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

  void ComplexType::ParseSequence(const rise::xml::CXMLNode& rNodeSequence,
                                  const std::string& sChoiceId /*= ""*/)
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
          bIsInlineArray = IsElementArray(*itChild);
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

    bIsSimpleContent = bIsSimple;

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
      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeRestriction =
          rNodeComplexContent.FindSubnode("restriction");

      if (itNodeRestriction != rNodeComplexContent.NodeEnd())
      {
        sParentName = itNodeRestriction->Attribute("base").AsString();

        for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeAttr =
             itNodeRestriction->FindSubnode("attribute");
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
  typedef std::list<Element> ElementList;
  typedef std::list<ComplexType> ComplexTypeList;
  typedef std::list<SimpleType> SimpleTypeList;
  typedef std::list<Attribute> AttributeList;
  typedef std::map<std::string, AttributeGroup> AttributeGroupMap;

  class WsdlParserImpl;

  struct WsdlParsers
  {
    static WsdlParserImpl& Parser(const std::string& sFile);
  };

  struct WsdlTypes
  {
    ElementList lsElements;
    ComplexTypeList lsComplexTypes;
    SimpleTypeList lsSimpleTypes;
    AttributeList lsAttributes;
    AttributeGroupMap mAttributeGroups;
    std::set<std::string> m_setNsAliases;
    std::set<std::string> m_setImports;
    std::set<std::string> m_setIncludes;
    WsdlParserImpl* m_pParser;

    void Init(WsdlParserImpl* pParser);
    void Parse(rise::xml::CXMLNode& rNodeWsdl, Project& rProject, Interface& rInterface);
    void ParseSchema(const rise::xml::CXMLNode& rSchema);
    void ImportStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst);
    void Import(rise::xml::CXMLNode& rNodeImport, Project& rProject, Interface& rInterface, bool bInclude);
    void ImportAll(rise::xml::CXMLNode& rNode, Project& rProject, Interface& rInterface);
  };


  //////////////////////////////////////////////////////////////////////////
  // wsdl parser impl

  class WsdlParserImpl
  {
  public:
    WsdlParserImpl():
      m_pParseSettings(NULL)
    {
      m_sRootNamespace = "::";
    }

    bool IsInit()
    {
      return !!m_pParseSettings;
    }

    void Init(const ParseSettings& rParseSettings)
    {
      m_pParseSettings = &rParseSettings;
      m_stWsdlTypes.Init(this);
      StringMap::const_iterator itRootNs = rParseSettings.mEnv.find("rootns");
      if (itRootNs != rParseSettings.mEnv.end() && !itRootNs->second.empty())
      {
        m_sRootNamespace = "::" + itRootNs->second + "::";
        rise::StrReplace(m_sRootNamespace, ".", "::", true);
      }
    }

    bool ParseTypeAny(const Element& rElement, DataType& rDataType)
    {
      RISE_ASSERTP(m_pParseSettings);
      if (m_pParseSettings->mEnv.count("use_schema_for_any") != 0)
      {
        const Element* pSchemaElem = NULL;
        const std::string& sNamespace = rElement.stType.sNamespace;
        // find DataSource declaration
        for (ElementList::const_iterator itElem = m_stWsdlTypes.lsElements.begin();
          itElem != m_stWsdlTypes.lsElements.end() && !pSchemaElem; ++itElem)
        {
          const Element& rChildElem = *itElem;
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

    // part is an params list or result
    void ParsePart(Member& rMember, const rise::xml::CXMLNode& rPart,
                   WsdlTypes& rWsdlTypes, bool bIsResponse, bool bFault)
    {
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrType = rPart.FindAttribute("element");
      if (itAttrType != rPart.AttrEnd())
      { // reference to element type
        const std::string& sElementPrefixName = itAttrType->sAttrValue.AsString();
        const std::string& sElementName = StripPrefix(sElementPrefixName);
        const std::string& sElementNamespace = FindNamespaceUri(rPart, GetPrefix(sElementPrefixName));

        // finding element of part
        Element* pElement = FindQNameType(sElementName, sElementNamespace, rWsdlTypes.lsElements);
        RISE_ASSERTES(pElement, rise::CLogicNoItemException, "Element " + sElementPrefixName
                      + " is not found, while parsing part");

        if (!bFault)
        {
          DataType stDataType;
          Struct* pstStruct = NULL;
          bool bOptimizeStruct = false;

          pElement->bIsMessage = true;
          ElementToData(*pElement, stDataType, rWsdlTypes);
          FixAbstractDataType(stDataType);

          if (!bIsResponse)
          {
            FixParamDataType(stDataType);
          }
          if (stDataType.eType == DataType::TypeStruct)
          {
            pstStruct = const_cast<Struct*>(GetStruct(stDataType.sNamespace + stDataType.sName,
                                                      m_stInterface));
            RISE_ASSERTS(pstStruct, "Can't find struct declaration: " +
                         stDataType.sNamespace + stDataType.sName);

            // expand element to operation's arguments only in case: inline element without references
            bOptimizeStruct = !pElement->lsComplexTypes.empty() &&
                pstStruct && pstStruct->sParentName.empty() &&
                (pstStruct->mOptions.empty() ||
                  (pstStruct->mOptions.size() == 1 && pstStruct->mOptions.count("hidden")));
          }

          if (bIsResponse) // response
          {
            rMember.mOptions["responseElement"] = pElement->sName;
            if (!!pstStruct && pstStruct->lsMembers.empty())
            {
              rMember.stReturn.stDataType.sName = "void";
              pstStruct->mOptions["hidden"];
              pstStruct->bExtern = true;
            }
            else
            {
              if (bOptimizeStruct)
              {
                const Param& rParam = pstStruct->lsMembers.front();
                // check result element name because it may be renamed
                StringMap::const_iterator itResultElement = rParam.mOptions.find("elementName");
                rMember.mOptions["resultElement"] = itResultElement != rParam.mOptions.end() ?
                    itResultElement->second : rParam.sName;
                rMember.stReturn.stDataType = rParam.stDataType;
              }
              else
              {
                rMember.stReturn.stDataType = stDataType;

                // unhide struct
                if (pstStruct && pstStruct->mOptions.count("hidden"))
                {
                  pstStruct->mOptions.erase("hidden");
                  pstStruct->bExtern = false;
                  pElement->bIsMessage = false;
                }
              }
            }
          }
          else // request
          {
            if (bOptimizeStruct)
            {
              rMember.mOptions["requestElement"] = pElement->sName;
              for (std::list<Param>::const_iterator itParam = pstStruct->lsMembers.begin();
                   itParam != pstStruct->lsMembers.end(); ++itParam)
              {
                rMember.lsParams.push_back(*itParam);
                FixParamDataType(rMember.lsParams.back().stDataType);
              }
            }
            else
            { // simple parameter
              Param stParam;
              stParam.sName = pElement->sName;
              stParam.stDataType = stDataType;
              rMember.lsParams.push_back(stParam);
              rMember.mOptions["inlineRequestElement"] = "true";

              // unhide struct
              if (pstStruct && pstStruct->mOptions.count("hidden"))
              {
                pstStruct->mOptions.erase("hidden");
                pstStruct->bExtern = false;
                pElement->bIsMessage = false;
              }
            }
          }
        }
      }
      else
      if (!bFault)
      { // inline part declaration
        const std::string& sPartName = rPart.Attribute("name").AsString();
        rise::xml::CXMLNode::TXMLAttrConstIterator itType = rPart.FindAttribute("type");
        RISE_ASSERTES(itType != rPart.AttrEnd(), rise::CLogicNoItemException,
          "Unknown part type: " + sPartName);

        const std::string& sType = itType->sAttrValue.AsString();

        Param stParam;
        stParam.sName = StripPrefix(sPartName);

        const std::string& sName = StripPrefix(sType);
        const std::string& sPrefix = GetPrefix(sType);
        const std::string& sNamespace = FindNamespaceUri(rPart, sPrefix);

        GetCppType(QName(sName, sPrefix, sNamespace), stParam.stDataType);
        RISE_ASSERTES(!stParam.stDataType.sName.empty(), rise::CLogicNoItemException, "Unknown part type");

        if (!bIsResponse)
        {
          FixParamDataType(stParam.stDataType);
          rMember.lsParams.push_back(stParam);
        }
        else
        {
          rMember.stReturn = stParam;
        }
      }
    }


    void ParseRequest(Member& rMember, const rise::xml::CXMLNode& rMessage, WsdlTypes& rWsdlTypes)
    {
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodePart = rMessage.FindSubnode("part");
          itNodePart != rMessage.NodeEnd();
          itNodePart = rMessage.FindSubnode("part", ++itNodePart))
      {
        ParsePart(rMember, *itNodePart, rWsdlTypes, false, false);
      }
    }

    void ParseResponse(Member& rMember, const rise::xml::CXMLNode& rMessage, WsdlTypes& rWsdlTypes)
    {
      ParsePart(rMember, rMessage.Subnode("part"), rWsdlTypes, true, false);
    }

    void ParseFault(Member& rMember, const rise::xml::CXMLNode& rMessage, WsdlTypes& rWsdlTypes)
    {
      ParsePart(rMember, rMessage.Subnode("part"), rWsdlTypes, false, true);
    }

    void ParseOperation(Member& rMember, const rise::xml::CXMLNode& rOperation,
                        const rise::xml::CXMLNode& rDefs, WsdlTypes& rWsdlTypes)
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

      // response
      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeFault =
        rOperation.FindSubnode("fault");
      if (itNodeFault != rOperation.NodeEnd())
      {
        std::string sResponseName = StripPrefix(itNodeFault->Attribute("message").AsString());

        rise::xml::CXMLNode::TXMLNodeConstIterator itMessage =
          rDefs.FindNodeMatch("message", rise::xml::SXMLAttribute("name", sResponseName));

        RISE_ASSERTES(itMessage != rDefs.NodeEnd(), rise::CLogicNoItemException,
          "Can't find message definition(output) for: " + sResponseName);

        ParseFault(rMember, *itMessage, rWsdlTypes);
        bHasOutput = true;
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

        rise::xml::CXMLNode::TXMLAttrConstIterator itAttrTransport =
            itBindingTransport->FindAttribute("transport");
        if (itAttrTransport == itBindingTransport->AttrEnd())
        {
          return false;
        }

        if (itAttrTransport->sAttrValue != "http://schemas.xmlsoap.org/soap/http")
        { // not a soap over http transport
          return false;
        }

        // checking soap version, must be 1.1
        const std::string& sTransportUri = FindNamespaceUri(*itBindingTransport,
                                                            itBindingTransport->Namespace());
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

    void ParseSoapAction(const std::string& sPortTypeName, const std::string& sOperationName,
                         const rise::xml::CXMLNode& rDefs, std::string& sSoapAction)
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

    void ParseService(Interface& rInterface, const rise::xml::CXMLNode& rDefs, WsdlTypes& rWsdlTypes)
    {
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeService = rDefs.FindSubnode("service");
            itNodeService != rDefs.NodeEnd(); itNodeService = rDefs.FindSubnode("service", ++itNodeService))
      {
        rInterface.lsClasses.push_back(Class());
        Class& rClass = rInterface.lsClasses.back();
        const rise::xml::CXMLNode& rService = *itNodeService;
        rClass.sName = rService.Attribute("name").AsString();

        StringMap::const_iterator itTns = rInterface.mOptions.find("targetNamespace");
        if (itTns != rInterface.mOptions.end())
        {
          rClass.mOptions["targetNamespace"] = itTns->second;
        }

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

    static bool IsStructUsesOther(const Struct& rStruct1, const Struct& rStruct2)
    {
      bool bResult = false;
      for (std::list<Param>::const_iterator itMember = rStruct1.lsMembers.begin();
           !bResult && itMember != rStruct1.lsMembers.end(); ++itMember)
      {
        const DataType& rDataType = itMember->stDataType;
        bResult = rDataType.eType == DataType::TypeStruct &&
            rDataType.sName == rStruct2.sName &&
            (rDataType.sNamespace == rStruct2.sNamespace ||
              (rStruct2.sNamespace.empty() &&
                (rDataType.sNamespace.empty() || rDataType.sNamespace == "::")));
      }

      for (std::list<Struct>::const_iterator itStruct = rStruct1.lsStructs.begin();
           !bResult && itStruct != rStruct1.lsStructs.end(); ++itStruct)
      {
        bResult = IsStructUsesOther(*itStruct, rStruct2);
      }

      return bResult || rStruct1.sParentName == rStruct2.sName;
    }

    void FixStructOrder(std::list<Struct>& rlsStructs)
    {
      if (!rlsStructs.empty())
      {
        for (int n = rlsStructs.size(); n > 0; --n)
        {
          bool bWasReordered = false;
          for (std::list<Struct>::iterator itStruct1 = rlsStructs.begin();
               itStruct1 != rlsStructs.end(); ++itStruct1)
          {
            std::list<Struct>::iterator itStruct2 = itStruct1;
            ++itStruct2;
            for (; itStruct2 != rlsStructs.end(); ++itStruct2)
            {
              if (!itStruct1->bForward && !itStruct2->bForward)
              {
                if (IsStructUsesOther(*itStruct1, *itStruct2))
                {
                  rise::LogDebug() << "Moving struct [" << itStruct2->sName
                                   << "] before to [" << itStruct1->sName << "]";
                  rlsStructs.splice(itStruct1, rlsStructs, itStruct2++);
                  bWasReordered = true;
                  break;
                }
              }
            }
          }
          if (!bWasReordered)
          {
            return;
          }
        }

        rise::LogWarning() << "Can't reorder structures. Circular dependecies is possible";
      }
    }

    void ParseInterface(rise::xml::CXMLNode& rRootNode, Project& rProject)
    {
      RISE_ASSERTP(m_pParseSettings);
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

      for (SimpleTypeList::const_iterator itSimple = m_stWsdlTypes.lsSimpleTypes.begin();
        itSimple != m_stWsdlTypes.lsSimpleTypes.end(); ++itSimple)
      {
        SimpleTypeToData(*itSimple, m_stWsdlTypes, true);
      }

      for (ComplexTypeList::const_iterator itComplex = m_stWsdlTypes.lsComplexTypes.begin();
            itComplex != m_stWsdlTypes.lsComplexTypes.end(); ++itComplex)
      {
        if (!itComplex->bIsMessagePart)
        {
          ComplexTypeToData(*itComplex, m_stWsdlTypes);
        }
      }

      for (SimpleTypeList::const_iterator itSimple = m_stWsdlTypes.lsSimpleTypes.begin();
        itSimple != m_stWsdlTypes.lsSimpleTypes.end(); ++itSimple)
      {
        SimpleTypeToData(*itSimple, m_stWsdlTypes);
      }

//      for (TElementMap::const_iterator itElement = m_stWsdlTypes.mElements.begin();
//        itElement != m_stWsdlTypes.mElements.end(); ++itElement)
//      {
//        WriteInlineTypes(itElement->second, m_stWsdlTypes);
//      }

//      m_stInterface.lsStructs.sort(SortStructByParent);

      if (!bSchema)
      {
        ParseService(m_stInterface, rRootNode, m_stWsdlTypes);
      }

      DataType stDataTypeTmp;
      for (ElementList::const_iterator itElement = m_stWsdlTypes.lsElements.begin();
        itElement != m_stWsdlTypes.lsElements.end(); ++itElement)
      {
        if (!itElement->bIsExtern)
        {
          ElementToData(*itElement, stDataTypeTmp, m_stWsdlTypes);
        }
      }
    }

    Interface& Parse(const std::string& sFileUri, Project& rProject)
    {
      RISE_ASSERTP(m_pParseSettings);
      std::string::size_type nPos = sFileUri.find_last_of("/\\");
      bool bRemote = sFileUri.find("://") != std::string::npos;
      const std::string& sInterfaceFilePath = (nPos != std::string::npos && !bRemote) ?
                                              sFileUri.substr(0, nPos + 1) : "";
      std::string sInterfaceFileName = (nPos != std::string::npos) ? sFileUri.substr(nPos + 1) : sFileUri;

      nPos = sInterfaceFileName.find('?'); // remove GET parameters from interface file name
      if (nPos != std::string::npos)
      {
        sInterfaceFileName.erase(nPos);
      }

      for (std::list<Interface>::iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        if (itInterface->sFileName == sInterfaceFileName &&
            itInterface->sFilePath == sInterfaceFilePath)
        {
          rise::LogWarning() << "Already parsed";
          return *itInterface; // already parsed
        }
      }

      // parse new interface
      {
        rise::xml::CXMLDocument tWsdlDoc;
        rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();

        if (bRemote)
        {
          std::ifstream ifStream(sInterfaceFileName.c_str());
          if (ifStream.good())
          {
            std::cout << "Using cached [" << sInterfaceFileName << "] for " << sFileUri << std::endl;
            tWsdlDoc.LoadFromStream(ifStream);
            ifStream.close();
          }
          else
          {
            std::string sFileData;
            std::cout << "Downloading the " << sFileUri << std::endl;
            RISE_ASSERTS(HttpClient::Get(sFileUri, sFileData), "Can't download file: " + sFileUri);
            if (!m_pParseSettings->mEnv.count("do_not_save_wsdl"))
            {
              std::cout << "Saving [" << sInterfaceFileName << "]" << std::endl;
              std::ofstream ofStream(sInterfaceFileName.c_str());
              if (!ofStream.good())
              {
                rise::LogWarning() << "Can't save [" << sInterfaceFileName << "]";
              }
              else
              {
                ofStream << sFileData;
                ofStream.close();
              }
            }
            std::istringstream issData(sFileData);
            tWsdlDoc.LoadFromStream(issData);
          }
        }
        else
        {
          tWsdlDoc.LoadFromFile(sFileUri);
        }

        for (rise::xml::CXMLNode::TXMLAttrConstIterator itTns = rDefs.AttrBegin();
             itTns != rDefs.AttrEnd(); ++itTns)
        {
          m_stInterface.mOptions[itTns->sAttrName] = itTns->sAttrValue.AsString();
        }

        // fill in interface name
        m_stInterface.sFileName = sInterfaceFileName;
        m_stInterface.sFilePath = sInterfaceFilePath;
        m_stInterface.sNamespace = TnsToCppNs(StringMapValue(m_stInterface.mOptions, "targetNamespace"));
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
        FixId(m_stInterface.sName);

        rProject.lsInterfaces.push_back(m_stInterface);
        Interface& rProjectThisInterface = rProject.lsInterfaces.back();

        ParseInterface(rDefs, rProject);

        // fix enum member names
        FixEnums(m_stInterface.lsEnums, m_stInterface.lsStructs, &m_stInterface.lsTypedefs);
        FixEnumsInStruct(m_stInterface.lsStructs);

        FixStructInheritance(m_stInterface.lsStructs);

        // fix structures
        FixStructOrder(m_stInterface.lsStructs);

        rProjectThisInterface = m_stInterface;

        // put namespaces into cache
        NamespacesCache::mCache[sFileUri] = rDefs.GetNsList();
      }

      return m_stInterface;
    }

    Interface& GetInterface()
    {
      return m_stInterface;
    }

    const WsdlTypes& GetWsdlTypes()
    {
      return m_stWsdlTypes;
    }

    const ParseSettings& GetParseSettings() const
    {
      RISE_ASSERTP(m_pParseSettings);
      return *m_pParseSettings;
    }

  private:
    void DataTypeFromName(const std::string& sName, const std::string& sNamespace, DataType& rDataType,
                          const WsdlTypes& rWsdlTypes)
    {
      // struct/typedef
      const SimpleType* pSimpleType = FindQNameType(sName, sNamespace, rWsdlTypes.lsSimpleTypes);
      if (!pSimpleType)
      {
        const ComplexType* pComplexType = FindQNameType(sName, sNamespace, rWsdlTypes.lsComplexTypes);
        if (!pComplexType)
        {
          rDataType.eType = DataType::TypeUnknown;
        }
        else
        {
          rDataType = ComplexTypeToData(*pComplexType, rWsdlTypes);
        }
      }
      else
      {
        rDataType = SimpleTypeToData(*pSimpleType, rWsdlTypes);
      }
    }

    DataType SimpleTypeToData(const SimpleType& rSimpleType, const WsdlTypes& rWsdlTypes,
                              bool bWriteAsForward = false)
    {
      DataType stDataType;

      // write enum
      if (!rSimpleType.lsEnumMembers.empty())
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
          stEnum.lsMembers = rSimpleType.lsEnumMembers;

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
              if (!bWriteAsForward)
              {
                rise::LogError() << "Can't find owner struct: " << stEnum.sName.substr(0, nPos);
              }
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
        if (rSimpleType.sName.empty())
        {
          GetCppType(rSimpleType.stBaseType, stDataType);
        }
        else
        {
          std::string sTypedefName = rSimpleType.sName;
          bool bNameFixed = FixId(sTypedefName);

          std::list<Typedef>::iterator itTypedef = m_stInterface.lsTypedefs.begin();
          for (; itTypedef != m_stInterface.lsTypedefs.end(); ++itTypedef)
          {
            if (itTypedef->sName == sTypedefName)
            {
              break;
            }
          }

          if (itTypedef != m_stInterface.lsTypedefs.end() && !itTypedef->bForward)
          {
            stDataType.eType = DataType::TypeTypedef;
            stDataType.sName = itTypedef->sName;
            stDataType.sNamespace = itTypedef->sNamespace;
          }
          else
          {
            Typedef* pstTypedef = NULL;
            if (itTypedef == m_stInterface.lsTypedefs.end())
            {
              Typedef stTypedef;
              stTypedef.sName = sTypedefName;
              if (bNameFixed)
              {
                stTypedef.mOptions["elementName"] = rSimpleType.sName;
              }
              stTypedef.sNamespace = TnsToCppNs(rSimpleType.sNamespace);
              stTypedef.sDescr = rSimpleType.sDescr;
              stTypedef.sDetail = rSimpleType.sDetail;

              m_stInterface.lsTypedefs.insert(m_stInterface.lsTypedefs.end(), stTypedef);

              pstTypedef = &m_stInterface.lsTypedefs.back();
            }
            else
            {
              pstTypedef = &*itTypedef;
            }

            stDataType.eType = DataType::TypeTypedef;
            stDataType.sName = pstTypedef->sName;
            stDataType.sNamespace = pstTypedef->sNamespace;

            if (bWriteAsForward)
            {
              return stDataType;
            }

            DataType stTypedefDataType;
            GetCppType(rSimpleType.stBaseType, stTypedefDataType);
            if (stTypedefDataType.eType == DataType::TypeUnknown)
            {
              DataTypeFromName(rSimpleType.stBaseType.sName, rSimpleType.stBaseType.sNamespace,
                               stTypedefDataType, rWsdlTypes);
            }
            pstTypedef->stDataType = stTypedefDataType;


            if (!rSimpleType.lsRestrictions.empty())
            {
              for (std::list< SimpleType::StringPair >::const_iterator itRestr =
                   rSimpleType.lsRestrictions.begin();
                   itRestr != rSimpleType.lsRestrictions.end(); ++itRestr)
              {
                pstTypedef->mOptions["restriction-" + itRestr->first] = itRestr->second;
              }
            }
          }
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
        bool bIsAttrOptional = pAttr->bIsOptional;
        while (pAttr->bIsRef)
        {
          const Attribute* pAttrTarget = FindQNameType(pAttr->stType.sName, pAttr->stType.sNamespace,
                                                       m_stWsdlTypes.lsAttributes);
          RISE_ASSERTS(pAttrTarget, "Can't find attribute declaration for [" + pAttr->stType.GetNsName() +
                       "] while parsing [" + m_stInterface.sFileName + "]");
          pAttr = pAttrTarget;
        }

        Param stMember;
        stMember.mOptions["isAttribute"] = "true";
        if (!sGroupName.empty())
        {
          stMember.mOptions["attributeGroupName"] = sGroupName;
        }
        stMember.sName = pAttr->sName;
        if (FixId(stMember.sName))
        {
          stMember.mOptions["elementName"] = pAttr->sName;
        }

        if (pAttr->stType.sName.empty()) // use string as default attribute value type
        {
          stMember.stDataType.sName = "string";
          stMember.stDataType.sNamespace = "std::";
//          stMember.stDataType.sUsedName = "std::string";
          stMember.stDataType.eType = DataType::TypeString;
        }
        else
        {
          GetCppType(pAttr->stType, stMember.stDataType);

//          // do not optimize namespace if member name equals data type name
//          bool bDoNotOptimizeNs = stMember.stDataType.sName == stMember.sName;
//          if (bDoNotOptimizeNs && stMember.stDataType.sNamespace.empty())
//          {
//            stMember.stDataType.sNamespace = "::";
//          }

//          stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;

//          if (!bDoNotOptimizeNs)
//          {
//            OptimizeCppNs(stMember.stDataType.sUsedName, rstStruct.sNamespace);
//          }
        }
        if (!pAttr->sDefault.empty())
        {
          stMember.mOptions["defaultValue"] = pAttr->sDefault;
        }
        if (bIsAttrOptional)
        {
          WrapTypeInTemplate(stMember.stDataType, "Optional");
        }
        stMember.sDescr = itAttr->sDescr;
        stMember.sDetail = itAttr->sDetail;
        rstStruct.lsMembers.push_back(stMember);
      }
    }

    bool HasDerivedTypes(const std::string& sComplexTypeName, const ElementList& rElements)
    {
      bool bResult = false;

      for (ElementList::const_iterator itElem = rElements.begin();
           itElem != rElements.end() && !bResult; ++itElem)
      {
        const Element& rElem = *itElem;
        for (SimpleTypeList::const_iterator itType = rElem.lsSimpleTypes.begin();
             itType != rElem.lsSimpleTypes.end(); ++itType)
        {
          const SimpleType& rType = *itType;
          if (rType.stBaseType.sName == sComplexTypeName)
          {
            return true;
          }
        }

        bResult |= HasDerivedTypes(sComplexTypeName, rElem.lsComplexTypes);
      }

      return bResult;
    }

    bool HasDerivedTypes(const std::string& sComplexTypeName, const ComplexTypeList& rTypes)
    {
      bool bResult = false;
      for (ComplexTypeList::const_iterator itType = rTypes.begin();
           itType != rTypes.end(); ++itType)
      {
        const ComplexType& rType = *itType;
        if (rType.sParentName == sComplexTypeName)
        {
          return true;
        }

        bResult |= HasDerivedTypes(sComplexTypeName, rType.lsElements);
      }

      return bResult;
    }

    bool HasDerivedTypes(const std::string& sComplexTypeName, const WsdlTypes& rWsdlTypes)
    {
      if (HasDerivedTypes(sComplexTypeName, rWsdlTypes.lsElements))
      {
        return true;
      }

      if (HasDerivedTypes(sComplexTypeName, rWsdlTypes.lsComplexTypes))
      {
        return true;
      }

      return false;
    }

    void FixAbstractDataType(DataType& rDataType)
    {
      if (rDataType.eType == DataType::TypeStruct)
      {
        std::string sNsName = rDataType.sNamespace;
        if (!rDataType.sOwnerName.empty())
        {
          sNsName += rDataType.sOwnerName + "::";
        }
        sNsName += rDataType.sName;

        const Struct* pstStruct = GetStruct(sNsName, m_stInterface);

        if (!pstStruct)
        {
          rise::LogWarning() << "Can't find struct declaration: " << rDataType.sNamespace
                             << rDataType.sName;
        }
        else
        {
          StringMap::const_iterator itAbstract = pstStruct->mOptions.find("abstract");
          if (itAbstract != pstStruct->mOptions.end())
          {
            WrapTypeInTemplate(rDataType, "Abstract");
          }
        }
      }
    }

    DataType ComplexTypeToData(const ComplexType& rComplexType, const WsdlTypes& rWsdlTypes,
                               Struct* pstOwnerStruct = NULL, bool bIsMessagePart = false)
    {
      const BaseType* pBaseType = NULL;
      DataType stDataType;

      GetCppType(rComplexType, stDataType, &pBaseType);

      if (stDataType.eType == DataType::TypeTypedef)
      {
        return stDataType;
      }

      if (stDataType.eType == DataType::TypeStruct)
      {
        // if complex type is not related to the messagePart
        RISE_ASSERT(pBaseType);
        Struct* pstStruct = const_cast<Struct*>(static_cast<const Struct*>(pBaseType));
        if (!bIsMessagePart && !!pstStruct->mOptions.count("hidden") && pstStruct->bExtern)
        {
          pstStruct->bExtern = false;
          pstStruct->mOptions.erase("hidden");
        }
        return stDataType;
      }

      // single type array without any attributes. optimize to list with given element name
//      if (rComplexType.lsElements.size() == 1 &&
//          rComplexType.lsAttributes.empty() &&
//          !rComplexType.bHasAnyAttribute &&
//          !HasDerivedTypes(rComplexType.sName, rWsdlTypes))
//      {
//        const Element& rElem = rComplexType.lsElements.front();
//        if (rElem.bIsArray)
//        {
//          Typedef stTypedef;
//          stTypedef.bExtern = rComplexType.bIsExtern;
//          stTypedef.sName = stDataType.sName;
//          stTypedef.sNamespace = stDataType.sNamespace;
//          stTypedef.sDescr = rComplexType.sDescr;
//          stTypedef.sDetail = rComplexType.sDetail;

//          ElementToData(rElem, stTypedef.stDataType, rWsdlTypes);

//          if (!rElem.sName.empty())
//          {
//            stTypedef.mOptions["elementName"] = rElem.sName;
//          }

//          m_stInterface.lsTypedefs.push_back(stTypedef);

//          DataType stResDataType;
//          stResDataType.eType = DataType::TypeTypedef;
//          stResDataType.sName = stTypedef.sName;
//          stResDataType.sNamespace = stTypedef.sNamespace;

//          return stResDataType;
//        }
//      }

      if (rComplexType.bIsSimpleContent &&
          rComplexType.lsAttributes.empty() &&
          !rComplexType.bHasAnyAttribute &&
          !HasDerivedTypes(rComplexType.sName, rWsdlTypes))
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
          DataTypeFromName(rComplexType.sParentName, rComplexType.sParentNs, stTypedef.stDataType,
                           m_stWsdlTypes);
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
          if (rComplexType.bIsAbstract)
          {
            stStruct.mOptions["abstract"] = "true";
          }

          if (rComplexType.bIsInlineArray)
          {
            // wrap struct into inline array
            Typedef stTypedef;
            static_cast<BaseType&>(stTypedef) = static_cast<const BaseType&>(stStruct);
            stStruct.sName += "Item";
            stStruct.mOptions["choiceArrayItem"] = "true";

            stTypedef.stDataType.sName = "list";
            stTypedef.stDataType.sNamespace = "std::";
            stTypedef.stDataType.eType = DataType::TypeTemplate;
            stTypedef.mOptions["useParentElement"] = "true";

            DataType stTypedefDataType;
            stTypedefDataType.eType = DataType::TypeTypedef;
            stTypedefDataType.sName = stStruct.sName;
            stTypedefDataType.sNamespace = stStruct.sNamespace;
            stTypedef.stDataType.lsParams.push_back(stTypedefDataType);

            m_stInterface.lsTypedefs.push_back(stTypedef);

            stDataType.eType = DataType::TypeTypedef;
          }
          else
          {
            stDataType.eType = DataType::TypeStruct;
          }

          if (rComplexType.sParentName == rComplexType.sName)
          {
            if (!rComplexType.lsElements.empty())
            {
              rise::LogError() << "Additional Elements found while inheriting from ComplexType with same name."
                                  "Additional Elements will be ignored. While parsing: ["
                               << rComplexType.sName << "] ns: [" << rComplexType.sNamespace << "] \n: ["
                               << rComplexType.sParentName << "] ns: [" << rComplexType.sParentNs << "]";
            }
            return stDataType;
          }

          // inheritance
          stStruct.sParentName = StripPrefix(rComplexType.sParentName);
          if (!stStruct.sParentName.empty())
          {
            DataType stParentType;
            GetCppType(rComplexType.sParentName, rComplexType.sParentNs, stParentType);
            stStruct.sParentNamespace = stParentType.sNamespace;
            stStruct.sParentName = stParentType.sName;
          }

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
            if (pstOwnerStruct)
            {
              stStruct.sOwnerName =
                  pstOwnerStruct->sOwnerName.empty() ?
                  pstOwnerStruct->sName :
                  pstOwnerStruct->sOwnerName + "::" + pstOwnerStruct->sName;
              pstStruct = &*(pstOwnerStruct->lsStructs.insert(pstOwnerStruct->lsStructs.end(), stStruct));
              stDataType.sOwnerName = stStruct.sOwnerName;
              stDataType.sPrefix = "struct";
            }
            else
            {
              m_stInterface.lsStructs.push_back(stStruct);
              pstStruct = &m_stInterface.lsStructs.back();

              if (bIsMessagePart)
              {
                pstStruct->mOptions["hidden"];
                pstStruct->bExtern = true;
              }
            }
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
            Element* pTargetElem = FindQNameType(pElement->stType.sName, pElement->stType.sNamespace,
                                                 m_stWsdlTypes.lsElements);

            RISE_ASSERTS(pTargetElem, "Can't find element declaration for: [" + pElement->stType.GetNsName()
                         + "]. from [" + m_stInterface.sFileName + "]");
            pElement = pTargetElem;
          }

          Param stMember;
          stMember.sName = StripPrefix(pElement->sName);
          stMember.sDescr = pElement->sDescr;
          stMember.sDetail = pElement->sDetail;

          if (FixId(stMember.sName))
          {
            stMember.mOptions["elementName"] = StripPrefix(pElement->sName);
          }

          if (!itElement->bIsRef && itElement->stType.sName.empty())
          { // inline element
            ElementToData(*itElement, stMember.stDataType, rWsdlTypes, pstStruct);
          }
          else
          {
            ElementToData(*itElement, stMember.stDataType, rWsdlTypes);
          }

          if (!pElement->lsComplexTypes.empty())
          {
            const ComplexType& rComplexType = pElement->lsComplexTypes.front();
            if (rComplexType.lsElements.size() == 1)
            {
              const Element& rElem = rComplexType.lsElements.front();
              if (rElem.bIsArray)
              {
                const std::string& sElemName = StripPrefix(rElem.sName);
                if (!sElemName.empty() && sElemName != "Item")
                {
                  stMember.mOptions["elementName"] = sElemName;
                }
              }
            }
          }

          if (itElement->bIsArray)
          {
            stMember.mOptions["useParentElement"] = "true";
          }

          if (stMember.sName.empty())
          {
            stMember.sName = "tUnnamed" + rise::ToStr(nUnnamedElemNo++);
            stMember.mOptions["useParentElement"] = "true";
          }

          // optimize template argument type
//          if (stMember.stDataType.eType == DataType::TypeTemplate)
//          {
//            const std::string& sOwnerName =
//                pstStruct->sNamespace +
//                (pstStruct->sOwnerName.empty() ? "" : (pstStruct->sOwnerName + "::")) +
//                pstStruct->sName + "::";

//            RISE_ASSERTS(!stMember.stDataType.lsParams.empty(), "type of " + stMember.sName +
//                        " is template, but no template arg is defined");
//            DataType& rDataType = stMember.stDataType.lsParams.front();

//            if (rDataType.eType == DataType::TypeStruct ||
//                rDataType.eType == DataType::TypeTypedef ||
//                rDataType.eType == DataType::TypeEnum)
//            {
//              // do not optimize namespace if member name equals data type name
//              bool bDoNotOptimizeNs = rDataType.sName == stMember.sName;
//              if (bDoNotOptimizeNs && rDataType.sNamespace.empty())
//              {
//                rDataType.sNamespace = "::";
//              }

//              rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;

//              if (!bDoNotOptimizeNs)
//              {
//                OptimizeCppNs(rDataType.sUsedName, sOwnerName);
//              }
//            }
//          }
//          else
//          if (stMember.stDataType.eType == DataType::TypeStruct ||
//              stMember.stDataType.eType == DataType::TypeTypedef ||
//              stMember.stDataType.eType == DataType::TypeEnum)
//          {
//            // do not optimize namespace if member name equals data type name
//            bool bDoNotOptimizeNs = stMember.stDataType.sName == stMember.sName;
//            if (bDoNotOptimizeNs && stMember.stDataType.sNamespace.empty())
//            {
//              stMember.stDataType.sNamespace = "::";
//            }

//            const std::string& sOwnerName =
//                pstStruct->sNamespace +
//                (pstStruct->sOwnerName.empty() ? "" : (pstStruct->sOwnerName + "::")) +
//                pstStruct->sName + "::";
//            stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;

//            if (!bDoNotOptimizeNs)
//            {
//              OptimizeCppNs(stMember.stDataType.sUsedName, sOwnerName);
//            }
//          }

          if (!pElement->sDefault.empty())
          {
            stMember.mOptions["defaultValue"] = pElement->sDefault;
          }

          if (!sChoiceId.empty())
          {
            stMember.mOptions["choiceId"] = sChoiceId;
            WrapTypeInTemplate(stMember.stDataType, "Optional", "staff::", true);
          }

          FixAbstractDataType(stMember.stDataType);

          pstStruct->lsMembers.push_back(stMember);
        }

        if (rComplexType.bHasAnyAttribute)
        {
          Param stMember;
          stMember.sName = "lsAnyAttributes";
          stMember.stDataType.sName = "anyAttribute";
          stMember.stDataType.sNamespace = "staff::";
//          stMember.stDataType.sUsedName = "staff::anyAttribute";
          stMember.stDataType.eType = DataType::TypeGeneric;
          stMember.sDescr = rComplexType.sAnyAttrDescr;
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
                         "Can't find attribute group declaration for: [" + pAttrGroup->GetNsName()
                         + "]. from [" + m_stInterface.sFileName + "]");
            pAttrGroup = &itTargetElem->second;
          }

          WriteAttributesToStruct(pAttrGroup->lsAttributes, *pstStruct, pAttrGroup->sName);
        }

      }

      return stDataType;
    }

    void ElementToData(const Element& rElement, DataType& rDataType, const WsdlTypes& rWsdlTypes,
                       Struct* pstOwnerStruct = NULL)
    {
      const Element* pElement = &rElement;
      rDataType.eType = DataType::TypeUnknown;

      while (pElement->bIsRef)
      {
        Element* pTargetElem = FindQNameType(pElement->stType.sName, pElement->stType.sNamespace,
                                             m_stWsdlTypes.lsElements);

        RISE_ASSERTS(pTargetElem, "Can't find element declaration for: [" + pElement->stType.GetNsName()
                     + "]. from [" + m_stInterface.sFileName + "]");
        pElement = pTargetElem;
      }

      bool bIsArray = rElement.bIsArray || pElement->bIsArray;

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
            rDataType = ComplexTypeToData(*itComplexSubtype, rWsdlTypes, pstOwnerStruct, pElement->bIsMessage);
          }
        }
        else // reference to type
        {
          ComplexType* pComplexType =
              FindQNameType(pElement->stType.sName, pElement->stType.sNamespace, m_stWsdlTypes.lsComplexTypes);
          if (!pComplexType)
          {
            const SimpleType* pSimpleType =
                FindQNameType(pElement->stType.sName, pElement->stType.sNamespace, m_stWsdlTypes.lsSimpleTypes);
            if (pSimpleType)
            {
              rDataType = SimpleTypeToData(*pSimpleType, m_stWsdlTypes);
            }
          }
          else
          {
            rDataType = ComplexTypeToData(*pComplexType, m_stWsdlTypes, pstOwnerStruct, pElement->bIsMessage);
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
          rDataType = ComplexTypeToData(pElement->lsComplexTypes.front(), rWsdlTypes, pstOwnerStruct,
                                        pElement->bIsMessage);
        }
        else
        {
//          rise::LogWarning() << "Untyped element: [" << pElement->sName << "]. Using as DataObject ";
          rDataType.sName = "DataObject";
          rDataType.sNamespace = "staff::";
          rDataType.eType = DataType::TypeDataObject;
          rDataType.sPrefix.erase();
          rDataType.sOwnerName.erase();
        }
      }

      if (rDataType.eType == DataType::TypeUnknown)
      {
        GetCppType(pElement->stType, rDataType);
      }

      if (bIsArray) // wrap in array
      {
        WrapTypeInTemplate(rDataType, "list", "std::");
      }

      if (pElement->bIsOptional || rElement.bIsOptional)
      {
        WrapTypeInTemplate(bIsArray ? rDataType.lsParams.front() : rDataType, "Optional");
      }
      else
      if (pElement->bIsNillable || rElement.bIsNillable)
      {
        WrapTypeInTemplate(bIsArray ? rDataType.lsParams.front() : rDataType, "Nillable");
      }
    }

    void GetCppTypeByName(const std::string& sName, DataType& rDataType)
    {
      if (sName == "string")
      {
        rDataType.sName = "string";
        rDataType.sNamespace = "std::";
        rDataType.eType = DataType::TypeString;
      }
      else
      if (sName == "duration" ||
          sName == "dateTime" ||
          sName == "time" ||
          sName == "date" ||
          sName == "gYearMonth" ||
          sName == "gYear" ||
          sName == "gMonthDay" ||
          sName == "gDay" ||
          sName == "gMonth" ||
          sName == "anyURI" ||
          sName == "QName" ||
          sName == "NOTATION" ||
          sName == "normalizedString" ||
          sName == "token" ||
          sName == "language" ||
          sName == "IDREFS" ||
          sName == "ENTITIES" ||
          sName == "NMTOKEN" ||
          sName == "NMTOKENS" ||
          sName == "Name" ||
          sName == "NCName" ||
          sName == "ID" ||
          sName == "IDREF" ||
          sName == "ENTITY" ||
          sName == "anySimpleType" ||
          sName == "anyType")
      {
        rDataType.sName = sName;
        rDataType.sNamespace = "staff::";
        rDataType.eType = DataType::TypeString;
      }
      else
      if (sName == "DataObject")
      {
        rDataType.sName = "DataObject";
        rDataType.sNamespace = "staff::";
        rDataType.eType = DataType::TypeDataObject;
      }
      else
      {
        rDataType.eType = DataType::TypeGeneric;
        if (sName == "boolean")
        {
          rDataType.sName = "bool";
        }
        else
        if (sName == "integer")
        {
          rDataType.sName = "int";
        }
        else
        if (sName == "unsignedLong")
        {
          rDataType.sName = "unsigned long";
        }
        else
        if (sName == "unsignedInt")
        {
          rDataType.sName = "unsigned int";
        }
        else
        if (sName == "unsignedShort")
        {
          rDataType.sName = "unsigned short";
        }
        else
        if (sName == "decimal" ||
            sName == "hexBinary" ||
            sName == "base64Binary" ||
            sName == "nonPositiveInteger" ||
            sName == "negativeInteger" ||
            sName == "nonNegativeInteger" ||
            sName == "byte" ||
            sName == "unsignedByte" ||
            sName == "positiveInteger")
        {
          rDataType.sName = sName;
          rDataType.sNamespace = "staff::";
        }
        else
        {
          rDataType.sName = sName;
        }
      }
    }

    void GetCppType(const QName& stQName, DataType& rDataType, const BaseType** ppBaseType = NULL)
    {
      bool bIsXmlType = stQName.sNamespace == "http://www.w3.org/2001/XMLSchema" ||
                        stQName.sNamespace == "http://schemas.xmlsoap.org/wsdl/";

      rDataType.sNamespace.erase();
      if (bIsXmlType)
      {
        GetCppTypeByName(stQName.sName, rDataType);
      }
      else
      if (stQName.sName == "DataObject") // non xsd:any, may have additional schema
      {
        rDataType.sName = "DataObject";
        rDataType.sNamespace = "staff::";
        rDataType.eType = DataType::TypeDataObject;
      }
      else
      if (stQName.sName == "anyType") // non xsd:anyType
      {
        rDataType.sName = stQName.sName;
        rDataType.sNamespace = "staff::";
        rDataType.eType = DataType::TypeString;
      }
      else
      { // not an wsdl type, some typedef or struct
        rDataType.sName = stQName.sName;
        rDataType.sNamespace = TnsToCppNs(stQName.sNamespace);
        FixId(rDataType.sName);

        rise::StrReplace(rDataType.sName, ".", "::", true);

        rDataType.eType = DataType::TypeUnknown;

        const BaseType* pBaseType = GetBaseType(rDataType.sNamespace + rDataType.sName, m_stInterface);
        if (pBaseType)
        {
          switch (pBaseType->eType)
          {
            case BaseType::TypeStruct:
            {
              rDataType.eType = DataType::TypeStruct;
              break;
            }
            case BaseType::TypeTypedef:
            {
              rDataType.eType = DataType::TypeTypedef;
              break;
            }
            case BaseType::TypeEnum:
            {
              rDataType.eType = DataType::TypeEnum;
              break;
            }
            default:;
          }
        }

        if (ppBaseType)
        {
          *ppBaseType = pBaseType;
        }
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
        return m_sRootNamespace;
      }

      nPosBegin += 3;
      // skip address
      nPosBegin = sNamespace.find("/", nPosBegin);
      if (nPosBegin == std::string::npos)
      {
        return m_sRootNamespace;
      }

      std::string::size_type nPosNewBegin = sNamespace.find_last_of("/");
      if (nPosNewBegin == std::string::npos || nPosNewBegin < nPosBegin)
      {
        return m_sRootNamespace;
      }
      nPosBegin = nPosNewBegin + 1;

      std::string::size_type nPosEnd = sNamespace.find_last_of('.');
      if (nPosEnd == std::string::npos || nPosEnd == (sNamespace.size() - 1) || nPosEnd < nPosBegin)
      {
        return m_sRootNamespace;
      }

      // check namespace is lowercase
      char chNamespace = sNamespace[nPosBegin];
      if (tolower(chNamespace) != chNamespace)
      {
        return m_sRootNamespace;
      }

      const std::string& sServiceName = sNamespace.substr(nPosEnd + 1);
      if (sServiceName == "xsd" || sServiceName == "wsdl")
      {
        return m_sRootNamespace;
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

    void FixEnums(std::list<Enum>& rlsEnums, const std::list<Struct>& rlsStructs,
                  const std::list<Typedef>* plsTypedefs = NULL)
    { // check and fix enums for conflicts
      std::set<std::string> setGlobalEnumValues;
      bool bHasConflicts = false;

      for (std::list<Struct>::const_iterator itStruct = rlsStructs.begin();
           itStruct != rlsStructs.end(); ++itStruct)
      {
        setGlobalEnumValues.insert(itStruct->sName);
      }

      if (plsTypedefs)
      {
        for (std::list<Typedef>::const_iterator itTypedef = plsTypedefs->begin();
             itTypedef != plsTypedefs->end(); ++itTypedef)
        {
          setGlobalEnumValues.insert(itTypedef->sName);
        }
      }

      for (std::list<Enum>::const_iterator itEnum = rlsEnums.begin();
           itEnum != rlsEnums.end() && !bHasConflicts; ++itEnum)
      {
        if (!itEnum->mOptions.count("renamed")) // do not check renamed enums
        {
          for (std::list<Enum::EnumMember>::const_iterator itMember = itEnum->lsMembers.begin();
               itMember != itEnum->lsMembers.end(); ++itMember)
          {
            if (!setGlobalEnumValues.count(itMember->sName))
            {
              setGlobalEnumValues.insert(itMember->sName);
            }
            else
            {
              rise::LogWarning() << "conflicting enum names [" << itMember->sName
                                 << "] (first orccurence) renaming all enums members...";
              bHasConflicts = true;
              break;
            }
          }
        }
      }

      if (bHasConflicts) // rename all enums to form EnumName_EnumMember, updating values
      {
        for (std::list<Enum>::iterator itEnum = rlsEnums.begin(); itEnum != rlsEnums.end(); ++itEnum)
        {
          if (!itEnum->mOptions.count("renamed")) // do not rename enum twice
          {
            itEnum->mOptions["renamed"] = "true";
            for (std::list<Enum::EnumMember>::iterator itMember = itEnum->lsMembers.begin();
                 itMember != itEnum->lsMembers.end(); ++itMember)
            {
              if (itMember->sValue.empty()) // set correct element value
              {
                itMember->sValue = itMember->sName;
              }
              itMember->sName = itEnum->sName + "_" + itMember->sName;
            }
          }
        }
      }
    }

    void FixEnumsInStruct(std::list<Struct>& rlsStructs)
    {
      for (std::list<Struct>::iterator itStruct = rlsStructs.begin(); itStruct != rlsStructs.end(); ++itStruct)
      {
        FixEnums(itStruct->lsEnums, itStruct->lsStructs);
        FixEnumsInStruct(itStruct->lsStructs);
      }
    }

    void FixStructInheritance(std::list<Struct>& rlsStruct)
    {
      for (std::list<Struct>::iterator itStruct = rlsStruct.begin();
           itStruct != rlsStruct.end(); ++itStruct)
      {
        Struct& rstStruct = *itStruct;
        if (!rstStruct.sParentName.empty())
        {
          const std::string& sParentNsName = rstStruct.sParentNamespace + rstStruct.sParentName;
          const Struct* pstParent = GetStruct(sParentNsName, m_stInterface);
          if (!pstParent)
          {
            Param stMember;
            stMember.mOptions["useParentElement"] = "true";
            stMember.sName = "tParent";
            GetCppTypeByName(rstStruct.sParentName, stMember.stDataType);
            stMember.stDataType.sName = rstStruct.sParentName;
            stMember.stDataType.sNamespace = rstStruct.sParentNamespace;
            rstStruct.lsMembers.push_front(stMember);
            rstStruct.sParentName.erase();
            rstStruct.sParentNamespace.erase();
          }
        }
        FixStructInheritance(rstStruct.lsStructs);
      }
    }

  private:
    Interface m_stInterface;
    WsdlTypes m_stWsdlTypes;
    std::string m_sRootNamespace;
    const ParseSettings* m_pParseSettings;
  };

  void WsdlTypes::Init(WsdlParserImpl* pParser)
  {
    m_pParser = pParser;
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
          Element& rElement = *lsElements.insert(lsElements.end(), Element());
          rElement.Parse(*itType);
        }
        else
        if (sType == "complexType")
        {
          ComplexType& rComplexType = *lsComplexTypes.insert(lsComplexTypes.end(), ComplexType());
          rComplexType.Parse(*itType);
        }
        else
        if (sType == "simpleType")
        {
          SimpleType& rSimpleType = *lsSimpleTypes.insert(lsSimpleTypes.end(), SimpleType());
          rSimpleType.Parse(*itType);
        }
        else
        if (sType == "attribute")
        {
          Attribute& rAttr = *lsAttributes.insert(lsAttributes.end(), Attribute());
          rAttr.Parse(*itType);
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
        if (sType == "import" || sType == "include")
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

  void ReplacePrefix(Element& rElement, const std::string& sNamespace, const std::string& sPrefix,
                     bool bInclude)
  {
    rElement.bIsExtern = true;
    if (!bInclude && rElement.sNamespace == sNamespace)
    {
      rElement.sPrefix = sPrefix;
    }

    for (std::list<SimpleType>::iterator itSimpleType = rElement.lsSimpleTypes.begin();
        itSimpleType != rElement.lsSimpleTypes.end(); ++itSimpleType)
    {
      itSimpleType->bIsExtern = true;
      if (!bInclude && itSimpleType->sNamespace == sNamespace)
      {
        itSimpleType->sPrefix = sPrefix;
      }
    }

    for (std::list<ComplexType>::iterator itComplexType = rElement.lsComplexTypes.begin();
        itComplexType != rElement.lsComplexTypes.end(); ++itComplexType)
    {
      ComplexType& rComplexType = *itComplexType;
      rComplexType.bIsExtern = true;

      if (!bInclude && rComplexType.sNamespace == sNamespace)
      {
        rComplexType.sPrefix = sPrefix;
      }

      for (std::list<Element>::iterator itElement = rComplexType.lsElements.begin();
          itElement != rComplexType.lsElements.end(); ++itElement)
      {
        ReplacePrefix(*itElement, sNamespace, sPrefix, bInclude);
      }
    }
  }

  template <typename Type>
  bool IsBaseTypeExists(const BaseType& rBaseType, const std::list<Type>& rlsTypes)
  {
    bool bFound = false;
    for (typename std::list<Type>::const_iterator itType = rlsTypes.begin();
        itType != rlsTypes.end(); ++itType)
    {
      if (itType->sName == rBaseType.sName &&
          itType->sNamespace == rBaseType.sNamespace &&
          itType->sOwnerName == rBaseType.sOwnerName)
      {
        bFound = true;
        break;
      }
    }
    return bFound;
  }

  void ImportEnums(const std::list<Enum>& rlsSrc, std::list<Enum>& rlsDst)
  {
    for (std::list<Enum>::const_iterator itEnum = rlsSrc.begin();
        itEnum != rlsSrc.end(); ++itEnum)
    {
      if (!IsBaseTypeExists(*itEnum, rlsDst))
      {
        rlsDst.push_back(*itEnum);
        rlsDst.back().bExtern = true;
      }
    }
  }

  void WsdlTypes::ImportStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst)
  {
    for (std::list<Struct>::const_iterator itStruct = rlsSrc.begin();
        itStruct != rlsSrc.end(); ++itStruct)
    {
      if (!IsBaseTypeExists(*itStruct, rlsDst))
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
  }

  void WsdlTypes::Import(rise::xml::CXMLNode& rNodeImport, Project& rProject, Interface& rInterface,
                         bool bInclude)
  {
    std::string sImportNs;
    rise::xml::CXMLNode::TXMLAttrConstIterator itNamespace = rNodeImport.FindAttribute("namespace");
    if (itNamespace != rNodeImport.AttrEnd())
    {
      sImportNs = itNamespace->sAttrValue.AsString();
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

    const std::string& sImportHash = sImportNs + ":" + sSchemaLocation;
    if (!bInclude)
    {
      if (m_setImports.count(sImportHash))
      {
        rise::LogDebug() << "[" << sImportHash << "] is already imported";
        return;
      }
      m_setImports.insert(sImportHash);
    }
    else
    {
      if (m_setIncludes.count(sImportHash))
      {
        rise::LogDebug() << "[" << sImportHash << "] is already included";
        return;
      }
      m_setIncludes.insert(sImportHash);
    }

    RISE_ASSERTP(m_pParser);

    if (!bInclude && m_setNsAliases.count(sImportNs) != 0)
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
      RISE_ASSERTS(pNodeDefinitions, "Can't find definitions node in [" + rInterface.sFileName + "]");

      rise::xml::CXMLNode::TXMLNsList& rlsNamespaces = pNodeDefinitions->GetNsList();
      rlsNamespaces.push_back(rise::xml::SXMLNamespace(sPrefix, sImportNs));
    }

    std::string sImportNsPrefix;

    if (bInclude)
    {
      rise::LogDebug2() << "Including [" << sSchemaLocation
                        << "] from: [" << rInterface.sFileName << "]";
    }
    else
    {
      GetNsPrefix(rNodeImport, sImportNs, sImportNsPrefix);
      rise::LogDebug2() << "Importing [" << sSchemaLocation << "] into namespace: [" << sImportNs
                        << "] from: [" << rInterface.sFileName << "]";
    }

    WsdlParserImpl& rWsdlParser = WsdlParsers::Parser(sSchemaLocation);
    Interface* pNewInterface = NULL;

    if (!rWsdlParser.IsInit())
    {
      rWsdlParser.Init(m_pParser->GetParseSettings());
      pNewInterface = &rWsdlParser.Parse(m_pParser->GetParseSettings().sInDir + sSchemaLocation, rProject);
    }
    else
    {
      pNewInterface = &rWsdlParser.GetInterface();
    }

    Include& rInclude = *rInterface.lsIncludes.insert(rInterface.lsIncludes.end(), Include());
    rInclude.sInterfaceName = pNewInterface->sName;
    rInclude.sNamespace = pNewInterface->sNamespace;
    rInclude.sFileName = pNewInterface->sFileName;
    rInclude.sFilePath = pNewInterface->sFilePath;
    rInclude.sTargetNs = StringMapValue(pNewInterface->mOptions, "targetNamespace");

    // import elements
    const WsdlTypes& rImportedWsdlTypes = rWsdlParser.GetWsdlTypes();
    for (ElementList::const_iterator itElement = rImportedWsdlTypes.lsElements.begin();
        itElement != rImportedWsdlTypes.lsElements.end(); ++itElement)
    {
      Element& rElement = *lsElements.insert(lsElements.end(), *itElement);
      ReplacePrefix(rElement, sImportNs, sImportNsPrefix, bInclude);
    }

    // import extern attributes
    const AttributeList& rmImpAttrs = rWsdlParser.GetWsdlTypes().lsAttributes;
    for (AttributeList::const_iterator itAttr = rmImpAttrs.begin(); itAttr != rmImpAttrs.end(); ++itAttr)
    {
      Attribute& rAttr = *lsAttributes.insert(lsAttributes.end(), *itAttr);
      if (!bInclude)
      {
        rAttr.sPrefix = sImportNsPrefix;
      }
    }

    ImportEnums(pNewInterface->lsEnums, rInterface.lsEnums);
    ImportStruct(pNewInterface->lsStructs, rInterface.lsStructs);

    // use extern typedefs
    for (std::list<Typedef>::const_iterator itTypedef = pNewInterface->lsTypedefs.begin();
        itTypedef != pNewInterface->lsTypedefs.end(); ++itTypedef)
    {
      if (!IsBaseTypeExists(*itTypedef, rInterface.lsTypedefs))
      {
        Typedef stTypedef = *itTypedef;
        stTypedef.sName = itTypedef->sName;
        stTypedef.sNamespace = itTypedef->sNamespace;
        stTypedef.sDescr = itTypedef->sDescr;
        stTypedef.sDetail= itTypedef->sDetail;
        stTypedef.bExtern = true;
        rInterface.lsTypedefs.push_back(stTypedef);
      }
    }

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
    RISE_ASSERTS(pNodeDefinitions, "Can't find definitions node in [" + rInterface.sFileName + "]");

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

      if (!bFound && !itImpNs->sNs.empty())
      {
        lsNamespaces.push_back(*itImpNs);
      }
    }

    rise::LogDebug2() << (bInclude ? "Including" : "Importing") << " [" << sSchemaLocation << "] complete";
  }

  void WsdlTypes::ImportAll(rise::xml::CXMLNode& rNode, Project& rProject, Interface& rInterface)
  {
    for (rise::xml::CXMLNode::TXMLNodeIterator itNodeChild = rNode.NodeBegin();
      itNodeChild != rNode.NodeEnd(); ++itNodeChild)
    {
      if (itNodeChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        if (itNodeChild->NodeName() == "import")
        {
          Import(*itNodeChild, rProject, rInterface, false);
        }
        else
        if (itNodeChild->NodeName() == "include")
        {
          Import(*itNodeChild, rProject, rInterface, true);
        }
      }
    }
  }

  WsdlParserImpl& WsdlParsers::Parser(const std::string& sFile)
  {
    static std::map<std::string, WsdlParserImpl> mMap;
    return mMap[sFile];
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
      WsdlParserImpl& rWsdlParser = WsdlParsers::Parser(*itFileName);
      if (!rWsdlParser.IsInit())
      {
        rWsdlParser.Init(rParseSettings);
        std::string sFileName = rParseSettings.sInDir + *itFileName;

        rise::LogDebug() << "Processing: " << *itFileName;

        rWsdlParser.Parse(sFileName, rProject);
      }
      else
      {
        rise::LogDebug() << *itFileName << " already processed";
      }
    }
  }

  const std::string WsdlParser::m_sId = "wsdl";
}
}
