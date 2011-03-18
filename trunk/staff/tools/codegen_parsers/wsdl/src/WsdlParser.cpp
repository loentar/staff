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
#include "WsdlParser.h"

RISE_DECLARE_PLUGIN(staff::codegen::CWsdlParser);

namespace staff
{
namespace codegen
{
  struct NamespacesCache
  {
    typedef std::map<std::string, rise::xml::CXMLNode::TXMLNsList> TNamespacesCacheMap;
    static TNamespacesCacheMap mCache;
  };

  NamespacesCache::TNamespacesCacheMap NamespacesCache::mCache;

  struct SSimpleType;
  struct SComplexType;

  struct SQName
  {
    std::string sName;
    std::string sPrefix;
    std::string sNamespace;
    std::string sDescr;
    std::string sDetail;

    SQName()
    {
    }

    SQName(const std::string& sNameIn, const std::string& sPrefixIn, const std::string& sNamespaceIn):
      sName(sNameIn), sPrefix(sPrefixIn), sNamespace(sNamespaceIn)
    {
    }

    SQName(const std::string& sNameIn, const std::string& sPrefixIn, const std::string& sNamespaceIn,
           const std::string& sDescrIn):
      sName(sNameIn), sPrefix(sPrefixIn), sNamespace(sNamespaceIn), sDescr(sDescrIn)
    {
    }

    std::string GetNsName() const
    {
      return sPrefix.empty() ? sName : (sPrefix + ":" + sName);
    }
  };

  std::ostream& operator<<(std::ostream& rOut, const SQName& rstQName)
  {
    return rOut << rstQName.GetNsName() << " = " << rstQName.sNamespace << " [" << rstQName.sDescr << "]";
  }

  //////////////////////////////////////////////////////////////////////////
  struct SAttribute: public SQName
  {
    SQName stType;
    std::string sValue;
    std::string sDefault;
    bool bIsRef;

    SAttribute();
    SAttribute(const std::string& sNameIn, const std::string& sPrefixIn, const std::string& sNamespaceIn,
           const std::string& sValueIn, const std::string& sDefaultIn, const std::string& sDescrIn = "");

    SAttribute& Parse(const rise::xml::CXMLNode& rNodeAttr);
    void FromType(const rise::xml::CXMLNode& rNodeElement, const std::string& sAttrType);
  };

  //////////////////////////////////////////////////////////////////////////
  struct SElement: public SQName
  {
    SQName stType;
    bool bIsArray;
    bool bIsExtern;
    bool bIsRef;
    std::string sDefault;

    std::list<SSimpleType> lsSimpleTypes;
    std::list<SComplexType> lsComplexTypes;

    SElement();

    void FromType(const rise::xml::CXMLNode& rNodeElement, const std::string& sElemType);
    SElement& Parse(const rise::xml::CXMLNode& rNodeElement);

  private:
    bool GetType(const rise::xml::CXMLNode& rElement, std::string& sCppTypeName);
  };

  //////////////////////////////////////////////////////////////////////////
  struct SSimpleType: public SQName
  {
    bool bIsExtern;
    SQName stBaseType;
    TStringList lsEnumValues;

    SSimpleType();
    SSimpleType& Parse(const rise::xml::CXMLNode& rNodeSimpleType);
  };

  //////////////////////////////////////////////////////////////////////////
  struct SComplexType: public SQName
  {
    std::string sElementName;
    std::list<SElement> lsElements;
    std::list<SAttribute> lsAttributes;
    std::string sParentName;
    std::string sParentNs;
    bool bIsChoice;
    bool bIsExtern;
    bool bIsMessagePart;
    bool bIsSimpleContent;

    SComplexType();

    SComplexType& Parse(const rise::xml::CXMLNode& rNodeComplexType);

  private:
    void ParseSequence(const rise::xml::CXMLNode& rNodeSequence);

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

    rise::LogWarning() << "Can't find prefix declaration [" << sPrefix << "]";
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

    RISE_THROWS(rise::CLogicNoItemException, "Can't find target namespace with nsuri: [" + sNamespaceUri
                + "] prefix: [" + sPrefix + "] for: " + rise::ToStr(rNode));
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

  void ReadDoc(const rise::xml::CXMLNode& rNode, std::string& sDescr, std::string& sDetail)
  {
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




  //////////////////////////////////////////////////////////////////////////
  SAttribute::SAttribute():
     bIsRef(false)
  {
  }

  SAttribute::SAttribute(const std::string& sNameIn, const std::string& sPrefixIn, const std::string& sNamespaceIn,
         const std::string& sValueIn, const std::string& sDefaultIn, const std::string& sDescrIn /*= ""*/):
    SQName(sNameIn, sPrefixIn, sNamespaceIn, sDescrIn),
    sValue(sValueIn), sDefault(sDefaultIn), bIsRef(false)
  {
  }

  SAttribute& SAttribute::Parse(const rise::xml::CXMLNode& rNodeAttr)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeAttr.FindAttribute("name");

    itAttr = rNodeAttr.FindAttribute("name");
    if (itAttr != rNodeAttr.AttrEnd())
    {
      sName = itAttr->sAttrValue.AsString();
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
      FromType(rNodeAttr, itAttr->sAttrValue.AsString());
    }
    else
    { // reference to another type
      itAttr = rNodeAttr.FindAttribute("ref");
      if (itAttr != rNodeAttr.AttrEnd())
      {
        bIsRef = true;
        FromType(rNodeAttr, itAttr->sAttrValue.AsString());
      }
      else
      {
        rise::LogError() << "Unknown attr type: " << sName;
      }
    }

    return *this;
  }

  void SAttribute::FromType(const rise::xml::CXMLNode& rNodeElement, const std::string& sAttrType)
  {
    stType.sName = StripPrefix(sAttrType);
    stType.sPrefix = GetPrefix(sAttrType);
    stType.sNamespace = FindNamespaceUri(rNodeElement, stType.sPrefix);
  }


  //////////////////////////////////////////////////////////////////////////
  SElement::SElement():
    bIsArray(false), bIsExtern(false), bIsRef(false)
  {
  }

  void SElement::FromType(const rise::xml::CXMLNode& rNodeElement, const std::string& sElemType)
  {
    stType.sName = StripPrefix(sElemType);
    stType.sPrefix = GetPrefix(sElemType);
    stType.sNamespace = FindNamespaceUri(rNodeElement, stType.sPrefix);
  }

  SElement& SElement::Parse( const rise::xml::CXMLNode& rNodeElement )
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

    itAttr = rNodeElement.FindAttribute("maxOccurs");
    bIsArray = (itAttr != rNodeElement.AttrEnd()) && (itAttr->sAttrValue.AsString() == "unbounded");

    //  if type exists, element is simple
    itAttr = rNodeElement.FindAttribute("type");
    if (itAttr != rNodeElement.AttrEnd())
    { // element is simple
      FromType(rNodeElement, itAttr->sAttrValue.AsString());
    }
    else
    { // element is complex
      // reference to another type
      itAttr = rNodeElement.FindAttribute("ref");
      if (itAttr != rNodeElement.AttrEnd())
      { // find element
        bIsRef = true;
        FromType(rNodeElement, itAttr->sAttrValue.AsString());
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
              lsComplexTypes.push_back(SComplexType());
              lsComplexTypes.back().sElementName = sName;
              lsComplexTypes.back().Parse(*itChild);
            }
            else
            if (sNodeName == "simpleType")
            {
              lsSimpleTypes.push_back(SSimpleType());
              lsSimpleTypes.back().Parse(*itChild);
            }
          }
        }//for
      }
    }

    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  SSimpleType::SSimpleType():
    bIsExtern(false)
  {
  }

  SSimpleType& SSimpleType::Parse(const rise::xml::CXMLNode& rNodeSimpleType)
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

          // process enumeration
          for (rise::xml::CXMLNode::TXMLNodeConstIterator itSubChild = itChild->NodeBegin();
            itSubChild != itChild->NodeEnd(); ++itSubChild)
          {
            if (itSubChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
            {
              const std::string& sSubNodeName = itSubChild->NodeName();
              if (sSubNodeName == "enumeration")
              {
                lsEnumValues.push_back(itSubChild->Attribute("value").AsString());
              }
            }
          }

        }
      }
    }

    return *this;
  }


  //////////////////////////////////////////////////////////////////////////
  SComplexType::SComplexType():
    bIsChoice(false), bIsExtern(false), bIsMessagePart(false), bIsSimpleContent(false)
  {
  }

  SComplexType& SComplexType::Parse( const rise::xml::CXMLNode& rNodeComplexType )
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
          bIsChoice = true;
          ParseSequence(*itChild);
        }
        else
        if (sNodeName == "group")
        {
          ParseSequence(GetGroupDeclaration(*itChild));
        }
        else
        if (sNodeName == "attribute")
        {
          lsAttributes.push_back(SAttribute());
          lsAttributes.back().Parse(*itChild);
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

  void SComplexType::ParseSequence( const rise::xml::CXMLNode& rNodeSequence )
  {
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = rNodeSequence.NodeBegin();
      itChild != rNodeSequence.NodeEnd(); ++itChild)
    {
      if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sNodeName = itChild->NodeName();
        if (sNodeName == "element")
        {
          SElement stElement;
          stElement.Parse(*itChild);
          if (stElement.stType.sName == "anyType")
          {
            stElement.stType.sName = "DataObject";
          }
          lsElements.push_back(stElement);
        }
        else
        if (sNodeName == "any")
        {
          SElement stElement;
          rise::xml::CXMLNode::TXMLAttrConstIterator itAttrNs = itChild->FindAttribute("namespace");
          stElement.Parse(*itChild);
          stElement.stType.sName = "DataObject";
          if (itAttrNs != itChild->AttrEnd())
          {
            stElement.stType.sNamespace = itAttrNs->sAttrValue.AsString();
          }
          lsElements.push_back(stElement);
        }
        else
        if (sNodeName == "sequence")
        {
          ParseSequence(*itChild);
        }
        else
        if (sNodeName == "choice")
        {
          bIsChoice = true;
          ParseSequence(*itChild);
        }
        else
        if (sNodeName == "group")
        {
          ParseSequence(GetGroupDeclaration(*itChild));
        }
        else
        if (sNodeName != "annotation" && sNodeName != "documentation") // already parsed
        {
          rise::LogWarning() << "Unsupported sequence with \"" << sNodeName << "\"";
        }
      }
    }
  }

  void SComplexType::ParseComplexContent( const rise::xml::CXMLNode& rNodeComplexContent, bool bIsSimple )
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
          rise::xml::CXMLNode::TXMLAttrConstIterator itAttrRef = itNodeAttr->FindAttribute("ref");
          if (itAttrRef != itNodeAttr->AttrEnd())
          {
            std::string sRef = StripPrefix(itAttrRef->sAttrValue.AsString());

            rise::xml::CXMLNode::TXMLAttrConstIterator itAttrRefType = itNodeAttr->AttrBegin();
            for (; itAttrRefType != itNodeAttr->AttrEnd();
                ++itAttrRefType)
            {
              std::string sAttrName = StripPrefix(itAttrRefType->sAttrName);

              if (sAttrName == sRef)
              {
                break;
              }
            }

            if (itAttrRefType != itNodeAttr->AttrEnd())
            {
              std::string sType = itAttrRefType->sAttrValue.AsString();

              if (sType.size() != 0)
              {
                SElement stElemType;
                stElemType.bIsArray = (sType.substr(sType.size() - 2) == "[]");
                if (stElemType.bIsArray)
                {
                  sType.erase(sType.size() - 2);
                }

                stElemType.stType.sName = sType;
                stElemType.sName = "elem" + stElemType.stType.sName;
                GetTns(*itNodeAttr, stElemType.sNamespace, stElemType.sPrefix);

                lsElements.push_back(stElemType);
              }
            }
          }
        }
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////
  typedef std::map<std::string, SElement> TElementMap;
  typedef std::map<std::string, SComplexType> TComplexTypeMap;
  typedef std::map<std::string, SSimpleType> TSimpleTypeMap;
  typedef std::map<std::string, SAttribute> TAttributeMap;

  class CWsdlParserImpl;

  struct SWsdlTypes
  {
    TElementMap mElements;
    TComplexTypeMap mComplexTypes;
    TSimpleTypeMap mSimpleTypes;
    TAttributeMap mAttributes;
    std::set<std::string> m_setNsAliases;
    CWsdlParserImpl* m_pParser;

    SWsdlTypes(CWsdlParserImpl* pParser);
    void Parse(rise::xml::CXMLNode& rNodeWsdl, SProject& rProject, SInterface& rInterface);
    void ParseSchema(const rise::xml::CXMLNode& rSchema);
    void ImportStruct(const std::list<SStruct>& rlsSrc, std::list<SStruct>& rlsDst);
    void Import(rise::xml::CXMLNode& rNodeImport, SProject& rProject, SInterface& rInterface);
    void ImportAll(rise::xml::CXMLNode& rNode, SProject& rProject, SInterface& rInterface);
  };


  //////////////////////////////////////////////////////////////////////////
  // wsdl parser impl

  class CWsdlParserImpl
  {
  public:
    CWsdlParserImpl(const SParseSettings& rParseSettings):
      m_stWsdlTypes(this), m_rParseSettings(rParseSettings)
    {
      m_sRootNamespace = "::";
      TStringMap::const_iterator itRootNs = rParseSettings.mEnv.find("rootns");
      if (itRootNs != rParseSettings.mEnv.end() && !itRootNs->second.empty())
      {
        m_sRootNamespace = "::" + itRootNs->second + "::";
        rise::StrReplace(m_sRootNamespace, ".", "::", true);
      }
    }

    bool ParseTypeAny(const SElement& rElement, SDataType& rDataType)
    {
      if (m_rParseSettings.mEnv.count("use_schema_for_any") != 0)
      {
        const SElement* pSchemaElem = NULL;
        const std::string& sNamespace = rElement.stType.sNamespace;
        // find DataSource declaration
        for (TElementMap::const_iterator itElem = m_stWsdlTypes.mElements.begin();
          itElem != m_stWsdlTypes.mElements.end() && !pSchemaElem; ++itElem)
        {
          const SElement& rChildElem = itElem->second;
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

    void ParsePartElement(const SElement& rElement, SMember& rMember, bool bIsResponse, int nRecursionLevel = 0)
    {
      // parse element operation parameters
      // declared as complex type
      if (!rElement.lsComplexTypes.empty())
      {
        // inline complex type
        for (std::list<SComplexType>::const_iterator itComplexType = rElement.lsComplexTypes.begin();
          itComplexType != rElement.lsComplexTypes.end(); ++itComplexType)
        {
          const SComplexType& rComplexType = *itComplexType;
          if (bIsResponse && rComplexType.lsElements.empty())
          {
            rMember.stReturn.stDataType.sName = "void";
          }
          else
          {
            for (std::list<SElement>::const_iterator itParamElement = rComplexType.lsElements.begin();
              itParamElement != rComplexType.lsElements.end(); ++itParamElement)
            {
              const SElement& rChildElement = *itParamElement;

              if (!rChildElement.stType.sName.empty()) // type is set: processing elem as struct
              {
                SParam stParam;

                stParam.sName = rChildElement.sName;

                SDataType stDataType;

                // trying to parse attached schema to "any" using namespace
                if (rChildElement.stType.sName != "DataObject" || rChildElement.sNamespace.empty()
                    || !ParseTypeAny(rChildElement, stDataType))
                {
                  GetCppType(rChildElement.stType, stDataType);
                }
                if (rChildElement.bIsArray)
                {
                  stParam.stDataType.lsParams.push_back(stDataType);
                  stParam.stDataType.eType = SDataType::ETemplate;
                  stParam.stDataType.sName = "list";
                  stParam.stDataType.sNamespace = "std::";
                }
                else
                {
                  stDataType.sUsedName = stDataType.sNamespace + stDataType.sName;
                  OptimizeCppNs(stDataType.sUsedName, m_stInterface.sNamespace);
                  stParam.stDataType = stDataType;
                }
                stParam.stDataType.sNodeName = rChildElement.sName;

                if (!rComplexType.lsAttributes.empty())
                {
                  // create structure-wrapper for element with attributes
                  SStruct stStruct;
                  stStruct.bForward = false;
                  stStruct.sName = rElement.sName;
                  stStruct.sNamespace = TnsToCppNs(rElement.sNamespace);
                  stStruct.sDescr = rElement.sDescr;
                  stStruct.sDetail = rElement.sDetail;

                  stParam.mOptions["useParentElement"] = "true";
                  stStruct.lsMembers.push_back(stParam);

                  for (std::list<SAttribute>::const_iterator itAttr = rComplexType.lsAttributes.begin();
                    itAttr != rComplexType.lsAttributes.end(); ++itAttr)
                  {
                    SParam stMember;
                    stMember.mOptions["isAttribute"] = "true";
                    if (!itAttr->sDefault.empty())
                    {
                      stMember.mOptions["defaultValue"] = itAttr->sDefault;
                    }
                    stMember.sName = itAttr->sName;
                    if (itAttr->sValue.empty()) // use string as default attribute value type
                    {
                      stMember.stDataType.sName = "string";
                      stMember.stDataType.sNamespace = "std::";
                      stMember.stDataType.sUsedName = "std::string";
                      stMember.stDataType.eType = SDataType::EString;
                    }
                    else
                    {
                      GetCppType(itAttr->sValue, itAttr->sNamespace, stMember.stDataType);
                      stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
                      OptimizeCppNs(stMember.stDataType.sUsedName, stStruct.sNamespace);
                    }
                    stStruct.lsMembers.push_back(stMember);
                  }

                  m_stInterface.lsStructs.push_back(stStruct);

                  stParam.stDataType.eType = SDataType::EStruct;
                  stParam.stDataType.sName = stStruct.sName;
                  stParam.stDataType.sNamespace = stStruct.sNamespace;
                  stParam.stDataType.sUsedName = stStruct.sNamespace + stParam.stDataType.sName;
                  stParam.stDataType.lsParams.clear();
                  OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);
                }

                OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);

                if (!bIsResponse)
                { // request
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
                    rMember.mOptions["responseElement"] = rElement.sName;
                    if (!rChildElement.sName.empty())
                    {
                      rMember.mOptions["resultElement"] = rChildElement.sName;
                    }
                  }
                  else
                  if (nRecursionLevel == 1)
                  {
                    rMember.mOptions["resultElement"] = rElement.sName;
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
                    rMember.mOptions["responseElement"] = rElement.sName;
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
        SParam stParam;
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
        SParam stParam;

        stParam.sName = rElement.sName;

        {// search in complex types
          TComplexTypeMap::iterator itComplexType =
              m_stWsdlTypes.mComplexTypes.find(rElement.stType.GetNsName());
          if (itComplexType != m_stWsdlTypes.mComplexTypes.end())
          {
            SComplexType& rComplexType = itComplexType->second;
            rComplexType.bIsMessagePart = true;

            if (rComplexType.lsElements.empty())
            { // assume void type
              stParam.stDataType.sName = "void";
            }
            else
            if (rComplexType.lsElements.size() == 1 && !rComplexType.lsElements.front().bIsArray) // inline complex type with element name
            {
              const SElement& rElem = rComplexType.lsElements.front();

              GetCppType(rElem.stType, stParam.stDataType);

              if (stParam.stDataType.eType == SDataType::EUnknown)
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
            TSimpleTypeMap::const_iterator itSimpleType =
                m_stWsdlTypes.mSimpleTypes.find(rElement.stType.GetNsName());
            if (itSimpleType != m_stWsdlTypes.mSimpleTypes.end())
            {
              stParam.stDataType = SimpleTypeToData(itSimpleType->second, m_stWsdlTypes);
            }
            else // type is generic or unknown
            {
              GetCppType(rElement.stType, stParam.stDataType);
              if (stParam.stDataType.eType == SDataType::EUnknown)
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
          FixParamDataType(stParam.stDataType);

          if (nRecursionLevel == 0 && rElement.sName != rMember.sName)
          {
            rMember.mOptions["requestElement"] = rElement.sName;
          }

          rMember.mOptions["inlineRequestElement"] = "true";

          rMember.lsParams.push_back(stParam);
        }
        else
        {
          if (nRecursionLevel == 0)
          {
            rMember.mOptions["responseElement"] = rElement.sName;
          }
          rMember.stReturn = stParam;
          rMember.stReturn.stDataType.sNodeName = rElement.sName;
        }
      }
    }

    // part is an params list or result
    void ParsePart(SMember& rMember, const rise::xml::CXMLNode& rPart,
                   const SWsdlTypes& rWsdlTypes, bool bIsResponse)
    {
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrType = rPart.FindAttribute("element");
      if (itAttrType != rPart.AttrEnd())
      { // reference to element type
        const std::string& sElementNsName = itAttrType->sAttrValue.AsString();

        // finding element of part
        TElementMap::const_iterator itElement = rWsdlTypes.mElements.find(sElementNsName);
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

        SParam stParam;
        stParam.sName = StripPrefix(sPartName);
        stParam.stDataType.sNodeName = stParam.sName;

        const std::string& sName = StripPrefix(sType);
        const std::string& sPrefix = GetPrefix(sType);
        const std::string& sNamespace = FindNamespaceUri(rPart, sPrefix);

        GetCppType(SQName(sName, sPrefix, sNamespace), stParam.stDataType);
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


    void ParseRequest(SMember& rMember, const rise::xml::CXMLNode& rMessage, const SWsdlTypes& rWsdlTypes)
    {
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodePart = rMessage.FindSubnode("part");
          itNodePart != rMessage.NodeEnd();
          itNodePart = rMessage.FindSubnode("part", ++itNodePart))
      {
        ParsePart(rMember, *itNodePart, rWsdlTypes, false);
      }
    }

    void ParseResponse(SMember& rMember, const rise::xml::CXMLNode& rMessage, const SWsdlTypes& rWsdlTypes)
    {
      ParsePart(rMember, rMessage.Subnode("part"), rWsdlTypes, true);
    }


    void ParseOperation(SMember& rMember, const rise::xml::CXMLNode& rOperation, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
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

    void ParseService(SClass& rClass, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
    {
      const rise::xml::CXMLNode& rService = rDefs.Subnode("service");
      rClass.sName = rService.Attribute("name").AsString();

      rise::xml::CXMLNode::TXMLNodeConstIterator itNodePort = rService.FindSubnode("port");
      if (itNodePort != rService.NodeEnd())
      {
        rise::xml::CXMLNode::TXMLNodeConstIterator itNodeAddress = itNodePort->FindSubnode("address");
        if (itNodeAddress != itNodePort->NodeEnd())
        {
          rClass.mOptions["serviceUri"] = itNodeAddress->Attribute("location").AsString();
        }
      }

      const rise::xml::CXMLNode& rPortType = rDefs.Subnode("portType");
      const std::string& sPortTypeName = rPortType.Attribute("name").AsString();

      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeOp = rPortType.NodeBegin();
        itNodeOp != rPortType.NodeEnd(); ++itNodeOp)
      {
        if (itNodeOp->NodeType() == rise::xml::CXMLNode::ENTGENERIC &&
          itNodeOp->NodeName() == "operation")
        {
          SMember tOperationMember;
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

      ReadDoc(rService, rClass.sDescr, rClass.sDetail);

      rClass.sNamespace = TnsToCppNs(GetTns(rDefs));
    }

//    void WriteInlineTypes(const SElement& rElement, const SWsdlTypes& rWsdlTypes)
//    {
//      for (std::list<SComplexType>::const_iterator itComplex = rElement.lsComplexTypes.begin();
//        itComplex != rElement.lsComplexTypes.end(); ++itComplex)
//      {
//        for (std::list<SElement>::const_iterator itElement = itComplex->lsElements.begin();
//          itElement != itComplex->lsElements.end(); ++itElement)
//        {
//          SDataType stDt;
//          ElementToData(*itElement, stDt, rWsdlTypes);
//          WriteInlineTypes(*itElement, rWsdlTypes);
//        }
//      }
//    }

//    static bool SortStructByParent(const SStruct& rStruct1, const SStruct& rStruct2)
//    {
//      return rStruct1.sName == rStruct2.sParentName;
//    }

    void ParseInterface(rise::xml::CXMLNode& rRootNode, SProject& rProject)
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

      for (TComplexTypeMap::const_iterator itComplex = m_stWsdlTypes.mComplexTypes.begin();
            itComplex != m_stWsdlTypes.mComplexTypes.end(); ++itComplex)
      {
        if (!itComplex->second.bIsMessagePart)
        {
          ComplexTypeToData(itComplex->second, m_stWsdlTypes);
        }
      }

      for (TSimpleTypeMap::const_iterator itSimple = m_stWsdlTypes.mSimpleTypes.begin();
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
        SClass tServiceClass;

        ParseService(tServiceClass, rRootNode, m_stWsdlTypes);
        tServiceClass.mOptions["targetNamespace"] = m_stInterface.sTargetNs;
        m_stInterface.lsClasses.push_back(tServiceClass);
      }
    }

    SInterface& ParseFile( const std::string& sFileName, SProject& rProject )
    {
      std::string::size_type nPos = sFileName.find_last_of("/\\");
      const std::string& sInterfaceFileName = (nPos != std::string::npos) ?
                                              sFileName.substr(nPos + 1) : sFileName;

      for (std::list<SInterface>::iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        if (itInterface->sFileName == sInterfaceFileName)
        {
          return *itInterface; // already parsed
        }
      }

      rProject.lsInterfaces.push_back(m_stInterface);
      SInterface& rProjectThisInterface = rProject.lsInterfaces.back();

      // parse new interface
      {
        rise::xml::CXMLDocument tWsdlDoc;
        rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();
        tWsdlDoc.LoadFromFile(sFileName);

        m_stInterface.sTargetNs = rDefs.Attribute("targetNamespace").AsString();

        // fill in interface name
        m_stInterface.sFileName = sInterfaceFileName;
        std::string::size_type nPos = m_stInterface.sFileName.find_last_of("\\/");
        if (nPos != std::string::npos)
        {
          m_stInterface.sFileName.erase(0, nPos + 1);
        }

        m_stInterface.sName = sInterfaceFileName;

        m_stInterface.sNamespace = TnsToCppNs(m_stInterface.sTargetNs);

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

        ParseInterface(rDefs, rProject);

        rProjectThisInterface = m_stInterface;

        // put namespaces into cache
        NamespacesCache::mCache[sFileName] = rDefs.GetNsList();
      }

      return m_stInterface;
    }

    const SWsdlTypes& GetWsdlTypes()
    {
      return m_stWsdlTypes;
    }

    const SParseSettings& GetParseSettings() const
    {
      return m_rParseSettings;
    }

  private:
    void DataTypeFromName(const std::string& sDataType, SDataType& rDataType, const SWsdlTypes& rWsdlTypes)
    {
      // struct/typedef
      TSimpleTypeMap::const_iterator itSimpleType = rWsdlTypes.mSimpleTypes.find(sDataType);
      if (itSimpleType != rWsdlTypes.mSimpleTypes.end())
      {
        rDataType = SimpleTypeToData(itSimpleType->second, rWsdlTypes);
      }
      else
      {
        TComplexTypeMap::const_iterator itComplexType = rWsdlTypes.mComplexTypes.find(sDataType);
        if (itComplexType != rWsdlTypes.mComplexTypes.end())
        {
          rDataType = ComplexTypeToData(itComplexType->second, rWsdlTypes);
        }
        else
        {
          rDataType.eType = SDataType::EUnknown;
        }
      }
    }

    SDataType SimpleTypeToData(const SSimpleType& rSimpleType, const SWsdlTypes& rWsdlTypes)
    {
      SDataType stDataType;

      // write enum
      if (!rSimpleType.lsEnumValues.empty() && rSimpleType.stBaseType.sName == "string")
      {
        SEnum stEnum;

        stEnum.sName = rSimpleType.sName;
        stEnum.sNamespace = TnsToCppNs(rSimpleType.sNamespace);
        rise::StrReplace(stEnum.sName, ".", "::", true);

        stDataType.eType = SDataType::EEnum;
        stDataType.sName = stEnum.sName;
        stDataType.sNamespace = stEnum.sNamespace;

        const SBaseType* pEnum = GetBaseType(stEnum.sNamespace + stEnum.sName, m_stInterface, SBaseType::EEnum);
        if (!pEnum)
        {
          stEnum.bForward = false;
          stEnum.sDescr = rSimpleType.sDescr;
          stEnum.sDetail = rSimpleType.sDetail;
          stEnum.mOptions["baseType"] = rSimpleType.stBaseType.sName;

          for (TStringList::const_iterator itValue = rSimpleType.lsEnumValues.begin();
            itValue != rSimpleType.lsEnumValues.end(); ++itValue)
          {
            stEnum.lsMembers.push_back(SEnum::SEnumMember());
            stEnum.lsMembers.back().sName = *itValue;
          }

          std::string::size_type nPos = stEnum.sName.rfind("::");
          if (nPos != std::string::npos)
          {
            const std::string& sOwnerName = stEnum.sName.substr(0, nPos);
            // has a owner
            SStruct* pstOwner = const_cast<SStruct*>(GetStruct(sOwnerName, m_stInterface));
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
        std::list<STypedef>::const_iterator itTypedef = m_stInterface.lsTypedefs.begin();
        for (; itTypedef != m_stInterface.lsTypedefs.end(); ++itTypedef)
        {
          if (itTypedef->sName == rSimpleType.sName)
          {
            break;
          }
        }

        if (itTypedef != m_stInterface.lsTypedefs.end())
        {
          stDataType.eType = SDataType::ETypedef;
          stDataType.sName = itTypedef->sName;
          stDataType.sNamespace = itTypedef->sNamespace;
        }
        else
        if (!rSimpleType.sName.empty())
        {
          STypedef stTypedef;
          stTypedef.sName = rSimpleType.sName;
          stTypedef.sNamespace = TnsToCppNs(rSimpleType.sNamespace);
          stTypedef.sDescr = rSimpleType.sDescr;
          stTypedef.sDetail = rSimpleType.sDetail;

          m_stInterface.lsTypedefs.push_back(stTypedef);

          STypedef& rstTypedef = m_stInterface.lsTypedefs.back();

          std::string sEnumValues;
          for (TStringList::const_iterator itValue = rSimpleType.lsEnumValues.begin();
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

          SDataType stTypedefDataType;
          GetCppType(rSimpleType.stBaseType, stTypedefDataType);
          if (stTypedefDataType.eType == SDataType::EUnknown)
          {
            DataTypeFromName(rSimpleType.stBaseType.sName, stTypedefDataType, rWsdlTypes);
          }
          rstTypedef.stDataType = stTypedefDataType;

          stDataType.eType = SDataType::ETypedef;
          stDataType.sName = rstTypedef.sName;
          stDataType.sNamespace = rstTypedef.sNamespace;
        }
        else
        {
          GetCppType(rSimpleType.stBaseType, stDataType);
        }
      }

      return stDataType;
    }

    SDataType ComplexTypeToData(const SComplexType& rComplexType, const SWsdlTypes& rWsdlTypes,
                                const std::string& sForceParentName = "", const std::string& sForceParentNs = "")
    {
      SDataType stDataType;

      GetCppType(rComplexType, stDataType);

      if (stDataType.eType == SDataType::ETypedef ||
          stDataType.eType == SDataType::EStruct)
      {
        stDataType.sNodeName = stDataType.sName;
        return stDataType;
      }

      // single type array
      if (rComplexType.lsElements.size() == 1)
      {
        const SElement& rElem = rComplexType.lsElements.front();
        if (rElem.bIsArray)
        {
          STypedef stTypedef;
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

          SDataType stResDataType;
          stResDataType.eType = SDataType::ETypedef;
          stResDataType.sName = stTypedef.sName;
          stResDataType.sNamespace = stTypedef.sNamespace;

          return stResDataType;
        }
      }

      if (rComplexType.bIsSimpleContent)
      {
        STypedef stTypedef;
        stTypedef.bExtern = rComplexType.bIsExtern;
        stTypedef.sName = stDataType.sName;
        stTypedef.sNamespace = stDataType.sNamespace;
        stTypedef.sDescr = rComplexType.sDescr;
        stTypedef.sDetail = rComplexType.sDetail;

        stTypedef.stDataType = stDataType;

        GetCppType(rComplexType.sParentName, rComplexType.sParentNs, stTypedef.stDataType);

        if (stTypedef.stDataType.eType == SDataType::EUnknown)
        {
          DataTypeFromName(rComplexType.sParentName, stTypedef.stDataType, m_stWsdlTypes);
        }

        m_stInterface.lsTypedefs.push_back(stTypedef);

        SDataType stResDataType;
        stResDataType.eType = SDataType::ETypedef;
        stResDataType.sName = stTypedef.sName;
        stResDataType.sNamespace = stTypedef.sNamespace;

        return stResDataType;
      }

      if (!rComplexType.sName.empty())
      {
        SStruct* pstStruct = NULL;

        {
          SStruct stStruct;

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
              SDataType stParentType;
              GetCppType(rComplexType.sParentName, rComplexType.sParentNs, stParentType);

              stStruct.sParentNamespace = stParentType.sNamespace;
              stStruct.sParentName = stStruct.sParentNamespace + stParentType.sName;
              OptimizeCppNs(stStruct.sParentName, m_stInterface.sNamespace);
            }
          }

          // class name getter
          if (rComplexType.bIsChoice)
          {
            rise::LogWarning() << "choice is not fully implemented yet: [" << rComplexType.sName << "]";

            SParam stClassNameGetter;
            stClassNameGetter.sName = "sClassName";
            stClassNameGetter.stDataType.sName = "string";
            stClassNameGetter.stDataType.sNamespace = "std::";
            stClassNameGetter.stDataType.sUsedName = "std::string";
            stClassNameGetter.stDataType.eType = SDataType::EString;
            stStruct.lsMembers.push_back(stClassNameGetter);
          }

          stDataType.eType = SDataType::EStruct;

          std::string::size_type nPos = stStruct.sName.rfind("::");
          if (nPos != std::string::npos)
          {
            const std::string& sOwnerName = stStruct.sName.substr(0, nPos);
            // has a owner
            SStruct* pstOwner = const_cast<SStruct*>(GetStruct(sOwnerName, m_stInterface));
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

        for (std::list<SElement>::const_iterator itElement = rComplexType.lsElements.begin();
          itElement != rComplexType.lsElements.end(); ++itElement)
        {
          const SElement* pElement = &*itElement;
          while (pElement->bIsRef)
          {
            TElementMap::const_iterator itTargetElem = m_stWsdlTypes.mElements.find(pElement->stType.GetNsName());

            RISE_ASSERTS(itTargetElem != m_stWsdlTypes.mElements.end(),
                         "Can't find element declaration for: " + pElement->stType.GetNsName());
            pElement = &itTargetElem->second;
          }

          SParam stMember;
          stMember.sName = StripPrefix(pElement->sName);
          stMember.sDescr = pElement->sDescr;
          stMember.sDetail = pElement->sDetail;
          if (rComplexType.bIsChoice)
          {
            ElementToData(*pElement, stMember.stDataType, rWsdlTypes,
                          pstStruct->sName, pstStruct->sNamespace);
          }
          else
          {
            ElementToData(*pElement, stMember.stDataType, rWsdlTypes);
          }
//          if (!rComplexType.bIsChoice)
          {
            if (stMember.sName.empty())
            {
              stMember.sName = "tUnnamed" + rise::ToStr(nUnnamedElemNo++);
              stMember.mOptions["useParentElement"] = "true";
            }

            // optimize template argument type
            if (stMember.stDataType.eType == SDataType::ETemplate)
            {
              const std::string& sOwnerName =
                  pstStruct->sNamespace +
                  (pstStruct->sOwnerName.empty() ? "" : (pstStruct->sOwnerName + "::")) +
                  pstStruct->sName + "::";

              RISE_ASSERTS(!stMember.stDataType.lsParams.empty(), "type of " + stMember.sName +
                          " is template, but no template arg is defined");
              SDataType& rDataType = stMember.stDataType.lsParams.front();

              if (rDataType.eType == SDataType::EStruct ||
                  rDataType.eType == SDataType::ETypedef ||
                  rDataType.eType == SDataType::EEnum)
              {
                rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
                OptimizeCppNs(rDataType.sUsedName, sOwnerName);
              }
            }
            else
            if (stMember.stDataType.eType == SDataType::EStruct ||
                stMember.stDataType.eType == SDataType::ETypedef ||
                stMember.stDataType.eType == SDataType::EEnum)
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

            pstStruct->lsMembers.push_back(stMember);
          }
        }

        for (std::list<SAttribute>::const_iterator itAttr = rComplexType.lsAttributes.begin();
          itAttr != rComplexType.lsAttributes.end(); ++itAttr)
        {
          const SAttribute* pAttr = &*itAttr;
          while (pAttr->bIsRef)
          {
            TAttributeMap::const_iterator itTargetElem = m_stWsdlTypes.mAttributes.find(pAttr->stType.GetNsName());

            RISE_ASSERTS(itTargetElem != m_stWsdlTypes.mAttributes.end(),
                         "Can't find attribute declaration for: " + pAttr->stType.GetNsName());
            pAttr = &itTargetElem->second;
          }

          SParam stMember;
          stMember.mOptions["isAttribute"] = "true";
          stMember.sName = pAttr->sName;
          if (pAttr->sValue.empty()) // use string as default attribute value type
          {
            stMember.stDataType.sName = "string";
            stMember.stDataType.sNamespace = "std::";
            stMember.stDataType.sUsedName = "std::string";
            stMember.stDataType.eType = SDataType::EString;
          }
          else
          {
            GetCppType(pAttr->sValue, pAttr->sNamespace, stMember.stDataType);
            stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
            OptimizeCppNs(stMember.stDataType.sUsedName, pstStruct->sNamespace);
          }
          if (!pAttr->sDefault.empty())
          {
            stMember.mOptions["defaultValue"] = pAttr->sDefault;
          }
          pstStruct->lsMembers.push_back(stMember);
        }
      }

      return stDataType;
    }

    void ElementToData(const SElement& rElement, SDataType& rDataType, const SWsdlTypes& rWsdlTypes,
                       const std::string& sForceParentName = "", const std::string& sForceParentNs = "")
    {
      const SElement* pElement = &rElement;

      rDataType.eType = SDataType::EUnknown;

      while (pElement->bIsRef)
      {
        TElementMap::const_iterator itTargetElem = m_stWsdlTypes.mElements.find(pElement->stType.GetNsName());

        RISE_ASSERTS(itTargetElem != m_stWsdlTypes.mElements.end(),
                     "Can't find element declaration for: " + pElement->stType.GetNsName());
        pElement = &itTargetElem->second;
      }

      if (!pElement->stType.sName.empty())
      {
        // inline complex/simple type declaration
        if (!pElement->lsSimpleTypes.empty())
        {
          for (std::list<SSimpleType>::const_iterator itSimpleSubtype = pElement->lsSimpleTypes.begin();
            itSimpleSubtype != pElement->lsSimpleTypes.end(); ++itSimpleSubtype)
          {
            rDataType = SimpleTypeToData(*itSimpleSubtype, rWsdlTypes);
          }
        }
        else
        if (!pElement->lsComplexTypes.empty())
        {
          for (std::list<SComplexType>::const_iterator itComplexSubtype = pElement->lsComplexTypes.begin();
            itComplexSubtype != pElement->lsComplexTypes.end(); ++itComplexSubtype)
          {
            rDataType = ComplexTypeToData(*itComplexSubtype, rWsdlTypes, sForceParentName, sForceParentNs);
          }
        }
        else // reference to type
        {
          TComplexTypeMap::const_iterator itComplexType = m_stWsdlTypes.mComplexTypes.find(pElement->stType.GetNsName());
          if (itComplexType != m_stWsdlTypes.mComplexTypes.end())
          {
            rDataType = ComplexTypeToData(itComplexType->second, m_stWsdlTypes);
          }
          else
          {
            TSimpleTypeMap::const_iterator itSimpleType = m_stWsdlTypes.mSimpleTypes.find(pElement->stType.GetNsName());
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

      if (rDataType.eType != SDataType::EUnknown)
      {
        if (pElement->bIsArray) // wrap in array
        {
          rDataType.sNodeName = pElement->sName;
          rDataType.lsParams.push_back(rDataType);
          rDataType.sName = "list";
          rDataType.sNamespace = "std::";
          rDataType.eType = SDataType::ETemplate;
          rDataType.sUsedName.erase();
        }
      }
      else
      {
        if (pElement->bIsArray)
        {
          SDataType stTempl;
          GetCppType(pElement->stType, stTempl);
          stTempl.sNodeName = pElement->sName;
          stTempl.sUsedName = stTempl.sNamespace + stTempl.sName;
          OptimizeCppNs(stTempl.sUsedName, m_stInterface.sNamespace);

          rDataType.sName = "list";
          rDataType.sNamespace = "std::";
          rDataType.eType = SDataType::ETemplate;
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
    }

    void GetCppType(const SQName& stQName, SDataType& rDataType)
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
          rDataType.eType = SDataType::EString;
        }
        else
        if (stQName.sName == "DataObject")
        {
          rDataType.sName = "CDataObject";
          rDataType.sNamespace = "staff::";
          rDataType.eType = SDataType::EDataObject;
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
          rDataType.eType = SDataType::EGeneric;
        }
        rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
      }
      else
      if (stQName.sName == "DataObject") // non xsd:any, may have additional schema
      {
        rDataType.sName = "CDataObject";
        rDataType.sNamespace = "staff::";
        rDataType.eType = SDataType::EDataObject;
      }
      else
      if (stQName.sName == "anyType") // non xsd:anyType
      {
        rDataType.sName = stQName.sName;
        rDataType.sNamespace = "staff::";
        rDataType.eType = SDataType::EGeneric;
      }
      else
      { // not an wsdl type, some typedef or struct
        rDataType.sName = stQName.sName;
        rDataType.sNamespace = TnsToCppNs(stQName.sNamespace);

        for (std::list<STypedef>::const_iterator itTypedef = m_stInterface.lsTypedefs.begin();
            itTypedef != m_stInterface.lsTypedefs.end(); ++itTypedef)
        {
          if (itTypedef->sName == rDataType.sName &&
              itTypedef->sNamespace == rDataType.sNamespace)
          {
            rDataType.eType = SDataType::ETypedef;
            return;
          }
        }

        rise::StrReplace(rDataType.sName, ".", "::", true);

        const SStruct* pstStruct = GetStruct(rDataType.sNamespace + rDataType.sName, m_stInterface);
        if (pstStruct)
        {
          rDataType.eType = SDataType::EStruct;
          return;
        }

        rDataType.eType = SDataType::EUnknown;
      }
    }

    void GetCppType(const std::string& sPrefixName, const std::string& sNamespace, SDataType& rDataType)
    {
      std::string::size_type nPos = sPrefixName.find_last_of(':');
      const std::string& sPrefix = (nPos != std::string::npos) ? sPrefixName.substr(0, nPos) : "";
      const std::string& sName = (nPos != std::string::npos) ? sPrefixName.substr(nPos + 1) : sPrefixName;

      GetCppType(SQName(sName, sPrefix, sNamespace), rDataType);
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
    void FixParamDataType(SDataType& rDataType)
    {
      if ((rDataType.eType == SDataType::EString) ||
          (rDataType.eType == SDataType::EDataObject) ||
          (rDataType.eType == SDataType::ETemplate) ||
          (rDataType.eType == SDataType::ETypedef) ||
          (rDataType.eType == SDataType::EStruct))
      {
        rDataType.bIsConst = true;
        rDataType.bIsRef = true;
      }
    }

  private:
    SInterface m_stInterface;
    SWsdlTypes m_stWsdlTypes;
    std::string m_sRootNamespace;
    const SParseSettings& m_rParseSettings;
  };

  SWsdlTypes::SWsdlTypes(CWsdlParserImpl* pParser):
    m_pParser(pParser)
  {
  }

  void SWsdlTypes::Parse(rise::xml::CXMLNode& rNodeWsdl, SProject& rProject, SInterface& rInterface)
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

  void SWsdlTypes::ParseSchema(const rise::xml::CXMLNode& rSchema)
  {
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rSchema.NodeBegin();
      itType != rSchema.NodeEnd(); ++itType)
    {
      if (itType->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sType = itType->NodeName();
        if (sType == "element")
        {
          SElement stElement;
          stElement.Parse(*itType);
          mElements[stElement.GetNsName()] = stElement;
        }
        else
        if (sType == "complexType")
        {
          SComplexType stComplexType;
          stComplexType.Parse(*itType);
          mComplexTypes[stComplexType.GetNsName()] = stComplexType;
        }
        else
        if (sType == "simpleType")
        {
          SSimpleType stSimpleType;
          stSimpleType.Parse(*itType);
          mSimpleTypes[stSimpleType.GetNsName()] = stSimpleType;
        }
        else
        if (sType == "attribute")
        {
          SAttribute stAttr;
          stAttr.Parse(*itType);
          mAttributes[stAttr.GetNsName()] = stAttr;
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

  void ReplacePrefix(SElement& rElement, const std::string& sNamespace, const std::string& sPrefix)
  {
    rElement.bIsExtern = true;
    if (rElement.sNamespace == sNamespace)
    {
      rElement.sPrefix = sPrefix;
    }

    for (std::list<SSimpleType>::iterator itSimpleType = rElement.lsSimpleTypes.begin();
        itSimpleType != rElement.lsSimpleTypes.end(); ++itSimpleType)
    {
      itSimpleType->bIsExtern = true;
      if (itSimpleType->sNamespace == sNamespace)
      {
        itSimpleType->sPrefix = sPrefix;
      }
    }

    for (std::list<SComplexType>::iterator itComplexType = rElement.lsComplexTypes.begin();
        itComplexType != rElement.lsComplexTypes.end(); ++itComplexType)
    {
      SComplexType& rComplexType = *itComplexType;
      rComplexType.bIsExtern = true;

      if (rComplexType.sNamespace == sNamespace)
      {
        rComplexType.sPrefix = sPrefix;
      }

      for (std::list<SElement>::iterator itElement = rComplexType.lsElements.begin();
          itElement != rComplexType.lsElements.end(); ++itElement)
      {
        ReplacePrefix(*itElement, sNamespace, sPrefix);
      }
    }
  }

  void SWsdlTypes::ImportStruct(const std::list<SStruct>& rlsSrc, std::list<SStruct>& rlsDst)
  {
    for (std::list<SStruct>::const_iterator itStruct = rlsSrc.begin();
        itStruct != rlsSrc.end(); ++itStruct)
    {
      rlsDst.push_back(SStruct());
      SStruct& rstStruct = rlsDst.back();
      rstStruct.sName = itStruct->sName;
      rstStruct.sNamespace = itStruct->sNamespace;
      rstStruct.sParentName = itStruct->sParentName;
      rstStruct.sDescr = itStruct->sDescr;
      rstStruct.sDetail = itStruct->sDetail;
      rstStruct.bExtern = true;
      rstStruct.sOwnerName = itStruct->sOwnerName;
      ImportStruct(itStruct->lsStructs, rstStruct.lsStructs);
    }
  }

  void SWsdlTypes::Import(rise::xml::CXMLNode& rNodeImport, SProject& rProject, SInterface& rInterface)
  {
    const std::string& sImportNs = rNodeImport.Attribute("namespace");
    std::string sShemaLocation;
    rise::xml::CXMLNode::TXMLAttrConstIterator itLocation = rNodeImport.FindAttribute("location");
    if (itLocation != rNodeImport.AttrEnd())
    {
      sShemaLocation = itLocation->sAttrValue.AsString();
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
      sShemaLocation = itLocation->sAttrValue.AsString();
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
          tWsdlDoc.LoadFromFile(sShemaLocation);
        }
        catch(rise::CException&)
        { // retry with include option
          tWsdlDoc.LoadFromFile(m_pParser->GetParseSettings().sInDir + '/' + sShemaLocation);
        }

        m_pParser->ParseInterface(rDefs, rProject);
      }
      catch (rise::CException& rException)
      {
        rise::LogWarning() << "Cannot import xsd schema \"" << sShemaLocation
            << "\": " << rException.GetDescr() << ". Service's interface may be incomplete.";
      }*/
    }

    std::string sImportNsPrefix;
    GetNsPrefix(rNodeImport, sImportNs, sImportNsPrefix);

    rise::LogDebug2() << "Importing: " << sShemaLocation << " into namespace: " << sImportNs;

    CWsdlParserImpl tWsdlParser(m_pParser->GetParseSettings());
    try
    {
      SInterface& rNewInterface = tWsdlParser.ParseFile(m_pParser->GetParseSettings().sInDir + '/'
                                                        + sShemaLocation, rProject);

      // import operations
      const SWsdlTypes& rImportedWsdlTypes = tWsdlParser.GetWsdlTypes();
      for (TElementMap::const_iterator itElement = rImportedWsdlTypes.mElements.begin();
          itElement != rImportedWsdlTypes.mElements.end(); ++itElement)
      {
        SElement& rElement = mElements[sImportNsPrefix + ":" + itElement->second.sName];
        rElement = itElement->second;
        ReplacePrefix(rElement, sImportNs, sImportNsPrefix);
      }

      ImportStruct(rNewInterface.lsStructs, rInterface.lsStructs);

      // use extern typedefs
      for (std::list<STypedef>::const_iterator itTypedef = rNewInterface.lsTypedefs.begin();
          itTypedef != rNewInterface.lsTypedefs.end(); ++itTypedef)
      {
        STypedef stTypedef = *itTypedef;
        stTypedef.sName = itTypedef->sName;
        stTypedef.sNamespace = itTypedef->sNamespace;
        stTypedef.sDescr = itTypedef->sDescr;
        stTypedef.sDetail= itTypedef->sDetail;
        stTypedef.bExtern = true;
        rInterface.lsTypedefs.push_back(stTypedef);
      }

      SInclude stInclude;
      stInclude.sInterfaceName = rNewInterface.sName;
      stInclude.sNamespace = rNewInterface.sNamespace;
      stInclude.sFileName = rNewInterface.sFileName;
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

      const rise::xml::CXMLNode::TXMLNsList& lsImportedNamespaces = NamespacesCache::mCache[sShemaLocation];
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

      rise::LogDebug2() << "Importing: " << sShemaLocation << " is complete";
    }
    catch (rise::CException& rException)
    {
      rise::LogWarning() << "Cannot import xsd schema \"" << sShemaLocation
          << "\": " << rException.GetDescr() << ". Service's interface may be incomplete.";
    }
  }

  void SWsdlTypes::ImportAll(rise::xml::CXMLNode& rNode, SProject& rProject, SInterface& rInterface)
  {
    for(rise::xml::CXMLNode::TXMLNodeIterator itNodeChild = rNode.FindSubnode("import");
      itNodeChild != rNode.NodeEnd(); itNodeChild = rNode.FindSubnode("import", ++itNodeChild))
    {
      Import(*itNodeChild, rProject, rInterface);
    }
  }

  // wsdl parser

  CWsdlParser::CWsdlParser()
  {
  }

  CWsdlParser::~CWsdlParser()
  {
  }


  const std::string& CWsdlParser::GetId()
  {
    return m_sId;
  }

  void CWsdlParser::Process(const SParseSettings& rParseSettings, SProject& rProject)
  {
    for (TStringList::const_iterator itFileName = rParseSettings.lsFiles.begin();
          itFileName != rParseSettings.lsFiles.end(); ++itFileName)
    {
      CWsdlParserImpl tWsdlParser(rParseSettings);
      std::string sFileName = rParseSettings.sInDir + "/" + *itFileName;

      rise::LogDebug() << "Processing wsdl: " << *itFileName;

      tWsdlParser.ParseFile(sFileName, rProject);
    }
  }

  const std::string CWsdlParser::m_sId = "wsdl";
}
}
