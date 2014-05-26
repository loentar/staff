/*
 *  Copyright 2010 Utkin Dmitry
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

#include <staff/utils/Log.h>
#include <staff/utils/File.h>
#include <staff/common/Exception.h>
#include <staff/common/Runtime.h>
#include <staff/xml/Attribute.h>
#include <staff/xml/Document.h>
#include <staff/xml/XmlReader.h>
#include "DataSource.h"

namespace staff
{
namespace das
{
  DataType::DataType():
    eType(Void), bExtern(false), bNillable(false)
  {
  }


  DataSource::DataSource()
  {
  }

  DataSource::~DataSource()
  {
  }

  void DataSource::Load(xml::Element& rDataSourceNode, const std::string& sFileName)
  {
    LogDebug() << "Loading datasource file: " << sFileName;

    m_sFileName = sFileName;

    std::string::size_type nPos = sFileName.find_last_of(STAFF_PATH_SEPARATOR);
    if (nPos != std::string::npos)
    {
      m_sDataSourcesDir = sFileName.substr(0, nPos + 1);
    }

    ProcessIncludes(rDataSourceNode);

    m_lsProviders.clear();
    std::string sId;
    const xml::Attribute* pAttr = NULL;
    xml::Element* pProvider = rDataSourceNode.GetFirstChildElement();
    while ((pProvider = rDataSourceNode.FindChildElementByName("provider", pProvider)) != NULL)
    {
      pAttr = pProvider->FindAttribute("id");

      if (pAttr)
      {
        sId = pAttr->GetValue();
      }
      else
      {
        sId.clear();
      }

      if (m_lsProviders.empty())
      {
        m_sDefaultProviderId = sId;
      }
      else
      {
        STAFF_ASSERT(!sId.empty(), "When using multiple providers you must set unique id for each");
      }

      ProcessIncludes(*pProvider);

      for (ProvidersInfoList::const_iterator itProv = m_lsProviders.begin();
           itProv != m_lsProviders.end(); ++itProv)
      {
        STAFF_ASSERT(itProv->sId != sId, "Duplicated provider id: " + sId);
      }

      m_lsProviders.push_back(ProviderInfo());
      ProviderInfo& rInfo = m_lsProviders.back();

      rInfo.sName = pProvider->GetAttributeValue("name");
      rInfo.sId = sId;
      rInfo.tConfig = *pProvider;
      pProvider = pProvider->GetNextSiblingElement();
    }

    m_sNamespace.erase();
    m_lsIncludes.clear();
    const xml::Attribute* pNs = rDataSourceNode.FindAttribute("namespace");
    if (pNs)
    {
      m_sNamespace = pNs->GetValue();
    }

    m_sName = rDataSourceNode.GetAttributeValue("name");

    ParseDescr(rDataSourceNode, m_sDescr);

    // options
    const xml::Element* pOptions = rDataSourceNode.FindChildElementByName("options");
    if (pOptions)
    {
      for (const xml::Element* pOption = pOptions->GetFirstChildElement();
           pOption; pOption = pOption->GetNextSiblingElement())
      {
        m_mOptions[pOption->GetName()] = pOption->GetTextValue();
      }
    }

    // Types
    m_lsTypes.clear();

    const xml::Element* pTypes = rDataSourceNode.FindChildElementByName("types");
    if (pTypes)
    {
      for (const xml::Element* pType = pTypes->GetFirstChildElement();
            pType; pType = pType->GetNextSiblingElement())
      {
        if (pType->GetName() == "include")
        { // include
          IncludeTypes(pType->GetAttributeValue("filename"));
        }
        else
        { // type declaration
          DataType stType;
          ParseType(*pType, stType);
          m_lsTypes.push_back(stType);
        }
      }
    }

    // Operations
    m_lsOperations.clear();
    xml::Element& rOperations = rDataSourceNode.GetChildElementByName("operations");
    ProcessIncludes(rOperations);
    for (const xml::Element* pOperation = rOperations.GetFirstChildElement();
         pOperation; pOperation = pOperation->GetNextSiblingElement())
    {
      if (pOperation->GetName() == "operation")
      {
        Operation stOperation;

        stOperation.sName = pOperation->GetAttributeValue("name");

        // options
        const xml::Element* pOptions = pOperation->FindChildElementByName("options");
        if (pOptions)
        {
          for (const xml::Element* pOption = pOptions->GetFirstChildElement();
               pOption; pOption = pOption->GetNextSiblingElement())
          {
            stOperation.mOptions[pOption->GetName()] = pOption->GetTextValue();
          }
        }

        const xml::Element* pScript = pOperation->FindChildElementByName("script");
        if (pScript)
        {
          stOperation.tScript = *pScript;
        }
        else
        {
          pScript = pOperation->FindChildElementByName("execute");
          STAFF_ASSERT(pScript, "cannot find <script> or <execute> element");
          stOperation.tScript.SetName("script");
          stOperation.tScript.AppendChild(pScript->CloneElement());
        }

        ParseDescr(*pOperation, stOperation.sDescr);

        const xml::Element* pReturn = pOperation->FindChildElementByName("return");
        if (pReturn)
        {
          ParseType(*pReturn, stOperation.stReturn);
          if (stOperation.stReturn.eType == DataType::List ||
              stOperation.stReturn.eType == DataType::Struct)
          {
            stOperation.stReturn = GetType(stOperation.stReturn.sType);
          }
        }

        const xml::Element* pParams = pOperation->FindChildElementByName("params");
        if (pParams)
        {
          for (const xml::Element* pType = pParams->GetFirstChildElement();
                pType; pType = pType->GetNextSiblingElement())
          {
            DataType stType;
            ParseType(*pType, stType);
            stOperation.lsParams.push_back(stType);
          }
        }

        m_lsOperations.push_back(stOperation);
      }
    }
  }

  const IncludesList& DataSource::GetIncludes() const
  {
    return m_lsIncludes;
  }

  void DataSource::ParseType(const xml::Element& rNode, DataType& rType)
  {
    rType.sName = rNode.GetName();

    const std::string& sType = rNode.GetAttributeValue("type");

    ParseDescr(rNode, rType.sDescr);

    if (sType == "struct")
    {
      rType.eType = DataType::Struct;
      rType.sType = sType;

      for (const xml::Element* pType = rNode.GetFirstChildElement();
            pType; pType = pType->GetNextSiblingElement())
      {
        DataType stType;
        ParseType(*pType, stType);
        rType.lsChilds.push_back(stType);
      }
    }
    else
    if (sType == "list")
    {
      rType.eType = DataType::List;
      rType.sType = rNode.GetAttributeValue("itemtype");

      STAFF_ASSERT(rType.sType != "void", "list item cannot be \"void\"");

      const DataType* pItemType = FindType(rType.sType);

      if (pItemType != NULL)
      { // ref
        STAFF_ASSERT(pItemType->eType != DataType::List, "list item cannot be \"list\"");
        rType.lsChilds.push_back(*pItemType);
      }
      else
      { // simple
        DataType tItemType;
        tItemType.sName = "Item";
        tItemType.sType = rType.sType;
        if (rType.sType == "DataObject" || rType.sType == "staff::DataObject")
        {
          tItemType.eType = DataType::DataObject;
        }
        else
        {
          tItemType.eType = DataType::Generic;
        }
        rType.lsChilds.push_back(tItemType);
      }
    }
    else
    if (sType == "void")
    {
      rType.eType = DataType::Void;
      rType.sType = sType;
    }
    else
    if (sType == "DataObject" || sType == "staff::DataObject")
    {
      rType.eType = DataType::DataObject;
      rType.sType = sType;
    }
    else
    {
      const DataType* pType = FindType(sType);

      if (pType != NULL)
      { // ref
        rType.eType = pType->eType;
      }
      else
      { // simple
        rType.eType = DataType::Generic;
      }
      rType.sType = sType;
      const xml::Attribute* pAttrNillable = rNode.FindAttribute("nillable");
      rType.bNillable = pAttrNillable && pAttrNillable->GetValue() == "true";
    }
  }

  void DataSource::IncludeTypes(const std::string& sFileName)
  {
    xml::Document tTypesDoc;

    LogDebug() << "Including file: " << (m_sDataSourcesDir + sFileName);
    xml::XmlFileReader(m_sDataSourcesDir + sFileName).ReadDocument(tTypesDoc);

    m_lsIncludes.push_back(Include());
    Include& rstInclude = m_lsIncludes.back();
    rstInclude.sFileName = sFileName;

    const xml::Element& rNodeTypes = tTypesDoc.GetRootElement();
    for (const xml::Element* pType = rNodeTypes.GetFirstChildElement();
         pType; pType = pType->GetNextSiblingElement())
    {
      DataType stType;
      ParseType(*pType, stType);

      rstInclude.lsTypes.push_back(stType);
      stType.bExtern = true;
      m_lsTypes.push_back(stType);
    }
  }

  void DataSource::ParseDescr(const xml::Element& rNode, std::string& sDescr)
  {
    const xml::Attribute* pDescr = rNode.FindAttribute("descr");
    if (pDescr)
    {
      sDescr = pDescr->GetValue();
    }
  }

  const std::string& DataSource::GetName() const
  {
    return m_sName;
  }

  const std::string& DataSource::GetDescr() const
  {
    return m_sDescr;
  }

  const std::string& DataSource::GetNamespace() const
  {
    return m_sNamespace;
  }

  const std::string& DataSource::GetDefaultProvideId() const
  {
    return m_sDefaultProviderId;
  }

  const ProvidersInfoList& DataSource::GetProviders() const
  {
    return m_lsProviders;
  }

  const StringMap& DataSource::GetOptions() const
  {
    return m_mOptions;
  }

  const DataType& DataSource::GetType(const std::string& sTypeName) const
  {
    const DataType* pType = FindType(sTypeName);
    STAFF_ASSERT(pType, "Type " + sTypeName + " is not found");
    return *pType;
  }

  const DataType* DataSource::FindType(const std::string& sTypeName) const
  {
    DataTypesList::const_iterator pType = m_lsTypes.begin();
    for (; pType != m_lsTypes.end(); ++pType)
    {
      if (pType->sName == sTypeName)
      {
        return &*pType;
      }
    }

    return NULL;
  }

  const Operation& DataSource::GetOperation(const std::string& sOperationName) const
  {
    OperationsList::const_iterator pOperation = m_lsOperations.begin();
    for (; pOperation != m_lsOperations.end(); ++pOperation)
    {
      if (pOperation->sName == sOperationName)
      {
        return *pOperation;
      }
    }

    STAFF_THROW_ASSERT("Operation " + sOperationName + " is not found");
  }

  const DataTypesList& DataSource::GetTypes() const
  {
    return m_lsTypes;
  }

  const OperationsList& DataSource::GetOperations() const
  {
    return m_lsOperations;
  }

  void DataSource::ProcessIncludes(xml::Element& rNode)
  {
    xml::Element* pChildToRemove = NULL;
    for (xml::Element* pInclude = rNode.FindChildElementByName("include");
      pInclude; pInclude = rNode.FindChildElementByName("include", pInclude))
    {
      const std::string& sFileName = pInclude->GetAttributeValue("filename");

      xml::Document tDoc;

      LogDebug() << "Including file: " << (m_sDataSourcesDir + sFileName);
      xml::XmlFileReader(m_sDataSourcesDir + sFileName).ReadDocument(tDoc);

      xml::Element& rNodeTypes = tDoc.GetRootElement();
      xml::Element* pNextChild = NULL;
      for (xml::Element* pChild = rNodeTypes.GetFirstChildElement();
            pChild; pChild = pNextChild)
      {
        pNextChild = pChild->GetNextSiblingElement();
        rNode.InsertChildBefore(pChild->Detach(), pInclude);
      }

      pChildToRemove = pInclude;
      pInclude = pInclude->GetNextSiblingElement();
      rNode.RemoveChild(pChildToRemove);
    }

  }

}
}
