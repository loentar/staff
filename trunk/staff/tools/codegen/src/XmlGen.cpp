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
#include <staff/utils/stringutils.h>
#include <staff/utils/tostring.h>
#include <staff/utils/Log.h>
#include <staff/utils/Exception.h>
#include <staff/xml/Document.h>
#include <staff/xml/Element.h>
#include <staff/codegen/Interface.h>
#include "XmlGen.h"

namespace staff
{
namespace codegen
{

  std::string TypeToString(const DataType::Type eType)
  {
    switch (eType)
    {
    case DataType::TypeGeneric:
      return "generic";

    case DataType::TypeString:
      return "string";

    case DataType::TypeDataObject:
      return "dataobject";

    case DataType::TypeEnum:
      return "enum";

    case DataType::TypeStruct:
      return "struct";

    case DataType::TypeTypedef:
      return "typedef";

    case DataType::TypeTemplate:
      return "template";

    default:
      return "unknown";
    }
  }


  template<typename Type>
  xml::Element& operator<<(xml::Element& rStream, const std::list<Type>& rList)
  {
    for(typename std::list<Type>::const_iterator it = rList.begin(); it != rList.end(); ++it)
    {
      rStream << *it;
    }
    return rStream;
  }

  bool GetDataType(std::string& sOut, const DataType& rDataType, bool bAsUsed = false,
                   bool bNoModifiers = false)
  {
    if (!bNoModifiers && rDataType.bIsConst)
    {
      sOut += "const ";
    }

    if (!bNoModifiers && !rDataType.sPrefix.empty())
    {
      sOut += rDataType.sPrefix + " ";
    }

    if (bAsUsed && !rDataType.sUsedName.empty())
    {
      sOut += rDataType.sUsedName;
    }
    else
    {
      sOut += rDataType.sNamespace;
      if (!rDataType.sOwnerName.empty())
      {
        sOut += rDataType.sOwnerName + "::";
      }
      sOut += rDataType.sName;
    }

    bool bIsTemplate = !rDataType.lsParams.empty();
    if (!bIsTemplate)
    {
      if (!bNoModifiers)
      {
        sOut += (rDataType.bIsRef ? "&" : "");
      }
      return false;
    }

    sOut += "<";
    std::string::size_type nBeginTemplatePos = sOut.size();
    bool bSpace = !rDataType.lsParams.back().lsParams.empty();
    bool bFirst = true;

    for (std::list<DataType>::const_iterator it = rDataType.lsParams.begin();
         it != rDataType.lsParams.end(); ++it)
    {
      if (!bFirst)
      {
        sOut += ", ";
      }
      bIsTemplate = GetDataType(sOut, *it, bAsUsed);
      if (bFirst)
      {
        if (sOut.substr(nBeginTemplatePos, 2) == "::")
        {
          bSpace = true;
        }
        bFirst = false;
      }
    }
    if (bSpace) // first parameter begins with :: or last parameter was template: insert spaces
    {
      sOut.insert(nBeginTemplatePos, " ");
      sOut += ' ';
    }
    sOut += ">";

    if (!bNoModifiers)
    {
      sOut += (rDataType.bIsRef ? "&" : "");
    }
    return true;
  }

  xml::Element& operator<<(xml::Element& rNodeDataTypes, const DataType& rDataType)
  {
    std::string sUsedTypedef;
    GetDataType(sUsedTypedef, rDataType, true);

    std::string sNsName;
    GetDataType(sNsName, rDataType, false, true);

    std::string sType;
    GetDataType(sType, rDataType);
    rNodeDataTypes.SetValue(sType);

    rNodeDataTypes.CreateChildComment(" Is const ");
    rNodeDataTypes.CreateChildElement("IsConst", rDataType.bIsConst);
    rNodeDataTypes.CreateChildComment(" Is reference ");
    rNodeDataTypes.CreateChildElement("IsRef", rDataType.bIsRef);
    rNodeDataTypes.CreateChildComment(" Type name as used ");
    rNodeDataTypes.CreateChildElement("UsedName", !rDataType.sUsedName.empty()
                                      ? rDataType.sUsedName : sUsedTypedef);
    rNodeDataTypes.CreateChildComment(" Type name ");
    rNodeDataTypes.CreateChildElement("Name", rDataType.sName);
    rNodeDataTypes.CreateChildComment(" Type ns name ");
    rNodeDataTypes.CreateChildElement("NsName", sNsName);
    rNodeDataTypes.CreateChildComment(" Type namespace ");
    rNodeDataTypes.CreateChildElement("Namespace", rDataType.sNamespace);
    rNodeDataTypes.CreateChildComment(" Type ");
    rNodeDataTypes.CreateChildElement("Type", TypeToString(rDataType.eType));

    rNodeDataTypes.CreateChildComment(" Used typedef ");
    rNodeDataTypes.CreateChildElement("UsedTypedef", sUsedTypedef);

    rNodeDataTypes.CreateChildComment(" Template params ");
    xml::Element& rNodeTemplateParams = rNodeDataTypes.CreateChildElement("TemplateParams");
    int nNum = 1;
    for(std::list<DataType>::const_iterator it = rDataType.lsParams.begin();
        it != rDataType.lsParams.end(); ++it, ++nNum)
    {
      rNodeTemplateParams.CreateChildElement("TemplateParam" + ToString(nNum)) << *it;
    }

    if (rDataType.eType == DataType::TypeUnknown)
    {
      LogWarning() << "Unknown datatype: " << (rDataType.sNamespace + rDataType.sName);
    }

    return rNodeDataTypes;
  }

  xml::Element& operator<<(xml::Element& rNodeParams, const Param& rParam)
  {
    if (rParam.sName.size() != 0 && rParam.stDataType.sName != "void")
    {
      rNodeParams.CreateChildComment(" Parameter ");
      xml::Element& rNodeParam = rNodeParams.CreateChildElement("Param", "");

      rNodeParam.CreateChildComment(" Parameter name ");
      rNodeParam.CreateChildElement("Name", rParam.sName);
      rNodeParam.CreateChildComment(" Parameter description ");
      rNodeParam.CreateChildElement("Description", rParam.sDescr);
      rNodeParam.CreateChildComment(" Detailed parameter description ");
      rNodeParam.CreateChildElement("Detail", rParam.sDetail);
      rNodeParam.CreateChildComment(" Options ");
      xml::Element& rNodeOptions = rNodeParam.CreateChildElement("Options");

      for (StringMap::const_iterator itOption = rParam.mOptions.begin();
          itOption != rParam.mOptions.end(); ++itOption)
      {
        rNodeOptions.CreateChildElement(itOption->first, itOption->second);
      }
      rNodeParam.CreateChildComment(" Parameter type ");
      rNodeParam.CreateChildElement("DataType") << rParam.stDataType;

      std::string sValue = rNodeParams.GetValue();
      if (!sValue.empty())
      {
        sValue += ", ";
      }

      GetDataType(sValue, rParam.stDataType, true);
      sValue += " " + rParam.sName;
      rNodeParams.SetValue(sValue);
    }

    return rNodeParams;
  }

  xml::Element& operator<<(xml::Element& rNodeMembers, const Member& rMember)
  {
    rNodeMembers.CreateChildComment(" Operation ");
    xml::Element& rNodeMember = rNodeMembers.CreateChildElement("Member");

    rNodeMember.CreateChildComment(" Operation name ");
    rNodeMember.CreateChildElement("Name", rMember.sName);
    rNodeMember.CreateChildComment(" Operation description ");
    rNodeMember.CreateChildElement("Description", rMember.sDescr);
    rNodeMember.CreateChildComment(" Operation detailed description ");
    rNodeMember.CreateChildElement("Detail", rMember.sDetail);

    rNodeMember.CreateChildComment(" Function is non-mutable ");
    rNodeMember.CreateChildElement("IsConst", rMember.bIsConst);
    rNodeMember.CreateChildElement("Const", rMember.bIsConst ? " const" : "");
    rNodeMember.CreateChildComment(" Function is asynchronous ");
    rNodeMember.CreateChildElement("IsAsynch", rMember.bIsAsynch);

    rNodeMembers.CreateChildComment(" Options ");
    xml::Element& rNodeOptions = rNodeMember.CreateChildElement("Options");

    for (StringMap::const_iterator itOption = rMember.mOptions.begin();
        itOption != rMember.mOptions.end(); ++itOption)
    {
      rNodeOptions.CreateChildElement(itOption->first, itOption->second);
    }

    rNodeMember.CreateChildElement("Params", "") << rMember.lsParams;

    rNodeMember.CreateChildComment(" Return ");
    xml::Element& rNodeReturn = rNodeMember.CreateChildElement("Return");
    rNodeReturn << rMember.stReturn.stDataType;
    rNodeReturn.CreateChildComment(" Response Name ");
    rNodeReturn.CreateChildElement("ResponseName", rMember.stReturn.sName);

    return rNodeMembers;
  }


  xml::Element& operator<<(xml::Element& rNodeEnumMembers, const Enum::EnumMember& rEnumMember)
  {
    rNodeEnumMembers.CreateChildComment(" Enum members ");
    xml::Element& rNodeMember = rNodeEnumMembers.CreateChildElement("Member");

    rNodeMember.CreateChildComment(" Operation name ");
    rNodeMember.CreateChildElement("Name", rEnumMember.sName);
    rNodeMember.CreateChildComment(" Value ");
    rNodeMember.CreateChildElement("Value", rEnumMember.sValue);
    rNodeMember.CreateChildComment(" Description ");
    rNodeMember.CreateChildElement("Description", rEnumMember.sDescr);

    return rNodeEnumMembers;
  }

  void WriteCppNamespace(xml::Element& rNode, const std::string& sNamespace)
  {
    std::string sRealNamespace =
      (sNamespace.substr(0, 2) == "::") ? sNamespace.substr(2) : sNamespace;

    std::string sOpeningNs;
    std::string sEndingNs;
    std::string::size_type nPos = 0;
    std::string::size_type nPrevPos = 0;
    while((nPos = sRealNamespace.find("::", nPos)) != std::string::npos)
    {
      sOpeningNs += "namespace " + sRealNamespace.substr(nPrevPos, nPos - nPrevPos) + "\n{\n";
      sEndingNs += "}\n";
      nPos += 2;
      nPrevPos = nPos;
    }

    rNode.CreateChildComment(" Opening namespace ");
    rNode.CreateChildElement("OpeningNs", sOpeningNs);
    rNode.CreateChildComment(" Closing namespace ");
    rNode.CreateChildElement("EndingNs", sEndingNs);
  }

  xml::Element& operator<<(xml::Element& rNodeClasses, const Class& rClass)
  {
    xml::Element& rNodeClass = rNodeClasses.CreateChildElement("Class");
    std::string sServiceName =
      (((rClass.sName[0] == 'C' || rClass.sName[0] == 'I') && (toupper(rClass.sName[1]) == rClass.sName[1]))
          ? rClass.sName.substr(1) : rClass.sName);

    std::string sServiceNamespace = (rClass.sNamespace.substr(0, 2) == "::") ?
                                    rClass.sNamespace.substr(2) : rClass.sNamespace;
    StringReplace(sServiceNamespace, "::", ".", true);

    std::string sServiceNsName = sServiceNamespace + sServiceName;

    rNodeClass.CreateChildComment(" Service class name ");
    rNodeClass.CreateChildElement("Name", rClass.sName);
    rNodeClass.CreateChildComment(" Service class namespace ");
    rNodeClass.CreateChildElement("Namespace", rClass.sNamespace);
    rNodeClass.CreateChildComment(" Service class name with namespace ");
    rNodeClass.CreateChildElement("NsName", rClass.sNamespace + rClass.sName);
    rNodeClass.CreateChildComment(" Service name ");
    rNodeClass.CreateChildElement("ServiceName", sServiceName);
    rNodeClass.CreateChildComment(" Service name with namespace ");
    rNodeClass.CreateChildElement("ServiceNsName", sServiceNsName);
    rNodeClass.CreateChildComment(" Service namespace ");
    rNodeClass.CreateChildElement("ServiceNamespace", sServiceNamespace);
    rNodeClass.CreateChildComment(" Service description ");
    rNodeClass.CreateChildElement("Description", rClass.sDescr);
    rNodeClass.CreateChildComment(" Service detailed description ");
    rNodeClass.CreateChildElement("Detail", rClass.sDetail);

    rNodeClass.CreateChildComment(" Options ");
    xml::Element& rNodeOptions = rNodeClass.CreateChildElement("Options");

    for (StringMap::const_iterator itOption = rClass.mOptions.begin();
        itOption != rClass.mOptions.end(); ++itOption)
    {
      rNodeOptions.CreateChildElement(itOption->first, itOption->second);
    }

    rNodeClass.CreateChildComment(" Axis2/c modules to engage ");
    xml::Element& rNodeModules = rNodeClass.CreateChildElement("Modules");

    for (StringList::const_iterator itModule = rClass.lsModules.begin();
        itModule != rClass.lsModules.end(); ++itModule)
    {
      rNodeModules.CreateChildElement("Module", *itModule);
    }

    rNodeClass.CreateChildComment(" Service operations ");
    rNodeClass.CreateChildElement("Members") << rClass.lsMembers;

    WriteCppNamespace(rNodeClass, rClass.sNamespace);

    return rNodeClasses;
  }

  xml::Element& operator<<(xml::Element& rNodeEnums, const Enum& rEnum)
  {
    STAFF_ASSERT(!rEnum.bForward || rEnum.bExtern,
                "Enum \"" + rEnum.sName + "\" is not fully declared");

    xml::Element& rNodeEnum = rNodeEnums.CreateChildElement("Enum");

    rNodeEnum.CreateChildComment(" Enum name ");
    rNodeEnum.CreateChildElement("Name", rEnum.sName);
    rNodeEnum.CreateChildComment(" Enum name with namespace");
    rNodeEnum.CreateChildElement("NsName", rEnum.sNamespace +
                                 (rEnum.sOwnerName.empty() ? "" : (rEnum.sOwnerName + "::")) + rEnum.sName);
    rNodeEnum.CreateChildComment(" Enum namespace");
    rNodeEnum.CreateChildElement("Namespace", rEnum.sNamespace);
    rNodeEnum.CreateChildComment(" Enum owner name");
    rNodeEnum.CreateChildElement("Owner", rEnum.sOwnerName);

    rNodeEnum.CreateChildComment(" Description ");
    rNodeEnum.CreateChildElement("Description", rEnum.sDescr);
    rNodeEnum.CreateChildComment(" Detailed description ");
    rNodeEnum.CreateChildElement("Detail", rEnum.sDetail);

    rNodeEnum.CreateChildComment(" Is defined in other interface ");
    rNodeEnum.CreateChildElement("Extern", rEnum.bExtern);

    rNodeEnum.CreateChildComment(" Enum fields ");
    rNodeEnum.CreateChildElement("Members") << rEnum.lsMembers;

    WriteCppNamespace(rNodeEnum, rEnum.sNamespace);

    rNodeEnum.CreateChildComment(" Options ");
    xml::Element& rNodeOptions = rNodeEnum.CreateChildElement("Options");

    for (StringMap::const_iterator itOption = rEnum.mOptions.begin();
        itOption != rEnum.mOptions.end(); ++itOption)
    {
      rNodeOptions.CreateChildElement(itOption->first, itOption->second);
    }

    return rNodeEnums;
  }

  xml::Element& operator<<(xml::Element& rNodeStructs, const Struct& rStruct)
  {
    STAFF_ASSERT(!rStruct.bForward || rStruct.bExtern,
                "Struct \"" + rStruct.sName + "\" is not fully declared");

    xml::Element& rNodeStruct = rNodeStructs.CreateChildElement("Struct");

    rNodeStruct.CreateChildComment(" Struct name ");
    rNodeStruct.CreateChildElement("Name", rStruct.sName);
    rNodeStruct.CreateChildComment(" Struct name with namespace");
    rNodeStruct.CreateChildElement("NsName", rStruct.sNamespace +
                                   (rStruct.sOwnerName.empty() ? "" : (rStruct.sOwnerName + "::")) +
                                   rStruct.sName);
    rNodeStruct.CreateChildComment(" Struct namespace");
    rNodeStruct.CreateChildElement("Namespace", rStruct.sNamespace);
    rNodeStruct.CreateChildComment(" Struct owner name");
    rNodeStruct.CreateChildElement("Owner", rStruct.sOwnerName);

    // parent
    std::string::size_type nPos = rStruct.sParentName.find_last_of("::");
    const std::string sParentName = nPos != std::string::npos ?
                                    rStruct.sParentName.substr(nPos + 1) : rStruct.sParentName;
    rNodeStruct.CreateChildComment(" Parent struct name ");
    rNodeStruct.CreateChildElement("ParentName", sParentName);
    rNodeStruct.CreateChildComment(" Parent struct name as used by declaration ");
    rNodeStruct.CreateChildElement("ParentUsedName", rStruct.sParentName);
    rNodeStruct.CreateChildComment(" Parent struct actual namespace ");
    rNodeStruct.CreateChildElement("ParentNamespace", rStruct.sParentNamespace);
    rNodeStruct.CreateChildComment(" Parent struct with actual namespace ");
    rNodeStruct.CreateChildElement("ParentNsName", rStruct.sParentNamespace + sParentName);

    rNodeStruct.CreateChildComment(" Description ");
    rNodeStruct.CreateChildElement("Description", rStruct.sDescr);
    rNodeStruct.CreateChildComment(" Detailed description ");
    rNodeStruct.CreateChildElement("Detail", rStruct.sDetail);

    rNodeStruct.CreateChildComment(" Is defined in other interface ");
    rNodeStruct.CreateChildElement("Extern", rStruct.bExtern);

    rNodeStruct.CreateChildComment(" Struct fields ");
    rNodeStruct.CreateChildElement("Members", "") << rStruct.lsMembers;

    WriteCppNamespace(rNodeStruct, rStruct.sNamespace);

    rNodeStruct.CreateChildComment(" Options ");
    xml::Element& rNodeOptions = rNodeStruct.CreateChildElement("Options");

    for (StringMap::const_iterator itOption = rStruct.mOptions.begin();
        itOption != rStruct.mOptions.end(); ++itOption)
    {
      rNodeOptions.CreateChildElement(itOption->first, itOption->second);
    }

    rNodeStruct.CreateChildComment(" Sub enums ");
    rNodeStruct.CreateChildElement("Enums") << rStruct.lsEnums;

    rNodeStruct.CreateChildComment(" Sub structs ");
    rNodeStruct.CreateChildElement("Structs") << rStruct.lsStructs;

    return rNodeStructs;
  }

  xml::Element& operator<<(xml::Element& rNodeTypedefs, const Typedef& rTypedef)
  {
    xml::Element& rNodeTypedef = rNodeTypedefs.CreateChildElement("Typedef");

    rNodeTypedef.CreateChildComment(" Typedef name ");
    rNodeTypedef.CreateChildElement("Name", rTypedef.sName);
    rNodeTypedef.CreateChildComment(" Typedef name with namespace ");
    rNodeTypedef.CreateChildElement("NsName", rTypedef.sNamespace + rTypedef.sName);
    rNodeTypedef.CreateChildComment(" Typedef namespace ");
    rNodeTypedef.CreateChildElement("Namespace", rTypedef.sNamespace);
    rNodeTypedef.CreateChildComment(" Description ");
    rNodeTypedef.CreateChildElement("Description", rTypedef.sDescr);
    rNodeTypedef.CreateChildComment(" Detailed description ");
    rNodeTypedef.CreateChildElement("Detail", rTypedef.sDetail);

    rNodeTypedef.CreateChildComment(" Source datatype ");
    rNodeTypedef.CreateChildElement("DataType") << rTypedef.stDataType;

    rNodeTypedef.CreateChildComment(" Defined in other interface ");
    rNodeTypedef.CreateChildElement("Extern", rTypedef.bExtern);

    WriteCppNamespace(rNodeTypedef, rTypedef.sNamespace);

    rNodeTypedef.CreateChildComment(" Options ");
    xml::Element& rNodeOptions = rNodeTypedef.CreateChildElement("Options");

    for (StringMap::const_iterator itOption = rTypedef.mOptions.begin();
        itOption != rTypedef.mOptions.end(); ++itOption)
    {
      rNodeOptions.CreateChildElement(itOption->first, itOption->second);
    }

    return rNodeTypedefs;
  }

  xml::Element& operator<<(xml::Element& rNodeInterfaces, const Interface& rInterface)
  {
    xml::Element& rNodeInterface = rNodeInterfaces.CreateChildElement("Interface");

    rNodeInterface.CreateChildComment(" Interface name ");
    rNodeInterface.CreateChildElement("Name", rInterface.sName);

    rNodeInterface.CreateChildComment(" Interface namespace ");
    rNodeInterface.CreateChildElement("Namespace", rInterface.sNamespace);

    rNodeInterface.CreateChildComment(" Interface name with namespace ");
    rNodeInterface.CreateChildElement("NsName", rInterface.sNamespace + rInterface.sName);

    rNodeInterface.CreateChildComment(" Interface file name ");
    rNodeInterface.CreateChildElement("FileName", rInterface.sFileName);

    rNodeInterface.CreateChildComment(" Interface file path ");
    rNodeInterface.CreateChildElement("FilePath", rInterface.sFilePath);

    rNodeInterface.CreateChildComment(" Options ");
    xml::Element& rNodeOptions = rNodeInterface.CreateChildElement("Options");

    for (StringMap::const_iterator itOption = rInterface.mOptions.begin();
        itOption != rInterface.mOptions.end(); ++itOption)
    {
      rNodeOptions.CreateChildElement(itOption->first, itOption->second);
    }

    // included files
    rNodeInterface.CreateChildComment(" Included files ");
    xml::Element& rNodeIncludes = rNodeInterface.CreateChildElement("Includes");

    for (std::list<Include>::const_iterator itInclude = rInterface.lsIncludes.begin();
        itInclude != rInterface.lsIncludes.end(); ++itInclude)
    {
      std::string sNamespace = itInclude->sNamespace.substr(0, 2) == "::" ?
                            itInclude->sNamespace.substr(2) : itInclude->sNamespace;
      StringReplace(sNamespace, "::", ".", true);

      xml::Element& rNodeInclude = rNodeIncludes.CreateChildElement("Include");
      rNodeInclude.CreateChildComment(" Interface name ");
      rNodeInclude.CreateChildElement("Name", itInclude->sInterfaceName);
      rNodeInclude.CreateChildComment(" Interface name ");
      rNodeInclude.CreateChildElement("Namespace", itInclude->sNamespace);
      rNodeInclude.CreateChildComment(" Interface ns name ");
      rNodeInclude.CreateChildElement("NsName", sNamespace + itInclude->sInterfaceName);
      rNodeInclude.CreateChildComment(" File name ");
      rNodeInclude.CreateChildElement("FileName", itInclude->sFileName);
      rNodeInclude.CreateChildComment(" File path ");
      rNodeInclude.CreateChildElement("FilePath", itInclude->sFilePath);
      rNodeInclude.CreateChildComment(" Target namespace ");
      rNodeInclude.CreateChildElement("TargetNamespace", itInclude->sTargetNs);
    }

    rNodeInterface.CreateChildComment(" Interface enums ");
    rNodeInterface.CreateChildElement("Enums") << rInterface.lsEnums;

    rNodeInterface.CreateChildComment(" Interface structs ");
    rNodeInterface.CreateChildElement("Structs") << rInterface.lsStructs;

    rNodeInterface.CreateChildComment(" Interface typedefs ");
    rNodeInterface.CreateChildElement("Typedefs") << rInterface.lsTypedefs;

    rNodeInterface.CreateChildComment(" Interface services ");
    rNodeInterface.CreateChildElement("Classes") << rInterface.lsClasses;

    return rNodeInterfaces;
  }

  xml::Element& operator<<(xml::Element& rRootNode, const Project& rProject)
  {
    rRootNode.SetName("Project");
    rRootNode.CreateChildElement("Name", rProject.sName);
    rRootNode.CreateChildComment(" Component namespace ");
    rRootNode.CreateChildElement("Namespace", rProject.sNamespace);
    WriteCppNamespace(rRootNode, rProject.sNamespace);

    rRootNode.CreateChildComment(" Project interfaces ");
    rRootNode.CreateChildElement("Interfaces") << rProject.lsInterfaces;

    return rRootNode;
  }
}
}
