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
#include <staff/common/DataObject.h>
#include "DataAccessService.h"
#include <staff/client/ServiceFactory.h>
#include "DasParser.h"

RISE_DECLARE_PLUGIN(staff::CDasParser);

namespace staff
{

  CDasParser::CDasParser()
  {
  }

  CDasParser::~CDasParser()
  {
  }


  const std::string& CDasParser::GetId()
  {
    return m_sId;
  }

  void CDasParser::Process(const SParseSettings& rParseSettings, SProject& rProject)
  {
    TStringMap::const_iterator itServiceUri = rParseSettings.mEnv.find("serviceuri");

    std::string sServiceUri;
    if (itServiceUri != rParseSettings.mEnv.end())
    {
      sServiceUri = itServiceUri->second;
    }

    for (TStringList::const_iterator itDataSourceName = rParseSettings.lsFiles.begin();
          itDataSourceName != rParseSettings.lsFiles.end(); ++itDataSourceName)
    {
      std::string sDataSource = *itDataSourceName;
      SInterface stInterface;

      std::cout << "Processing DAS: " << *itDataSourceName << std::endl;

      std::auto_ptr<das::DataAccessService> pDataAccessService
          (CServiceFactory::Inst().GetService<das::DataAccessService>(sServiceUri));

      pDataAccessService->SetDataSource(sDataSource);
      const CDataObject& rdoInterface = pDataAccessService->GetInterface().FirstChild();
      Parse(rdoInterface, stInterface, rProject);
      rProject.sNamespace = stInterface.sNamespace;

      pDataAccessService->FreeDataSource();

      rProject.lsInterfaces.push_back(stInterface);
    }
  }

  void CDasParser::ParseTypes(const CDataObject& rdoTypes, const std::string& sNamespace, SInterface& rInterface)
  {
    for (CDataObject::ConstIterator itType = rdoTypes.Begin();
        itType != rdoTypes.End(); ++itType)
    {
      const CDataObject& rdoType = *itType;
      const std::string& sType = rdoType.GetChildByLocalName("Type").GetText();
      const std::string& sName = rdoType.GetChildByLocalName("Name").GetText();
      bool bExtern = rdoType.GetChildByLocalName("Extern").GetText() == "True";

      if (sType == "struct")
      {
        SStruct stStruct;
        stStruct.sName = sName;
        stStruct.sNamespace = sNamespace;
        stStruct.bExtern = bExtern;

        const CDataObject& rdoMembers = rdoType.GetChildByLocalName("Members");

        for (CDataObject::ConstIterator itChildType = rdoMembers.Begin();
            itChildType != rdoMembers.End(); ++itChildType)
        {
          const CDataObject& rdoChildType = *itChildType;
          SParam stMember;
          stMember.sName = rdoChildType.GetChildByLocalName("Name").GetText();
          stMember.stDataType.sName = rdoChildType.GetChildByLocalName("DataType").GetText();
          stMember.sDescr = rdoChildType.GetChildByLocalName("Descr").GetText();

//          const std::string& sType = rdoChildType.GetChildByLocalName("Type").GetText();
//          if (sType == "struct")
//          {
//            stMember.stDataType.eType = SDataType::EStruct;
//          }
//          else
//          if (sType == "list")
//          {
//            stMember.stDataType.eType = SDataType::ETypedef;
//          }

          FixDataType(stMember.stDataType, rInterface, sNamespace);
          stMember.stDataType.sUsedName = stMember.stDataType.sNamespace + stMember.stDataType.sName;
          OptimizeCppNs(stMember.stDataType.sUsedName, stStruct.sNamespace);

          stStruct.lsMember.push_back(stMember);
        }

        stStruct.sDescr = rdoType.GetChildByLocalName("Descr").GetText();
        stStruct.bForward = false;

        rInterface.lsStruct.push_back(stStruct);
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
        stTypedef.sDescr = rdoType.GetChildByLocalName("Descr").GetText();

        SDataType stItemDataType;
        stItemDataType.sName = rdoType.GetChildByLocalName("ItemType").GetText();
        FixDataType(stItemDataType, rInterface, sNamespace);
        stItemDataType.sUsedName = stItemDataType.sNamespace + stItemDataType.sName;
        OptimizeCppNs(stItemDataType.sUsedName, stTypedef.sNamespace);

        stTypedef.bExtern = bExtern;

        stTypedef.stDataType.lsParams.push_back(stItemDataType);

        rInterface.lsTypedef.push_back(stTypedef);
      }
      else
      if (sType == "generic" || sType == "dataobject") // typedef
      {
        STypedef stTypedef;
        stTypedef.sName = rdoType.GetChildByLocalName("Type").GetText();
        stTypedef.sNamespace = sNamespace;
        stTypedef.stDataType.sName = sName;
        FixDataType(stTypedef.stDataType, rInterface, sNamespace);

        stTypedef.sDescr = rdoType.GetChildByLocalName("Descr").GetText();
        stTypedef.bExtern = bExtern;

        rInterface.lsTypedef.push_back(stTypedef);
      }
    }
  }

  void CDasParser::Parse(const CDataObject& rdoInterface, SInterface& rInterface, SProject& rProject)
  {
    rInterface.sName = rdoInterface.GetChildByLocalName("Name").GetText();

    std::string sNamespace = rdoInterface.GetChildByLocalName("Namespace").GetText();
    rise::StrReplace(sNamespace, ".", "::", true);
    sNamespace = "::" + sNamespace + "::";

    rInterface.sNamespace = sNamespace;

    const CDataObject& rdoIncludes = rdoInterface.GetChildByLocalName("Includes");
    for (CDataObject::ConstIterator itInclude = rdoIncludes.Begin();
        itInclude != rdoIncludes.End(); ++itInclude)
    {
      const CDataObject& rdoInclude = *itInclude;
      const std::string& sName = rdoInclude.GetChildByLocalName("Name").GetText();
      bool bExists = false;

      for (std::list<SInterface>::const_iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        if (itInterface->sName == sName)
        {
          bExists = true;
        }
      }

      SInclude stInclude;
      stInclude.sInterfaceName = sName;
      stInclude.sFileName = sName + ".h";
      stInclude.sNamespace = rdoInclude.GetChildByLocalName("Namespace").GetText();
      rise::StrReplace(stInclude.sNamespace, ".", "::", true);
      stInclude.sNamespace = "::" + stInclude.sNamespace + "::";
      rInterface.lsInclude.push_back(stInclude);

      if (!bExists)
      {
        SInterface stInterface;
        stInterface.sName = sName;
        stInterface.sFileName = stInclude.sFileName;
        stInterface.sNamespace = stInclude.sNamespace;
        ParseTypes(rdoInclude.GetChildByLocalName("Types"), stInclude.sNamespace, stInterface);
        rProject.lsInterfaces.push_back(stInterface);
      }
    }

    const CDataObject& rdoTypes = rdoInterface.GetChildByLocalName("Types");

    ParseTypes(rdoTypes, sNamespace, rInterface);


    SClass stClass;
    stClass.sName = rInterface.sName;
    stClass.sNamespace = sNamespace;
    stClass.sDescr = rdoInterface.GetChildByLocalName("Descr").GetText();

    const CDataObject& rdoOperations = rdoInterface.GetChildByLocalName("Operations");

    for (CDataObject::ConstIterator itOperation = rdoOperations.Begin();
        itOperation != rdoOperations.End(); ++itOperation)
    {
      const CDataObject& rdoOperation = *itOperation;

      SMember stMember;

      stMember.sName = rdoOperation.GetChildByLocalName("Name").GetText();
      stMember.sDescr = rdoOperation.GetChildByLocalName("Descr").GetText();

      stMember.stReturn.stDataType.sName = rdoOperation.GetChildByLocalName("Return").GetText();
      FixDataType(stMember.stReturn.stDataType, rInterface, sNamespace);

      const CDataObject& rdoParams = rdoOperation.GetChildByLocalName("Params");
      for (CDataObject::ConstIterator itParam = rdoParams.Begin();
          itParam != rdoParams.End(); ++itParam)
      {
        const CDataObject& rdoParam = *itParam;
        SParam stParam;

        stParam.sName = rdoParam.GetChildByLocalName("Name").GetText();
        stParam.stDataType.sName = rdoParam.GetChildByLocalName("Type").GetText();

        FixDataType(stParam.stDataType, rInterface, sNamespace);
        stParam.stDataType.sUsedName = stParam.stDataType.sNamespace + stParam.stDataType.sName;
        OptimizeCppNs(stParam.stDataType.sUsedName, sNamespace);
        stParam.stDataType.sNodeName = stParam.sName;

        if (stParam.stDataType.eType == SDataType::EString ||
            stParam.stDataType.eType == SDataType::EStruct ||
            stParam.stDataType.eType == SDataType::ETypedef)
        {
          stParam.stDataType.bIsConst = true;
          stParam.stDataType.bIsRef = true;
        }

        stMember.lsParamList.push_back(stParam);
      }

      stClass.lsMember.push_back(stMember);
    }

    rInterface.lsClass.push_back(stClass);
  }

  bool CDasParser::FixDataType(SDataType& rDataType, const SInterface& rInterface, const std::string& sNamespace)
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

  void CDasParser::OptimizeCppNs(std::string& sOptimizeNs, const std::string& sCurrentNs)
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


  const std::string CDasParser::m_sId = "das";
}
