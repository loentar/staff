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

RISE_DECLARE_PLUGIN(staff::codegen::CDasDataSourceParser);

namespace staff
{
namespace codegen
{

  CDasDataSourceParser::CDasDataSourceParser()
  {
  }

  CDasDataSourceParser::~CDasDataSourceParser()
  {
  }


  const std::string& CDasDataSourceParser::GetId()
  {
    return m_sId;
  }

  void CDasDataSourceParser::Process(const SParseSettings& rParseSettings, SProject& rProject)
  {
    for (TStringList::const_iterator itFile = rParseSettings.lsFiles.begin();
        itFile != rParseSettings.lsFiles.end(); ++itFile)
    {
      const std::string& sFileName = rParseSettings.sInDir + "/" + *itFile;

      m_sDir = sFileName.substr(0, sFileName.find_last_of('/') + 1);

      rise::xml::CXMLDocument tDocDataSources;
      tDocDataSources.LoadFromFile(sFileName);

      const rise::xml::CXMLNode& rNodeDataSources = tDocDataSources.GetRoot();

      if (rNodeDataSources.NodeName() == "types")
      {
        SInterface stInterface;
        const std::string& sFileNameOnly = sFileName.substr(sFileName.find_last_of('/') + 1);
        std::string::size_type nPos = sFileNameOnly.find_last_of('.');
        stInterface.sFileName = sFileNameOnly;

        stInterface.sName = (nPos != std::string::npos) ?  sFileNameOnly.substr(0, nPos) : sFileNameOnly;

        std::string sNamespace;
        rise::xml::CXMLNode::TXMLAttrConstIterator itNs = rNodeDataSources.FindAttribute("namespace");
        if (itNs != rNodeDataSources.AttrEnd())
        {
          sNamespace = itNs->sAttrValue.AsString();
        }

        rise::StrReplace(sNamespace, ".", "::", true);
        sNamespace = "::" + sNamespace + "::";

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
          ParseProject(*itNodeDataSource, rProject);
        }
      }

    }

    if (!rProject.lsInterfaces.empty())
    {
      rProject.sNamespace = rProject.lsInterfaces.front().sNamespace;
    }
  }

  void CDasDataSourceParser::ParseTypes(const rise::xml::CXMLNode& rNodeTypes, SProject& rProject,
                                        SInterface& rInterface, const std::string& sNamespace)
  {
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rNodeTypes.NodeBegin();
          itType != rNodeTypes.NodeEnd(); ++itType)
    {
      const rise::xml::CXMLNode& rNodeType = *itType;
      const std::string& sName = rNodeType.NodeName();
      if (sName == "include")
      {
        const std::string& sFileName = rNodeType.Attribute("filename");

        bool bFound = false;

        for (std::list<SInterface>::const_iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
        {
          if (itInterface->sFileName == sFileName)
          {
            SInclude stInclude;
            stInclude.sFileName = itInterface->sFileName;
            stInclude.sNamespace = itInterface->sNamespace;
            stInclude.sInterfaceName = itInterface->sName;
            rInterface.lsIncludes.push_back(stInclude);
            bFound = true;
            break;
          }
        }

        if (!bFound)
        {
          SInterface stInterface;

          std::string::size_type nPos = sFileName.find_last_of('.');
          if (nPos != std::string::npos)
          {
            stInterface.sName = sFileName.substr(0, nPos);
          }
          else
          {
            stInterface.sName = sFileName;
          }

          stInterface.sFileName = sFileName;
          stInterface.sNamespace = sNamespace;

          rise::LogDebug() << "including " << sFileName;
          rise::xml::CXMLDocument tTypesDoc;
          tTypesDoc.LoadFromFile(m_sDir + sFileName);
          const rise::xml::CXMLNode& rNodeTypes = tTypesDoc.GetRoot();
          ParseTypes(rNodeTypes, rProject, stInterface, sNamespace);


          SInclude stInclude;
          stInclude.sFileName = sFileName;
          stInclude.sNamespace = sNamespace;
          stInclude.sInterfaceName = stInterface.sName;
          rInterface.lsIncludes.push_back(stInclude);

          for (std::list<SStruct>::const_iterator itStruct = stInterface.lsStructs.begin();
            itStruct != stInterface.lsStructs.end(); ++itStruct)
          {
            rInterface.lsStructs.push_back(*itStruct);
            rInterface.lsStructs.back().bExtern = true;
          }

          for (std::list<STypedef>::const_iterator itTypedef = stInterface.lsTypedefs.begin();
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
          SStruct stStruct;
          stStruct.sName = sName;
          stStruct.sNamespace = sNamespace;
          stStruct.sDescr = sDescr;
          stStruct.bForward = false;

          for (rise::xml::CXMLNode::TXMLNodeConstIterator itMember = itType->NodeBegin();
                itMember != itType->NodeEnd(); ++itMember)
          {
            const rise::xml::CXMLNode& rMember = *itMember;
            SParam stMember;

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
          STypedef stTypedef;
          stTypedef.sName = sName;
          stTypedef.sNamespace = sNamespace;
          stTypedef.stDataType.sName = "list";
          stTypedef.stDataType.sNamespace = "std::";
          stTypedef.stDataType.sUsedName = "std::list";
          stTypedef.stDataType.eType = SDataType::ETemplate;
          ParseDescr(rNodeType, stTypedef.sDescr);

          SDataType stItemDataType;
          stItemDataType.sName = rNodeType.Attribute("itemtype").AsString();
          FixDataType(stItemDataType, rInterface, sNamespace);
          stItemDataType.sUsedName = stItemDataType.sNamespace + stItemDataType.sName;
          OptimizeCppNs(stItemDataType.sUsedName, stTypedef.sNamespace);

          stTypedef.stDataType.lsParams.push_back(stItemDataType);

          rInterface.lsTypedefs.push_back(stTypedef);
        }
        else
        {
          STypedef stTypedef;
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


  void CDasDataSourceParser::ParseProject(const rise::xml::CXMLNode& rDataSourceNode, SProject& rProject)
  {
    SInterface stInterface;

    stInterface.sName = rDataSourceNode.Attribute("name").AsString();

    std::string sNamespace;
    rise::xml::CXMLNode::TXMLAttrConstIterator itNs = rDataSourceNode.FindAttribute("namespace");
    if (itNs != rDataSourceNode.AttrEnd())
    {
      sNamespace = itNs->sAttrValue.AsString();
    }

    rise::StrReplace(sNamespace, ".", "::", true);
    sNamespace = "::" + sNamespace + "::";

    stInterface.sNamespace = sNamespace;

    // Types
    const rise::xml::CXMLNode& rNodeTypes = rDataSourceNode.Subnode("types");
    ParseTypes(rNodeTypes, rProject, stInterface, sNamespace);

    // class

    SClass stClass;
    stClass.sName = stInterface.sName;
    stClass.sNamespace = sNamespace;

    ParseDescr(rDataSourceNode, stClass.sDescr);

    // Operations
    const rise::xml::CXMLNode& rOperations = rDataSourceNode.Subnode("operations");
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itOperation = rOperations.NodeBegin();
          itOperation != rOperations.NodeEnd(); ++itOperation)
    {
      const rise::xml::CXMLNode& rOperation = *itOperation;
      SMember stMember;

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
        SParam stParam;
        stParam.sName = itParam->NodeName();

        stParam.stDataType.sName = itParam->Attribute("type").AsString();

        FixDataType(stParam.stDataType, stInterface, sNamespace);
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace
                                                 + stParam.stDataType.sName;
        OptimizeCppNs(stParam.stDataType.sUsedName, sNamespace);
        stParam.stDataType.sNodeName = stParam.sName;

        if (stParam.stDataType.eType == SDataType::EString ||
            stParam.stDataType.eType == SDataType::EStruct ||
            stParam.stDataType.eType == SDataType::ETypedef)
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

  void CDasDataSourceParser::ParseDescr(const rise::xml::CXMLNode& rNode, std::string& sDescr)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itDescr = rNode.FindAttribute("descr");
    if (itDescr != rNode.AttrEnd())
    {
      sDescr = itDescr->sAttrValue.AsString();
    }
  }

  bool CDasDataSourceParser::FixDataType(SDataType& rDataType, const SInterface& rInterface, const std::string& sNamespace)
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

    for (std::list<STypedef>::const_iterator itTypedef = rInterface.lsTypedefs.begin();
        itTypedef != rInterface.lsTypedefs.end(); ++itTypedef)
    {
      const STypedef& rTypedef = *itTypedef;
      if (rTypedef.sName == rDataType.sName)
      {
        rDataType.eType = SDataType::ETypedef;
        rDataType.sNamespace = sNamespace;
        return true;
      }
    }

    for (std::list<SStruct>::const_iterator itStruct = rInterface.lsStructs.begin();
        itStruct != rInterface.lsStructs.end(); ++itStruct)
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

  const std::string CDasDataSourceParser::m_sId = "dasdatasource";
}
}
