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

#include <rise/common/ExceptionTemplate.h>
#include <rise/xml/XMLDocument.h>
#include <staff/common/Runtime.h>
#include "DataSource.h"

namespace staff
{
namespace das
{
  DataType::DataType():
    eType(Void), bExtern(false)
  {
  }


  DataSource::DataSource()
  {
  }

  DataSource::~DataSource()
  {
  }

  void DataSource::Load(const rise::xml::CXMLNode& rDataSourceNode, const std::string& sFileName)
  {
    rise::LogDebug() << "Loading datasource file: " << sFileName;

    m_lsProviders.clear();
    std::string sId;
    rise::xml::CXMLNode::TXMLAttrConstIterator itAttr;
    rise::xml::CXMLNode::TXMLNodeConstIterator itProvider = rDataSourceNode.NodeBegin();
    while ((itProvider = rDataSourceNode.FindSubnode("provider", itProvider)) != rDataSourceNode.NodeEnd())
    {
      itAttr = itProvider->FindAttribute("id");

      if (itAttr != rDataSourceNode.AttrEnd())
      {
        sId = itAttr->sAttrValue.AsString();
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
        RISE_ASSERTS(!sId.empty(), "When using multiple providers you should set unique id for each");
      }

      for (ProvidersInfoList::const_iterator itProv = m_lsProviders.begin();
           itProv != m_lsProviders.end(); ++itProv)
      {
        RISE_ASSERTS(itProv->sId != sId, "Duplicated provider id: " + sId);
      }

      m_lsProviders.push_back(ProviderInfo());
      ProviderInfo& rInfo = m_lsProviders.back();

      rInfo.sName = itProvider->Attribute("name").AsString();
      rInfo.sId = sId;
      rInfo.tConfig = *itProvider;
      ++itProvider;
    }

    m_sFileName = sFileName;
    m_sNamespace.erase();
    m_lsIncludes.clear();
    rise::xml::CXMLNode::TXMLAttrConstIterator itNs = rDataSourceNode.FindAttribute("namespace");
    if (itNs != rDataSourceNode.AttrEnd())
    {
      m_sNamespace = itNs->sAttrValue.AsString();
    }

    m_sName = rDataSourceNode.Attribute("name").AsString();

    ParseDescr(rDataSourceNode, m_sDescr);

    // Types
    m_lsTypes.clear();

    rise::xml::CXMLNode::TXMLNodeConstIterator itTypes = rDataSourceNode.FindSubnode("types");
    if (itTypes != rDataSourceNode.NodeEnd())
    {
      const rise::xml::CXMLNode& rTypes = *itTypes;
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rTypes.NodeBegin();
            itType != rTypes.NodeEnd(); ++itType)
      {
        const rise::xml::CXMLNode& rNodeType = *itType;
        if (rNodeType.NodeName() == "include")
        { // include
          IncludeTypes(rNodeType.Attribute("filename").AsString());
        }
        else
        { // type declaration
          DataType stType;
          ParseType(rNodeType, stType);
          m_lsTypes.push_back(stType);
        }
      }
    }

    // Operations
    m_lsOperations.clear();
    const rise::xml::CXMLNode& rOperations = rDataSourceNode.Subnode("operations");
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itOperation = rOperations.NodeBegin();
          itOperation != rOperations.NodeEnd(); ++itOperation)
    {
      const rise::xml::CXMLNode& rOperation = *itOperation;
      Operation stOperation;

      stOperation.sName = rOperation.Attribute("name").AsString();

      rise::xml::CXMLNode::TXMLNodeConstIterator itScript = rOperation.FindSubnode("script");
      if (itScript != rOperation.NodeEnd())
      {
        stOperation.tScript = *itScript;
      }
      else
      {
        itScript = rOperation.FindSubnode("execute");
        RISE_ASSERTS(itScript != rOperation.NodeEnd(), "cannot find <script> or <execute> element");
        stOperation.tScript.NodeName() = "script";
        stOperation.tScript.AddSubNode("") = *itScript;
      }

      ParseDescr(rOperation, stOperation.sDescr);

      rise::xml::CXMLNode::TXMLNodeConstIterator itReturn = rOperation.FindSubnode("return");
      if (itReturn != rOperation.NodeEnd())
      {
        const rise::xml::CXMLNode& rNodeReturn = *itReturn;
        ParseType(rNodeReturn, stOperation.stReturn);
        if (stOperation.stReturn.eType == DataType::List ||
            stOperation.stReturn.eType == DataType::Struct)
        {
          stOperation.stReturn = GetType(stOperation.stReturn.sType);
        }
      }

      const rise::xml::CXMLNode& rParams = rOperation.Subnode("params");
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rParams.NodeBegin();
            itType != rParams.NodeEnd(); ++itType)
      {
        DataType stType;
        ParseType(*itType, stType);
        stOperation.lsParams.push_back(stType);
      }

      m_lsOperations.push_back(stOperation);
    }
  }

  const IncludesList& DataSource::GetIncludes() const
  {
    return m_lsIncludes;
  }

  void DataSource::ParseType(const rise::xml::CXMLNode& rNode, DataType& rType)
  {
    rType.sName = rNode.NodeName();

    const std::string& sType = rNode.Attribute("type");

    ParseDescr(rNode, rType.sDescr);

    if (sType == "struct")
    {
      rType.eType = DataType::Struct;
      rType.sType = sType;

      for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rNode.NodeBegin();
            itType != rNode.NodeEnd(); ++itType)
      {
        DataType stType;
        ParseType(*itType, stType);
        rType.lsChilds.push_back(stType);
      }
    }
    else
    if (sType == "list")
    {
      rType.eType = DataType::List;
      rType.sType = rNode.Attribute("itemtype").AsString();

      RISE_ASSERTS(rType.sType != "void", "list item cannot be \"void\"");

      const DataType* pItemType = FindType(rType.sType);

      if (pItemType != NULL)
      { // ref
        RISE_ASSERTS(pItemType->eType != DataType::List, "list item cannot be \"list\"");
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
    }
  }

  void DataSource::IncludeTypes(const std::string& sFileName)
  {
    rise::xml::CXMLDocument tTypesDoc;

    std::string sDataSourcesDir = m_sFileName;

    std::string::size_type nPos = sDataSourcesDir.find_last_of(RISE_PATH_SEPARATOR);
    if (nPos != std::string::npos)
    {
      sDataSourcesDir.erase(nPos + 1);
    }

    rise::LogDebug() << "Including file: " << (sDataSourcesDir + sFileName);
    tTypesDoc.LoadFromFile(sDataSourcesDir + sFileName);

    m_lsIncludes.push_back(Include());
    Include& rstInclude = m_lsIncludes.back();
    rstInclude.sFileName = sFileName;

    const rise::xml::CXMLNode& rNodeTypes = tTypesDoc.GetRoot();
    for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rNodeTypes.NodeBegin();
          itType != rNodeTypes.NodeEnd(); ++itType)
    {
      DataType stType;
      ParseType(*itType, stType);

      rstInclude.lsTypes.push_back(stType);
      stType.bExtern = true;
      m_lsTypes.push_back(stType);
    }
  }

  void DataSource::ParseDescr(const rise::xml::CXMLNode& rNode, std::string& sDescr)
  {
    rise::xml::CXMLNode::TXMLAttrConstIterator itDescr = rNode.FindAttribute("descr");
    if (itDescr != rNode.AttrEnd())
    {
      sDescr = itDescr->sAttrValue.AsString();
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

  const DataType& DataSource::GetType(const std::string& sTypeName) const
  {
    const DataType* pType = FindType(sTypeName);
    RISE_ASSERTS(pType, "Type " + sTypeName + " is not found");
    return *pType;
  }

  const DataType* DataSource::FindType(const std::string& sTypeName) const
  {
    DataTypesList::const_iterator itType = m_lsTypes.begin();
    for (; itType != m_lsTypes.end(); ++itType)
    {
      if (itType->sName == sTypeName)
      {
        return &*itType;
      }
    }

    return NULL;
  }

  const Operation& DataSource::GetOperation(const std::string& sOperationName) const
  {
    OperationsList::const_iterator itOperation = m_lsOperations.begin();
    for (; itOperation != m_lsOperations.end(); ++itOperation)
    {
      if (itOperation->sName == sOperationName)
      {
        return *itOperation;
      }
    }

    RISE_THROWS(rise::CLogicNoItemException, "Operation " + sOperationName + " is not found");
  }

  const DataTypesList& DataSource::GetTypes() const
  {
    return m_lsTypes;
  }

  const OperationsList& DataSource::GetOperations() const
  {
    return m_lsOperations;
  }

}
}
