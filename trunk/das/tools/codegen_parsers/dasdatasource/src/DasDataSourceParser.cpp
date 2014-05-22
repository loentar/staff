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
#include <staff/utils/Log.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/PluginExport.h>
#include <staff/common/Exception.h>
#include <staff/xml/Document.h>
#include <staff/xml/Attribute.h>
#include <staff/xml/Element.h>
#include <staff/xml/XmlReader.h>
#include <staff/codegen/tools.h>
#include "DasDataSourceParser.h"

STAFF_DECLARE_PLUGIN(staff::codegen::DasDataSourceParser)

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
      StringReplace(sRootNs, ".", "::", true);
    }

    for (StringList::const_iterator itFile = rParseSettings.lsFiles.begin();
        itFile != rParseSettings.lsFiles.end(); ++itFile)
    {
      const std::string& sFileName = rParseSettings.sInDir + *itFile;

      m_sDir = sFileName.substr(0, sFileName.find_last_of('/') + 1);

      xml::Document tDocDataSources;
      xml::XmlFileReader(sFileName).ReadDocument(tDocDataSources);

      xml::Element& rNodeDataSources = tDocDataSources.GetRootElement();

      if (rNodeDataSources.GetName() == "types")
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
        const xml::Attribute* pNs = rNodeDataSources.FindAttribute("namespace");
        if (pNs)
        {
          sNamespace = pNs->GetValue();
        }

        StringReplace(sNamespace, ".", "::", true);
        sNamespace = sRootNs + sNamespace + "::";

        stInterface.sNamespace = sNamespace;

        ParseTypes(rNodeDataSources, rProject, stInterface, sNamespace);

        rProject.lsInterfaces.push_back(stInterface);
      }
      else
      {
        for (xml::Element* pNodeDataSource = rNodeDataSources.GetFirstChildElement();
            pNodeDataSource; pNodeDataSource = pNodeDataSource->GetNextSiblingElement())
        {
          if (pNodeDataSource->GetName() == "datasource")
          {
            ParseProject(*pNodeDataSource, rProject, sRootNs);
          }
        }
      }

    }

    if (!rProject.lsInterfaces.empty())
    {
      rProject.sNamespace = rProject.lsInterfaces.front().sNamespace;
    }
  }

  void DasDataSourceParser::ImportInterface(Interface& rInterface, const Interface& rImportedInterface)
  {
    Include stInclude;
    stInclude.sFileName = rImportedInterface.sFileName;
    stInclude.sFilePath = rImportedInterface.sFilePath;
    stInclude.sNamespace = rImportedInterface.sNamespace;
    stInclude.sInterfaceName = rImportedInterface.sName;
    rInterface.lsIncludes.push_back(stInclude);

    for (std::list<Struct>::const_iterator itStruct = rImportedInterface.lsStructs.begin();
      itStruct != rImportedInterface.lsStructs.end(); ++itStruct)
    {
      rInterface.lsStructs.push_back(*itStruct);
      rInterface.lsStructs.back().bExtern = true;
    }

    for (std::list<Typedef>::const_iterator itTypedef = rImportedInterface.lsTypedefs.begin();
      itTypedef != rImportedInterface.lsTypedefs.end(); ++itTypedef)
    {
      rInterface.lsTypedefs.push_back(*itTypedef);
      rInterface.lsTypedefs.back().bExtern = true;
    }

  }

  void DasDataSourceParser::ParseTypes(const xml::Element& rNodeTypes, Project& rProject,
                                        Interface& rInterface, const std::string& sNamespace)
  {
    for (const xml::Element* pType = rNodeTypes.GetFirstChildElement();
          pType; pType = pType->GetNextSiblingElement())
    {
      const std::string& sName = pType->GetName();
      if (sName == "include")
      {
        const std::string& sFileName = pType->GetAttributeValue("filename");

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
            ImportInterface(rInterface, *itInterface);
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

          LogDebug() << "including " << sFileName;
          xml::Document tTypesDoc;
          xml::XmlFileReader(m_sDir + sFileName).ReadDocument(tTypesDoc);
          const xml::Element& rNodeTypes = tTypesDoc.GetRootElement();
          ParseTypes(rNodeTypes, rProject, stInterface, sNamespace);

          ImportInterface(rInterface, stInterface);

          rProject.lsInterfaces.push_back(stInterface);
        }
      }
      else
      {
        const std::string& sType = pType->GetAttributeValue("type");
        std::string sDescr;

        ParseDescr(*pType, sDescr);

        if (sType == "struct")
        {
          const xml::Attribute* pAttrNillable = NULL;
          Struct stStruct;
          stStruct.sName = sName;
          stStruct.sNamespace = sNamespace;
          stStruct.sDescr = sDescr;
          stStruct.bForward = false;

          for (const xml::Element* pMember = pType->GetFirstChildElement();
                pMember; pMember = pMember->GetNextSiblingElement())
          {
            Param stMember;

            stMember.sName = pMember->GetName();
            ParseDescr(*pMember, stMember.sDescr);

            stMember.stDataType.sName = pMember->GetAttributeValue("type");
            FixDataType(stMember.stDataType, rInterface, sNamespace);

            pAttrNillable = pMember->FindAttribute("nillable");
            if (pAttrNillable && pAttrNillable->GetValue() == "true")
            {
              stMember.stDataType.lsParams.push_front(stMember.stDataType);
              stMember.stDataType.lsParams.resize(1);
              stMember.stDataType.sName = "Nillable";
              stMember.stDataType.sNamespace = "staff::";
              stMember.stDataType.eType = DataType::TypeTemplate;
              stMember.stDataType.sUsedName.erase();
            }
            else
            {
              stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
              OptimizeCppNs(stMember.stDataType.sUsedName, stStruct.sNamespace);
            }

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
          ParseDescr(*pType, stTypedef.sDescr);

          DataType stItemDataType;
          stItemDataType.sName = pType->GetAttributeValue("itemtype");
          FixDataType(stItemDataType, rInterface, sNamespace);
          stItemDataType.sUsedName = stItemDataType.sNamespace + stItemDataType.sName;
          OptimizeCppNs(stItemDataType.sUsedName, stTypedef.sNamespace);

          stTypedef.stDataType.lsParams.push_back(stItemDataType);

          rInterface.lsTypedefs.push_back(stTypedef);
        }
        else
        {
          Typedef stTypedef;
          stTypedef.sName = pType->GetAttributeValue("type");
          stTypedef.sNamespace = sNamespace;
          stTypedef.stDataType.sName = sName;
          FixDataType(stTypedef.stDataType, rInterface, sNamespace);
          ParseDescr(*pType, stTypedef.sDescr);
          stTypedef.stDataType.sUsedName = stTypedef.stDataType.sNamespace + stTypedef.stDataType.sName;
          OptimizeCppNs(stTypedef.stDataType.sNamespace, stTypedef.sNamespace);

          rInterface.lsTypedefs.push_back(stTypedef);
        }
      }
    }
  }


  void DasDataSourceParser::ParseProject(xml::Element& rDataSourceNode, Project& rProject,
                                         const std::string& sRootNs)
  {
    Interface stInterface;

    ProcessIncludes(rDataSourceNode);

    stInterface.sName = rDataSourceNode.GetAttributeValue("name");

    std::string sNamespace;
    const xml::Attribute* pNs = rDataSourceNode.FindAttribute("namespace");
    if (pNs)
    {
      sNamespace = pNs->GetValue();
    }

    StringReplace(sNamespace, ".", "::", true);
    sNamespace = sRootNs + sNamespace + "::";

    stInterface.sNamespace = sNamespace;

    // Types
    const xml::Element* pNodeTypes = rDataSourceNode.FindChildElementByName("types");
    if (pNodeTypes)
    {
      ParseTypes(*pNodeTypes, rProject, stInterface, sNamespace);
    }


    // class
    Class stClass;
    stClass.sName = stInterface.sName;
    stClass.sNamespace = sNamespace;

    ParseDescr(rDataSourceNode, stClass.sDescr);

    // Options
    xml::Element* pOptions = rDataSourceNode.FindChildElementByName("options");
    if (pOptions)
    {
      for (xml::Element* pOption = pOptions->GetFirstChildElement();
           pOption; pOption = pOption->GetNextSiblingElement())
      {
        stInterface.mOptions[pOption->GetName()] = pOption->GetTextValue();
      }
    }

    // Operations
    xml::Element& rOperations = rDataSourceNode.GetChildElementByName("operations");
    ProcessIncludes(rOperations);
    for (xml::Element* pOperation = rOperations.GetFirstChildElement();
          pOperation; pOperation = pOperation->GetNextSiblingElement())
    {
      if (pOperation->GetName() == "operation")
      {
        Member stMember;

        stMember.sName = pOperation->GetAttributeValue("name");

        ParseDescr(*pOperation, stMember.sDescr);

        // Options
        xml::Element* pOptions = pOperation->FindChildElementByName("options");
        if (pOptions)
        {
          for (xml::Element* pOption = pOptions->GetFirstChildElement();
               pOption; pOption = pOption->GetNextSiblingElement())
          {
            stMember.mOptions[pOption->GetName()] = pOption->GetTextValue();
          }
        }

        const xml::Attribute* pAttrNillable = NULL;

        xml::Element* pReturn = pOperation->FindChildElementByName("return");
        if (pReturn)
        {
          stMember.stReturn.stDataType.sName = pReturn->GetAttributeValue("type");
          FixDataType(stMember.stReturn.stDataType, stInterface, sNamespace);
          pAttrNillable = pOperation->FindAttribute("nillable");
          if (pAttrNillable && pAttrNillable->GetValue() == "true")
          {
            stMember.stReturn.stDataType.lsParams.push_front(stMember.stReturn.stDataType);
            stMember.stReturn.stDataType.lsParams.resize(1);
            stMember.stReturn.stDataType.sName = "Nillable";
            stMember.stReturn.stDataType.sNamespace = "staff::";
            stMember.stReturn.stDataType.eType = DataType::TypeTemplate;
            stMember.stReturn.stDataType.sUsedName.erase();
          }
          else
          {
            stMember.stReturn.stDataType.sUsedName = stMember.stReturn.stDataType.sNamespace
                                                     + stMember.stReturn.stDataType.sName;
            OptimizeCppNs(stMember.stReturn.stDataType.sUsedName, stClass.sNamespace);
          }
        }
        else
        {
          stMember.stReturn.stDataType.sName = "void";
        }

        const xml::Element& rParams = pOperation->GetChildElementByName("params");
        for (const xml::Element* pParam = rParams.GetFirstChildElement();
              pParam; pParam = pParam->GetNextSiblingElement())
        {
          Param stParam;
          stParam.sName = pParam->GetName();

          stParam.stDataType.sName = pParam->GetAttributeValue("type");

          FixDataType(stParam.stDataType, stInterface, sNamespace);

          pAttrNillable = pParam->FindAttribute("nillable");
          if (pAttrNillable && pAttrNillable->GetValue() == "true")
          {
            stParam.stDataType.lsParams.push_front(stParam.stDataType);
            stParam.stDataType.lsParams.resize(1);
            stParam.stDataType.sName = "Nillable";
            stParam.stDataType.sNamespace = "staff::";
            stParam.stDataType.eType = DataType::TypeTemplate;
            stParam.stDataType.sUsedName.erase();
          }
          else
          {
            stParam.stDataType.sUsedName = stParam.stDataType.sNamespace
                                                     + stParam.stDataType.sName;
            OptimizeCppNs(stParam.stDataType.sUsedName, sNamespace);
          }

          if (stParam.stDataType.eType == DataType::TypeString ||
              stParam.stDataType.eType == DataType::TypeStruct ||
              stParam.stDataType.eType == DataType::TypeTypedef||
              stParam.stDataType.eType == DataType::TypeTemplate)
          {
            stParam.stDataType.bIsConst = true;
            stParam.stDataType.bIsRef = true;
          }

          stMember.lsParams.push_back(stParam);
        }

        stClass.lsMembers.push_back(stMember);
      }
    }

    stInterface.lsClasses.push_back(stClass);

    rProject.lsInterfaces.push_back(stInterface);
  }

  void DasDataSourceParser::ParseDescr(const xml::Element& rNode, std::string& sDescr)
  {
    const xml::Attribute* pDescr = rNode.FindAttribute("descr");
    if (pDescr)
    {
      sDescr = pDescr->GetValue();
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

  void DasDataSourceParser::ProcessIncludes(xml::Element& rNode)
  {
    xml::Element* pChildToRemove = NULL;
    for (xml::Element* pInclude = rNode.FindChildElementByName("include");
      pInclude; pInclude = rNode.FindChildElementByName("include", pInclude))
    {
      const std::string& sFileName = pInclude->GetAttributeValue("filename");

      xml::Document tDoc;

      LogDebug() << "Including file: " << (m_sDir + sFileName);
      xml::XmlFileReader(m_sDir + sFileName).ReadDocument(tDoc);

      xml::Element& rNodeTypes = tDoc.GetRootElement();
      xml::Element* pNextChild = NULL;
      for (xml::Element* pChild = rNodeTypes.GetFirstChildElement();
            pChild; pChild = pNextChild)
      {
        pNextChild = pChild->GetNextSiblingElement();
        rNode.AppendChild(pChild->Detach());
      }

      pChildToRemove = pInclude;
      pInclude = pInclude->GetNextSiblingElement();
      rNode.RemoveChild(pChildToRemove);
    }

  }

  const std::string DasDataSourceParser::m_sId = "dasdatasource";
}
}
