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
#include <fstream>
#include <sstream>
#include <staff/utils/Log.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/tostring.h>
#include <staff/utils/SharedPtr.h>
#include <staff/utils/PluginExport.h>
#include <staff/utils/Exception.h>
#include <staff/utils/File.h>
#include <staff/xml/Document.h>
#include <staff/xml/Element.h>
#include <staff/xml/Attribute.h>
#include <staff/xml/Namespace.h>
#include <staff/xml/XmlReader.h>
#include <staff/codegen/tools.h>
#include "HttpClient.h"
#include "WsdlParser.h"

STAFF_DECLARE_PLUGIN(staff::codegen::WsdlParser)

namespace staff
{
namespace codegen
{
  struct DeclarationsCache
  {
    typedef std::map<std::string, xml::Element> DeclarationsCacheMap;
    static DeclarationsCacheMap mCache;
  };

  DeclarationsCache::DeclarationsCacheMap DeclarationsCache::mCache;

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

    void FromType(const xml::Element& rElement, const std::string& sType);
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
    std::string sForm;

    Attribute();
    Attribute& Parse(const xml::Element& rElemAttr);
  };

  //////////////////////////////////////////////////////////////////////////
  struct AttributeGroup: public QName
  {
    bool bIsRef;
    std::list<Attribute> lsAttributes;

    AttributeGroup();
    AttributeGroup& Parse(const xml::Element& rElemAttributeGroup);
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
    std::string sForm;

    std::list<SimpleType> lsSimpleTypes;
    std::list<ComplexType> lsComplexTypes;

    Element();
    Element& Parse(const xml::Element& rElemElement);

  private:
    bool GetType(const xml::Element& rElement, std::string& sCppTypeName);
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
    SimpleType& Parse(const xml::Element& rElemSimpleType);
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
    bool bIsSoapArray;

    ComplexType();
    ComplexType& Parse(const xml::Element& rElemComplexType);

  private:
    unsigned nLastChoiceId;

    void ParseSequence(const xml::Element& rElemSequence, const std::string& sChoiceId = "");

    void ParseComplexAttr(const xml::Element& rElemAttr);
    void ParseComplexContent(const xml::Element& rElemComplexContent, bool bIsSimple);
  };


  //////////////////////////////////////////////////////////////////////////

  const std::string& GetTns(const xml::Element& rElem)
  {
    for (const xml::Element* pElem = &rElem; pElem; pElem = pElem->GetParent())
    {
      const xml::Attribute* pAttr = pElem->FindAttribute("targetNamespace");
      if (pAttr)
      {
        return pAttr->GetValue();
      }
    }

    static std::string sEmptyTns;
    return sEmptyTns;
  }

  const std::string& FindNamespaceUri(const xml::Element& rElem, const std::string& sPrefix)
  {
    const xml::Namespace* pNs = rElem.FindNamespaceDeclarationByPrefix(sPrefix);

    if (!pNs)
    {
      static const std::string sEmpty;

      if (!sPrefix.empty())
      {
        LogWarning() << "Can't find prefix declaration [" << sPrefix
                           << "] for node [" << rElem.GetPrefixName() << "]";
      }

      return sEmpty;
    }
    else
    {
      return pNs->GetUri();
    }
  }

  void GetTns(const xml::Element& rElem, std::string& sNamespaceUri, std::string& sPrefix)
  {
    sNamespaceUri.erase();
    sPrefix.erase();

    for (const xml::Element* pElem = &rElem; pElem; pElem = pElem->GetParent())
    {
      const xml::Attribute* pAttr = pElem->FindAttribute("targetNamespace");
      if (pAttr)
      {
        sNamespaceUri = pAttr->GetValue();
        break;
      }
    }

    const xml::Namespace* pNs = rElem.FindNamespaceDeclarationByUri(sNamespaceUri);
    if (pNs)
    {
      sPrefix = pNs->GetPrefix();
    }
  }


  void GetNsPrefix(const xml::Element& rElem, const std::string& sNamespaceUri, std::string& sPrefix)
  {
    const xml::Namespace* pNs = rElem.FindNamespaceDeclarationByUri(sNamespaceUri);
    if (pNs)
    {
      sPrefix = pNs->GetPrefix();
    }
    else
    {
      sPrefix.erase();
    }
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

  bool ReadDoc(const xml::Element& rElem, std::string& sDoc, bool bSingleLine = true)
  {
    const xml::Element* pDoc = rElem.FindChildElementByName("documentation");
    if (pDoc)
    {
      sDoc = pDoc->GetTextValue();
    }
    else
    {
      pDoc = rElem.FindChildElementByName("annotation");
      if (pDoc)
      {
        pDoc = pDoc->FindChildElementByName("documentation");
        if (pDoc)
        {
          sDoc = pDoc->GetTextValue();
        }
      }
    }

    if (!sDoc.empty())
    {
#ifdef __linux__
      StringReplace(sDoc, "\r", "", true);
#endif
      if (bSingleLine)
      {
        StringReplace(sDoc, "\n", " ", true);
      }
      return true;
    }

    return false;
  }

  bool ReadDoc(const xml::Element& rElem, std::string& sDescr, std::string& sDetail)
  {
    bool bReaded = ReadDoc(rElem, sDescr, false);

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
          StringTrimLeft(sDetail);
          StringTrimLeft(sDescr);
          break;
        }
        ++nPos;
      }

      StringReplace(sDescr, "\n", " ", true);
      NormalizeString(sDescr);
    }
    return bReaded;
  }

  const xml::Element* FindElemMatch(const xml::Element& rParent,
                                    const std::string& sMatchElemName,
                                    const std::string& sMatchAttrName,
                                    const std::string& sMatchAttrValue)
  {
    const xml::Element* pChild = rParent.GetFirstChildElement();
    const xml::Attribute* pAttr = NULL;
    while ((pChild = rParent.FindChildElementByName(sMatchElemName, pChild)) != NULL)
    {
      pAttr = pChild->FindAttribute(sMatchAttrName);
      if (pAttr && pAttr->GetValue() == sMatchAttrValue)
      {
        break;
      }
      pChild = pChild->GetNextSiblingElement();
    }
    return pChild;
  }

  const xml::Element& GetGroupDeclaration(const xml::Element& rGroup)
  {
    const std::string& sGroupName = rGroup.GetAttributeValue("ref");
    const xml::Element* pElemSchema = rGroup.GetParent();
    for (; pElemSchema && pElemSchema->GetName() != "schema"; pElemSchema = pElemSchema->GetParent());
    STAFF_ASSERT(pElemSchema, "Can't find schema declaration");
    const xml::Element* pGroupDecl = FindElemMatch(*pElemSchema, "group", "name", sGroupName);
    STAFF_ASSERT(pGroupDecl, "Can't find group \"" + sGroupName + "\" declaration");
    return *pGroupDecl;
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
        LogWarning() << "Type " << sType << " already marked as " << sTemplateName;
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

  bool IsElementArray(const xml::Element& rElement)
  {
    const xml::Attribute* pAttr = rElement.FindAttribute("maxOccurs");
    return pAttr && pAttr->GetValue() != "0" && pAttr->GetValue() != "1";
  }

  template <typename Type>
  const Type* FindQNameType(const std::string& sName, const std::string& sNamespace,
                            const std::list<Type>& rlsTypes)
  {
    for (typename std::list<Type>::const_iterator pType = rlsTypes.begin();
         pType != rlsTypes.end(); ++pType)
    {
      if (pType->sName == sName && pType->sNamespace == sNamespace)
      {
        return &*pType;
      }
    }

    return NULL;
  }

  template <typename Type>
  Type* FindQNameType(const std::string& sName, const std::string& sNamespace,
                      std::list<Type>& rlsTypes)
  {
    for (typename std::list<Type>::iterator pType = rlsTypes.begin();
         pType != rlsTypes.end(); ++pType)
    {
      if (pType->sName == sName && pType->sNamespace == sNamespace)
      {
        return &*pType;
      }
    }

    return NULL;
  }

  //////////////////////////////////////////////////////////////////////////
  void QName::FromType(const xml::Element& rElement, const std::string& sType)
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

  Attribute& Attribute::Parse(const xml::Element& rElemAttr)
  {
    const xml::Attribute* pAttr = rElemAttr.FindAttribute("name");

    if (pAttr)
    {
      sName = pAttr->GetValue();
    }

    pAttr = rElemAttr.FindAttribute("use");
    if (pAttr)
    {
      const std::string& sUse = pAttr->GetValue();
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
        LogWarning() << "attribute use [" << sUse << "] not supported";
      }
    }

    pAttr = rElemAttr.FindAttribute("default");
    if (pAttr)
    {
      sDefault = pAttr->GetValue();
    }

    pAttr = rElemAttr.FindAttribute("form");
    if (pAttr)
    {
      sForm = pAttr->GetValue();
    }

    GetTns(rElemAttr, sNamespace, sPrefix);

    ReadDoc(rElemAttr, sDescr, sDetail);
    //  if type exists, attr is simple
    pAttr = rElemAttr.FindAttribute("type");
    if (pAttr)
    { // attr is simple
      stType.FromType(rElemAttr, pAttr->GetValue());
    }
    else
    { // reference to another type
      pAttr = rElemAttr.FindAttribute("ref");
      if (pAttr)
      {
        const std::string& sValue = pAttr->GetValue();
        if (sValue.substr(0, 4) == "xml:")
        {
          const std::string& sName = StripPrefix(sValue);
          std::string sPrefix = rElemAttr.GetPrefix();
          if (!sPrefix.empty())
          {
            sPrefix += ':';
          }

          if (sName == "base")
          {
            stType.FromType(rElemAttr, sPrefix + "anyURI");
          }
          else
          if (sName == "lang")
          {
            stType.FromType(rElemAttr, sPrefix + "lang");
          }
          else
          if (sName == "space")
          {
            stType.FromType(rElemAttr, sPrefix + "NCName");
          }
          else
          if (sName == "id")
          {
            stType.FromType(rElemAttr, sPrefix + "ID");
          }
          else
          {
            LogError() << "Invalid xml ref: [" << sValue << "]";
          }
        }
        else
        {
          bIsRef = true;
          stType.FromType(rElemAttr, sValue);
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

  Element& Element::Parse(const xml::Element& rElemElement)
  {
    const xml::Attribute* pAttr = rElemElement.FindAttribute("name");

    sName = pAttr ? pAttr->GetValue() : "";

    GetTns(rElemElement, sNamespace, sPrefix);

    ReadDoc(rElemElement, sDescr, sDetail);

    const xml::Attribute* pElemAttr = rElemElement.FindAttribute("default");
    if (pElemAttr)
    {
      sDefault = pElemAttr->GetValue();
    }

    pAttr = rElemElement.FindAttribute("nillable");
    bIsNillable = pAttr && (pAttr->GetValue() == "true");

    pAttr = rElemElement.FindAttribute("form");
    if (pAttr)
    {
      sForm = pAttr->GetValue();
    }

    bIsArray = IsElementArray(rElemElement);

    if (!bIsArray)
    {
      pAttr = rElemElement.FindAttribute("minOccurs");
      bIsOptional = (pAttr) && (pAttr->GetValue() == "0");
    }

    //  if type exists, element is simple
    pAttr = rElemElement.FindAttribute("type");
    if (pAttr)
    { // element is simple
      stType.FromType(rElemElement, pAttr->GetValue());
    }
    else
    { // element is complex
      // reference to another type
      pAttr = rElemElement.FindAttribute("ref");
      if (pAttr)
      { // find element
        bIsRef = true;
        stType.FromType(rElemElement, pAttr->GetValue());
      }
      else
      {
        // has subtypes
        for (const xml::Element* pChild = rElemElement.GetFirstChildElement();
          pChild; pChild = pChild->GetNextSiblingElement())
        {
          const std::string& sElemName = pChild->GetName();
          if (sElemName == "complexType")
          {
            const xml::Element& rElemComplexType = *pChild;
            lsComplexTypes.push_back(ComplexType());
            ComplexType& rComplexType = lsComplexTypes.back();
            rComplexType.Parse(*pChild);

            if (!rElemComplexType.IsAttributeExists("name"))
            {
              rComplexType.sName = sName;
              rComplexType.sDescr = sDescr;
              rComplexType.sDetail = sDetail;
            }
          }
          else
          if (sElemName == "simpleType")
          {
            lsSimpleTypes.push_back(SimpleType());
            lsSimpleTypes.back().Parse(*pChild);
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

  SimpleType& SimpleType::Parse(const xml::Element& rElemSimpleType)
  {
    const xml::Attribute* pAttr = rElemSimpleType.FindAttribute("name");

    sName = pAttr ? pAttr->GetValue() : "";

    GetTns(rElemSimpleType, sNamespace, sPrefix);

    ReadDoc(rElemSimpleType, sDescr, sDetail);

    const xml::Attribute* pAttrBase = rElemSimpleType.FindAttribute("base");
    if (pAttrBase)
    {
      stBaseType.sPrefix = GetPrefix(pAttrBase->GetValue());
      stBaseType.sName = StripPrefix(pAttrBase->GetValue());
      stBaseType.sNamespace = FindNamespaceUri(rElemSimpleType, stBaseType.sPrefix);
    }

    for (const xml::Element* pChild = rElemSimpleType.GetFirstChildElement();
      pChild; pChild = pChild->GetNextSiblingElement())
    {
      const std::string& sElemName = pChild->GetName();
      if (sElemName == "restriction")
      {
        const std::string& sBaseType = pChild->GetAttributeValue("base");
        stBaseType.sPrefix = GetPrefix(sBaseType);
        stBaseType.sName = StripPrefix(sBaseType);
        stBaseType.sNamespace = FindNamespaceUri(rElemSimpleType, stBaseType.sPrefix);

        // process restriction
        for (const xml::Element* pSubChild = pChild->GetFirstChildElement();
          pSubChild; pSubChild = pSubChild->GetNextSiblingElement())
        {
          const xml::Element& rSubChild = *pSubChild;
          const std::string& sSubChildElemName = rSubChild.GetName();

          if (sSubChildElemName == "enumeration")
          {
            Enum::EnumMember stMember;
            const std::string& sValue = rSubChild.GetAttributeValue("value");
            const xml::Attribute* pCppEnumAttr = rSubChild.FindAttribute("enum");
            if (pCppEnumAttr)
            {
              stMember.sName = pCppEnumAttr->GetValue();
              if (FixId(stMember.sName))
              {
                LogWarning() << "Cpp enum name was renamed ["
                                   << pCppEnumAttr->GetValue() << "] => ["
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
          if (sSubChildElemName != "annotation" && sSubChildElemName != "documentation")
          {
            lsRestrictions.push_back(StringPair(sSubChildElemName, rSubChild.GetAttributeValue("value")));
          }
        }
      }
      else
      if (sElemName == "union")
      { // represent union as string
        lsRestrictions.push_back(StringPair(sElemName, pChild->GetAttributeValue("memberTypes")));

        stBaseType.sName = "string";
        stBaseType.sNamespace = "http://www.w3.org/2001/XMLSchema";
      }
      else
      if (sElemName == "list")
      { // represent union as string
        lsRestrictions.push_back(StringPair(sElemName, pChild->GetAttributeValue("itemType")));

        stBaseType.sName = "string";
        stBaseType.sNamespace = "http://www.w3.org/2001/XMLSchema";
      }
      else
      if (sElemName != "annotation" && sElemName != "documentation")
      {
        LogWarning() << "Unsupported [" << sElemName << "] in simple type";
      }
    }

    return *this;
  }


  //////////////////////////////////////////////////////////////////////////
  ComplexType::ComplexType():
    bIsExtern(false), bIsMessagePart(false), bIsSimpleContent(false),
    bHasAnyAttribute(false), bIsAbstract(false), bIsInlineArray(false), bIsSoapArray(false),
    nLastChoiceId(0)
  {
  }

  ComplexType& ComplexType::Parse(const xml::Element& rElemComplexType)
  {
    const xml::Attribute* pAttr = rElemComplexType.FindAttribute("name");
    if (pAttr)
    {
      sName = pAttr->GetValue();
    }

    pAttr = rElemComplexType.FindAttribute("abstract");
    if (pAttr)
    {
      bIsAbstract = pAttr->GetValue() == "true";
    }

    GetTns(rElemComplexType, sNamespace, sPrefix);

    ReadDoc(rElemComplexType, sDescr, sDetail);

    const xml::Attribute* pAttrBase = rElemComplexType.FindAttribute("base");
    if (pAttrBase)
    {
      const std::string& sBaseName = StripPrefix(pAttrBase->GetValue());
      const std::string& sBaseNs = FindNamespaceUri(rElemComplexType, GetPrefix(pAttrBase->GetValue()));

      if (sBaseName == "Array" && sBaseNs == "http://schemas.xmlsoap.org/soap/encoding/")
      { // soap array
        bIsSoapArray = true;
        ParseSequence(rElemComplexType);
        return *this;
      }
      else
      {
        sParentName = sBaseName;
        sParentNs = sBaseNs;
      }
    }

    for (const xml::Element* pChild = rElemComplexType.GetFirstChildElement();
      pChild; pChild = pChild->GetNextSiblingElement())
    {
      const std::string& sElemName = pChild->GetName();
      if (sElemName == "sequence" || sElemName == "all")
      {
        ParseSequence(*pChild);
      }
      else
      if (sElemName == "complexContent")
      {
        ParseComplexContent(*pChild, false);
      }
      else
      if (sElemName == "simpleContent")
      {
        ParseComplexContent(*pChild, true);
      }
      else
      if (sElemName == "choice")
      {
        ParseSequence(*pChild, ToString(nLastChoiceId++));
        bIsInlineArray = IsElementArray(*pChild);
      }
      else
      if (sElemName == "group")
      {
        ParseSequence(GetGroupDeclaration(*pChild));
      }
      else
      if (sElemName == "attribute")
      {
        lsAttributes.push_back(Attribute());
        lsAttributes.back().Parse(*pChild);
      }
      else
      if (sElemName == "anyAttribute")
      {
        bHasAnyAttribute = true;
        ReadDoc(*pChild, sAnyAttrDescr);
      }
      else
      if (sElemName == "attributeGroup")
      {
        lsAttributeGroups.insert(lsAttributeGroups.end(), AttributeGroup())->Parse(*pChild);
      }
      else
      if (sElemName != "annotation" && sElemName != "documentation") // already parsed
      {
        LogWarning() << "Unsupported complexType with \"" << sElemName << "\"";
      }
    }

    return *this;
  }

  void ComplexType::ParseSequence(const xml::Element& rElemSequence,
                                  const std::string& sChoiceId /*= ""*/)
  {
    for (const xml::Element* pChild = rElemSequence.GetFirstChildElement();
      pChild; pChild = pChild->GetNextSiblingElement())
    {
      const std::string& sElemName = pChild->GetName();
      if (sElemName == "element")
      {
        Element& rElement = *lsElements.insert(lsElements.end(), Element());
        rElement.Parse(*pChild);
        rElement.sChoiceId = sChoiceId;
      }
      else
      if (sElemName == "any")
      {
        Element& rElement = *lsElements.insert(lsElements.end(), Element());
        const xml::Attribute* pAttrNs = pChild->FindAttribute("namespace");
        rElement.Parse(*pChild);
        rElement.stType.sName = "DataObject";
        rElement.sChoiceId = sChoiceId;
        if (pAttrNs)
        {
          rElement.stType.sNamespace = pAttrNs->GetValue();
        }
      }
      else
      if (sElemName == "sequence")
      {
        ParseSequence(*pChild, sChoiceId);
      }
      else
      if (sElemName == "choice")
      {
        ParseSequence(*pChild, ToString(nLastChoiceId++));
        bIsInlineArray = IsElementArray(*pChild);
      }
      else
      if (sElemName == "group")
      {
        ParseSequence(GetGroupDeclaration(*pChild), sChoiceId);
      }
      else
      if (sElemName != "annotation" && sElemName != "documentation") // already parsed
      {
        LogWarning() << "Unsupported sequence with \"" << sElemName << "\"";
      }
    }
  }


  void ComplexType::ParseComplexAttr(const xml::Element& rElemAttr)
  {
    const xml::Attribute* pAttrRef = rElemAttr.FindAttribute("ref");
    if (pAttrRef)
    {
      const std::string& sRef = StripPrefix(pAttrRef->GetValue());

      const xml::Attribute* pAttrRefType = rElemAttr.GetFirstAttribute();
      for (; pAttrRefType; pAttrRefType = pAttrRefType->GetNextSibling())
      {
        if (StripPrefix(pAttrRefType->GetName()) == sRef)
        {
          break;
        }
      }

      if (pAttrRefType)
      {
        std::string sType = pAttrRefType->GetValue();

        if (!sType.empty())
        {
          Element stElemType;

          stElemType.stType.sName = StripPrefix(sType);
          stElemType.sName = stElemType.stType.sName;
          GetTns(rElemAttr, stElemType.sNamespace, stElemType.sPrefix);

          lsElements.push_back(stElemType);
        }
      }
    }
    else
    {
      lsAttributes.insert(lsAttributes.end(), Attribute())->Parse(rElemAttr);
    }
  }

  void ComplexType::ParseComplexContent(const xml::Element& rElemComplexContent, bool bIsSimple)
  {
    const xml::Element* pElemExtension = rElemComplexContent.FindChildElementByName("extension");

    bIsSimpleContent = bIsSimple;

    if (pElemExtension)
    {
      sParentName = pElemExtension->GetAttributeValue("base");
      sParentNs = FindNamespaceUri(rElemComplexContent, GetPrefix(sParentName));

      for (const xml::Element* pChild = pElemExtension->GetFirstChildElement();
        pChild; pChild = pChild->GetNextSiblingElement())
      {
        const std::string& sElemName = pChild->GetName();
        if (sElemName == "sequence" || sElemName == "all")
        {
          ParseSequence(*pChild);
        }
        else
        if (sElemName == "attribute")
        {
          ParseComplexAttr(*pChild);
        }
      }
    }
    else
    {
      const xml::Element* pElemRestriction = rElemComplexContent.FindChildElementByName("restriction");

      if (pElemRestriction)
      {
        const std::string& sBase = pElemRestriction->GetAttributeValue("base");

        const std::string& sBaseName = StripPrefix(sBase);
        const std::string& sBaseNs = FindNamespaceUri(*pElemRestriction, GetPrefix(sBase));
        if (sBaseName == "Array" && sBaseNs == "http://schemas.xmlsoap.org/soap/encoding/")
        {
          bIsSoapArray = true;
          const xml::Element* pElemAttr = pElemRestriction->FindChildElementByName("attribute");
          if (pElemAttr)
          {
            const xml::Attribute* pAttr = pElemAttr->FindAttribute("arrayType");
            if (pAttr)
            {
              lsAttributes.push_back(Attribute());
              Attribute& rAttr = lsAttributes.back();
              rAttr.sName = pAttr->GetName();
              rAttr.stType.sName = StripPrefix(pAttr->GetValue());
              rAttr.stType.sPrefix = GetPrefix(pAttr->GetValue());
              rAttr.stType.sNamespace = FindNamespaceUri(rElemComplexContent, rAttr.stType.sPrefix);
            }
            else
            {
              LogWarning() << "Could not find arrayType attribute";
            }
          }
          else
          {
            ParseSequence(*pElemRestriction);
          }
        }
        else
        {
          sParentName = sBase;
          for (const xml::Element* pElemAttr = pElemRestriction->FindChildElementByName("attribute");
               pElemAttr; pElemAttr = pElemRestriction->FindChildElementByName("attribute", pElemAttr))
          {
            ParseComplexAttr(*pElemAttr);
            pElemAttr = pElemAttr->GetNextSiblingElement();
          }
        }
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////
  AttributeGroup::AttributeGroup():
    bIsRef(false)
  {
  }

  AttributeGroup& AttributeGroup::Parse(const xml::Element& rElemAttributeGroup)
  {
    const xml::Attribute* pAttr = rElemAttributeGroup.FindAttribute("ref");
    if (pAttr)
    {
      bIsRef = true;
      FromType(rElemAttributeGroup, pAttr->GetValue());
    }
    else
    {
      pAttr = rElemAttributeGroup.FindAttribute("name");
      if (pAttr)
      {
        sName = pAttr->GetValue();
      }

      GetTns(rElemAttributeGroup, sNamespace, sPrefix);
      ReadDoc(rElemAttributeGroup, sDescr, sDetail);

      for (const xml::Element* pElemAttr = rElemAttributeGroup.GetFirstChildElement();
          pElemAttr; pElemAttr = pElemAttr->GetNextSiblingElement())
      {
        if (pElemAttr->GetName() != "attribute")
        {
          if (pElemAttr->GetName() != "annotation" && pElemAttr->GetName() != "documentation")
          {
            LogWarning() << "Unsupported [" << pElemAttr->GetName() << "] within attributeGroup";
          }
        }
        else
        {
          lsAttributes.insert(lsAttributes.end(), Attribute())->Parse(*pElemAttr);
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
    void Parse(xml::Element& rElemWsdl, Project& rProject, Interface& rInterface);
    void ParseSchema(const xml::Element& rSchema);
    void GetInterfaceAttrs(const xml::Element &rSchema, Interface &Interface);
    void ImportStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst);
    void Import(xml::Element& rElemImport, Project& rProject, Interface& rInterface, bool bInclude);
    void ImportAll(xml::Element& rElem, Project& rProject, Interface& rInterface);
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
        StringReplace(m_sRootNamespace, ".", "::", true);
      }
    }

    bool ParseTypeAny(const Element& rElement, DataType& rDataType)
    {
      STAFF_ASSERT_PARAM(m_pParseSettings);
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
    void ParsePart(Member& rMember, const xml::Element& rPart,
                   WsdlTypes& rWsdlTypes, bool bIsResponse, bool bFault)
    {
      const xml::Attribute* pAttrType = rPart.FindAttribute("element");
      if (pAttrType)
      { // reference to element type
        const std::string& sElementPrefixName = pAttrType->GetValue();
        const std::string& sElementName = StripPrefix(sElementPrefixName);
        const std::string& sElementNamespace = FindNamespaceUri(rPart, GetPrefix(sElementPrefixName));

        // finding element of part
        Element* pElement = FindQNameType(sElementName, sElementNamespace, rWsdlTypes.lsElements);
        STAFF_ASSERT(pElement, "Element " + sElementPrefixName + " is not found, while parsing part");

        if (!bFault)
        {
          DataType stDataType;
          Struct* pstStruct = NULL;
          bool bUnwrapStruct = false;
          bool bUnhideStruct = false;

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
            STAFF_ASSERT(pstStruct, "Can't find struct declaration: " +
                         stDataType.sNamespace + stDataType.sName);

            if (!m_pParseSettings->mEnv.count("dontunwrap"))
            {
              // unwrap element to operation's arguments only in case: inline element without references
              // and no attributes in response
              bUnwrapStruct = !pElement->lsComplexTypes.empty() &&
                  (!bIsResponse ||
                    (pElement->lsComplexTypes.front().lsAttributes.empty() &&
                    pElement->lsComplexTypes.front().lsAttributeGroups.empty())) &&
                  pstStruct && pstStruct->sParentName.empty()
                      && (!pstStruct->bExtern || !!pstStruct->mOptions.count("hidden")) &&
                  (pstStruct->lsMembers.empty() || !pstStruct->lsMembers.front().mOptions.count("choice")) &&
                  (pstStruct->mOptions.empty() ||
                    (pstStruct->mOptions.size() == 1 && pstStruct->mOptions.count("hidden"))) &&
                  pstStruct->lsStructs.empty() && pstStruct->lsEnums.empty();
            }
          }

          if (bIsResponse) // response
          {
            if (pElement->bIsExtern)
            {
              rMember.mOptions["responseTargetNamespace"] = pElement->sNamespace;
            }
            rMember.mOptions["responseElement"] = pElement->sName;
            if (!!pstStruct && (!pstStruct->bExtern || !!pstStruct->mOptions.count("hidden"))
                && pstStruct->lsMembers.empty())
            {
              rMember.stReturn.stDataType.sName = "void";
              pstStruct->mOptions["hidden"];
              pstStruct->bExtern = true;
            }
            else
            {
              // unwrap result only if response have a single child element
              if (bUnwrapStruct && pstStruct->lsMembers.size() == 1 && !m_pParseSettings->mEnv.count("dontunwrapresponse"))
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
                bUnhideStruct = true;
              }
            }
          }
          else // request
          {
            if (pElement->bIsExtern)
            {
              rMember.mOptions["requestTargetNamespace"] = pElement->sNamespace;
            }
            rMember.mOptions["requestElement"] = pElement->sName;
            if (bUnwrapStruct)
            {
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
              bUnhideStruct = true;
            }
          }

          // unhide struct
          if (bUnhideStruct && pstStruct && pstStruct->mOptions.count("hidden"))
          {
            pstStruct->mOptions.erase("hidden");
            pstStruct->bExtern = false;
            pElement->bIsMessage = false;
          }
        }
      }
      else
      if (!bFault)
      { // inline part declaration
        const std::string& sPartName = rPart.GetAttributeValue("name");
        const xml::Attribute* pType = rPart.FindAttribute("type");
        STAFF_ASSERT(pType, "Unknown part type: " + sPartName);

        const std::string& sType = pType->GetValue();

        Param stParam;

        const std::string& sName = StripPrefix(sType);
        const std::string& sPrefix = GetPrefix(sType);
        const std::string& sNamespace = FindNamespaceUri(rPart, sPrefix);

        GetCppType(QName(sName, sPrefix, sNamespace), stParam.stDataType);
        STAFF_ASSERT(!stParam.stDataType.sName.empty(), "Unknown part type");

        if (!bIsResponse)
        {
          stParam.sName = StripPrefix(sPartName);
          if (FixId(stParam.sName))
          {
            stParam.mOptions["elementName"] = StripPrefix(sPartName);
          }

          FixParamDataType(stParam.stDataType);
          rMember.lsParams.push_back(stParam);
        }
        else
        {
          FixAbstractDataType(stParam.stDataType);
          rMember.stReturn = stParam;
          rMember.mOptions["resultElement"] = StripPrefix(sPartName);
        }
      }
    }


    void ParseRequest(Member& rMember, const xml::Element& rMessage, WsdlTypes& rWsdlTypes)
    {
      for (const xml::Element* pElemPart = rMessage.FindChildElementByName("part");
          pElemPart; pElemPart = rMessage.FindChildElementByName("part", pElemPart))
      {
        ParsePart(rMember, *pElemPart, rWsdlTypes, false, false);
        pElemPart = pElemPart->GetNextSiblingElement();
      }
    }

    void ParseResponse(Member& rMember, const xml::Element& rMessage, WsdlTypes& rWsdlTypes)
    {
      const xml::Element& rPart = rMessage.GetChildElementByName("part");
      ParsePart(rMember, rPart, rWsdlTypes, true, false);
//      if (!rPart.IsAttributeExists("element"))
//      {
//        rMember.mOptions["responseElement"] = rMessage.GetAttributeValue("name");
//      }
    }

    void ParseFault(Member& rMember, const xml::Element& rMessage, WsdlTypes& rWsdlTypes)
    {
      ParsePart(rMember, rMessage.GetChildElementByName("part"), rWsdlTypes, false, true);
    }

    void ParseOperation(Member& rMember, const xml::Element& rOperation,
                        const xml::Element& rDefs, WsdlTypes& rWsdlTypes)
    {
      bool bHasInput = false;
      bool bHasOutput = false;
      rMember.sName = rOperation.GetAttributeValue("name");

      const xml::Element* pElemInput = rOperation.FindChildElementByName("input");
      if (pElemInput)
      {
        // request
        const std::string& sRequestName = StripPrefix(pElemInput->GetAttributeValue("message"));

        const xml::Element* pMessage = FindElemMatch(rDefs, "message", "name", sRequestName);

        STAFF_ASSERT(pMessage, "Can't find message definition(input) for: " + sRequestName);

        ParseRequest(rMember, *pMessage, rWsdlTypes);
        bHasInput = true;
      } // else notification message

      // response
      const xml::Element* pElemOutput = rOperation.FindChildElementByName("output");
      if (pElemOutput)
      {
        std::string sResponseName = StripPrefix(pElemOutput->GetAttributeValue("message"));

        const xml::Element* pMessage = FindElemMatch(rDefs, "message", "name", sResponseName);

        STAFF_ASSERT(pMessage, "Can't find message definition(output) for: " + sResponseName);

        ParseResponse(rMember, *pMessage, rWsdlTypes);
        bHasOutput = true;
      }
      else
      { // one way message
        rMember.stReturn.stDataType.sName = "void";
      }

      // fault
      const xml::Element* pElemFault = rOperation.FindChildElementByName("fault");
      if (pElemFault)
      {
        std::string sFaultName = StripPrefix(pElemFault->GetAttributeValue("message"));

        const xml::Element* pMessage = FindElemMatch(rDefs, "message", "name", sFaultName);

        STAFF_ASSERT(pMessage, "Can't find message definition(fault) for: " + sFaultName);

        ParseFault(rMember, *pMessage, rWsdlTypes);
        bHasOutput = true;
      }

      if (bHasInput && !bHasOutput)
      {
        rMember.mOptions["mep"] = "in-only";
      }

      ReadDoc(rOperation, rMember.sDescr, rMember.sDetail);
    }

    class FindElemBinding
    {
    public:
      FindElemBinding(const std::string& sTypeName):
        m_sTypeName(StripPrefix(sTypeName))
      {}

      bool operator()(const xml::Element& rFindElem)
      {
        if (rFindElem.GetName() != "binding")
        {
          return false;
        }

        const xml::Attribute* pType = rFindElem.FindAttribute("type");
        if (!pType)
        {
          return false;
        }

        // filter soap transport
        const xml::Element* pBindingTransport = rFindElem.FindChildElementByName("binding");
        if (!pBindingTransport)
        {
          return false;
        }

        const xml::Attribute* pAttrTransport = pBindingTransport->FindAttribute("transport");
        if (!pAttrTransport)
        {
          return false;
        }

        if (pAttrTransport->GetValue() != "http://schemas.xmlsoap.org/soap/http")
        { // not a soap over http transport
          return false;
        }

        // checking soap version, must be 1.1
        const std::string& sTransportUri =
            FindNamespaceUri(*pBindingTransport, pBindingTransport->GetPrefix());
        if (sTransportUri.empty())
        {
          std::string sName = "<noname>";
          const xml::Attribute* pBindingName = rFindElem.FindAttribute("name");
          if (pBindingName)
          {
            sName = pBindingName->GetValue();
          }

          LogWarning() << "can't find namespace declaration for binding with name: " << sName;
          return false;
        }

        if (sTransportUri != "http://schemas.xmlsoap.org/wsdl/soap/")
        { // not a soap 1.1
          return false;
        }

        return StripPrefix(pType->GetValue()) == m_sTypeName;
      }

    private:
      std::string m_sTypeName;
    };

    void ParseSoapAction(const std::string& sPortTypeName, const std::string& sOperationName,
                         const xml::Element& rDefs, std::string& sSoapAction)
    {
      FindElemBinding tFindElemBinding(sPortTypeName);
      const xml::Element* pElemBinding = rDefs.GetFirstChildElement();
      for (; pElemBinding && !tFindElemBinding(*pElemBinding);
           pElemBinding = pElemBinding->GetNextSiblingElement());

      if (pElemBinding)
      {
        const xml::Element* pElemOperationName =
            FindElemMatch(*pElemBinding, "operation", "name", sOperationName);

        if (pElemOperationName)
        {
          const xml::Element* pElemOperation = pElemOperationName->FindChildElementByName("operation");

          if (pElemOperation)
          {
            const xml::Attribute* pAttrSoapAction = pElemOperation->FindAttribute("soapAction");

            if (pAttrSoapAction)
            {
              sSoapAction = pAttrSoapAction->GetValue();
            }
          }
        }
      }
    }

    void ParseService(Interface& rInterface, const xml::Element& rDefs, WsdlTypes& rWsdlTypes)
    {
      for (const xml::Element* pElemService = rDefs.FindChildElementByName("service");
           pElemService; pElemService =
             rDefs.FindChildElementByName("service", pElemService->GetNextSiblingElement()))
      {
        rInterface.lsClasses.push_back(Class());
        Class& rClass = rInterface.lsClasses.back();
        const xml::Element& rService = *pElemService;
        rClass.sName = rService.GetAttributeValue("name");
        FixId(rClass.sName);

        StringMap::const_iterator itTns = rInterface.mOptions.find("targetNamespace");
        if (itTns != rInterface.mOptions.end())
        {
          rClass.mOptions["targetNamespace"] = itTns->second;
        }

        const xml::Element* pElemPort = rService.FindChildElementByName("port");
        if (pElemPort)
        {
          const xml::Element* pElemAddress = pElemPort->FindChildElementByName("address");
          if (pElemAddress)
          {
            rClass.mOptions["serviceUri"] = pElemAddress->GetAttributeValue("location");
          }
          rClass.mOptions["soapVersion"] = "1.1";

          const std::string& sBindingName = StripPrefix(pElemPort->GetAttributeValue("binding"));

          const xml::Element* pBinding = FindElemMatch(rDefs, "binding", "name", sBindingName);
          STAFF_ASSERT(pBinding, "Can't find service binding: " + sBindingName);

          const std::string& sPortTypeName = StripPrefix(pBinding->GetAttributeValue("type"));

          const xml::Element* pPortType = FindElemMatch(rDefs, "portType", "name", sPortTypeName);

          STAFF_ASSERT(pPortType, "Can't find portType: " + sPortTypeName);

          for (const xml::Element* pElemOp = pPortType->GetFirstChildElement();
               pElemOp; pElemOp = pElemOp->GetNextSiblingElement())
          {
            if (pElemOp->GetName() == "operation")
            {
              Member tOperationMember;
              ParseOperation(tOperationMember, *pElemOp, rDefs, rWsdlTypes);
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
                  LogDebug() << "Moving struct [" << itStruct2->sName
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

        LogWarning() << "Can't reorder structures. Circular dependecies is possible";
      }
    }

    void ParseInterface(xml::Element& rRootElem, Project& rProject)
    {
      STAFF_ASSERT_PARAM(m_pParseSettings);
      bool bSchema = rRootElem.GetName() == "schema";
      STAFF_ASSERT(bSchema || rRootElem.GetName() == "definitions",
        "Invalid wsdl/xsd root node name: [" + rRootElem.GetName() + "]");

      if (!bSchema)
      {
        m_stWsdlTypes.Parse(rRootElem, rProject, m_stInterface);
      }
      else
      {
        // import all before parsing schema
        // import xsd in definitions
        m_stWsdlTypes.GetInterfaceAttrs(rRootElem, m_stInterface);
        m_stWsdlTypes.ImportAll(rRootElem, rProject, m_stInterface);
        m_stWsdlTypes.ParseSchema(rRootElem);
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
        ParseService(m_stInterface, rRootElem, m_stWsdlTypes);
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

    Interface& Parse(std::string sFileUri, Project& rProject, const Interface* pParentInterface = NULL)
    {
      STAFF_ASSERT_PARAM(m_pParseSettings);
      std::string::size_type nPos = sFileUri.find_last_of("/\\");
      std::string::size_type nProtocolPos = sFileUri.find("://");
      bool bRemote = nProtocolPos != std::string::npos;

      std::string sInterfaceFilePath = (nPos != std::string::npos && !bRemote) ?
                                              sFileUri.substr(0, nPos + 1) : "";
      std::string sInterfaceFileName = (nPos != std::string::npos) ? sFileUri.substr(nPos + 1) : sFileUri;

      if (pParentInterface)
      {
        StringMap::const_iterator itUri = pParentInterface->mOptions.find("url");
        if (itUri != pParentInterface->mOptions.end())
        {
          if (!bRemote) // relative path to included schema/wsdl
          {
            std::string sParentUriPath = itUri->second;
            nPos = sParentUriPath.find_last_of('/');
            if (nPos != std::string::npos)
            {
              sParentUriPath.erase(nPos + 1);
            }

            sFileUri.insert(0, sParentUriPath);
            sInterfaceFilePath.erase();
            bRemote = true;
          }

          nProtocolPos = sFileUri.find("://");
        }
      }

      {
        const std::string::size_type nSize = m_pParseSettings->sInDir.size();
        if (nSize && sInterfaceFilePath.substr(0, nSize) == m_pParseSettings->sInDir)
        {
          sInterfaceFilePath.erase(0, nSize);
        }
      }

      if (bRemote && sInterfaceFileName.empty())
      {
        sInterfaceFileName = sFileUri.substr(nProtocolPos + 3, sFileUri.size() - nProtocolPos - 4);
        StringReplace(sInterfaceFileName, "/", "_", true);
        StringReplace(sInterfaceFileName, ".", "_", true);
      }

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
          LogWarning() << "Already parsed";
          return *itInterface; // already parsed
        }
      }

      // parse new interface
      {
        xml::Document tWsdlDoc;
        xml::Element& rDefs = tWsdlDoc.GetRootElement();

        if (bRemote)
        {
          static const std::string sCacheDir = ".staff_codegen_cache" STAFF_PATH_SEPARATOR;
          std::string sCachedName = sFileUri;
          StringReplace(sCachedName, "/", "_", true);
          StringReplace(sCachedName, ":", "_", true);
          sCachedName.insert(0, sCacheDir);

          std::ifstream ifStream(sCachedName.c_str());
          if (ifStream.good())
          {
            std::cout << "Using cached [" << sCachedName << "] for " << sFileUri << std::endl;
            xml::XmlReader(ifStream).ReadDocument(tWsdlDoc);
            ifStream.close();
          }
          else
          {
            std::string sFileData;
            std::cout << "Downloading the " << sFileUri << std::endl;
            STAFF_ASSERT(HttpClient::Get(sFileUri, sFileData), "Can't download file: " + sFileUri);
            if (!m_pParseSettings->mEnv.count("do_not_save_wsdl"))
            {
              File(sCacheDir).Mkdirs();
              std::cout << "Saving [" << sCachedName << "]" << std::endl;
              std::ofstream ofStream(sCachedName.c_str());
              if (!ofStream.good())
              {
                LogWarning() << "Can't save [" << sCachedName << "]";
              }
              else
              {
                ofStream << sFileData;
                ofStream.close();
              }
            }
            std::istringstream issData(sFileData);
            xml::XmlReader(issData).ReadDocument(tWsdlDoc);
          }
          m_stInterface.mOptions["url"] = sFileUri;
        }
        else
        {
          xml::XmlFileReader(sFileUri).ReadDocument(tWsdlDoc);
        }

        // fill in interface name
        m_stInterface.sFileName = sInterfaceFileName;
        m_stInterface.sFilePath = sInterfaceFilePath;
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
        m_stInterface.sNamespace = TnsToCppNs(StringMapValue(m_stInterface.mOptions, "targetNamespace"));

        // fix enum member names
        FixEnums(m_stInterface.lsEnums, m_stInterface.lsStructs, &m_stInterface.lsTypedefs);
        FixEnumsInStruct(m_stInterface.lsStructs);

        FixStructInheritance(m_stInterface.lsStructs);

        // fix structures
        FixStructOrder(m_stInterface.lsStructs);

        rProjectThisInterface = m_stInterface;

        // put namespaces into cache
        DeclarationsCache::mCache[sFileUri].CloneElement(rDefs, false);
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
      STAFF_ASSERT_PARAM(m_pParseSettings);
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
          stEnum.sName = "UnnamedEnum" + ToString(nUnnamedEnumId++);
        }
        stEnum.sNamespace = TnsToCppNs(rSimpleType.sNamespace);
        StringReplace(stEnum.sName, ".", "::", true);

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
                LogError() << "Can't find owner struct: " << stEnum.sName.substr(0, nPos);
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
          STAFF_ASSERT(pAttrTarget, "Can't find attribute declaration for [" + pAttr->stType.GetNsName() +
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
        if (!pAttr->sForm.empty())
        {
          stMember.mOptions["form"] = pAttr->sForm;
        }
        if (bIsAttrOptional)
        {
          WrapTypeInTemplate(stMember.stDataType, "Optional");
        }
        stMember.sDescr = pAttr->sDescr;
        stMember.sDetail = pAttr->sDetail;
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
        for (SimpleTypeList::const_iterator pType = rElem.lsSimpleTypes.begin();
             pType != rElem.lsSimpleTypes.end(); ++pType)
        {
          const SimpleType& rType = *pType;
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
      for (ComplexTypeList::const_iterator pType = rTypes.begin();
           pType != rTypes.end(); ++pType)
      {
        const ComplexType& rType = *pType;
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
          LogWarning() << "Can't find struct declaration: " << rDataType.sNamespace
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
        STAFF_ASSERT(pBaseType, "Can't get base type");
        Struct* pstStruct = const_cast<Struct*>(static_cast<const Struct*>(pBaseType));
        if (!bIsMessagePart && !!pstStruct->mOptions.count("hidden") && pstStruct->bExtern)
        {
          pstStruct->bExtern = false;
          pstStruct->mOptions.erase("hidden");
        }
        return stDataType;
      }

      if (rComplexType.bIsSoapArray)
      {
        Typedef stTypedef;
        stTypedef.bForward = false;
        stTypedef.bExtern = rComplexType.bIsExtern;
        stTypedef.sName = stDataType.sName;
        stTypedef.sNamespace = stDataType.sNamespace;
        stTypedef.sDescr = rComplexType.sDescr;
        stTypedef.sDetail = rComplexType.sDetail;

        // restriction method
        if (rComplexType.lsElements.empty())
        {
          const Attribute* pAttrArrayType = NULL;
          for (std::list<Attribute>::const_iterator itAttr = rComplexType.lsAttributes.begin();
               itAttr != rComplexType.lsAttributes.end(); ++itAttr)
          {
            if (itAttr->sName == "arrayType")
            {
              pAttrArrayType = &*itAttr;
              break;
            }
          }

          STAFF_ASSERT(pAttrArrayType, "Couldn't find arrayType attribute for soap array type: ["
                       + rComplexType.sName + "]");

          std::string::size_type nPos = pAttrArrayType->stType.sName.find_first_of('[');
          STAFF_ASSERT(nPos != std::string::npos, "Can't find array type declaration in: ["
                       + pAttrArrayType->stType.sName + "]");
          const std::string& sArrayBounds = pAttrArrayType->stType.sName.substr(nPos);
          const std::string& sArrayTypeName = pAttrArrayType->stType.sName.substr(0, nPos);

          DataType stArrayDataType;
          GetCppType(sArrayTypeName, pAttrArrayType->stType.sNamespace,
                           stArrayDataType);

          if (stArrayDataType.eType == DataType::TypeUnknown)
          {
            DataTypeFromName(sArrayTypeName, pAttrArrayType->stType.sNamespace,
                             stArrayDataType, m_stWsdlTypes);
          }

          std::string::size_type nBounds = 0;
          while ((nBounds = sArrayBounds.find('[', nBounds + 1)) != std::string::npos)
          {
            DataType stDataTypeTmp = stArrayDataType;
            stArrayDataType.sName = "Array";
            stArrayDataType.sNamespace = "staff::";
            stArrayDataType.eType = DataType::TypeTemplate;
            stArrayDataType.lsParams.resize(1);
            stArrayDataType.lsParams.front() = stDataTypeTmp;
          }

          stTypedef.mOptions["arrayBounds"] = sArrayBounds;

          stTypedef.stDataType.lsParams.push_back(stArrayDataType);
        }
        else // array as sub element
        {
          if (rComplexType.lsElements.size() > 1)
          {
            LogWarning() << "Size of elements in SOAP Array > 1. Only first will be used.";
          }

          const Element& rArrayElement = rComplexType.lsElements.front();
          STAFF_ASSERT(rArrayElement.bIsArray, "SOAP Array element should have "
                       "maxOccurs=\"unbounded\" attribute");

          ElementToData(rArrayElement, stTypedef.stDataType, m_stWsdlTypes);

          if (!rArrayElement.sName.empty())
          {
            stTypedef.mOptions["elementName"] = rArrayElement.sName;
          }
        }

        // also rewrite std::list after sub element declaration
        stTypedef.stDataType.sName = "Array";
        stTypedef.stDataType.sNamespace = "staff::";
        stTypedef.stDataType.eType = DataType::TypeTemplate;

        m_stInterface.lsTypedefs.push_back(stTypedef);

        DataType stResDataType;
        stResDataType.eType = DataType::TypeTypedef;
        stResDataType.sName = stTypedef.sName;
        stResDataType.sNamespace = stTypedef.sNamespace;

        return stResDataType;
      }

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
            stTypedef.mOptions["itemName"] = stStruct.sName;

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
              LogError() << "Additional Elements found while inheriting from ComplexType with same name."
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
              LogError() << "Can't find owner struct: " << stStruct.sName.substr(0, nPos);
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

        STAFF_ASSERT(pstStruct, "Can't get struct");

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

            STAFF_ASSERT(pTargetElem, "Can't find element declaration for: [" + pElement->stType.GetNsName()
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

          if (itElement->bIsRef)
          {
            stMember.mOptions["isRef"] = "true";
          }

          if (!itElement->bIsRef && itElement->stType.sName.empty())
          { // inline element
            ElementToData(*itElement, stMember.stDataType, rWsdlTypes, pstStruct);
          }
          else
          {
            ElementToData(*itElement, stMember.stDataType, rWsdlTypes);
          }

          if (itElement->bIsArray)
          {
            stMember.mOptions["useParentElement"] = "true";
            stMember.mOptions["itemName"] = pElement->sName;
          }

          if (stMember.sName.empty())
          {
            stMember.sName = "tUnnamed" + ToString(nUnnamedElemNo++);
            stMember.mOptions["useParentElement"] = "true";
          }

          if (!pElement->sDefault.empty())
          {
            stMember.mOptions["defaultValue"] = pElement->sDefault;
          }

          if (!pElement->sForm.empty())
          {
            stMember.mOptions["form"] = pElement->sForm;
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
                m_stWsdlTypes.mAttributeGroups.find(pAttrGroup->sNamespace + ":" + pAttrGroup->sName);

            STAFF_ASSERT(itTargetElem != m_stWsdlTypes.mAttributeGroups.end(),
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

        STAFF_ASSERT(pTargetElem, "Can't find element declaration for: [" + pElement->stType.GetNsName()
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
//          LogWarning() << "Untyped element: [" << pElement->sName << "]. Using as DataObject ";
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

      if (pElement->bIsNillable || rElement.bIsNillable)
      {
        WrapTypeInTemplate(bIsArray ? rDataType.lsParams.front() : rDataType, "Nillable");
      }
      if (pElement->bIsOptional || rElement.bIsOptional)
      {
        WrapTypeInTemplate(bIsArray ? rDataType.lsParams.front() : rDataType, "Optional");
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

        StringReplace(rDataType.sName, ".", "::", true);

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
      std::string::size_type nPosAddrBegin = nPosBegin;

      // address
      nPosBegin = sNamespace.find("/", nPosBegin);
      if (nPosBegin == std::string::npos)
      {
        return m_sRootNamespace;
      }

      if (m_pParseSettings->mEnv.count("javans"))
      {
        // use java-style namespace parsing: model.myproject.org => ::org::myproject::model
        std::string sAddress = sNamespace.substr(nPosAddrBegin, nPosBegin - nPosAddrBegin);
        std::string::size_type nPos = sAddress.find(':', nPosBegin);
        // remove port
        if (nPos != std::string::npos)
        {
          sAddress.erase(nPos);
        }

        // invert namespace sequence
        nPosBegin = 0;
        for (;;)
        {
          nPos = sAddress.find('.', nPosBegin);
          if (nPos != std::string::npos)
          {
            sCppNamespace = sAddress.substr(nPosBegin, nPos - nPosBegin) + "::" + sCppNamespace;
          }
          else
          {
            sCppNamespace = sAddress.substr(nPosBegin) + "::" + sCppNamespace;
            break;
          }
          nPosBegin = nPos + 1;
        }

        return m_sRootNamespace + sCppNamespace;
      }

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

      sCppNamespace = sNamespace.substr(nPosBegin);

      StringReplace(sCppNamespace, ".", "::", true);

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
        for (std::list<Typedef>::const_iterator pTypedef = plsTypedefs->begin();
             pTypedef != plsTypedefs->end(); ++pTypedef)
        {
          setGlobalEnumValues.insert(pTypedef->sName);
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
              LogWarning() << "conflicting enum names [" << itMember->sName
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

  void WsdlTypes::Parse(xml::Element& rElemWsdl, Project& rProject, Interface& rInterface)
  {
    // import all before parsing schema
    // import xsd in definitions
    ImportAll(rElemWsdl, rProject, rInterface);

    // import xsd in types
    for (xml::Element* pTypes = rElemWsdl.FindChildElementByName("types");
        pTypes; pTypes = rElemWsdl.FindChildElementByName("types", pTypes->GetNextSiblingElement()))
    {
      ImportAll(*pTypes, rProject, rInterface);
      for (xml::Element* pElemChild = pTypes->FindChildElementByName("schema");
        pElemChild; pElemChild = pTypes->FindChildElementByName("schema",
                                                                pElemChild->GetNextSiblingElement()))
      {
        ImportAll(*pElemChild, rProject, rInterface);
      }
    }

    for(xml::Element* pTypes = rElemWsdl.FindChildElementByName("types");
        pTypes; pTypes = rElemWsdl.FindChildElementByName("types", pTypes->GetNextSiblingElement()))
    {
      for(xml::Element* pElemChild = pTypes->FindChildElementByName("schema");
        pElemChild; pElemChild = pTypes->FindChildElementByName("schema",
                                                                pElemChild->GetNextSiblingElement()))
      {
        ParseSchema(*pElemChild);
        GetInterfaceAttrs(*pElemChild, rInterface);
      }
    }
  }

  void WsdlTypes::GetInterfaceAttrs(const xml::Element& rSchema, Interface& rInterface)
  {
    for (const xml::Attribute* pAttr = rSchema.GetFirstAttribute();
         pAttr; pAttr = pAttr->GetNextSibling())
    {
      rInterface.mOptions[pAttr->GetName()] = pAttr->GetValue();
    }
  }

  void WsdlTypes::ParseSchema(const xml::Element& rSchema)
  {
    for (const xml::Element* pType = rSchema.GetFirstChildElement();
      pType; pType = pType->GetNextSiblingElement())
    {
      const std::string& sType = pType->GetName();
      if (sType == "element")
      {
        Element& rElement = *lsElements.insert(lsElements.end(), Element());
        rElement.Parse(*pType);
      }
      else
      if (sType == "complexType")
      {
        ComplexType& rComplexType = *lsComplexTypes.insert(lsComplexTypes.end(), ComplexType());
        rComplexType.Parse(*pType);
      }
      else
      if (sType == "simpleType")
      {
        SimpleType& rSimpleType = *lsSimpleTypes.insert(lsSimpleTypes.end(), SimpleType());
        rSimpleType.Parse(*pType);
      }
      else
      if (sType == "attribute")
      {
        Attribute& rAttr = *lsAttributes.insert(lsAttributes.end(), Attribute());
        rAttr.Parse(*pType);
      }
      else
      if (sType == "attributeGroup")
      {
        AttributeGroup stAttrGroup;
        stAttrGroup.Parse(*pType);
        mAttributeGroups[stAttrGroup.sNamespace + ":" + stAttrGroup.sName] = stAttrGroup;
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
        LogWarning() << "Unsupported schema node type: " << sType;
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
    for (typename std::list<Type>::const_iterator pType = rlsTypes.begin();
        pType != rlsTypes.end(); ++pType)
    {
      if (pType->sName == rBaseType.sName &&
          pType->sNamespace == rBaseType.sNamespace &&
          pType->sOwnerName == rBaseType.sOwnerName)
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
        rstStruct.mOptions = itStruct->mOptions;
        ImportEnums(itStruct->lsEnums, rstStruct.lsEnums);
        ImportStruct(itStruct->lsStructs, rstStruct.lsStructs);
      }
    }
  }

  void WsdlTypes::Import(xml::Element& rElemImport, Project& rProject, Interface& rInterface,
                         bool bInclude)
  {
    std::string sImportNs;
    const xml::Attribute* pNamespace = rElemImport.FindAttribute("namespace");
    if (pNamespace)
    {
      sImportNs = pNamespace->GetValue();
    }

    std::string sSchemaLocation;
    const xml::Attribute* pLocation = rElemImport.FindAttribute("location");
    if (pLocation)
    {
      sSchemaLocation = pLocation->GetValue();
    }
    else
    {
      pLocation = rElemImport.FindAttribute("schemaLocation");
      if (!pLocation)
      {
        return;
      }

      // says to import given namespace into current namespace
      m_setNsAliases.insert(sImportNs);
      sSchemaLocation = pLocation->GetValue();
    }

    const std::string& sImportHash = sImportNs + ":" + sSchemaLocation;
    if (!bInclude)
    {
      if (m_setImports.count(sImportHash))
      {
        LogDebug() << "[" << sImportHash << "] is already imported";
        return;
      }
      m_setImports.insert(sImportHash);
    }
    else
    {
      if (m_setIncludes.count(sImportHash))
      {
        LogDebug() << "[" << sImportHash << "] is already included";
        return;
      }
      m_setIncludes.insert(sImportHash);
    }

    STAFF_ASSERT_PARAM(m_pParser);

    if (!bInclude && m_setNsAliases.count(sImportNs) != 0)
    { // import into current namespace
      static int nUnnamedNsNum = 0;
      LogDebug2() << "Namespace: " << sImportNs << " is alias for current";

      const std::string& sPrefix = "ns" + ToString(++nUnnamedNsNum);

      // insert imported namespaces into current definitions
      xml::Element* pElemDefinitions = &rElemImport;
      for (; pElemDefinitions != NULL; pElemDefinitions = pElemDefinitions->GetParent())
      {
        if (pElemDefinitions->GetName() == "definitions" || pElemDefinitions->GetName() == "schema")
        {
          break;
        }
      }
      STAFF_ASSERT(pElemDefinitions, "Can't find definitions node in [" + rInterface.sFileName + "]");

      pElemDefinitions->DeclareNamespace(sImportNs, sPrefix);
    }

    std::string sImportNsPrefix;

    if (bInclude)
    {
      LogDebug2() << "Including [" << sSchemaLocation
                        << "] from: [" << rInterface.sFileName << "]";
    }
    else
    {
      GetNsPrefix(rElemImport, sImportNs, sImportNsPrefix);
      LogDebug2() << "Importing [" << sSchemaLocation << "] into namespace: [" << sImportNs
                        << "] from: [" << rInterface.sFileName << "]";
    }

    WsdlParserImpl& rWsdlParser = WsdlParsers::Parser(sSchemaLocation);
    Interface* pNewInterface = NULL;

    if (!rWsdlParser.IsInit())
    {
      rWsdlParser.Init(m_pParser->GetParseSettings());
      pNewInterface = &rWsdlParser.Parse(m_pParser->GetParseSettings().sInDir + sSchemaLocation,
                                         rProject, &rInterface);
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
    for (AttributeList::const_iterator pAttr = rmImpAttrs.begin(); pAttr != rmImpAttrs.end(); ++pAttr)
    {
      Attribute& rAttr = *lsAttributes.insert(lsAttributes.end(), *pAttr);
      if (!bInclude)
      {
        rAttr.sPrefix = sImportNsPrefix;
      }
    }

    ImportEnums(pNewInterface->lsEnums, rInterface.lsEnums);
    ImportStruct(pNewInterface->lsStructs, rInterface.lsStructs);

    // use extern typedefs
    for (std::list<Typedef>::const_iterator pTypedef = pNewInterface->lsTypedefs.begin();
        pTypedef != pNewInterface->lsTypedefs.end(); ++pTypedef)
    {
      if (!IsBaseTypeExists(*pTypedef, rInterface.lsTypedefs))
      {
        Typedef stTypedef = *pTypedef;
        stTypedef.sName = pTypedef->sName;
        stTypedef.sNamespace = pTypedef->sNamespace;
        stTypedef.sDescr = pTypedef->sDescr;
        stTypedef.sDetail = pTypedef->sDetail;
        stTypedef.bExtern = true;
        rInterface.lsTypedefs.push_back(stTypedef);
      }
    }

    // insert imported namespaces into current definitions or xsd's schema element
    xml::Element* pElemDefinitions = &rElemImport;
    for (; pElemDefinitions; pElemDefinitions = pElemDefinitions->GetParent())
    {
      if (pElemDefinitions->GetName() == "definitions" ||
          (pElemDefinitions->GetName() == "schema" && !pElemDefinitions->GetParent()))
      {
        break;
      }
    }
    STAFF_ASSERT(pElemDefinitions, "Can't find definitions node in [" + rInterface.sFileName + "]");


    const xml::Element& rImportedDeclElement = DeclarationsCache::mCache[sSchemaLocation];

    for (const xml::Namespace* pImpNs = rImportedDeclElement.GetFirstNamespace();
        pImpNs; pImpNs = pImpNs->GetNextSibling())
    {
      bool bFound = false;
      for (const xml::Namespace* pNs = pElemDefinitions->GetFirstNamespace();
           pNs; pNs = pNs->GetNextSibling())
      {
        if (pNs->GetPrefix() == pImpNs->GetPrefix())
        {
          bFound = true;
          break;
        }
      }

      if (!bFound && !pImpNs->GetPrefix().empty())
      {
        pElemDefinitions->DeclareNamespace(*pImpNs);
      }
    }

    LogDebug2() << (bInclude ? "Including" : "Importing") << " [" << sSchemaLocation << "] complete";
  }

  void WsdlTypes::ImportAll(xml::Element& rElem, Project& rProject, Interface& rInterface)
  {
    for (xml::Element* pElemChild = rElem.GetFirstChildElement();
      pElemChild; pElemChild = pElemChild->GetNextSiblingElement())
    {
      if (pElemChild->GetName() == "import")
      {
        Import(*pElemChild, rProject, rInterface, false);
      }
      else
      if (pElemChild->GetName() == "include")
      {
        Import(*pElemChild, rProject, rInterface, true);
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

        LogDebug() << "Processing: " << *itFileName;

        rWsdlParser.Parse(sFileName, rProject);
      }
      else
      {
        LogDebug() << *itFileName << " already processed";
      }
    }

    for (std::list<Interface>::iterator itInterface = rProject.lsInterfaces.begin();
         itInterface != rProject.lsInterfaces.end(); ++itInterface)
    {
      for (std::list<Class>::iterator itClass = itInterface->lsClasses.begin();
           itClass != itInterface->lsClasses.end(); ++itClass)
      {
        // check service class name for clashes with datatypes defined
        while (GetBaseType(itClass->sName, *itInterface))
        {
          itClass->sName += "_";
        }
      }
    }
  }

  const std::string WsdlParser::m_sId = "wsdl";
}
}
