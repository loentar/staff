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
  struct SSimpleType;
  struct SComplexType;

  //////////////////////////////////////////////////////////////////////////
  struct SElement
  {
    std::string sName;
    std::string sDescr;
    bool bIsArray;
    std::string sType;
    bool bIsXmlType;

    std::list<SSimpleType> lsSimpleTypes;
    std::list<SComplexType> lsComplexTypes;

    SElement();

    SElement& Parse(const rise::xml::CXMLNode& rNodeElement);

  private:
    bool GetType(const rise::xml::CXMLNode& rElement, std::string& sCppTypeName, bool& bIsXmlType);

  private:
    static int m_nUnnamedElementId;
  };

  //////////////////////////////////////////////////////////////////////////
  struct SSimpleType
  {
    std::string sName;
    std::string sDescr;
    std::string sBaseType;
    bool bIsXmlType;

    SSimpleType();
    SSimpleType& Parse(const rise::xml::CXMLNode& rNodeSimpleType);
  private:
    static int m_nUnnamedSimpleTypeId;
  };

  //////////////////////////////////////////////////////////////////////////
  struct SComplexType
  {
    std::string sName;
    std::string sDescr;
    std::string sElementName;
    std::list<SElement> lsElements;
    std::string sParentType;
    bool bParentIsXmlType;

    SComplexType();

    SComplexType& Parse(const rise::xml::CXMLNode& rNodeComplexType);

  private:
    void ParseSequence(const rise::xml::CXMLNode& rNodeSequence);

    void ParseComplexContent(const rise::xml::CXMLNode& rNodeComplexContent);
    static int m_nUnnamedComplexTypeId;
  };

  //////////////////////////////////////////////////////////////////////////
  const rise::xml::SXMLNamespace* FindNamespace( const rise::xml::CXMLNode& rNode, const std::string& sPrefix )
  {
    for(const rise::xml::CXMLNode* pNode = &rNode; pNode != NULL; pNode = pNode->GetParent())
    {
      const rise::xml::CXMLNode::TXMLNsList& rNsList = pNode->GetNsList();
      for (rise::xml::CXMLNode::TXMLNsList::const_iterator itNs = rNsList.begin();
        itNs != rNsList.end(); ++itNs)
      {
        if (itNs->sNs == sPrefix)
        {
          return &*itNs;
        }
      }
    }

    return NULL;
  }

  void GetType( const rise::xml::CXMLNode& rElement, const std::string& sAttrTypeName,
                         std::string& sCppTypeName, bool& bIsXmlType )
  {
    std::string::size_type nPos = sAttrTypeName.find_last_of(':');
    std::string sPrefix = (nPos != std::string::npos) ? sAttrTypeName.substr(0, nPos) : "";
    std::string sTypeName = (nPos != std::string::npos) ? sAttrTypeName.substr(nPos + 1) : sAttrTypeName;

    const rise::xml::SXMLNamespace* pElementNs = FindNamespace(rElement, sPrefix);
    RISE_ASSERTS(pElementNs, "Namespace declaration is not found for prefix [" + sPrefix + "]");

    bIsXmlType = pElementNs->sUri == "http://www.w3.org/2001/XMLSchema" ||
                 pElementNs->sUri == "http://schemas.xmlsoap.org/wsdl/";

    if (bIsXmlType)
    {
      if (sTypeName == "string")
      {
        sCppTypeName = "std::string";
      }
      else
      if (sTypeName == "decimal" ||
        sTypeName == "duration" ||
        sTypeName == "dateTime" ||
        sTypeName == "time" ||
        sTypeName == "date" ||
        sTypeName == "gYearMonth" ||
        sTypeName == "gYear" ||
        sTypeName == "gMonthDay" ||
        sTypeName == "gDay" ||
        sTypeName == "gMonth" ||
        sTypeName == "hexBinary" ||
        sTypeName == "base64Binary" ||
        sTypeName == "anyURI" ||
        sTypeName == "QName" ||
        sTypeName == "NOTATION" ||
        sTypeName == "normalizedString" ||
        sTypeName == "token" ||
        sTypeName == "language" ||
        sTypeName == "IDREFS" ||
        sTypeName == "ENTITIES" ||
        sTypeName == "NMTOKEN" ||
        sTypeName == "NMTOKENS" ||
        sTypeName == "Name" ||
        sTypeName == "NCName" ||
        sTypeName == "ID" ||
        sTypeName == "IDREF" ||
        sTypeName == "ENTITY" ||
        sTypeName == "integer" ||
        sTypeName == "nonPositiveInteger" ||
        sTypeName == "negativeInteger" ||
        sTypeName == "nonNegativeInteger" ||
        sTypeName == "byte" ||
        sTypeName == "unsignedLong" ||
        sTypeName == "unsignedInt" ||
        sTypeName == "unsignedShort" ||
        sTypeName == "unsignedByte" ||
        sTypeName == "positiveInteger" ||
        sTypeName == "anySimpleType")
      {
        sCppTypeName = "staff::" + sTypeName;
      }
      else
      {
        sCppTypeName = sTypeName;
      }
    }
    else
    {
      sCppTypeName = sTypeName;
    }
  }

  std::string StripNamespace(const std::string& sSymbol)
  {
    std::string::size_type nPos = sSymbol.find_last_of(':');
    if (nPos != std::string::npos)
    {
      return sSymbol.substr(nPos + 1);
    }

    return sSymbol;
  }

  std::string GetNamespace(const std::string& sSymbol)
  {
    std::string::size_type nPos = sSymbol.find_last_of(':');
    if (nPos != std::string::npos)
    {
      return sSymbol.substr(0, nPos + 1);
    }

    return "";
  }

  void ReadDescr(const rise::xml::CXMLNode& rNode, std::string& sDescr)
  {
    rise::xml::CXMLNode::TXMLNodeConstIterator itDoc = rNode.FindSubnode("documentation");
    if (itDoc != rNode.NodeEnd())
    {
      sDescr = itDoc->NodeContent().AsString();
    }
  }


  //////////////////////////////////////////////////////////////////////////
  SElement::SElement():
    bIsArray(false), bIsXmlType(false)
  {

  }

  SElement& SElement::Parse( const rise::xml::CXMLNode& rNodeElement )
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeElement.FindAttribute("name");

    sName = (itAttr != rNodeElement.AttrEnd()) ? itAttr->sAttrValue.AsString() :
      ("UnnamedElement" + rise::ToStr(++m_nUnnamedElementId));

    ReadDescr(rNodeElement, sDescr);

    itAttr = rNodeElement.FindAttribute("maxOccurs");
    bIsArray = (itAttr != rNodeElement.AttrEnd()) && (itAttr->sAttrValue.AsString() == "unbounded");

    if (!GetType(rNodeElement, sType, bIsXmlType))
    {
      itAttr = rNodeElement.FindAttribute("ref");
      if (itAttr != rNodeElement.AttrEnd())
      {
        ::staff::GetType(rNodeElement, itAttr->sAttrValue.AsString(), sType, bIsXmlType);
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
              sType = 'S' + sName;
            }
            else
            if (sNodeName == "simpleType")
            {
              SSimpleType stSimpleType;
              stSimpleType.Parse(*itChild);
              lsSimpleTypes.push_back(stSimpleType);
              sType = 'T' + sName;
            }
          }
        }//for
      }
    }

    return *this;
  }

  bool SElement::GetType( const rise::xml::CXMLNode& rElement, std::string& sCppTypeName, bool& bIsXmlType )
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itType = rElement.FindAttribute("type");

    if (itType == rElement.AttrEnd())
    {
      bIsXmlType = false;
      return false;
    }

    ::staff::GetType(rElement, itType->sAttrValue, sCppTypeName, bIsXmlType);
    return true;
  }

  int SElement::m_nUnnamedElementId = 0;


  //////////////////////////////////////////////////////////////////////////
  SSimpleType& SSimpleType::Parse( const rise::xml::CXMLNode& rNodeSimpleType )
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeSimpleType.FindAttribute("name");

    sName = (itAttr != rNodeSimpleType.AttrEnd()) ? itAttr->sAttrValue.AsString() :
      ("UnnamedSimpleType" + rise::ToStr(++m_nUnnamedSimpleTypeId));

    ReadDescr(rNodeSimpleType, sDescr);

    for (rise::xml::CXMLNode::TXMLNodeConstIterator itChild = rNodeSimpleType.NodeBegin();
      itChild != rNodeSimpleType.NodeEnd(); ++itChild)
    {
      if (itChild->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
      {
        const std::string& sNodeName = itChild->NodeName();
        if (sNodeName == "restriction")
        {
          GetType(*itChild, itChild->Attribute("base").AsString(), sBaseType, bIsXmlType);
        }
      }
    }

    return *this;
  }

  SSimpleType::SSimpleType():
    bIsXmlType(false)
  {
  }

  int SSimpleType::m_nUnnamedSimpleTypeId = 0;


  //////////////////////////////////////////////////////////////////////////
  SComplexType::SComplexType():
    bParentIsXmlType(false)
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
      if (sElementName.size() == 0)
      {
        sName = ("UnnamedComplexType" + rise::ToStr(++m_nUnnamedComplexTypeId));
      }
      else
      {
        sName = 'S' + sElementName;
      }
    }

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
          lsElements.push_back(stElement);
        }
        else
        if (sNodeName == "any")
        {
          SElement stElement;
          stElement.Parse(*itChild);
          stElement.sType = "staff::CDataObject";
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
      GetType(rNodeComplexContent, itNodeExtension->Attribute("base").AsString(), sParentType, bParentIsXmlType);

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
        GetType(rNodeComplexContent, itNodeRestriction->Attribute("base").AsString(), sParentType, bParentIsXmlType);

        for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeAttr = itNodeRestriction->FindSubnode("attribute");
            itNodeAttr != itNodeRestriction->NodeEnd();
            itNodeAttr = itNodeRestriction->FindSubnode("attribute", ++itNodeAttr))
        {
          rise::xml::CXMLNode::TXMLAttrConstIterator itAttrRef = itNodeAttr->FindAttribute("ref");
          if (itAttrRef != itNodeAttr->AttrEnd())
          {
            std::string sRef = StripNamespace(itAttrRef->sAttrValue.AsString());

            rise::xml::CXMLNode::TXMLAttrConstIterator itAttrRefType = itNodeAttr->AttrBegin();
            for (; itAttrRefType != itNodeAttr->AttrEnd();
                ++itAttrRefType)
            {
              std::string sAttrName = StripNamespace(itAttrRefType->sAttrName);

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

                ::staff::GetType(*itNodeAttr, sType, stElemType.sType, stElemType.bIsXmlType);

                stElemType.sName = "elem" + stElemType.sType;

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

  struct SWsdlTypes
  {
    TElementMap mElements;
    TComplexTypeMap mComplexTypes;
    TSimpleTypeMap mSimpleTypes;

    void Parse(rise::xml::CXMLNode& rNodeWsdl)
    {
      for(rise::xml::CXMLNode::TXMLNodeConstIterator itTypes = rNodeWsdl.FindSubnode("types");
          itTypes != rNodeWsdl.NodeEnd(); itTypes = rNodeWsdl.FindSubnode("types", ++itTypes))
      {
        for(rise::xml::CXMLNode::TXMLNodeConstIterator itSchema = itTypes->FindSubnode("schema");
          itSchema != itTypes->NodeEnd(); itSchema = itTypes->FindSubnode("schema", ++itSchema))
        {
          const rise::xml::CXMLNode& rSchema = *itSchema;

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
                mElements[stElement.sName] = stElement;
              }
              else
              if (sType == "complexType")
              {
                SComplexType stComplexType;
                stComplexType.Parse(*itType);
                mComplexTypes[stComplexType.sName] = stComplexType;
              }
              else
              if (sType == "simpleType")
              {
                SSimpleType stSimpleType;
                stSimpleType.Parse(*itType);
                mSimpleTypes[stSimpleType.sName] = stSimpleType;
              }
              else
              {
                rise::LogWarning() << "Unknown node type: " << sType;
              }
            }
          }
        }
      }

    }
  };


  //////////////////////////////////////////////////////////////////////////
  // wsdl parser impl

  class CWsdlParser::CWsdlParserImpl
  {
  public:
    CWsdlParserImpl():
      m_pInterface(NULL)
    {
    }


    void ParsePart(std::list<SParam>& rParts, const rise::xml::CXMLNode& rPart, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes, bool bIsResponse)
    {
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrType = rPart.FindAttribute("element");
      if (itAttrType != rPart.AttrEnd())
      { // element type
        std::string sElementName = StripNamespace(itAttrType->sAttrValue.AsString());

        TElementMap::const_iterator itElement = rWsdlTypes.mElements.find(sElementName);
        RISE_ASSERTES(itElement != rWsdlTypes.mElements.end(), rise::CLogicNoItemException,
          "Element " + itAttrType->sAttrValue.AsString() + " is not found, while parsing part");

        if (itElement->second.lsComplexTypes.size() != 0)
        {
          // struct
          for (std::list<SComplexType>::const_iterator itParamType = itElement->second.lsComplexTypes.begin();
            itParamType != itElement->second.lsComplexTypes.end(); ++itParamType)
          {
            for (std::list<SElement>::const_iterator itParamElement = itParamType->lsElements.begin();
              itParamElement != itParamType->lsElements.end(); ++itParamElement)
            {
              const SElement& rElem = *itParamElement;
              SParam stParam; // struct member

              stParam.sName = bIsResponse ? sElementName : rElem.sName;
              stParam.stDataType.sName = StripNamespace(rElem.sType);
              stParam.stDataType.sNamespace = GetNamespace(rElem.sType);
              stParam.stDataType.sNodeName = rElem.sName;
              stParam.stDataType.sUsedName = rElem.sType;

              rParts.push_back(stParam);
            }
          }
        }
        else
        { // write element
          SParam stParam;

          stParam.sName = bIsResponse ? sElementName : itElement->second.sName;
          stParam.stDataType.sName = StripNamespace(itElement->second.sType);
          stParam.stDataType.sNamespace = GetNamespace(itElement->second.sType);
          stParam.stDataType.sNodeName = itElement->second.sName;

          rParts.push_back(stParam);
        }
      }
      else
      {
        rise::xml::CXMLNode::TXMLAttrConstIterator itType = rPart.FindAttribute("type");
        RISE_ASSERTES(itType != rPart.AttrEnd(), rise::CLogicNoItemException,
          "Unknown part type: " + rPart.Attribute("name").AsString());

        bool bIsXmlType = false;
        std::string sTypeName;

        ::staff::GetType(rPart, itType->sAttrValue.AsString(), sTypeName, bIsXmlType);

        RISE_ASSERTES(sTypeName.size() != 0, rise::CLogicNoItemException, "Unknown part type");

        SParam stParam;
        stParam.sName = rPart.Attribute("name").AsString();
        stParam.stDataType.sNodeName = stParam.sName;
        stParam.stDataType.sName = sTypeName;

        rParts.push_back(stParam);
      }
    }


    void ParseRequest(SMember& rMember, const rise::xml::CXMLNode& rMessage, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
    {
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodePart = rMessage.FindSubnode("part");
          itNodePart != rMessage.NodeEnd();
          itNodePart = rMessage.FindSubnode("part", ++itNodePart))
      {
        ParsePart(rMember.lsParamList, *itNodePart, rDefs, rWsdlTypes, false);

        rise::xml::CXMLNode::TXMLAttrConstIterator itAttrType = itNodePart->FindAttribute("element");
        if (itAttrType != itNodePart->AttrEnd())
        {
          rMember.sNodeName = StripNamespace(itAttrType->sAttrValue.AsString());
        }
      }
    }

    void ParseResponse(SParam& rParam, const rise::xml::CXMLNode& rMessage, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
    {
      std::list<SParam> lsParams;
      ParsePart(lsParams, rMessage.Subnode("part"), rDefs, rWsdlTypes, true);

      if (lsParams.size() > 0)
      {
        rParam = lsParams.front();
      }
      else
      {
        rParam.stDataType.sName = "void";
      }
    }


    void ParseOperation(SMember& rMember, const rise::xml::CXMLNode& rOperation, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
    {
      rMember.sName = rOperation.Attribute("name").AsString();

      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeInput =
        rOperation.FindSubnode("input");
      if (itNodeInput != rOperation.NodeEnd())
      {
        // request
        std::string sRequestName = StripNamespace(itNodeInput->Attribute("message").AsString());

        rise::xml::CXMLNode::TXMLNodeConstIterator itMessage =
          rDefs.FindNodeMatch("message", rise::xml::SXMLAttribute("name", sRequestName));

        RISE_ASSERTES(itMessage != rDefs.NodeEnd(), rise::CLogicNoItemException,
          "Can't find message definition(input) for: " + sRequestName);

        ParseRequest(rMember, *itMessage, rDefs, rWsdlTypes);
      } // else notification message

      // response
      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeOutput =
        rOperation.FindSubnode("output");
      if (itNodeOutput != rOperation.NodeEnd())
      {
        std::string sResponseName = StripNamespace(itNodeOutput->Attribute("message").AsString());

        rise::xml::CXMLNode::TXMLNodeConstIterator itMessage =
          rDefs.FindNodeMatch("message", rise::xml::SXMLAttribute("name", sResponseName));

        RISE_ASSERTES(itMessage != rDefs.NodeEnd(), rise::CLogicNoItemException,
          "Can't find message definition(output) for: " + sResponseName);

        ParseResponse(rMember.stReturn, *itMessage, rDefs, rWsdlTypes);
      }
      else
      { // one way message
        rMember.stReturn.sName = "void";
      }

      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeDoc =
        rOperation.FindSubnode("documentation");
      if (itNodeDoc != rOperation.NodeEnd())
      {
        rMember.sDescr = itNodeDoc->NodeContent().AsString();
      }
    }

    class FindNodeBinding
    {
    public:
      FindNodeBinding(const std::string& sTypeName):
        m_sTypeName(StripNamespace(sTypeName))
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
        const rise::xml::SXMLNamespace* pTransportNamespace = FindNamespace(*itBindingTransport, itBindingTransport->Namespace());
        if (!pTransportNamespace)
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

        if (pTransportNamespace->sUri != "http://schemas.xmlsoap.org/wsdl/soap/")
        { // not a soap 1.1
          return false;
        }

        return StripNamespace(itType->sAttrValue.AsString()) == m_sTypeName;
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
          rClass.sServiceUri = itNodeAddress->Attribute("location").AsString();
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
          ParseSoapAction(sPortTypeName, tOperationMember.sName, rDefs, tOperationMember.sSoapAction);
          rClass.lsMember.push_back(tOperationMember);
        }
      }

      rise::xml::CXMLNode::TXMLNodeConstIterator itNodeOp = rService.FindSubnode("documentation");
      if (itNodeOp != rService.NodeEnd())
      {
        rClass.sDescr = itNodeOp->NodeContent().AsString();
      }
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

    bool FixDataType(SDataType& rDataType, const SInterface& rInterface, const std::string& sNamespace)
    {
      if (rDataType.sName == "string")
      {
        rDataType.sName = "string";
        rDataType.eType = SDataType::EString;
        rDataType.sNamespace = "std::";
        rDataType.sUsedName = "std::string";

        return true;
      }

      if (rDataType.sName == "DataObject")
      {
        rDataType.sName = "CDataObject";
        rDataType.eType = SDataType::EDataObject;
        rDataType.sNamespace = "staff::";
        rDataType.sUsedName = "staff::CDataObject";

        return true;
      }

      for (std::list<STypedef>::const_iterator itTypedef = rInterface.lsTypedef.begin();
          itTypedef != rInterface.lsTypedef.end(); ++itTypedef)
      {
        const STypedef& rTypedef = *itTypedef;
        if (rTypedef.sName == rDataType.sName)
        {
          rDataType.eType = SDataType::ETypedef;
          rDataType.sNamespace = sNamespace;
          return true;
        }
      }

      for (std::list<SStruct>::const_iterator itStruct = rInterface.lsStruct.begin();
          itStruct != rInterface.lsStruct.end(); ++itStruct)
      {
        const SStruct& rStruct = *itStruct;
        if (rStruct.sName == rDataType.sName)
        {
          rDataType.eType = SDataType::EStruct;
          rDataType.sNamespace = sNamespace;
          return true;
        }
      }

      return false;
    }

    void ApplyComponentNamespace(SInterface& rInterface)
    {
      SClass& rService = rInterface.lsClass.front();

      std::string sServiceName;
      std::string sNamespace;
      std::string::size_type nPos = rService.sServiceUri.find_last_of('/');
      if (nPos != std::string::npos)
      {
        sServiceName = rService.sServiceUri.substr(nPos + 1);
      }

      nPos = sServiceName.find_last_of('.', nPos);
      sNamespace = (nPos == std::string::npos) ? "." : sServiceName.substr(0, nPos) + ".";

      rise::StrReplace(sNamespace, ".", "::", true);

      rService.sNamespace = sNamespace;

      for (std::list<SMember>::iterator itMember = rService.lsMember.begin();
          itMember != rService.lsMember.end(); ++itMember)
      {
        SMember& rMember = *itMember;
        for (std::list<SParam>::iterator itParam = rMember.lsParamList.begin();
            itParam != rMember.lsParamList.end(); ++itParam)
        {
          SParam& rParam = *itParam;
          FixDataType(rParam.stDataType, rInterface, sNamespace);
          if (rParam.stDataType.eType == SDataType::EString ||
              rParam.stDataType.eType == SDataType::EStruct ||
              rParam.stDataType.eType == SDataType::ETypedef)
          {
            rParam.stDataType.bIsConst = true;
            rParam.stDataType.bIsRef = true;
          }
        }
      }

      for (std::list<STypedef>::iterator itTypedef = rInterface.lsTypedef.begin();
          itTypedef != rInterface.lsTypedef.end(); ++itTypedef)
      {
        itTypedef->sNamespace = sNamespace;
      }

      for (std::list<SStruct>::iterator itStruct = rInterface.lsStruct.begin();
          itStruct != rInterface.lsStruct.end(); ++itStruct)
      {
        itStruct->sNamespace = sNamespace;
      }
    }

    static bool SortStructByParent(const SStruct& rStruct1, const SStruct& rStruct2)
    {
      return rStruct1.sName == rStruct2.sParent;
    }

    void ParseInterface(rise::xml::CXMLNode& rNodeDefs, SInterface& rInterface)
    {
      m_pInterface = &rInterface;

      SWsdlTypes stWsdlTypes;
      stWsdlTypes.Parse(rNodeDefs);

      for (TComplexTypeMap::const_iterator itComplex = stWsdlTypes.mComplexTypes.begin();
            itComplex != stWsdlTypes.mComplexTypes.end(); ++itComplex)
      {
        SDataType stDt;
        ComplexTypeToData(itComplex->second, stDt, stWsdlTypes);
      }

      for (TSimpleTypeMap::const_iterator itSimple = stWsdlTypes.mSimpleTypes.begin();
        itSimple != stWsdlTypes.mSimpleTypes.end(); ++itSimple)
      {
        SDataType stDt;
        SimpleTypeToData(itSimple->second, stDt, stWsdlTypes);
      }

      for (TElementMap::const_iterator itElement = stWsdlTypes.mElements.begin();
        itElement != stWsdlTypes.mElements.end(); ++itElement)
      {
        WriteInlineTypes(itElement->second, stWsdlTypes);
      }

      rInterface.lsStruct.sort(SortStructByParent);

      SClass tServiceClass;

      ParseService(tServiceClass, rNodeDefs, stWsdlTypes);
      rInterface.lsClass.push_back(tServiceClass);

      ApplyComponentNamespace(rInterface);
    }

    void ParseFile( const std::string& sFileName, SInterface& rInterface )
    {
      rise::xml::CXMLDocument tWsdlDoc;
      rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();
      tWsdlDoc.LoadFromFile(sFileName);

      // validate root node name
      RISE_ASSERTES(rDefs.NodeName() == "definitions", rise::CLogicFormatException,
        "Invalid wsdl root node name: " + rDefs.NodeName());

      rInterface.sTargetNs = rDefs.Attribute("targetNamespace").AsString();

      // fill in interface name
      std::string::size_type nPos = sFileName.find_last_of("/\\");
      rInterface.sName = (nPos != std::string::npos) ? sFileName.substr(nPos + 1) : sFileName;

      nPos = rInterface.sName.find_last_of('.');
      if (nPos != std::string::npos)
      {
        rInterface.sName.erase(nPos);
      }

      nPos = rInterface.sName.find_last_of('.');
      if (nPos != std::string::npos)
      {
        rInterface.sName.erase(0, nPos + 1);
      }

      ParseInterface(rDefs, rInterface);
    }


  private:

    void DataTypeFromName(const std::string& sDataType, SDataType& rDataType, const SWsdlTypes& rWsdlTypes)
    {
      // struct/typedef
      TSimpleTypeMap::const_iterator itSimpleType = rWsdlTypes.mSimpleTypes.find(sDataType);
      if (itSimpleType != rWsdlTypes.mSimpleTypes.end())
      {
        SimpleTypeToData(itSimpleType->second, rDataType, rWsdlTypes);
      }
      else
      {
        TComplexTypeMap::const_iterator itComplexType = rWsdlTypes.mComplexTypes.find(sDataType);
        if (itComplexType != rWsdlTypes.mComplexTypes.end())
        {
          ComplexTypeToData(itComplexType->second, rDataType, rWsdlTypes);
        }
        else
        {
          rDataType.sName = sDataType;
          rDataType.sUsedName = sDataType;
          rise::LogWarning() << "Can't find element type declaration: " << sDataType;
        }
      }
    }

    void SimpleTypeToData(const SSimpleType& rSimpleType, SDataType& rDataType, const SWsdlTypes& rWsdlTypes)
    {
      std::list<STypedef>::const_iterator itTypedef = m_pInterface->lsTypedef.begin();
      for (; itTypedef != m_pInterface->lsTypedef.end(); ++itTypedef)
      {
        if (itTypedef->sName == rSimpleType.sName)
        {
          break;
        }
      }

      if (itTypedef != m_pInterface->lsTypedef.end())
      {
        rDataType = itTypedef->stDataType;
      }
      else
      {
        STypedef stTypedef;
        stTypedef.sName = rSimpleType.sName;
        stTypedef.sDescr = rSimpleType.sDescr;

        if (rSimpleType.bIsXmlType)
        {
          rDataType.sName = rSimpleType.sBaseType;
          rDataType.sUsedName = rSimpleType.sBaseType;
          stTypedef.stDataType.sName = rSimpleType.sBaseType;
          stTypedef.stDataType.sUsedName = rSimpleType.sBaseType;
        }
        else
        {
          DataTypeFromName(rSimpleType.sBaseType, stTypedef.stDataType, rWsdlTypes);
        }

        m_pInterface->lsTypedef.push_back(stTypedef);
      }
    }

    void ComplexTypeToData(const SComplexType& rComplexType, SDataType& rDataType, const SWsdlTypes& rWsdlTypes)
    {
      std::list<STypedef>::const_iterator itTypedef = m_pInterface->lsTypedef.begin();
      for (; itTypedef != m_pInterface->lsTypedef.end(); ++itTypedef)
      {
        if (itTypedef->sName == rComplexType.sName)
        {
          rDataType = itTypedef->stDataType;
          return;
        }
      }

      std::list<SStruct>::const_iterator itStruct = m_pInterface->lsStruct.begin();
      for (; itStruct != m_pInterface->lsStruct.end(); ++itStruct)
      {
        if (itStruct->sName == rComplexType.sName)
        {
          rDataType.sName = itStruct->sName;
          rDataType.sNodeName = itStruct->sName;
          rDataType.eType = SDataType::EStruct;
          return;
        }
      }

      // single type array
      if (rComplexType.lsElements.size() == 1)
      {
        if (rComplexType.lsElements.front().bIsArray)
        {
          STypedef stTypedef;
          stTypedef.sName = rComplexType.sName;
          stTypedef.sDescr = rComplexType.sDescr;

          ElementToData(rComplexType.lsElements.front(), stTypedef.stDataType, rWsdlTypes);

          m_pInterface->lsTypedef.push_back(stTypedef);
          return;
        }
      }

      {
        SStruct stStruct;
        stStruct.bForward = false;
        stStruct.sName = rComplexType.sName;
        stStruct.sDescr = rComplexType.sDescr;

        // inheritance
        stStruct.sParent = rComplexType.sParentType;

        for (std::list<SElement>::const_iterator itElement = rComplexType.lsElements.begin();
          itElement != rComplexType.lsElements.end(); ++itElement)
        {
          SParam stMember;
          stMember.sName = itElement->sName;
          ElementToData(*itElement, stMember.stDataType, rWsdlTypes);
          stMember.sDescr = itElement->sDescr;
          stStruct.lsMember.push_back(stMember);
        }

        rDataType.eType = SDataType::EStruct;
        rDataType.sName = stStruct.sName;
        rDataType.sUsedName = stStruct.sName;

        m_pInterface->lsStruct.push_back(stStruct);
      }
    }

    void ElementToData(const SElement& rElement, SDataType& rDataType, const SWsdlTypes& rWsdlTypes)
    {
      if (rElement.bIsArray)
      {
        SDataType stTempl;
        stTempl.sName = rElement.sType;
        stTempl.sUsedName = rElement.sType;
        stTempl.sNodeName = rElement.sName;

        rDataType.sName = "list";
        rDataType.sNamespace = "std::";
        rDataType.sUsedName = "std::list";
        rDataType.eType = SDataType::ETemplate;
        rDataType.lsParams.push_back(stTempl);
      }
      else
      {
        rDataType.sName = StripNamespace(rElement.sType);
        rDataType.sNamespace = GetNamespace(rElement.sType);
        rDataType.sNodeName = rElement.sName;
        rDataType.sUsedName = rElement.sType;
      }

      if (!rElement.bIsXmlType)
      {
        for (std::list<SSimpleType>::const_iterator itSimpleSubtype = rElement.lsSimpleTypes.begin();
          itSimpleSubtype != rElement.lsSimpleTypes.end(); ++itSimpleSubtype)
        {
          SDataType st;
          SimpleTypeToData(*itSimpleSubtype, st, rWsdlTypes);
        }

        for (std::list<SComplexType>::const_iterator itComplexSubtype = rElement.lsComplexTypes.begin();
          itComplexSubtype != rElement.lsComplexTypes.end(); ++itComplexSubtype)
        {
          SDataType st;
          ComplexTypeToData(*itComplexSubtype, st, rWsdlTypes);
        }
      }
    }

    SInterface* m_pInterface;
  };

  CWsdlParser::CWsdlParser():
    m_pImpl(new CWsdlParserImpl)
  {
  }

  CWsdlParser::~CWsdlParser()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
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
      std::string sFileName = rParseSettings.sInDir + "/" + *itFileName;
      staff::SInterface stInterface;

      std::cout << "Processing wsdl: " << *itFileName << std::endl;

      m_pImpl->ParseFile(sFileName, stInterface);

      rProject.lsInterfaces.push_back(stInterface);
    }
  }

  const std::string CWsdlParser::m_sId = "wsdl";
}
