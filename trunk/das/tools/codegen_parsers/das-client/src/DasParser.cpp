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
#include <staff/codegen/tools.h>
#include <staff/common/DataObject.h>
#include <staff/common/Exception.h>
#include "DataAccessService.h"
#include <staff/client/ServiceFactory.h>
#include "DasParser.h"

RISE_DECLARE_PLUGIN(staff::codegen::DasParser)

namespace staff
{
namespace codegen
{

  DasParser::DasParser()
  {
  }

  DasParser::~DasParser()
  {
  }


  const std::string& DasParser::GetId()
  {
    return m_sId;
  }

  void DasParser::Process(const ParseSettings& rParseSettings, Project& rProject)
  {
    try
    {
      std::string sRootNs = "::";
      StringMap::const_iterator itRootNs = rParseSettings.mEnv.find("rootns");
      if (itRootNs != rParseSettings.mEnv.end() && !itRootNs->second.empty())
      {
        sRootNs = "::" + itRootNs->second + "::";
        rise::StrReplace(sRootNs, ".", "::", true);
      }

      StringMap::const_iterator itServiceUri = rParseSettings.mEnv.find("serviceuri");

      std::string sServiceUri;
      if (itServiceUri != rParseSettings.mEnv.end())
      {
        sServiceUri = itServiceUri->second;
      }

      for (StringList::const_iterator itDataSourceName = rParseSettings.lsFiles.begin();
            itDataSourceName != rParseSettings.lsFiles.end(); ++itDataSourceName)
      {
        std::string sDataSource = *itDataSourceName;
        Interface stInterface;

        std::cout << "Processing DAS: " << *itDataSourceName << std::endl;

        std::auto_ptr<das::DataAccessService> pDataAccessService
            (ServiceFactory::Inst().GetService<das::DataAccessService>(sServiceUri));

        pDataAccessService->SetDataSource(sDataSource);
        const DataObject& rdoInterface = pDataAccessService->GetInterface();
        Parse(rdoInterface, stInterface, rProject, sRootNs);
        rProject.sNamespace = stInterface.sNamespace;

        pDataAccessService->FreeDataSource();

        rProject.lsInterfaces.push_back(stInterface);
      }
    }
    catch (const staff::RemoteException& rEx)
    {
      RISE_THROWS(rise::CInternalAssertException, rEx.GetString());
    }
  }

  void DasParser::ParseTypes(const DataObject& rdoTypes, const std::string& sNamespace, Interface& rInterface)
  {
    for (DataObject::ConstIterator itType = rdoTypes.Begin();
        itType != rdoTypes.End(); ++itType)
    {
      const DataObject& rdoType = *itType;
      const std::string& sType = rdoType.GetChildByLocalName("Type").GetText();
      const std::string& sName = rdoType.GetChildByLocalName("Name").GetText();
      bool bExtern = rdoType.GetChildByLocalName("Extern").GetText() == "True";

      if (sType == "struct")
      {
        Struct stStruct;
        stStruct.sName = sName;
        stStruct.sNamespace = sNamespace;
        stStruct.bExtern = bExtern;

        const DataObject& rdoMembers = rdoType.GetChildByLocalName("Members");

        for (DataObject::ConstIterator itChildType = rdoMembers.Begin();
            itChildType != rdoMembers.End(); ++itChildType)
        {
          const DataObject& rdoChildType = *itChildType;
          Param stMember;
          stMember.sName = rdoChildType.GetChildByLocalName("Name").GetText();
          stMember.stDataType.sName = rdoChildType.GetChildByLocalName("DataType").GetText();
          stMember.sDescr = rdoChildType.GetChildByLocalName("Descr").GetText();

//          const std::string& sType = rdoChildType.GetChildByLocalName("Type").GetText();
//          if (sType == "struct")
//          {
//            stMember.stDataType.eType = DataType::TypeStruct;
//          }
//          else
//          if (sType == "list")
//          {
//            stMember.stDataType.eType = DataType::TypeTypedef;
//          }

          FixDataType(stMember.stDataType, rInterface, sNamespace);
          stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
          OptimizeCppNs(stMember.stDataType.sUsedName, stStruct.sNamespace);

          stStruct.lsMembers.push_back(stMember);
        }

        stStruct.sDescr = rdoType.GetChildByLocalName("Descr").GetText();
        stStruct.bForward = false;

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
        stTypedef.sDescr = rdoType.GetChildByLocalName("Descr").GetText();

        DataType stItemDataType;
        stItemDataType.sName = rdoType.GetChildByLocalName("ItemType").GetText();
        FixDataType(stItemDataType, rInterface, sNamespace);
        stItemDataType.sUsedName = stItemDataType.sNamespace + stItemDataType.sName;
        OptimizeCppNs(stItemDataType.sUsedName, stTypedef.sNamespace);

        stTypedef.bExtern = bExtern;

        stTypedef.stDataType.lsParams.push_back(stItemDataType);

        rInterface.lsTypedefs.push_back(stTypedef);
      }
      else
      if (sType == "generic" || sType == "dataobject") // typedef
      {
        Typedef stTypedef;
        stTypedef.sName = rdoType.GetChildByLocalName("Type").GetText();
        stTypedef.sNamespace = sNamespace;
        stTypedef.stDataType.sName = sName;
        FixDataType(stTypedef.stDataType, rInterface, sNamespace);

        stTypedef.sDescr = rdoType.GetChildByLocalName("Descr").GetText();
        stTypedef.bExtern = bExtern;

        rInterface.lsTypedefs.push_back(stTypedef);
      }
    }
  }

  void DasParser::Parse(const DataObject& rdoInterface, Interface& rInterface, Project& rProject,
                         const std::string& sRootNs)
  {
    rInterface.sName = rdoInterface.GetChildByLocalName("Name").GetText();

    std::string sNamespace = rdoInterface.GetChildByLocalName("Namespace").GetText();
    rise::StrReplace(sNamespace, ".", "::", true);
    sNamespace = sRootNs + sNamespace + "::";

    rInterface.sNamespace = sNamespace;

    const DataObject& rdoIncludes = rdoInterface.GetChildByLocalName("Includes");
    for (DataObject::ConstIterator itInclude = rdoIncludes.Begin();
        itInclude != rdoIncludes.End(); ++itInclude)
    {
      const DataObject& rdoInclude = *itInclude;
      const std::string& sName = rdoInclude.GetChildByLocalName("Name").GetText();
      bool bExists = false;

      for (std::list<Interface>::const_iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        if (itInterface->sName == sName)
        {
          bExists = true;
        }
      }

      Include stInclude;
      stInclude.sInterfaceName = sName;
      stInclude.sFileName = sName + ".h";
      stInclude.sNamespace = rdoInclude.GetChildByLocalName("Namespace").GetText();
      rise::StrReplace(stInclude.sNamespace, ".", "::", true);
      stInclude.sNamespace = sRootNs + stInclude.sNamespace + "::";
      rInterface.lsIncludes.push_back(stInclude);

      if (!bExists)
      {
        Interface stInterface;
        stInterface.sName = sName;
        stInterface.sFileName = stInclude.sFileName;
        stInterface.sNamespace = stInclude.sNamespace;
        ParseTypes(rdoInclude.GetChildByLocalName("Types"), stInclude.sNamespace, stInterface);
        rProject.lsInterfaces.push_back(stInterface);
      }
    }

    const DataObject& rdoTypes = rdoInterface.GetChildByLocalName("Types");

    ParseTypes(rdoTypes, sNamespace, rInterface);


    Class stClass;
    stClass.sName = rInterface.sName;
    stClass.sNamespace = sNamespace;
    stClass.sDescr = rdoInterface.GetChildByLocalName("Descr").GetText();

    const DataObject& rdoOperations = rdoInterface.GetChildByLocalName("Operations");

    for (DataObject::ConstIterator itOperation = rdoOperations.Begin();
        itOperation != rdoOperations.End(); ++itOperation)
    {
      const DataObject& rdoOperation = *itOperation;

      Member stMember;

      stMember.sName = rdoOperation.GetChildByLocalName("Name").GetText();
      stMember.sDescr = rdoOperation.GetChildByLocalName("Descr").GetText();

      stMember.stReturn.stDataType.sName = rdoOperation.GetChildByLocalName("Return").GetText();
      FixDataType(stMember.stReturn.stDataType, rInterface, sNamespace);

      const DataObject& rdoParams = rdoOperation.GetChildByLocalName("Params");
      for (DataObject::ConstIterator itParam = rdoParams.Begin();
          itParam != rdoParams.End(); ++itParam)
      {
        const DataObject& rdoParam = *itParam;
        Param stParam;

        stParam.sName = rdoParam.GetChildByLocalName("Name").GetText();
        stParam.stDataType.sName = rdoParam.GetChildByLocalName("Type").GetText();

        FixDataType(stParam.stDataType, rInterface, sNamespace);
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;
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

    rInterface.lsClasses.push_back(stClass);
  }

  bool DasParser::FixDataType(DataType& rDataType, const Interface& rInterface, const std::string& sNamespace)
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


  const std::string DasParser::m_sId = "das";
}
}
