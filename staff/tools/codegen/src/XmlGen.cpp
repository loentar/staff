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
#include "InterfaceInfo.h"
#include "XmlGen.h"

using rise::xml::CXMLNode;

template<typename TType>
CXMLNode& operator<<(CXMLNode& rStream, const std::list<TType>& rList)
{
  for(typename std::list<TType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
    rStream << *it;
  return rStream;
}

bool GetDataType(std::string& sOut, const SDataType& rDataType)
{
  sOut += (rDataType.bIsConst ? "const " : "") + 
    (rDataType.sNamespace == "::" ? "" : rDataType.sNamespace) + rDataType.sName;

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
    bIsTemplate = GetDataType(sOut, *it);
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
    rNodeParam.AddSubNode(" Parameter type ", CXMLNode::ENTCOMMENT);
    rNodeParam.AddSubNode("DataType") << rParam.stDataType;

    if (rNodeParams.NodeContent() != "")
      rNodeParams.NodeContent().AsString() += ", ";
      
    GetDataType(rNodeParams.NodeContent().AsString(), rParam.stDataType);
    rNodeParams.NodeContent().AsString() += " " + rParam.sName;
  }

  return rNodeParams;
}

CXMLNode& operator<<(CXMLNode& rNodeMembers, const SMember& rMember)
{
  rNodeMembers.AddSubNode(" Operation ", CXMLNode::ENTCOMMENT);
  CXMLNode& rNodeMember = rNodeMembers.AddSubNode("Member");

  rNodeMember.AddSubNode(" Return ", CXMLNode::ENTCOMMENT);
  CXMLNode& rNodeReturn = rNodeMember.AddSubNode("Return");
  rNodeReturn << rMember.stReturn.stDataType;
  rNodeReturn.AddSubNode(" Response Name ", CXMLNode::ENTCOMMENT);
  rNodeReturn["ResponseName"] = rMember.stReturn.sName;

  rNodeMember.AddSubNode(" Operation name ", CXMLNode::ENTCOMMENT);
  rNodeMember["Name"] = rMember.sName;
  rNodeMember.AddSubNode(" Operation description ", CXMLNode::ENTCOMMENT);
  rNodeMember["Description"] = rMember.sDescr;
  rNodeMember.AddSubNode(" Soap action ", CXMLNode::ENTCOMMENT);
  rNodeMember["SoapAction"] = rMember.sSoapAction;
  rNodeMember.AddSubNode(" Function is non-mutable ", CXMLNode::ENTCOMMENT);
  rNodeMember["IsConst"] = rMember.bIsConst;
  rNodeMember["Const"] = rMember.bIsConst ? " const" : "";
  rNodeMember.AddSubNode("Params") << rMember.lsParamList;

  return rNodeMembers;
}

CXMLNode& operator<<(CXMLNode& rNodeClasses, const SClass& rClass)
{
  CXMLNode& rNodeClass = rNodeClasses.AddSubNode("Class");
  std::string sClassNs = 
    (rClass.sNamespace.substr(0, 2) == "::") ? rClass.sNamespace.substr(2) : rClass.sNamespace;
  std::string sServiceName = 
    (((rClass.sName[0] == 'C' || rClass.sName[0] == 'I') && (toupper(rClass.sName[1]) == rClass.sName[1])) 
        ? rClass.sName.substr(1) : rClass.sName);

  std::string sServiceNamespace = sClassNs;
  rise::StrReplace(sServiceNamespace, "::", ".", true);
  std::string sServiceNsName = sServiceNamespace + sServiceName;

  rNodeClass.AddSubNode(" Service class name ", CXMLNode::ENTCOMMENT);
  rNodeClass["Name"] = rClass.sName;
  rNodeClass.AddSubNode(" Service description ", CXMLNode::ENTCOMMENT);
  rNodeClass["Description"] = rClass.sDescr;
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
  rNodeClass.AddSubNode(" Service operations ", CXMLNode::ENTCOMMENT);
  rNodeClass.AddSubNode("Members") << rClass.lsMember;

  std::string sOpeningNs;
  std::string sEndingNs;
  std::string::size_type nPos = 0;
  std::string::size_type nPrevPos = 0;
  while((nPos = sClassNs.find("::", nPos)) != std::string::npos)
  {
    sOpeningNs += "namespace " + sClassNs.substr(nPrevPos, nPos - nPrevPos) + "\n{\n";
    sEndingNs += "}\n";
    nPos += 2;
    nPrevPos = nPos;
  }

  rNodeClass.AddSubNode(" Opening namespace ", CXMLNode::ENTCOMMENT);
  rNodeClass["OpeningNs"] = sOpeningNs;
  rNodeClass.AddSubNode(" Closing namespace ", CXMLNode::ENTCOMMENT);
  rNodeClass["EndingNs"] = sEndingNs;

  return rNodeClasses;
}

CXMLNode& operator<<(CXMLNode& rNodeStructs, const SStruct& rStruct)
{
  if (rStruct.bForward)
  {
    throw "Struct \"" + rStruct.sName + "\" is not fully declared";
  }

  CXMLNode& rNodeStruct = rNodeStructs.AddSubNode("Struct");

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
  rNodeStruct["Name"] = rStruct.sNamespace + rStruct.sName;
  rNodeStruct.AddSubNode(" Struct fields ", CXMLNode::ENTCOMMENT);
  rNodeStruct.AddSubNode("Members") << rStruct.lsMember;

  return rNodeStructs;
}

CXMLNode& operator<<(CXMLNode& rNodeTypedefs, const STypedef& rTypedef)
{
  CXMLNode& rNodeTypedef = rNodeTypedefs.AddSubNode("Typedef");
  std::string sMangledName = (rTypedef.sNamespace.substr(0, 2) == "::") ?
    rTypedef.sNamespace.substr(2) : rTypedef.sNamespace;
  rise::StrReplace(sMangledName, "::", "_", true);

  rNodeTypedef.AddSubNode(" Typedef name ", CXMLNode::ENTCOMMENT);
  rNodeTypedef["Name"] = rTypedef.sNamespace + rTypedef.sName;
  rNodeTypedef.AddSubNode(" Typedef native name ", CXMLNode::ENTCOMMENT);
  rNodeTypedef["NativeName"] = ((rTypedef.sName[0] == 'T') &&
       (toupper(rTypedef.sName[1]) == rTypedef.sName[1]) ? rTypedef.sName.substr(1) : rTypedef.sName);
  
  
  rNodeTypedef.AddSubNode(" Typedef name with namespace ", CXMLNode::ENTCOMMENT);
  rNodeTypedef["MangledName"] = sMangledName + rTypedef.sName;
  rNodeTypedef.AddSubNode(" Source datatype ", CXMLNode::ENTCOMMENT);
  rNodeTypedef.AddSubNode("DataType") << rTypedef.stDataType;

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

  return rNodeInterfaces;
}

CXMLNode& operator<<(CXMLNode& rRootNode, const SProject& rProject)
{
  rRootNode.NodeName() = "Project";
  rRootNode["Name"] = rProject.sName;
  rRootNode.AddSubNode(" Project interfaces ", CXMLNode::ENTCOMMENT);
  rRootNode.AddSubNode("Interfaces") << rProject.lsInterfaces;

  return rRootNode;
}
