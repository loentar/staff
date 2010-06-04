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

#include <list>
#include <rise/xml/XMLDocument.h>
#include <rise/xml/XMLNode.h>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include "Interface.h"
#include "XmlGen.h"

namespace staff
{
  using rise::xml::CXMLNode;

  std::ostream& operator<<( std::ostream& rStream, const SDataType::EDataType eDataType )
  {
    switch (eDataType)
    {
    case SDataType::EGeneric:
      rStream << "generic";
      break;

    case SDataType::EString:
      rStream << "string";
      break;

    case SDataType::EDataObject:
      rStream << "dataobject";
      break;

    case SDataType::EStruct:
      rStream << "struct";
      break;

    case SDataType::ETypedef:
      rStream << "typedef";
      break;

    case SDataType::ETemplate:
      rStream << "template";
      break;

    default:
      rStream << "unknown";
    }

    return rStream;
  }


  template<typename TType>
  CXMLNode& operator<<(CXMLNode& rStream, const std::list<TType>& rList)
  {
    for(typename std::list<TType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
      rStream << *it;
    return rStream;
  }

  bool GetDataType(std::string& sOut, const SDataType& rDataType, bool bAsUsed = false)
  {
    sOut += (rDataType.bIsConst ? "const " : "") +
      (bAsUsed ?
        (rDataType.sUsedName.size() != 0 ? rDataType.sUsedName : rDataType.sName) :
        (rDataType.sNamespace == "::" ? "" : rDataType.sNamespace) + rDataType.sName);

    bool bIsTemplate = rDataType.lsParams.size() != 0;
    if (!bIsTemplate)
    {
      sOut += (rDataType.bIsRef ? "&" : "");
      return false;
    }

    sOut += "< ";
    for(std::list<SDataType>::const_iterator it = rDataType.lsParams.begin(); it != rDataType.lsParams.end(); ++it)
    {
      if (it != rDataType.lsParams.begin())
        sOut += ", ";
      bIsTemplate = GetDataType(sOut, *it, bAsUsed);
    }
    if(bIsTemplate)
      sOut += ' ';
    sOut += " >";

    sOut += (rDataType.bIsRef ? "&" : "");
    return true;
  }

  CXMLNode& operator<<(CXMLNode& rNodeDataTypes, const SDataType& rDataType)
  {
    std::string sMangledName = (rDataType.sNamespace.substr(0, 2) == "::") ?
      rDataType.sNamespace.substr(2) : rDataType.sNamespace;
    rise::StrReplace(sMangledName, "::", "_", true);

    rNodeDataTypes.AddSubNode(" Is const ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["IsConst"] = rDataType.bIsConst;
    rNodeDataTypes.AddSubNode(" Is reference ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["IsRef"] = rDataType.bIsRef;
    rNodeDataTypes.AddSubNode(" Type name as used ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["UsedName"] = (rDataType.sUsedName.size() != 0 ? rDataType.sUsedName : rDataType.sName);
    rNodeDataTypes.AddSubNode(" Type name ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["Name"] = (rDataType.sNamespace == "::" ? "" : rDataType.sNamespace) + rDataType.sName;
    rNodeDataTypes.AddSubNode(" Node name ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["NodeName"] = rDataType.sNodeName;
    rNodeDataTypes.AddSubNode(" Type ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["Type"] = rDataType.eType;
    rNodeDataTypes.AddSubNode(" Typename with namespace ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["MangledName"] = sMangledName + rDataType.sName;

    std::string sNativeName;
    if (rDataType.eType == SDataType::ETypedef)
    {
      sNativeName = ((rDataType.sName[0] == 'T') &&
         (toupper(rDataType.sName[1]) == rDataType.sName[1]) ? rDataType.sName.substr(1) : rDataType.sName);
    }
    else
    if (rDataType.eType == SDataType::EStruct)
    {
      sNativeName = ((rDataType.sName[0] == 'S') &&
         (toupper(rDataType.sName[1]) == rDataType.sName[1]) ? rDataType.sName.substr(1) : rDataType.sName);
    }
    else
    {
      sNativeName = rDataType.sName;
    }

    rNodeDataTypes.AddSubNode(" Native name ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["NativeName"] = sNativeName;

    rNodeDataTypes.AddSubNode(" Used typedef ", CXMLNode::ENTCOMMENT);
    GetDataType(rNodeDataTypes["UsedTypedef"].AsString(), rDataType, true);

    rNodeDataTypes.NodeContent() = "";
    GetDataType(rNodeDataTypes.NodeContent().AsString(), rDataType);

    bool bIsTemplate = rDataType.lsParams.size() != 0;
    rNodeDataTypes.AddSubNode(" Template ", CXMLNode::ENTCOMMENT);
    rNodeDataTypes["IsTemplate"] = bIsTemplate;

    rNodeDataTypes.AddSubNode(" Template params ", CXMLNode::ENTCOMMENT);
    CXMLNode& rNodeTemplateParams = rNodeDataTypes.AddSubNode("TemplateParams");
    int nNum = 1;
    for(std::list<SDataType>::const_iterator it = rDataType.lsParams.begin(); it != rDataType.lsParams.end(); ++it, ++nNum)
      rNodeTemplateParams.AddSubNode("TemplateParam" + rise::ToStr(nNum)) << *it;

    if (rDataType.eType == SDataType::EUnknown)
    {
      rise::LogError() << "Unknown datatype: " << (rDataType.sNamespace + rDataType.sName);
    }

    return rNodeDataTypes;
  }

  CXMLNode& operator<<(CXMLNode& rNodeParams, const SParam& rParam)
  {
    if (rParam.sName.size() != 0 && rParam.stDataType.sName != "void")
    {
      rNodeParams.AddSubNode(" Parameter ", CXMLNode::ENTCOMMENT);
      CXMLNode& rNodeParam = rNodeParams.AddSubNode("Param");

      rNodeParam.AddSubNode(" Parameter name ", CXMLNode::ENTCOMMENT);
      rNodeParam["Name"] = rParam.sName;
      rNodeParam.AddSubNode(" Parameter description ", CXMLNode::ENTCOMMENT);
      rNodeParam["Description"] = rParam.sDescr;
      rNodeParam.AddSubNode(" Parameter type ", CXMLNode::ENTCOMMENT);
      rNodeParam.AddSubNode("DataType") << rParam.stDataType;

      if (rNodeParams.NodeContent() != "")
        rNodeParams.NodeContent().AsString() += ", ";

      GetDataType(rNodeParams.NodeContent().AsString(), rParam.stDataType, true);
      rNodeParams.NodeContent().AsString() += " " + rParam.sName;
    }

    return rNodeParams;
  }

  CXMLNode& operator<<(CXMLNode& rNodeMembers, const SMember& rMember)
  {
    rNodeMembers.AddSubNode(" Operation ", CXMLNode::ENTCOMMENT);
    CXMLNode& rNodeMember = rNodeMembers.AddSubNode("Member");

    rNodeMember.AddSubNode(" Operation name ", CXMLNode::ENTCOMMENT);
    rNodeMember["Name"] = rMember.sName;
    rNodeMember.AddSubNode(" Operation description ", CXMLNode::ENTCOMMENT);
    rNodeMember["Description"] = rMember.sDescr;
    rNodeMember.AddSubNode(" Operation detailed description ", CXMLNode::ENTCOMMENT);
    rNodeMember["Detail"] = rMember.sDetail;
    rNodeMember.AddSubNode(" Soap action ", CXMLNode::ENTCOMMENT);
    rNodeMember["SoapAction"] = rMember.sSoapAction;
    rNodeMember.AddSubNode(" Soap request node name ", CXMLNode::ENTCOMMENT);
    rNodeMember["NodeName"] = rMember.sNodeName;
    rNodeMember.AddSubNode(" Function is non-mutable ", CXMLNode::ENTCOMMENT);
    rNodeMember["IsConst"] = rMember.bIsConst;
    rNodeMember["Const"] = rMember.bIsConst ? " const" : "";
    rNodeMember.AddSubNode("Params") << rMember.lsParamList;

    rNodeMember.AddSubNode(" Return ", CXMLNode::ENTCOMMENT);
    CXMLNode& rNodeReturn = rNodeMember.AddSubNode("Return");
    rNodeReturn << rMember.stReturn.stDataType;
    rNodeReturn.AddSubNode(" Response Name ", CXMLNode::ENTCOMMENT);
    rNodeReturn["ResponseName"] = rMember.stReturn.sName;

    return rNodeMembers;
  }

  void WriteCppNamespace(CXMLNode& rNode, const std::string& sNamespace)
  {
    std::string sOpeningNs;
    std::string sEndingNs;
    std::string::size_type nPos = 0;
    std::string::size_type nPrevPos = 0;
    while((nPos = sNamespace.find("::", nPos)) != std::string::npos)
    {
      sOpeningNs += "namespace " + sNamespace.substr(nPrevPos, nPos - nPrevPos) + "\n{\n";
      sEndingNs += "}\n";
      nPos += 2;
      nPrevPos = nPos;
    }

    rNode.AddSubNode(" Opening namespace ", CXMLNode::ENTCOMMENT);
    rNode["OpeningNs"] = sOpeningNs;
    rNode.AddSubNode(" Closing namespace ", CXMLNode::ENTCOMMENT);
    rNode["EndingNs"] = sEndingNs;
  }

  CXMLNode& operator<<(CXMLNode& rNodeClasses, const SClass& rClass)
  {
    CXMLNode& rNodeClass = rNodeClasses.AddSubNode("Class");
    std::string sNamespace =
      (rClass.sNamespace.substr(0, 2) == "::") ? rClass.sNamespace.substr(2) : rClass.sNamespace;
    std::string sServiceName =
      (((rClass.sName[0] == 'C' || rClass.sName[0] == 'I') && (toupper(rClass.sName[1]) == rClass.sName[1]))
          ? rClass.sName.substr(1) : rClass.sName);

    std::string sServiceNamespace = sNamespace;
    rise::StrReplace(sServiceNamespace, "::", ".", true);
    std::string sServiceNsName = sServiceNamespace + sServiceName;

    rNodeClass.AddSubNode(" Service class name ", CXMLNode::ENTCOMMENT);
    rNodeClass["Name"] = rClass.sName;
    rNodeClass.AddSubNode(" Service description ", CXMLNode::ENTCOMMENT);
    rNodeClass["Description"] = rClass.sDescr;
    rNodeClass.AddSubNode(" Service detailed description ", CXMLNode::ENTCOMMENT);
    rNodeClass["Detail"] = rClass.sDetail;
    rNodeClass.AddSubNode(" Service URI ", CXMLNode::ENTCOMMENT);
    rNodeClass["ServiceUri"] = rClass.sServiceUri;
    rNodeClass.AddSubNode(" Service class name with namespace ", CXMLNode::ENTCOMMENT);
    rNodeClass["NsName"] = rClass.sNamespace + rClass.sName;
    rNodeClass.AddSubNode(" Service name ", CXMLNode::ENTCOMMENT);
    rNodeClass["ServiceName"] = sServiceName;
    rNodeClass.AddSubNode(" Service name with namespace ", CXMLNode::ENTCOMMENT);
    rNodeClass["ServiceNsName"] = sServiceNsName;
    rNodeClass.AddSubNode(" Service namespace ", CXMLNode::ENTCOMMENT);
    rNodeClass["ServiceNamespace"] = sServiceNamespace;
    rNodeClass.AddSubNode(" Component name ", CXMLNode::ENTCOMMENT);
    rNodeClass["ComponentName"] =
      sServiceNamespace.size() == 0 ? sServiceNamespace : sServiceNamespace.substr(0, sServiceNamespace.size() - 1);
    rNodeClass.AddSubNode(" Service class namespace ", CXMLNode::ENTCOMMENT);
    rNodeClass["Namespace"] = rClass.sNamespace;
    rNodeClass.AddSubNode(" Load service at startup ", CXMLNode::ENTCOMMENT);
    rNodeClass["LoadAtStartup"] = rClass.bLoadAtStartup ? "true" : "false";
    rNodeClass.AddSubNode(" Service operations ", CXMLNode::ENTCOMMENT);
    rNodeClass.AddSubNode("Members") << rClass.lsMember;

    WriteCppNamespace(rNodeClass, sNamespace);

    return rNodeClasses;
  }

  CXMLNode& operator<<(CXMLNode& rNodeStructs, const SStruct& rStruct)
  {
    if (rStruct.bForward && !rStruct.bExtern)
    {
      throw "Struct \"" + rStruct.sName + "\" is not fully declared";
    }

    CXMLNode& rNodeStruct = rNodeStructs.AddSubNode("Struct");

    std::string sNamespace =
      (rStruct.sNamespace.substr(0, 2) == "::") ? rStruct.sNamespace.substr(2) : rStruct.sNamespace;
    std::string sMangledName = (rStruct.sNamespace.substr(0, 2) == "::") ?
      rStruct.sNamespace.substr(2) : rStruct.sNamespace;
    rise::StrReplace(sMangledName, "::", "_", true);

    rNodeStruct.AddSubNode(" Native struct name ", CXMLNode::ENTCOMMENT);
    rNodeStruct["NativeName"] = ((rStruct.sName[0] == 'S') &&
      (toupper(rStruct.sName[1]) == rStruct.sName[1]) ? rStruct.sName.substr(1) : rStruct.sName);
    rNodeStruct.AddSubNode(" Struct name with namespace ", CXMLNode::ENTCOMMENT);
    rNodeStruct["MangledName"] = sMangledName + rStruct.sName;

    rNodeStruct.AddSubNode(" Parent struct(inherits) ", CXMLNode::ENTCOMMENT);
    rNodeStruct["Parent"] = (rStruct.sParent.find("::") == std::string::npos) && rStruct.sParent.size() != 0 ?
      (rStruct.sNamespace + rStruct.sParent) : rStruct.sParent;
    rNodeStruct.AddSubNode(" Parent struct(inherits) declaration ", CXMLNode::ENTCOMMENT);
    rNodeStruct["ParentDecl"] = (rStruct.sParent.size() != 0) ? (": public " + rStruct.sParent) : "";

    rNodeStruct.AddSubNode(" Struct name ", CXMLNode::ENTCOMMENT);
    rNodeStruct["Name"] = rStruct.sName;
    rNodeStruct.AddSubNode(" Struct name with namespace", CXMLNode::ENTCOMMENT);
    rNodeStruct["NsName"] = rStruct.sNamespace + rStruct.sName;
    rNodeStruct.AddSubNode(" Description ", CXMLNode::ENTCOMMENT);
    rNodeStruct["Description"] = rStruct.sDescr;
    rNodeStruct.AddSubNode(" Detailed description ", CXMLNode::ENTCOMMENT);
    rNodeStruct["Detail"] = rStruct.sDetail;

    rNodeStruct.AddSubNode(" Defined in other interface ", CXMLNode::ENTCOMMENT);
    rNodeStruct["Extern"] = rStruct.bExtern;

    rNodeStruct.AddSubNode(" Struct fields ", CXMLNode::ENTCOMMENT);
    rNodeStruct.AddSubNode("Members") << rStruct.lsMember;

    WriteCppNamespace(rNodeStruct, sNamespace);

    return rNodeStructs;
  }

  CXMLNode& operator<<(CXMLNode& rNodeTypedefs, const STypedef& rTypedef)
  {
    CXMLNode& rNodeTypedef = rNodeTypedefs.AddSubNode("Typedef");
    std::string sNamespace =
      (rTypedef.sNamespace.substr(0, 2) == "::") ? rTypedef.sNamespace.substr(2) : rTypedef.sNamespace;
    std::string sMangledName = (rTypedef.sNamespace.substr(0, 2) == "::") ?
      rTypedef.sNamespace.substr(2) : rTypedef.sNamespace;
    rise::StrReplace(sMangledName, "::", "_", true);

    rNodeTypedef.AddSubNode(" Typedef name ", CXMLNode::ENTCOMMENT);
    rNodeTypedef["Name"] = rTypedef.sName;
    rNodeTypedef.AddSubNode(" Typedef name with namespace ", CXMLNode::ENTCOMMENT);
    rNodeTypedef["NsName"] = rTypedef.sNamespace + rTypedef.sName;
    rNodeTypedef.AddSubNode(" Typedef native name ", CXMLNode::ENTCOMMENT);
    rNodeTypedef["NativeName"] = ((rTypedef.sName[0] == 'T') &&
         (toupper(rTypedef.sName[1]) == rTypedef.sName[1]) ? rTypedef.sName.substr(1) : rTypedef.sName);

    rNodeTypedef.AddSubNode(" Description ", CXMLNode::ENTCOMMENT);
    rNodeTypedef["Description"] = rTypedef.sDescr;

    rNodeTypedef.AddSubNode(" Typedef name with namespace ", CXMLNode::ENTCOMMENT);
    rNodeTypedef["MangledName"] = sMangledName + rTypedef.sName;
    rNodeTypedef.AddSubNode(" Source datatype ", CXMLNode::ENTCOMMENT);
    rNodeTypedef.AddSubNode("DataType") << rTypedef.stDataType;

    rNodeTypedef.AddSubNode(" Defined in other interface ", CXMLNode::ENTCOMMENT);
    rNodeTypedef["Extern"] = rTypedef.bExtern;

    WriteCppNamespace(rNodeTypedef, sNamespace);

    return rNodeTypedefs;
  }

  CXMLNode& operator<<(CXMLNode& rNodeInterfaces, const SInterface& rInterface)
  {
    CXMLNode& rNodeInterface = rNodeInterfaces.AddSubNode("Interface");

    rNodeInterface.AddSubNode(" Intarface name ", CXMLNode::ENTCOMMENT);
    rNodeInterface["Name"] = rInterface.sName;

    rNodeInterface.AddSubNode(" Target namespace ", CXMLNode::ENTCOMMENT);
    rNodeInterface["TargetNamespace"] = rInterface.sTargetNs;

    rNodeInterface.AddSubNode(" Interface file name ", CXMLNode::ENTCOMMENT);
    rNodeInterface["FileName"] = rInterface.sFileName;

    rNodeInterface.AddSubNode(" Interface services ", CXMLNode::ENTCOMMENT);
    rNodeInterface.AddSubNode("Classes") << rInterface.lsClass;

    rNodeInterface.AddSubNode(" Interface structs ", CXMLNode::ENTCOMMENT);
    rNodeInterface.AddSubNode("Structs") << rInterface.lsStruct;

    rNodeInterface.AddSubNode(" Interface typedefs ", CXMLNode::ENTCOMMENT);
    rNodeInterface.AddSubNode("Typedefs") << rInterface.lsTypedef;

    rNodeInterface.AddSubNode(" Included files ", CXMLNode::ENTCOMMENT);
    CXMLNode& rNodeIncludes = rNodeInterface.AddSubNode("Includes");

    for (std::list<SInclude>::const_iterator itInclude = rInterface.lsInclude.begin();
        itInclude != rInterface.lsInclude.end(); ++itInclude)
    {
      CXMLNode& rNodeInclude = rNodeIncludes.AddSubNode("Include");
      rNodeInclude.AddSubNode(" Interface name ", CXMLNode::ENTCOMMENT);
      rNodeInclude["Name"] = itInclude->sInterfaceName;
      rNodeInclude.AddSubNode(" File name ", CXMLNode::ENTCOMMENT);
      rNodeInclude["FileName"] = itInclude->sFileName;
    }

    return rNodeInterfaces;
  }

  CXMLNode& operator<<(CXMLNode& rRootNode, const SProject& rProject)
  {
    std::string sNamespace =
      (rProject.sNamespace.substr(0, 2) == "::") ? rProject.sNamespace.substr(2) : rProject.sNamespace;

    std::string sComponentName = sNamespace;
    rise::StrReplace(sComponentName, "::", ".", true);
    if (!sComponentName.empty())
    {
      sComponentName.erase(sComponentName.size() - 1);
    }

    rRootNode.NodeName() = "Project";
    rRootNode["Name"] = rProject.sName;
    rRootNode.AddSubNode(" Component namespace ", CXMLNode::ENTCOMMENT);
    rRootNode["Namespace"] = sNamespace;
    WriteCppNamespace(rRootNode, sNamespace);
    rRootNode.AddSubNode(" Component name ", CXMLNode::ENTCOMMENT);
    rRootNode["ComponentName"] = sComponentName;

    rRootNode.AddSubNode(" Project interfaces ", CXMLNode::ENTCOMMENT);
    rRootNode.AddSubNode("Interfaces") << rProject.lsInterfaces;

    return rRootNode;
  }
}
