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
#include "WsdlParser.h"

RISE_DECLARE_PLUGIN(staff::CWsdlParser);

namespace staff
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

    SQName()
    {
    }

    SQName(const std::string& sNameIn, const std::string& sPrefixIn, const std::string& sNamespaceIn):
      sName(sNameIn), sPrefix(sPrefixIn), sNamespace(sNamespaceIn)
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
  struct SElement: public SQName
  {
    SQName stType;
    bool bIsArray;

    std::list<SSimpleType> lsSimpleTypes;
    std::list<SComplexType> lsComplexTypes;

    SElement();

    void FromType(const rise::xml::CXMLNode& rNodeElement, const std::string& sElemType);
    SElement& Parse(const rise::xml::CXMLNode& rNodeElement);

  private:
    bool GetType(const rise::xml::CXMLNode& rElement, std::string& sCppTypeName);

  private:
    static int m_nUnnamedElementId;
  };

  //////////////////////////////////////////////////////////////////////////
  struct SSimpleType: public SQName
  {
    std::string sBaseType;

    SSimpleType();
    SSimpleType& Parse(const rise::xml::CXMLNode& rNodeSimpleType);

  private:
    static int m_nUnnamedSimpleTypeId;
  };

  //////////////////////////////////////////////////////////////////////////
  struct SComplexType: public SQName
  {
    std::string sElementName;
    std::list<SElement> lsElements;
    std::string sParentName;
    std::string sParentTns;

    SComplexType();

    SComplexType& Parse(const rise::xml::CXMLNode& rNodeComplexType);

  private:
    void ParseSequence(const rise::xml::CXMLNode& rNodeSequence);

    void ParseComplexContent(const rise::xml::CXMLNode& rNodeComplexContent);
    static int m_nUnnamedComplexTypeId;
  };

  //////////////////////////////////////////////////////////////////////////
  std::string FindNamespaceUri( const rise::xml::CXMLNode& rNode, const std::string& sPrefix )
  {
    for (const rise::xml::CXMLNode* pNode = &rNode; pNode != NULL; pNode = pNode->GetParent())
    {
      if (!sPrefix.empty())
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
      else
      { // find default namespace
        for (rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = pNode->AttrBegin();
          itAttr != pNode->AttrEnd(); ++itAttr)
        {
          if (itAttr->sAttrName == "xmlns")
          {
            return itAttr->sAttrValue;
          }
        }
      }
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

  void ReadDescr(const rise::xml::CXMLNode& rNode, std::string& sDescr)
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
      sDescr = itDoc->NodeContent().AsString();
    }
  }


  //////////////////////////////////////////////////////////////////////////
  SElement::SElement():
    bIsArray(false)
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

    sName = (itAttr != rNodeElement.AttrEnd()) ? itAttr->sAttrValue.AsString() :
      ("UnnamedElement" + rise::ToStr(++m_nUnnamedElementId));

    GetTns(rNodeElement, sNamespace, sPrefix);

    ReadDescr(rNodeElement, sDescr);

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
      {
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
              SComplexType stComplexType;
              stComplexType.sElementName = sName;
              stComplexType.Parse(*itChild);
              lsComplexTypes.push_back(stComplexType);
            }
            else
            if (sNodeName == "simpleType")
            {
              SSimpleType stSimpleType;
              stSimpleType.Parse(*itChild);
              lsSimpleTypes.push_back(stSimpleType);
            }
          }
        }//for
      }
    }

    return *this;
  }


  int SElement::m_nUnnamedElementId = 0;


  //////////////////////////////////////////////////////////////////////////
  SSimpleType& SSimpleType::Parse(const rise::xml::CXMLNode& rNodeSimpleType)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeSimpleType.FindAttribute("name");

    sName = (itAttr != rNodeSimpleType.AttrEnd()) ? itAttr->sAttrValue.AsString() :
      ("UnnamedSimpleType" + rise::ToStr(++m_nUnnamedSimpleTypeId));

    GetTns(rNodeSimpleType, sNamespace, sPrefix);

    ReadDescr(rNodeSimpleType, sDescr);

    for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = rNodeSimpleType.NodeBegin();
      itChild != rNodeSimpleType.NodeEnd(); ++itChild)
    {
      if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sNodeName = itChild->NodeName();
        if (sNodeName == "restriction")
        {
          sBaseType = itChild->Attribute("base").AsString();
        }
      }
    }

    return *this;
  }

  SSimpleType::SSimpleType()
  {
  }

  int SSimpleType::m_nUnnamedSimpleTypeId = 0;


  //////////////////////////////////////////////////////////////////////////
  SComplexType::SComplexType()
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
      if (sElementName.empty())
      {
        sName = ("UnnamedComplexType" + rise::ToStr(++m_nUnnamedComplexTypeId));
      }
      else
      {
        sName = sElementName;
      }
    }

    GetTns(rNodeComplexType, sNamespace, sPrefix);

    ReadDescr(rNodeComplexType, sDescr);

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
          ParseComplexContent(*itChild);
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
      }
    }
  }

  void SComplexType::ParseComplexContent( const rise::xml::CXMLNode& rNodeComplexContent )
  {
    rise::xml::CXMLNode::TXMLNodeConstIterator itNodeExtension = rNodeComplexContent.FindSubnode("extension");

    if (itNodeExtension != rNodeComplexContent.NodeEnd())
    {
      sParentName = itNodeExtension->Attribute("base").AsString();
      sParentTns = FindNamespaceUri(rNodeComplexContent, GetPrefix(sParentName));

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

  int SComplexType::m_nUnnamedComplexTypeId = 0;

  //////////////////////////////////////////////////////////////////////////
  typedef std::map<std::string, SElement> TElementMap;
  typedef std::map<std::string, SComplexType> TComplexTypeMap;
  typedef std::map<std::string, SSimpleType> TSimpleTypeMap;

  class CWsdlParserImpl;

  struct SWsdlTypes
  {
    TElementMap mElements;
    TComplexTypeMap mComplexTypes;
    TSimpleTypeMap mSimpleTypes;
    std::set<std::string> m_setNsAliases;
    CWsdlParserImpl* m_pParser;

    SWsdlTypes(CWsdlParserImpl* pParser);
    void Parse(rise::xml::CXMLNode& rNodeWsdl, SProject& rProject, SInterface& rInterface);
    void ParseSchema(const rise::xml::CXMLNode& rSchema);
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
    }

    void ParsePartElement(const SElement& rElement, SMember& rMember, bool bIsResponse/*, const std::string& sElementNsName*/, int nRecursionLevel = 0)
    {
      // parse element operation parameters
      // declared as complex type
      if (!rElement.lsComplexTypes.empty())
      {
        // complex type
        for (std::list<SComplexType>::const_iterator itParamType = rElement.lsComplexTypes.begin();
          itParamType != rElement.lsComplexTypes.end(); ++itParamType)
        {
          if (bIsResponse && itParamType->lsElements.empty())
          {
            rMember.stReturn.stDataType.sName = "void";
          }
          else
          {
            for (std::list<SElement>::const_iterator itParamElement = itParamType->lsElements.begin();
              itParamElement != itParamType->lsElements.end(); ++itParamElement)
            {
              const SElement& rChildElement = *itParamElement;

              if (!rChildElement.stType.sName.empty()) // type is set: processing elem as struct
              {
                SParam stParam; // struct member

  //              stParam.sName = bIsResponse ? StripPrefix(sElementNsName) : rElem.sName;
                stParam.sName = rChildElement.sName;

                GetCppType(rChildElement.stType, stParam.stDataType);
                stParam.stDataType.sNodeName = rChildElement.sName;
                stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;
                OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);

                if (!bIsResponse)
                {
                  FixParamDataType(stParam.stDataType);
                  rMember.lsParamList.push_back(stParam);
                }
                else
                {
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
                  FixParamDataType(rMember.stReturn.stDataType);
                }
              }
              else // type is not set: processing as inline complex or simple type
              {
                if (bIsResponse && nRecursionLevel == 0)
                {
                  rMember.mOptions["responseElement"] = rElement.sName;
                }
                ParsePartElement(rChildElement, rMember, bIsResponse, /*rElem.sName, */nRecursionLevel + 1);
              }
            }
          }
        }
      }
      else
      { // simple/extern type
        SParam stParam;

//        stParam.sName = bIsResponse ? StripPrefix(sElementNsName) : rElement.sName;
        stParam.sName = rElement.sName;
        rise::LogInfo() << "response: " << rElement.sName;

        GetCppType(rElement.stType, stParam.stDataType);
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;
        OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);
        if (!bIsResponse)
        {  // set node name for request
          stParam.stDataType.sNodeName = rElement.sName;
          FixParamDataType(stParam.stDataType);
          rMember.lsParamList.push_back(stParam);
        }
        else
        {
          if (nRecursionLevel == 0)
          {
            rMember.mOptions["responseElement"] = rElement.sName;
          }
          rMember.stReturn = stParam;
          rMember.stReturn.stDataType.sNodeName = rElement.sName;
          FixParamDataType(rMember.stReturn.stDataType);
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

        ParsePartElement(itElement->second, rMember, bIsResponse);//, sElementNsName);
      }
      else
      { // inline part declaration
        const std::string& sPartName = rPart.Attribute("name").AsString();
        rise::xml::CXMLNode::TXMLAttrConstIterator itType = rPart.FindAttribute("type");
        RISE_ASSERTES(itType != rPart.AttrEnd(), rise::CLogicNoItemException,
          "Unknown part type: " + sPartName);

        SParam stParam;
        stParam.sName = StripPrefix(sPartName);
        stParam.stDataType.sNodeName = stParam.sName;

        const std::string& sPartPrefix = GetPrefix(sPartName);
        const std::string& sNamespace = sPartPrefix.empty() ? GetTns(rPart) : FindNamespaceUri(rPart, sPartPrefix);

        GetCppType(itType->sAttrValue.AsString(), sNamespace, stParam.stDataType);
        RISE_ASSERTES(!stParam.stDataType.sName.empty(), rise::CLogicNoItemException, "Unknown part type");
        stParam.stDataType.sNodeName = stParam.stDataType.sName;
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;
        OptimizeCppNs(stParam.stDataType.sUsedName, m_stInterface.sNamespace);

        FixParamDataType(stParam.stDataType);

        if (!bIsResponse)
        {
          rMember.lsParamList.push_back(stParam);
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
      }
      else
      { // one way message
        rMember.stReturn.stDataType.sName = "void";
      }

      ReadDescr(rOperation, rMember.sDescr);
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
          rClass.lsMember.push_back(tOperationMember);
        }
      }

      ReadDescr(rService, rClass.sDescr);

      rClass.sNamespace = TnsToCppNs(GetTns(rDefs));
    }

    void WriteInlineTypes(const SElement& rElement, const SWsdlTypes& rWsdlTypes)
    {
      for (std::list<SComplexType>::const_iterator itComplex = rElement.lsComplexTypes.begin();
        itComplex != rElement.lsComplexTypes.end(); ++itComplex)
      {
        for (std::list<SElement>::const_iterator itElement = itComplex->lsElements.begin();
          itElement != itComplex->lsElements.end(); ++itElement)
        {
          SDataType stDt;
          ElementToData(*itElement, stDt, rWsdlTypes);
          WriteInlineTypes(*itElement, rWsdlTypes);
        }
      }
    }

    static bool SortStructByParent(const SStruct& rStruct1, const SStruct& rStruct2)
    {
      return rStruct1.sName == rStruct2.sParentName;
    }

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
        m_stWsdlTypes.ParseSchema(rRootNode);
      }

      for (TComplexTypeMap::const_iterator itComplex = m_stWsdlTypes.mComplexTypes.begin();
            itComplex != m_stWsdlTypes.mComplexTypes.end(); ++itComplex)
      {
        ComplexTypeToData(itComplex->second, m_stWsdlTypes);
      }

      for (TSimpleTypeMap::const_iterator itSimple = m_stWsdlTypes.mSimpleTypes.begin();
        itSimple != m_stWsdlTypes.mSimpleTypes.end(); ++itSimple)
      {
        SimpleTypeToData(itSimple->second, m_stWsdlTypes);
      }

      for (TElementMap::const_iterator itElement = m_stWsdlTypes.mElements.begin();
        itElement != m_stWsdlTypes.mElements.end(); ++itElement)
      {
        WriteInlineTypes(itElement->second, m_stWsdlTypes);
      }

      m_stInterface.lsStruct.sort(SortStructByParent);

      if (!bSchema)
      {
        SClass tServiceClass;

        ParseService(tServiceClass, rRootNode, m_stWsdlTypes);
        tServiceClass.mOptions["targetNamespace"] = m_stInterface.sTargetNs;
        m_stInterface.lsClass.push_back(tServiceClass);
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

      // parse new interface
      {
        rise::xml::CXMLDocument tWsdlDoc;
        rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();
        tWsdlDoc.LoadFromFile(sFileName);

        m_stInterface.sTargetNs = rDefs.Attribute("targetNamespace").AsString();

        // fill in interface name
        m_stInterface.sFileName = sInterfaceFileName;
        m_stInterface.sName = sInterfaceFileName;

        m_stInterface.sNamespace = TnsToCppNs(m_stInterface.sTargetNs);

        nPos = m_stInterface.sName.find_last_of('.');
        if (nPos != std::string::npos)
        {
          m_stInterface.sName.erase(nPos);
        }

        nPos = m_stInterface.sName.find_last_of('.');
        if (nPos != std::string::npos)
        {
          m_stInterface.sName.erase(0, nPos + 1);
        }

        ParseInterface(rDefs, rProject);

        rProject.lsInterfaces.push_back(m_stInterface);

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
          rDataType.sName = sDataType;
          rDataType.sUsedName = sDataType;
          rise::LogWarning() << "Can't find element type declaration: " << sDataType;
        }
      }
    }

    SDataType SimpleTypeToData(const SSimpleType& rSimpleType, const SWsdlTypes& rWsdlTypes)
    {
      SDataType stDataType;
      std::list<STypedef>::const_iterator itTypedef = m_stInterface.lsTypedef.begin();
      for (; itTypedef != m_stInterface.lsTypedef.end(); ++itTypedef)
      {
        if (itTypedef->sName == rSimpleType.sName)
        {
          break;
        }
      }

      if (itTypedef != m_stInterface.lsTypedef.end())
      {
        stDataType = itTypedef->stDataType;
      }
      else
      {
        STypedef stTypedef;
        stTypedef.sName = rSimpleType.sName;
        stTypedef.sNamespace = TnsToCppNs(rSimpleType.sNamespace);
        stTypedef.sDescr = rSimpleType.sDescr;

        if (stTypedef.sNamespace.empty())
        {
          stDataType.sName = rSimpleType.sBaseType;
          stDataType.sUsedName = rSimpleType.sBaseType;
          stTypedef.stDataType.sName = rSimpleType.sBaseType;
          stTypedef.stDataType.sUsedName = rSimpleType.sBaseType;
        }
        else
        {
          DataTypeFromName(rSimpleType.sBaseType, stTypedef.stDataType, rWsdlTypes);
        }

        m_stInterface.lsTypedef.push_back(stTypedef);
      }

      return stDataType;
    }

    SDataType ComplexTypeToData(const SComplexType& rComplexType, const SWsdlTypes& rWsdlTypes)
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
        if (rComplexType.lsElements.front().bIsArray)
        {
          STypedef stTypedef;
          stTypedef.sName = stDataType.sName;
          stTypedef.sNamespace = stDataType.sNamespace;
          stTypedef.sDescr = rComplexType.sDescr;

          ElementToData(rComplexType.lsElements.front(), stTypedef.stDataType, rWsdlTypes);

          m_stInterface.lsTypedef.push_back(stTypedef);
          return stTypedef.stDataType;
        }
      }

      {
        SStruct stStruct;
        stStruct.bForward = false;
        stStruct.sName = stDataType.sName;
        stStruct.sNamespace = stDataType.sNamespace;
        stStruct.sDescr = rComplexType.sDescr;

        // inheritance
        stStruct.sParentName = StripPrefix(rComplexType.sParentName);
        if (!stStruct.sParentName.empty())
        {
          stStruct.sParentNamespace = TnsToCppNs(rComplexType.sParentTns);
          if (!stStruct.sParentNamespace.empty())
          {
            stStruct.sParentName = stStruct.sParentNamespace + stStruct.sParentName;
            OptimizeCppNs(stStruct.sParentName, m_stInterface.sNamespace);
          }
        }

        for (std::list<SElement>::const_iterator itElement = rComplexType.lsElements.begin();
          itElement != rComplexType.lsElements.end(); ++itElement)
        {
          SParam stMember;
          stMember.sName = StripPrefix(itElement->sName);
          ElementToData(*itElement, stMember.stDataType, rWsdlTypes);
          stMember.sDescr = itElement->sDescr;
          stStruct.lsMember.push_back(stMember);
        }

        stDataType.eType = SDataType::EStruct;
        stDataType.sUsedName = stStruct.sNamespace + stStruct.sName;
        OptimizeCppNs(stDataType.sUsedName, m_stInterface.sNamespace);

        m_stInterface.lsStruct.push_back(stStruct);
      }

      return stDataType;
    }

    void ElementToData(const SElement& rElement, SDataType& rDataType, const SWsdlTypes& rWsdlTypes)
    {
      if (rElement.bIsArray)
      {
        SDataType stTempl;
        GetCppType(rElement.stType, stTempl);
        stTempl.sNodeName = rElement.sName;
        stTempl.sUsedName = stTempl.sNamespace + stTempl.sName;
        OptimizeCppNs(stTempl.sUsedName, m_stInterface.sNamespace);

        rDataType.sName = "list";
        rDataType.sNamespace = "std::";
        rDataType.sUsedName = "std::list";
        rDataType.eType = SDataType::ETemplate;
        rDataType.lsParams.push_back(stTempl);
      }
      else
      {
        GetCppType(rElement.stType, rDataType);
        rDataType.sNodeName = rElement.sName;
        rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
        OptimizeCppNs(rDataType.sUsedName, m_stInterface.sNamespace);
      }

      if (!rElement.stType.sName.empty()) // complex/simple type
      {
        for (std::list<SSimpleType>::const_iterator itSimpleSubtype = rElement.lsSimpleTypes.begin();
          itSimpleSubtype != rElement.lsSimpleTypes.end(); ++itSimpleSubtype)
        {
          SimpleTypeToData(*itSimpleSubtype, rWsdlTypes);
        }

        for (std::list<SComplexType>::const_iterator itComplexSubtype = rElement.lsComplexTypes.begin();
          itComplexSubtype != rElement.lsComplexTypes.end(); ++itComplexSubtype)
        {
          ComplexTypeToData(*itComplexSubtype, rWsdlTypes);
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
      }
      else
      if (stQName.sName == "DataObject") // non xsd:any, may have additional schema
      {
        rDataType.sName = "CDataObject";
        rDataType.sNamespace = "staff::";
        rDataType.eType = SDataType::EDataObject;
      }
      else
      { // not an wsdl type, some typedef or struct
        rDataType.sName = stQName.sName;
        rDataType.sNamespace = TnsToCppNs(stQName.sNamespace);

        for (std::list<STypedef>::const_iterator itTypedef = m_stInterface.lsTypedef.begin();
            itTypedef != m_stInterface.lsTypedef.end(); ++itTypedef)
        {
          if (itTypedef->sName == rDataType.sName &&
              itTypedef->sNamespace == rDataType.sNamespace)
          {
            rDataType.eType = SDataType::ETypedef;
            return;
          }
        }

        for (std::list<SStruct>::const_iterator itStruct = m_stInterface.lsStruct.begin();
             itStruct != m_stInterface.lsStruct.end(); ++itStruct)
        {
          if (itStruct->sName == rDataType.sName &&
              itStruct->sNamespace == rDataType.sNamespace)
          {
            rDataType.eType = SDataType::EStruct;
            return;
          }
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

      std::string sResult;

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
      if (nPosEnd == std::string::npos || nPosEnd < nPosBegin)
      {
        return "";
      }

      sResult = sNamespace.substr(nPosBegin, nPosEnd - nPosBegin);

      rise::StrReplace(sResult, ".", "::");

      return "::" + sResult + "::";
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

    void OptimizeCppNs(std::string& sOptimizeNs, const std::string& sCurrentNs)
    {
      // sOptimizeNs: ::samples::ticket::
      // sCurrentNs: ::samples::sharedtypes::
      // result: ticket::

      std::string::size_type nPos = 0;
      for (; nPos < sOptimizeNs.size() &&
             nPos < sCurrentNs.size() &&
             sOptimizeNs[nPos] == sCurrentNs[nPos]; ++nPos);

      if (nPos > 2)
      {
        sOptimizeNs.erase(0, nPos);
      }
    }

  private:
    SInterface m_stInterface;
    SWsdlTypes m_stWsdlTypes;
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
        if (sType == "import")
        { // already imported
        }
        else
        if (sType == "annotation")
        { // documentation
        }
        else
        {
          rise::LogWarning() << "Unknown node type: " << sType;
        }
      }
    }
  }

  void ReplacePrefix(SElement& rElement, const std::string& sNamespace, const std::string& sPrefix);

  void ReplacePrefix(SComplexType& rComplexType, const std::string& sNamespace, const std::string& sPrefix)
  {
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

  void ReplacePrefix(SElement& rElement, const std::string& sNamespace, const std::string& sPrefix)
  {
    if (rElement.sNamespace == sNamespace)
    {
      rElement.sPrefix = sPrefix;
    }

    for (std::list<SSimpleType>::iterator itSimpleType = rElement.lsSimpleTypes.begin();
        itSimpleType != rElement.lsSimpleTypes.end(); ++itSimpleType)
    {
      if (itSimpleType->sNamespace == sNamespace)
      {
        itSimpleType->sPrefix = sPrefix;
      }
    }

    for (std::list<SComplexType>::iterator itComplexType = rElement.lsComplexTypes.begin();
        itComplexType != rElement.lsComplexTypes.end(); ++itComplexType)
    {
      ReplacePrefix(*itComplexType, sNamespace, sPrefix);
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
      rise::LogDebug2() << "Namespace: " << sImportNs << " is alias for current";
      try
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
      }

      return;
    }

    std::string sImportNsPrefix;
    GetNsPrefix(rNodeImport, sImportNs, sImportNsPrefix);

    rise::LogDebug2() << "Importing: " << sShemaLocation << " into namespace: " << sImportNs;

    CWsdlParserImpl tWsdlParser(m_pParser->GetParseSettings());
    try
    {
      SInterface& rNewInterface = tWsdlParser.ParseFile(sShemaLocation, rProject);

      // import operations
      const SWsdlTypes& rImportedWsdlTypes = tWsdlParser.GetWsdlTypes();
      for (TElementMap::const_iterator itElement = rImportedWsdlTypes.mElements.begin();
          itElement != rImportedWsdlTypes.mElements.end(); ++itElement)
      {
        SElement& rElement = mElements[sImportNsPrefix + ":" + itElement->second.sName];
        rElement = itElement->second;
        ReplacePrefix(rElement, sImportNs, sImportNsPrefix);
      }

      // use extern structs
      for (std::list<SStruct>::const_iterator itStruct = rNewInterface.lsStruct.begin();
          itStruct != rNewInterface.lsStruct.end(); ++itStruct)
      {
        SStruct stStruct;
        stStruct.sName = itStruct->sName;
        stStruct.sNamespace = itStruct->sNamespace;
        stStruct.sParentName = itStruct->sParentName;
        stStruct.sDescr = itStruct->sDescr;
        stStruct.sDetail = itStruct->sDetail;
        stStruct.bExtern = true;
        rInterface.lsStruct.push_back(stStruct);
      }

      // use extern typedefs
      for (std::list<STypedef>::const_iterator itTypedef = rNewInterface.lsTypedef.begin();
          itTypedef != rNewInterface.lsTypedef.end(); ++itTypedef)
      {
        STypedef stTypedef = *itTypedef;
        stTypedef.sName = itTypedef->sName;
        stTypedef.sNamespace = itTypedef->sNamespace;
        stTypedef.sDescr = itTypedef->sDescr;
        stTypedef.bExtern = true;
        rInterface.lsTypedef.push_back(stTypedef);
      }

      SInclude stInclude;
      stInclude.sInterfaceName = rNewInterface.sName;
      stInclude.sNamespace = rNewInterface.sNamespace;
      stInclude.sFileName = rNewInterface.sFileName;
      stInclude.sTargetNs = rNewInterface.sTargetNs;
      rInterface.lsInclude.push_back(stInclude);

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
