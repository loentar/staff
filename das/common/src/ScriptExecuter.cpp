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

#include <string>
#include <map>
#include <rise/common/MutablePtr.h>
#include <staff/common/Attribute.h>
#include <staff/common/DataObject.h>
#include "DataSource.h"
#include "Executor.h"
#include "Provider.h"
#include "ScriptExecuter.h"

namespace staff
{
namespace das
{
  struct Var
  {
    DataType tType;
    std::string sValue;
    staff::DataObject tdoValue;
  };


  class ScriptExecuter::ScriptExecuterImpl
  {
  public:
    typedef std::map<std::string, Var> VarMap;

    //! list of executors
    typedef std::map<std::string, PExecutor> ExecutorsMap;

  public:
    ScriptExecuterImpl(const DataSource& rDataSource, Providers& rstProviders):
      m_rDataSource(rDataSource), m_rstProviders(rstProviders)
    {
    }

    DataObject ProcessOperation(const DataObject& rdoOperation)
    {
      const Operation* pOperation = &m_rDataSource.GetOperation(rdoOperation.GetLocalName());

      DataObject tdoResult(pOperation->sName + "Result");

      ProcessSequence(rdoOperation, pOperation->tScript, pOperation->stReturn, tdoResult);
      if (pOperation->stReturn.eType == DataType::Generic)
      {
        RISE_ASSERTS(!tdoResult.IsTextNull(), "Empty result for generic type!");
      }

      return tdoResult;
    }

    void ProcessSequence(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                                         const DataType& rReturnType, DataObject& rdoResult)
    {
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itOperator = rScript.NodeBegin();
          itOperator != rScript.NodeEnd(); ++itOperator)
      {
        const rise::xml::CXMLNode& rNodeOperator = *itOperator;
        if (rNodeOperator.NodeType() == rise::xml::CXMLNode::ENTGENERIC)
        {
          const std::string& sOperator = rNodeOperator.NodeName();

          if (sOperator == "var")
          {
            ProcessVar(rdoContext, rNodeOperator);
          }
          else
          if (sOperator == "foreach")
          {
            ProcessForeach(rdoContext, rNodeOperator, rReturnType, rdoResult);
          }
          else
          if (sOperator == "ifeq")
          {
            ProcessIfeq(rdoContext, rNodeOperator, rReturnType, rdoResult, true);
          }
          else
          if (sOperator == "ifneq")
          {
            ProcessIfeq(rdoContext, rNodeOperator, rReturnType, rdoResult, false);
          }
          else
          if (sOperator == "return")
          {
            ProcessReturn(rNodeOperator, rReturnType, rdoResult);
            return;
          }
          else
          if (sOperator == "execute")
          {
            ProcessExecute(rdoContext, rNodeOperator, rReturnType, rdoResult);
          }
          else
          if (sOperator == "log")
          {
            ProcessLog(rdoContext, rNodeOperator);
          }
          else
          if (sOperator == "fault")
          {
            ProcessFault(rdoContext, rNodeOperator);
          }
          else
          {
            RISE_THROWS(rise::CLogicNoItemException, "Invalid operator: [" + sOperator + "]");
          }
        }
      }
    }

    const std::string& GetProviderId(const rise::xml::CXMLNode& rScript) const
    {
      const rise::xml::CXMLNode* pElem = &rScript;
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttr;
      while (pElem)
      {
        const std::string& sNodeName = pElem->NodeName();
        if (sNodeName == "operation")
        {
          break;
        }
        else
        if (sNodeName == "execute" || sNodeName == "script")
        {
          itAttr = pElem->FindAttribute("providerid");
          if (itAttr != pElem->AttrEnd())
          {
            return itAttr->sAttrValue.AsString();
          }
        }
        else
        if (sNodeName == "provider")
        {
          itAttr = pElem->FindAttribute("id");
          if (itAttr != pElem->AttrEnd())
          {
            return itAttr->sAttrValue.AsString();
          }
        }
        pElem = pElem->GetParent();
      }

      return m_rstProviders.sDefaultId;
    }

    void ProcessExecute(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                        const DataType& rReturnType, DataObject& rdoResult)
    {
      std::string sExecute = rScript.NodeContent().AsString();
      rise::LogDebug1() << "Query is [" + sExecute + "]";

      sExecute = Eval(rdoContext, sExecute);
      rise::LogDebug1() << "Query with params [" + sExecute + "]";

      const std::string& sProviderId = GetProviderId(rScript);

      PExecutor& rpExec = m_mExec[sProviderId];
      if (!rpExec.Get())
      {
        ProvidersMap::iterator itProvider = m_rstProviders.mProviders.find(sProviderId);
        RISE_ASSERTS(itProvider != m_rstProviders.mProviders.end(), "Provider with id=["
                     + sProviderId + "] does not exists");

        rpExec = itProvider->second->GetExecutor();
        RISE_ASSERT(rpExec.Get());
      }

      switch (rpExec->GetType())
      {
        case IExecutor::TypeRaw:
        {
          IRawExecutor* pExec = static_cast<IRawExecutor*>(rpExec.Get());
          pExec->Execute(sExecute, rdoContext, rReturnType, rdoResult);
          break;
        }

        case IExecutor::TypeQuery:
        {
          IQueryExecutor* pExec = static_cast<IQueryExecutor*>(rpExec.Get());

          pExec->Execute(sExecute);

          StringList lsResult;

          if (rReturnType.eType == DataType::Generic)
          {
            if (pExec->GetNextResult(lsResult))
            {
              RISE_ASSERTS(lsResult.size() == 1, "Fields count does not match: " +
                  rise::ToStr(lsResult.size()) + " expected: 1");
              rdoResult.SetText(lsResult.front());
            }
          }
          else
          if (rReturnType.eType == DataType::Struct)
          {
            if (pExec->GetNextResult(lsResult))
            {
              RISE_ASSERTS(lsResult.size() == rReturnType.lsChilds.size(), "Fields count does not match: " +
                  rise::ToStr(lsResult.size()) + " expected: " + rise::ToStr(rReturnType.lsChilds.size()));

              StringList::const_iterator itResult = lsResult.begin();
              for (DataTypesList::const_iterator itType = rReturnType.lsChilds.begin();
                  itType != rReturnType.lsChilds.end(); ++itType, ++itResult)
              {
                rdoResult.CreateChild(itType->sName).SetText(*itResult);
              }
            }
          }
          else
          if (rReturnType.eType == DataType::DataObject ||
              (rReturnType.eType == DataType::List && rReturnType.lsChilds.front().eType == DataType::DataObject))
          {
            StringList lsFieldsNames;
            pExec->GetFieldsNames(lsFieldsNames);
            while (pExec->GetNextResult(lsResult))
            {
              staff::DataObject tdoItem = rdoResult.CreateChild("Item");

              for (StringList::const_iterator itResult = lsResult.begin(), itName = lsFieldsNames.begin();
                   itResult != lsResult.end(); ++itResult, ++itName)
              {
                tdoItem.CreateChild(*itName).SetText(*itResult);
              }
            }
          }
          else
          if (rReturnType.eType == DataType::List) // ---------------- list ----------------------------
          {
            const DataType& rItemType = rReturnType.lsChilds.front();
            if (rItemType.eType == DataType::Generic) // list of generics
            {
              if (pExec->GetNextResult(lsResult))
              {
                RISE_ASSERTS(lsResult.size() == 1, "Fields count does not match: " +
                             rise::ToStr(lsResult.size()) + " expected: 1");
                do
                {
                  rdoResult.CreateChild("Item").SetText(lsResult.front());
                }
                while (pExec->GetNextResult(lsResult));
              }
            }
            else
            if (rItemType.eType == DataType::Struct) // list of structs
            {
              if (pExec->GetNextResult(lsResult))
              {
                RISE_ASSERTS(lsResult.size() == rItemType.lsChilds.size(), "Fields count does not match: " +
                             rise::ToStr(lsResult.size()) + " expected: " + rise::ToStr(rItemType.lsChilds.size()));
                do
                {
                  staff::DataObject tdoItem = rdoResult.CreateChild("Item");

                  StringList::const_iterator itResult = lsResult.begin();
                  for (DataTypesList::const_iterator itType = rItemType.lsChilds.begin();
                      itType != rItemType.lsChilds.end(); ++itType, ++itResult)
                  {
                    tdoItem.CreateChild(itType->sName).SetText(*itResult);
                  }
                }
                while (pExec->GetNextResult(lsResult));
              }
            }
            else
            {
              RISE_THROWS(rise::CLogicNoItemException, "Unsupported list item type: " + rReturnType.sType);
            }
          }
          else
          if (rReturnType.eType != DataType::Void)
          {
            RISE_THROWS(rise::CLogicNoItemException, "Unsupported return type: " + rReturnType.sType);
          }
          break;
        }

        default:
          RISE_THROWS(rise::CLogicNoItemException, "Unknown executor type");
      }

    }

    void ProcessVar(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript)
    {
      const std::string& sVarName = rScript.Attribute("name").AsString();

      Var& rVar = m_mVars[sVarName];

      // get var type
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrType = rScript.FindAttribute("type");
      if (itAttrType != rScript.AttrEnd())
      {
        const std::string& sType = itAttrType->sAttrValue.AsString();

        const DataType* pType = m_rDataSource.FindType(sType);

        if (pType != NULL)
        { // datasource's type
          rVar.tType = *pType;
        }
        else
        { // parse type
          rVar.tType.sType = sType;
          if (sType == "DataObject")
          {
            rVar.tType.eType = DataType::DataObject;
            rVar.tdoValue.Create(sVarName);
          }
          else
          { // accept as generic
            rVar.tType.eType = DataType::Generic;
          }
        }
      }


      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrValue = rScript.FindAttribute("value");
      if (itAttrValue != rScript.AttrEnd())
      {
        rVar.sValue = Eval(rdoContext, itAttrValue->sAttrValue.AsString());
        if (rVar.tType.eType == DataType::Void)
        {
          rVar.tType.eType = DataType::Generic;
        }
      }
      else
      {
        rVar.tdoValue.Create(sVarName);
        if (rVar.tType.eType == DataType::Void)
        {
          rVar.tType.eType = DataType::DataObject;
        }
        ProcessSequence(rdoContext, rScript, rVar.tType, rVar.tdoValue);
        if (rVar.tType.eType == DataType::Generic)
        {
          rVar.sValue = rVar.tdoValue.GetText();
          rVar.tdoValue.Detach();
        }
      }
    }


    void ProcessForeach(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                         const DataType& rReturnType, DataObject& rdoResult)
    {
      DataObject tdoElement = rdoContext;

      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrElementName = rScript.FindAttribute("element");
      if (itAttrElementName != rScript.AttrEnd())
      {
        GetChild(tdoElement, itAttrElementName->sAttrValue.AsString(), tdoElement);
      }

      for (DataObject::Iterator itChild = tdoElement.Begin(); itChild != tdoElement.End(); ++itChild)
      {
        ProcessSequence(*itChild, rScript, rReturnType, rdoResult);
      }
    }

    void ProcessIfeq(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                     const DataType& rReturnType, DataObject& rdoResult, bool bEqual)
    {
      if ((Eval(rdoContext, rScript.Attribute("expr1").AsString()) ==
           Eval(rdoContext, rScript.Attribute("expr2").AsString())) == bEqual)
      {
        ProcessSequence(rdoContext, rScript, rReturnType, rdoResult);
      }
    }


    void ProcessReturn(const rise::xml::CXMLNode& rScript, const DataType& rReturnType, DataObject& rdoResult)
    {
      const std::string& sVarName = rScript.Attribute("var").AsString();
      RISE_ASSERTS(!sVarName.empty(), "variable name is empty");
      if (sVarName[0] == '$')
      {
        const std::string& sValue = Eval(DataObject(), sVarName);
        rdoResult.SetText(sValue);
      }
      else
      {
        VarMap::const_iterator itVar = m_mVars.find(sVarName);
        RISE_ASSERTS(itVar != m_mVars.end(), "Variable [" + sVarName + "] is undefined");
        const Var& rVar = itVar->second;
        RISE_ASSERTS(rVar.tType.eType == rReturnType.eType, "Types mismatch in [ return " + sVarName + "]");

        if (rReturnType.eType == DataType::Generic)
        {
          rdoResult.SetText(rVar.sValue);
        }
        else
        { // dataobject, list, struct
          rdoResult = rVar.tdoValue;
        }
      }
    }

    std::string Eval(const DataObject& rdoContext, const std::string& sExpr)
    {
      std::string sResult = sExpr;
      std::string::size_type nBegin = 0;
      std::string::size_type nEnd = 0;
      std::string::size_type nNameBegin = 0;
      std::string::size_type nNameEnd = 0;

      while ((nBegin = sResult.find('$', nEnd)) != std::string::npos)
      {
        std::string::size_type nSize = sResult.size();
        bool bIsRequest = false;

        RISE_ASSERTS((nBegin + 1) < nSize, "Unexpected '$' at end in [" + sResult + "]");
        if (sResult[nBegin + 1] == '(') // request field
        {
          nEnd = sResult.find(')', nBegin);
          RISE_ASSERTS(nEnd != std::string::npos, "Unexpected end of expression declaration: [" + sResult
                      + "] while processing [" + sResult.substr(nBegin));
          nNameBegin = nBegin + 2;
          nNameEnd = nEnd;
          ++nEnd;
          bIsRequest = true;
        }
        else
        if (sResult[nBegin + 1] == '{') // long variable name
        {
          nEnd = sResult.find('}', nBegin + 2);
          RISE_ASSERTS(nEnd != std::string::npos, "Unexpected end of expression declaration: [" + sResult
                      + "] while processing [" + sResult.substr(nBegin));
          nNameBegin = nBegin + 2;
          nNameEnd = nEnd;
          ++nEnd;
        }
        else
        { // short variable name
          nEnd = sResult.find_first_of(" \n\t\r,.;'\"~`!@#$%^&*()[]{}<>-+=/?", nBegin + 1);

          if (nEnd == std::string::npos)
          {
            nEnd = sResult.size();
          }

          nNameBegin = nBegin + 1;
          nNameEnd = nEnd;
        }

        if (nNameBegin == nNameEnd)
        {
          continue;
        }

        const std::string& sPath = sResult.substr(nNameBegin, nNameEnd - nNameBegin);

        std::string sValue;
        if (bIsRequest)
        { // request
          DataObject tdoResult;
          GetChild(rdoContext, sPath, tdoResult);
          RISE_ASSERTS(!tdoResult.IsNull(), "Node not found while processing eval. NodeName: [" + sPath + "]");
          sValue = tdoResult.GetText();
        }
        else
        { // variable
          // get var name
          std::string sVarName;
          std::string::size_type nPos = sPath.find('.', nBegin);
          if (nPos != std::string::npos)
          {
            sVarName = sPath.substr(0, nPos);
          }
          else
          {
            sVarName = sPath;
          }

          RISE_ASSERTS(!sVarName.empty(), "Invalid var name: [" + sPath + "]");

          if (sVarName == "nodeValue")
          {
            sValue = rdoContext.GetText();
          }
          else
          if (sVarName == "nodeValue")
          {
            sValue = rdoContext.GetLocalName();
          }
          else
          {
            VarMap::const_iterator itVar = m_mVars.find(sVarName);
            RISE_ASSERTS(itVar != m_mVars.end(), "Variable [" + sVarName + "] is undefined");

            const Var& rVar = itVar->second;
            RISE_ASSERTS(rVar.tType.eType != DataType::Void, "found void type variable [" + sVarName
                         + "] while evaluating expression [" + sExpr + "]");

            if (rVar.tType.eType == DataType::Generic)
            {
              sValue = rVar.sValue;
            }
            else
            { // dataobject, list, struct
              if (nPos != std::string::npos)
              {
                DataObject tdoResult;
                GetChild(rVar.tdoValue, sPath.substr(nPos + 1), tdoResult);
                const_cast<DataObject&>(rVar.tdoValue).SetOwner(true);
                RISE_ASSERTS(!tdoResult.IsNull(), "Node not found while processing eval. NodeName: [" + sPath + "]");
                sValue = tdoResult.GetText();
              }
              else
              {
                sValue = rVar.tdoValue.GetText();
              }
            }
          }
        } // if request

        sResult.replace(nBegin, nEnd - nBegin, sValue);
        nEnd = nBegin + sValue.size();
      } // while find '$'

      return sResult;
    }

    void ProcessLog(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript)
    {
      rise::LogInfo() << m_rDataSource.GetName() << ": " << Eval(rdoContext, rScript.NodeContent());
    }

    void ProcessFault(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript)
    {
      RISE_THROWS(rise::CInternalAssertException, "Error while invoking Operation [" + rdoContext.GetLocalName() +
                  "] in datasource [" + m_rDataSource.GetName() +
                  "]: " + Eval(rdoContext, rScript.NodeContent()));
    }

    void GetChild(const DataObject& rdoContext, const std::string& sChildPath, DataObject& rdoResult)
    {
      rdoResult = rdoContext;

      if (!sChildPath.empty())
      {
        std::string::size_type nSize = sChildPath.size();
        RISE_ASSERTS(nSize > 1, "invalid node name: [" + sChildPath + "]");

        std::string sPath =
            (sChildPath[0] == '$' && sChildPath[1] == '(' && sChildPath[nSize - 1] == ')') ?
            sChildPath.substr(2, nSize - 3) :
            sChildPath;
        std::string::size_type nBegin = 0;
        std::string::size_type nEnd = 0;
        std::string sName;
        do
        {
          nEnd = sPath.find('.', nBegin);
          if (nEnd == std::string::npos)
          {
            sName = sPath.substr(nBegin);
          }
          else
          {
            sName = sPath.substr(nBegin, nEnd - nBegin);
          }

          if (sName.empty()) // parent node
          {
            rdoResult = rdoResult.Parent();
            RISE_ASSERTS(!rdoResult.IsNull(), "parent of root element reached while GetChild[" + sChildPath + "]");
          }
          else
          {
            rdoResult.SetOwner(false);
            rdoResult = rdoResult.GetChildByLocalName(sName);
          }

          nBegin = nEnd + 1;
        }
        while(nEnd != std::string::npos);
      }
    }

  public:
    const DataSource& m_rDataSource;
    Providers& m_rstProviders;
    VarMap m_mVars;
    ExecutorsMap m_mExec;
  };


  ScriptExecuter::ScriptExecuter(const DataSource& rDataSource, Providers& rstProviders)
  {
    m_pImpl = new ScriptExecuterImpl(rDataSource, rstProviders);
  }

  ScriptExecuter::~ScriptExecuter()
  {
    delete m_pImpl;
  }

  DataObject ScriptExecuter::Process(const DataObject& rdoOperation)
  {
    return m_pImpl->ProcessOperation(rdoOperation);
  }

  void ScriptExecuter::Process(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                               const DataType& rReturnType, DataObject& rdoResult)
  {
    m_pImpl->ProcessSequence(rdoContext, rScript, rReturnType, rdoResult);
    if (rReturnType.eType == DataType::Generic)
    {
      RISE_ASSERTS(!rdoResult.IsTextNull(), "Empty result for generic type!");
    }
  }

  void ScriptExecuter::Process(const rise::xml::CXMLNode& rScript)
  {
    staff::DataObject doResult;
    m_pImpl->ProcessSequence(staff::DataObject(), rScript, DataType(), doResult);
  }


} // namespace das
} // namespace staff
