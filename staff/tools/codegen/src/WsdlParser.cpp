#include <rise/xml/XMLDocument.h>
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLAttribute.h>
#include <rise/xml/XMLNamespace.h>
#include "InterfaceInfo.h"
#include "WsdlParser.h"

struct SSimpleType;
struct SComplexType;

//////////////////////////////////////////////////////////////////////////
struct SElement
{
  std::string sName;
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
  RISE_ASSERTP(pElementNs);

  bIsXmlType = pElementNs->sUri == "http://www.w3.org/2001/XMLSchema";

  if (bIsXmlType &&
     (sTypeName == "string" ||
      sTypeName == "boolean" ||
      sTypeName == "decimal" ||
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
    )
  {
    sCppTypeName = "staff::" + sTypeName;
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

  itAttr = rNodeElement.FindAttribute("maxOccurs");
  bIsArray = (itAttr != rNodeElement.AttrEnd()) && (itAttr->sAttrValue.AsString() == "unbounded");

  if (!GetType(rNodeElement, sType, bIsXmlType))
  { 
    itAttr = rNodeElement.FindAttribute("ref");
    if (itAttr != rNodeElement.AttrEnd())
    {
      ::GetType(rNodeElement, itAttr->sAttrValue.AsString(), sType, bIsXmlType);
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

  ::GetType(rElement, itType->sAttrValue, sCppTypeName, bIsXmlType);
  return true;
}

int SElement::m_nUnnamedElementId = 0;


//////////////////////////////////////////////////////////////////////////
SSimpleType& SSimpleType::Parse( const rise::xml::CXMLNode& rNodeSimpleType )
{
  rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeSimpleType.FindAttribute("name");

  sName = (itAttr != rNodeSimpleType.AttrEnd()) ? itAttr->sAttrValue.AsString() : 
    ("UnnamedSimpleType" + rise::ToStr(++m_nUnnamedSimpleTypeId));

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
  rise::xml::CXMLNode::TXMLAttrConstIterator itAttr = rNodeComplexType.FindAttribute("name");

  if (itAttr != rNodeComplexType.AttrEnd())
  {
    sName = itAttr->sAttrValue.AsString();
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

              ::GetType(*itNodeAttr, sType, stElemType.sType, stElemType.bIsXmlType);

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


  void ParsePart(std::list<SParam>& rParts, const rise::xml::CXMLNode& rPart, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
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
        for (std::list<SComplexType>::const_iterator itParamType = itElement->second.lsComplexTypes.begin();
          itParamType != itElement->second.lsComplexTypes.end(); ++itParamType)
        {
          for (std::list<SElement>::const_iterator itParamElement = itParamType->lsElements.begin();
            itParamElement != itParamType->lsElements.end(); ++itParamElement)
          {
            const SElement& rElem = *itParamElement;
            SParam stParam;

            stParam.sName = rElem.sName;
            stParam.stDataType.sName = rElem.sType;
            stParam.stDataType.sNodeName = rElem.sName;

            rParts.push_back(stParam);
          }
        }
      }
      else
      { // write element
        SParam stParam;

        stParam.sName = itElement->second.sName;
        stParam.stDataType.sName = itElement->second.sType;
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

      ::GetType(rPart, itType->sAttrValue.AsString(), sTypeName, bIsXmlType);

      RISE_ASSERTES(sTypeName.size() != 0, rise::CLogicNoItemException, "Unknown part type");

      SParam stParam;
      stParam.sName = rPart.Attribute("name").AsString();
      stParam.stDataType.sNodeName = stParam.sName;
      stParam.stDataType.sName = sTypeName;

      rParts.push_back(stParam);
    }
  }


  void ParseRequest(std::list<SParam>& rParams, const rise::xml::CXMLNode& rMessage, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
  {
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodePart = rMessage.FindSubnode("part");
        itNodePart != rMessage.NodeEnd();
        itNodePart = rMessage.FindSubnode("part", ++itNodePart))
    {
      ParsePart(rParams, *itNodePart, rDefs, rWsdlTypes);
    }
  }

  void ParseResponse(SDataType& rParam, const rise::xml::CXMLNode& rMessage, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
  {
    std::list<SParam> lsParams;
    ParsePart(lsParams, rMessage.Subnode("part"), rDefs, rWsdlTypes);

    if (lsParams.size() > 0)
    {
      rParam = lsParams.front().stDataType;
    }
    else
    {
      rParam.sName = "void";
    }
  }
  

  void ParseOperation(SMember& rMember, const rise::xml::CXMLNode& rOperation, const rise::xml::CXMLNode& rDefs, const SWsdlTypes& rWsdlTypes)
  {
    std::string::size_type nPos = std::string::npos;
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

      ParseRequest(rMember.lsParamList, *itMessage, rDefs, rWsdlTypes);
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
  
  void ParseSoapAction(const std::string& sPortTypeName, const std::string& sOperationName, const rise::xml::CXMLNode& rDefs, std::string& sSoapAction)
  {

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

        return StripNamespace(itType->sAttrValue.AsString()) == m_sTypeName;
      }

    private:
      std::string m_sTypeName;
    };


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


  static bool SortStructByParent(const SStruct& rStruct1, const SStruct& rStruct2)
  {
    return rStruct1.sName == rStruct2.sParent;
  }

  void Parse(rise::xml::CXMLNode& rNodeDefs, SInterface& rInterface)
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

      if (rSimpleType.bIsXmlType)
      {
        rDataType.sName = rSimpleType.sBaseType;
        stTypedef.stDataType.sName = rSimpleType.sBaseType;
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

        ElementToData(rComplexType.lsElements.front(), stTypedef.stDataType, rWsdlTypes);

        m_pInterface->lsTypedef.push_back(stTypedef);
        return;
      }
    }

    {
      SStruct stStruct;
      stStruct.bForward = false;
      stStruct.sName = rComplexType.sName;

      // inheritance
      stStruct.sParent = rComplexType.sParentType;

      for (std::list<SElement>::const_iterator itElement = rComplexType.lsElements.begin();
        itElement != rComplexType.lsElements.end(); ++itElement)
      {
        SParam stMember;
        stMember.sName = itElement->sName;
        ElementToData(*itElement, stMember.stDataType, rWsdlTypes);
        stStruct.lsMember.push_back(stMember);
      }

      rDataType.eType = SDataType::EStruct;
      rDataType.sName = stStruct.sName;

      m_pInterface->lsStruct.push_back(stStruct);
    }
  }

  void ElementToData(const SElement& rElement, SDataType& rDataType, const SWsdlTypes& rWsdlTypes)
  {
    if (rElement.bIsArray)
    {
      SDataType stTempl;
      stTempl.sName = rElement.sType;
      stTempl.sNodeName = rElement.sName;

      rDataType.sName = "staff::Array";
      rDataType.eType = SDataType::ETemplate;
      rDataType.lsParams.push_back(stTempl);
    }
    else
    {
      rDataType.sName = rElement.sType;
      rDataType.sNodeName = rElement.sName;
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


CWsdlParser& CWsdlParser::Inst()
{
  if (m_pInst == NULL)
  {
    m_pInst = new CWsdlParser();
  }

  return *m_pInst;
}

void CWsdlParser::Parse( const std::string& sFileName, SInterface& rInterface )
{
  rise::xml::CXMLDocument tWsdlDoc;
  rise::xml::CXMLNode& rDefs = tWsdlDoc.GetRoot();
  tWsdlDoc.LoadFromFile(sFileName);

  // validate root node name
  RISE_ASSERTES(rDefs.NodeName() == "definitions", rise::CLogicFormatException, 
    "Invalid wsdl root node name: " + rDefs.NodeName());

  // fill in interface name
  std::string::size_type nPos = sFileName.find_last_of("/\\");
  rInterface.sName = (nPos != std::string::npos) ? sFileName.substr(nPos + 1) : sFileName;

  nPos = rInterface.sName.find_last_of('.');
  if (nPos != std::string::npos)
  {
    rInterface.sName.erase(nPos);
  }

  m_pImpl->Parse(rDefs, rInterface);

}

CWsdlParser* CWsdlParser::m_pInst = NULL;
