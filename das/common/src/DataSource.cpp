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
  Type::Type():
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

    m_tNodeProviderConfig = rDataSourceNode.Subnode("provider");

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

    m_sProviderName = m_tNodeProviderConfig.Attribute("name").AsString();

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
          Type stType;
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
        if (stOperation.stReturn.eType == Type::List ||
            stOperation.stReturn.eType == Type::Struct)
        {
          stOperation.stReturn = GetType(stOperation.stReturn.sType);
        }
      }

      const rise::xml::CXMLNode& rParams = rOperation.Subnode("params");
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rParams.NodeBegin();
            itType != rParams.NodeEnd(); ++itType)
      {
        Type stType;
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

  void DataSource::ParseType(const rise::xml::CXMLNode& rNode, Type& rType)
  {
    rType.sName = rNode.NodeName();

    const std::string& sType = rNode.Attribute("type");

    ParseDescr(rNode, rType.sDescr);

    if (sType == "struct")
    {
      rType.eType = Type::Struct;
      rType.sType = sType;

      for (rise::xml::CXMLNode::TXMLNodeConstIterator itType = rNode.NodeBegin();
            itType != rNode.NodeEnd(); ++itType)
      {
        Type stType;
        ParseType(*itType, stType);
        rType.lsChilds.push_back(stType);
      }
    }
    else
    if (sType == "list")
    {
      rType.eType = Type::List;
      rType.sType = rNode.Attribute("itemtype").AsString();

      RISE_ASSERTS(rType.sType != "void", "list item cannot be \"void\"");

      const Type* pItemType = FindType(rType.sType);

      if (pItemType != NULL)
      { // ref
        RISE_ASSERTS(pItemType->eType != Type::List, "list item cannot be \"list\"");
        rType.lsChilds.push_back(*pItemType);
      }
      else
      { // simple
        Type tItemType;
        tItemType.sName = "Item";
        tItemType.sType = rType.sType;
        if (rType.sType == "DataObject" || rType.sType == "staff::DataObject")
        {
          tItemType.eType = Type::DataObject;
        }
        else
        {
          tItemType.eType = Type::Generic;
        }
        rType.lsChilds.push_back(tItemType);
      }
    }
    else
    if (sType == "void")
    {
      rType.eType = Type::Void;
      rType.sType = sType;
    }
    else
    if (sType == "DataObject" || sType == "staff::DataObject")
    {
      rType.eType = Type::DataObject;
      rType.sType = sType;
    }
    else
    {
      const Type* pType = FindType(sType);

      if (pType != NULL)
      { // ref
        rType.eType = pType->eType;
      }
      else
      { // simple
        rType.eType = Type::Generic;
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
      Type stType;
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

  const std::string& DataSource::GetProviderName() const
  {
    return m_sProviderName;
  }

  const rise::xml::CXMLNode& DataSource::GetProviderConfig() const
  {
    return m_tNodeProviderConfig;
  }

  const Type& DataSource::GetType(const std::string& sTypeName) const
  {
    const Type* pType = FindType(sTypeName);
    RISE_ASSERTS(pType, "Type " + sTypeName + " is not found");
    return *pType;
  }

  const Type* DataSource::FindType(const std::string& sTypeName) const
  {
    TypesList::const_iterator itType = m_lsTypes.begin();
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

  const TypesList& DataSource::GetTypes() const
  {
    return m_lsTypes;
  }

  const OperationsList& DataSource::GetOperations() const
  {
    return m_lsOperations;
  }

}
}
