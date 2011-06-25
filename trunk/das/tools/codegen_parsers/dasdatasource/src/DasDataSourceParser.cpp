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

#include <memory>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/plugin/PluginExport.h>
#include <rise/xml/XMLDocument.h>
#include <staff/codegen/tools.h>
#include "DasDataSourceParser.h"

RISE_DECLARE_PLUGIN(staff::codegen::DasDataSourceParser)

namespace staff
{
namespace codegen
{

  DasDataSourceParser::DasDataSourceParser()
  {
  }

  DasDataSourceParser::~DasDataSourceParser()
  {
  }


  const std::string& DasDataSourceParser::GetId()
  {
    return m_sId;
  }

  void DasDataSourceParser::Process(const ParseSettings& rParseSettings, Project& rProject)
  {
    std::string sRootNs = "::";
    StringMap::const_iterator itRootNs = rParseSettings.mEnv.find("rootns");
    if (itRootNs != rParseSettings.mEnv.end() && !itRootNs->second.empty())
    {
      sRootNs = "::" + itRootNs->second + "::";
      rise::StrReplace(sRootNs, ".", "::", true);
    }

    for (StringList::const_iterator itFile = rParseSettings.lsFiles.begin();
        itFile != rParseSettings.lsFiles.end(); ++itFile)
    {
      const std::string& sFileName = rParseSettings.sInDir + *itFile;

      m_sDir = sFileName.substr(0, sFileName.find_last_of('/') + 1);

      rise::xml::CXMLDocument tDocDataSources;
      tDocDataSources.LoadFromFile(sFileName);

      const rise::xml::CXMLNode& rNodeDataSources = tDocDataSources.GetRoot();

      if (rNodeDataSources.NodeName() == "types")
      {
        Interface stInterface;

        std::string::size_type nPos = sFileName.find_last_of("/\\");
        stInterface.sFileName = (nPos != std::string::npos) ?
                                                sFileName.substr(nPos + 1) : sFileName;
        stInterface.sFilePath = (nPos != std::string::npos) ?
                                                sFileName.substr(0, nPos + 1) : "";

        stInterface.sName = stInterface.sFileName;
        nPos = stInterface.sName.find_last_of('.');
        if (nPos != std::string::npos)
        {
          stInterface.sName.erase(nPos);
        }

        std::string sNamespace;
        rise::xml::CXMLNode::TXMLAttrConstIterator itNs = rNodeDataSources.FindAttribute("namespace");
        if (itNs != rNodeDataSources.AttrEnd())
        {
          sNamespace = itNs->sAttrValue.AsString();
        }

        rise::StrReplace(sNamespace, ".", "::", true);
        sNamespace = sRootNs + sNamespace + "::";

        stInterface.sNamespace = sNamespace;

        ParseTypes(rNodeDataSources, rProject, stInterface, sNamespace);

        rProject.lsInterfaces.push_back(stInterface);
      }
      else
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeDataSource = rNodeDataSources.NodeBegin();
          itNodeDataSource != rNodeDataSources.NodeEnd(); ++itNodeDataSource)
      {
        if (itNodeDataSource->NodeName() == "datasource")
        {
          ParseProject(*itNodeDataSource, rProject, sRootNs);
        }
      }

    }

    if (!rProject.lsInterfaces.empty())
    {
      rProject.sNamespace = rProject.lsInterfaces.front().sNamespace;
    }
  }

  void DasDataSourceParser::ParseTypes(const rise::xml::CXMLNode& rNodeTypes, Project& rProject,
                                        Interface& rInterface, const std::string& sNamespace)
  {
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rNodeTypes.NodeBegin();
          itType != rNodeTypes.NodeEnd(); ++itType)
    {
      const rise::xml::CXMLNode& rNodeType = *itType;
      const std::string& sName = rNodeType.NodeName();
      if (sName == "include")
      {
        const std::string& sFileName = rNodeType.Attribute("filename");

        std::string::size_type nPos = sFileName.find_last_of("/\\");
        const std::string& sInterfaceFileName = (nPos != std::string::npos) ?
                                                sFileName.substr(nPos + 1) : sFileName;
        const std::string& sInterfaceFilePath = (nPos != std::string::npos) ?
                                                sFileName.substr(0, nPos + 1) : "";


        bool bFound = false;

        for (std::list<Interface>::const_iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
        {
          if (itInterface->sFileName == sInterfaceFileName &&
              itInterface->sFilePath == sInterfaceFilePath)
          {
            Include stInclude;
            stInclude.sFileName = itInterface->sFileName;
            stInclude.sFilePath = itInterface->sFilePath;
            stInclude.sNamespace = itInterface->sNamespace;
            stInclude.sInterfaceName = itInterface->sName;
            rInterface.lsIncludes.push_back(stInclude);
            bFound = true;
            break;
          }
        }

        if (!bFound)
        {
          Interface stInterface;

          std::string::size_type nPos = sInterfaceFileName.find_last_of('.');
          if (nPos != std::string::npos)
          {
            stInterface.sName = sInterfaceFileName.substr(0, nPos);
          }
          else
          {
            stInterface.sName = sInterfaceFileName;
          }

          stInterface.sFileName = sInterfaceFileName;
          stInterface.sFilePath = sInterfaceFilePath;
          stInterface.sNamespace = sNamespace;

          rise::LogDebug() << "including " << sFileName;
          rise::xml::CXMLDocument tTypesDoc;
          tTypesDoc.LoadFromFile(m_sDir + sFileName);
          const rise::xml::CXMLNode& rNodeTypes = tTypesDoc.GetRoot();
          ParseTypes(rNodeTypes, rProject, stInterface, sNamespace);


          Include stInclude;
          stInclude.sFileName = sInterfaceFileName;
          stInclude.sFilePath = sInterfaceFilePath;
          stInclude.sNamespace = sNamespace;
          stInclude.sInterfaceName = stInterface.sName;
          rInterface.lsIncludes.push_back(stInclude);

          for (std::list<Struct>::const_iterator itStruct = stInterface.lsStructs.begin();
            itStruct != stInterface.lsStructs.end(); ++itStruct)
          {
            rInterface.lsStructs.push_back(*itStruct);
            rInterface.lsStructs.back().bExtern = true;
          }

          for (std::list<Typedef>::const_iterator itTypedef = stInterface.lsTypedefs.begin();
            itTypedef != stInterface.lsTypedefs.end(); ++itTypedef)
          {
            rInterface.lsTypedefs.push_back(*itTypedef);
            rInterface.lsTypedefs.back().bExtern = true;
          }

          rProject.lsInterfaces.push_back(stInterface);
        }
      }
      else
      {
        const std::string& sType = rNodeType.Attribute("type");
        std::string sDescr;

        ParseDescr(rNodeType, sDescr);

        if (sType == "struct")
        {
          Struct stStruct;
          stStruct.sName = sName;
          stStruct.sNamespace = sNamespace;
          stStruct.sDescr = sDescr;
          stStruct.bForward = false;

          for (rise::xml::CXMLNode::TXMLNodeConstIterator itMember = itType->NodeBegin();
                itMember != itType->NodeEnd(); ++itMember)
          {
            const rise::xml::CXMLNode& rMember = *itMember;
            Param stMember;

            stMember.sName = rMember.NodeName();
            ParseDescr(rMember, stMember.sDescr);

            stMember.stDataType.sName = rMember.Attribute("type").AsString();
            FixDataType(stMember.stDataType, rInterface, sNamespace);
            stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
            OptimizeCppNs(stMember.stDataType.sUsedName, stStruct.sNamespace);

            stStruct.lsMembers.push_back(stMember);
          }

          rInterface.lsStructs.push_back(stStruct);
        }
        else
        if (sType == "list")
        {
          Typedef stTypedef;
          stTypedef.sName = sName;
          stTypedef.sNamespace = sNamespace;
          stTypedef.stDataType.sName = "list";
          stTypedef.stDataType.sNamespace = "std::";
          stTypedef.stDataType.sUsedName = "std::list";
          stTypedef.stDataType.eType = DataType::TypeTemplate;
          ParseDescr(rNodeType, stTypedef.sDescr);

          DataType stItemDataType;
          stItemDataType.sName = rNodeType.Attribute("itemtype").AsString();
          FixDataType(stItemDataType, rInterface, sNamespace);
          stItemDataType.sUsedName = stItemDataType.sNamespace + stItemDataType.sName;
          OptimizeCppNs(stItemDataType.sUsedName, stTypedef.sNamespace);

          stTypedef.stDataType.lsParams.push_back(stItemDataType);

          rInterface.lsTypedefs.push_back(stTypedef);
        }
        else
        {
          Typedef stTypedef;
          stTypedef.sName = rNodeType.Attribute("type").AsString();
          stTypedef.sNamespace = sNamespace;
          stTypedef.stDataType.sName = sName;
          FixDataType(stTypedef.stDataType, rInterface, sNamespace);
          ParseDescr(rNodeType, stTypedef.sDescr);
          stTypedef.stDataType.sUsedName = stTypedef.stDataType.sNamespace + stTypedef.stDataType.sName;
          OptimizeCppNs(stTypedef.stDataType.sNamespace, stTypedef.sNamespace);

          rInterface.lsTypedefs.push_back(stTypedef);
        }
      }
    }
  }


  void DasDataSourceParser::ParseProject(const rise::xml::CXMLNode& rDataSourceNode, Project& rProject,
                                          const std::string& sRootNs)
  {
    Interface stInterface;

    stInterface.sName = rDataSourceNode.Attribute("name").AsString();

    std::string sNamespace;
    rise::xml::CXMLNode::TXMLAttrConstIterator itNs = rDataSourceNode.FindAttribute("namespace");
    if (itNs != rDataSourceNode.AttrEnd())
    {
      sNamespace = itNs->sAttrValue.AsString();
    }

    rise::StrReplace(sNamespace, ".", "::", true);
    sNamespace = sRootNs + sNamespace + "::";

    stInterface.sNamespace = sNamespace;

    // Types
    const rise::xml::CXMLNode& rNodeTypes = rDataSourceNode.Subnode("types");
    ParseTypes(rNodeTypes, rProject, stInterface, sNamespace);

    // class

    Class stClass;
    stClass.sName = stInterface.sName;
    stClass.sNamespace = sNamespace;

    ParseDescr(rDataSourceNode, stClass.sDescr);

    // Operations
    const rise::xml::CXMLNode& rOperations = rDataSourceNode.Subnode("operations");
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itOperation = rOperations.NodeBegin();
          itOperation != rOperations.NodeEnd(); ++itOperation)
    {
      const rise::xml::CXMLNode& rOperation = *itOperation;
      Member stMember;

      stMember.sName = rOperation.Attribute("name").AsString();

      ParseDescr(rOperation, stMember.sDescr);

      rise::xml::CXMLNode::TXMLNodeConstIterator itReturn = rOperation.FindSubnode("return");
      if (itReturn != rOperation.NodeEnd())
      {
        stMember.stReturn.stDataType.sName = itReturn->Attribute("type").AsString();
        FixDataType(stMember.stReturn.stDataType, stInterface, sNamespace);
        stMember.stReturn.stDataType.sUsedName = stMember.stReturn.stDataType.sNamespace
                                                 + stMember.stReturn.stDataType.sName;
        OptimizeCppNs(stMember.stReturn.stDataType.sUsedName, stClass.sNamespace);
      }
      else
      {
        stMember.stReturn.stDataType.sName = "void";
      }

      const rise::xml::CXMLNode& rParams = rOperation.Subnode("params");
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itParam = rParams.NodeBegin();
            itParam != rParams.NodeEnd(); ++itParam)
      {
        Param stParam;
        stParam.sName = itParam->NodeName();

        stParam.stDataType.sName = itParam->Attribute("type").AsString();

        FixDataType(stParam.stDataType, stInterface, sNamespace);
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace
                                                 + stParam.stDataType.sName;
        OptimizeCppNs(stParam.stDataType.sUsedName, sNamespace);
        stParam.stDataType.sNodeName = stParam.sName;

        if (stParam.stDataType.eType == DataType::TypeString ||
            stParam.stDataType.eType == DataType::TypeStruct ||
            stParam.stDataType.eType == DataType::TypeTypedef)
        {
          stParam.stDataType.bIsConst = true;
          stParam.stDataType.bIsRef = true;
        }

        stMember.lsParams.push_back(stParam);
      }

      stClass.lsMembers.push_back(stMember);
    }

    stInterface.lsClasses.push_back(stClass);

    rProject.lsInterfaces.push_back(stInterface);
  }

  void DasDataSourceParser::ParseDescr(const rise::xml::CXMLNode& rNode, std::string& sDescr)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itDescr = rNode.FindAttribute("descr");
    if (itDescr != rNode.AttrEnd())
    {
      sDescr = itDescr->sAttrValue.AsString();
    }
  }

  bool DasDataSourceParser::FixDataType(DataType& rDataType, const Interface& rInterface, const std::string& sNamespace)
  {
    if (rDataType.sName == "string")
    {
      rDataType.sName = "string";
      rDataType.eType = DataType::TypeString;
      rDataType.sNamespace = "std::";
      rDataType.sUsedName = "std::string";

      return true;
    }

    if (rDataType.sName == "DataObject")
    {
      rDataType.eType = DataType::TypeDataObject;
      rDataType.sNamespace = "staff::";
      rDataType.sUsedName = "staff::DataObject";

      return true;
    }

    for (std::list<Typedef>::const_iterator itTypedef = rInterface.lsTypedefs.begin();
        itTypedef != rInterface.lsTypedefs.end(); ++itTypedef)
    {
      const Typedef& rTypedef = *itTypedef;
      if (rTypedef.sName == rDataType.sName)
      {
        rDataType.eType = DataType::TypeTypedef;
        rDataType.sNamespace = sNamespace;
        return true;
      }
    }

    for (std::list<Struct>::const_iterator itStruct = rInterface.lsStructs.begin();
        itStruct != rInterface.lsStructs.end(); ++itStruct)
    {
      const Struct& rStruct = *itStruct;
      if (rStruct.sName == rDataType.sName)
      {
        rDataType.eType = DataType::TypeStruct;
        rDataType.sNamespace = sNamespace;
        return true;
      }
    }

    return false;
  }

  const std::string DasDataSourceParser::m_sId = "dasdatasource";
}
}
