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
#include <exception>
#include <staff/utils/Log.h>
#include <staff/utils/SharedPtr.h>
#include <staff/utils/tostring.h>
#include <staff/utils/stringutils.h>
#include <staff/common/Attribute.h>
#include <staff/common/DataObject.h>
#include <staff/xml/Attribute.h>
#include <staff/xml/Element.h>
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
      m_rDataSource(rDataSource), m_rstProviders(rstProviders), m_bReturn(false)
    {
    }

    void ProcessSequence(const DataObject& rdoContext, const xml::Element& rScript,
                                         const DataType& rReturnType, DataObject& rdoResult)
    {
      for (const xml::Element* pOperator = rScript.GetFirstChildElement();
          pOperator && !m_bReturn; pOperator = pOperator->GetNextSiblingElement())
      {
        const std::string& sOperator = pOperator->GetName();

        if (sOperator == "var")
        {
          ProcessVar(rdoContext, *pOperator);
        }
        else
        if (sOperator == "foreach")
        {
          ProcessForeach(rdoContext, *pOperator, rReturnType, rdoResult);
        }
        else
        if (sOperator == "ifeq")
        {
          ProcessIfeq(rdoContext, *pOperator, rReturnType, rdoResult, true);
        }
        else
        if (sOperator == "ifneq")
        {
          ProcessIfeq(rdoContext, *pOperator, rReturnType, rdoResult, false);
        }
        else
        if (sOperator == "return")
        {
          ProcessReturn(rdoContext, *pOperator, rReturnType, rdoResult);
          m_bReturn = true;
          return;
        }
        else
        if (sOperator == "execute")
        {
          ProcessExecute(rdoContext, *pOperator, rReturnType, rdoResult);
        }
        else
        if (sOperator == "log")
        {
          ProcessLog(rdoContext, *pOperator);
        }
        else
        if (sOperator == "fault")
        {
          ProcessFault(rdoContext, *pOperator);
        }
        else
        if (sOperator == "trycatch")
        {
          ProcessTryCatch(rdoContext, *pOperator, rReturnType, rdoResult);
        }
        else
        {
          STAFF_THROW_ASSERT("Invalid operator: [" + sOperator + "]");
        }
      }
    }

    const std::string& GetProviderId(const xml::Element& rScript) const
    {
      const xml::Element* pElem = &rScript;
      const xml::Attribute* pAttr = NULL;
      while (pElem)
      {
        const std::string& sNodeName = pElem->GetName();
        if (sNodeName == "operation")
        {
          break;
        }
        else
        if (sNodeName == "execute" || sNodeName == "script")
        {
          pAttr = pElem->FindAttribute("providerid");
          if (pAttr)
          {
            return pAttr->GetValue();
          }
        }
        else
        if (sNodeName == "provider")
        {
          pAttr = pElem->FindAttribute("id");
          if (pAttr)
          {
            return pAttr->GetValue();
          }
        }
        pElem = pElem->GetParent();
      }

      return m_rstProviders.sDefaultId;
    }

    void ProcessExecute(const DataObject& rdoContext, const xml::Element& rScript,
                        const DataType& rReturnType, DataObject& rdoResult)
    {
      std::string sExecute = rScript.GetValue();
      LogDebug1() << "Query is [" + sExecute + "]";

      sExecute = Eval(rdoContext, sExecute);
      LogDebug1() << "Query with params [" + sExecute + "]";

      const std::string& sProviderId = GetProviderId(rScript);

      PExecutor& rpExec = m_mExec[sProviderId];
      if (!rpExec.Get())
      {
        ProvidersMap::iterator itProvider = m_rstProviders.mProviders.find(sProviderId);
        STAFF_ASSERT(itProvider != m_rstProviders.mProviders.end(), "Provider with id=["
                     + sProviderId + "] does not exists");

        rpExec = itProvider->second->GetExecutor();
        STAFF_ASSERT(rpExec.Get(), "Couldn't get Executor");
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
              STAFF_ASSERT(lsResult.size() == 1, "Fields count does not match: " +
                  ToString(lsResult.size()) + " expected: 1");
              rdoResult.SetText(lsResult.front());
            }
          }
          else
          if (rReturnType.eType == DataType::Struct)
          {
            if (pExec->GetNextResult(lsResult))
            {
              STAFF_ASSERT(lsResult.size() == rReturnType.lsChilds.size(), "Fields count does not match: " +
                  ToString(lsResult.size()) + " expected: " + ToString(rReturnType.lsChilds.size()));

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
                STAFF_ASSERT(lsResult.size() == 1, "Fields count does not match: " +
                             ToString(lsResult.size()) + " expected: 1");
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
                STAFF_ASSERT(lsResult.size() == rItemType.lsChilds.size(), "Fields count does not match: " +
                             ToString(lsResult.size()) + " expected: " + ToString(rItemType.lsChilds.size()));
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
              STAFF_THROW_ASSERT("Unsupported list item type: " + rReturnType.sType);
            }
          }
          else
          if (rReturnType.eType != DataType::Void)
          {
            STAFF_THROW_ASSERT("Unsupported return type: " + rReturnType.sType);
          }
          break;
        }

        default:
          STAFF_THROW_ASSERT("Unknown executor type");
      }

      const xml::Attribute* pAttr = rScript.FindAttribute("result");
      if (pAttr)
      {
        const std::string& sValue = pAttr->GetValue();
        if (sValue == "required")
        {
          STAFF_ASSERT(!rdoResult.IsTextNull() || !rdoResult.FirstChild().IsNull(),
                       "Empty result, when result is required");
        }
        else
        {
          STAFF_ASSERT(sValue == "optional", "Invalid \"result\" attribute value. "
                       "Valid values are: \"optional\"(default), \"required\"");
        }
      }

      pAttr = rScript.FindAttribute("trim");
      if (pAttr && pAttr->GetValue() == "true" && !rdoResult.IsTextNull())
      {
        std::string sText = rdoResult.GetText();
        StringTrim(sText);
        rdoResult.SetText(sText);
      }

    }

    void ProcessVar(const DataObject& rdoContext, const xml::Element& rScript)
    {
      const std::string& sVarName = rScript.GetAttributeValue("name");

      Var& rVar = m_mVars[sVarName];

      // get var type
      const xml::Attribute* pAttrType = rScript.FindAttribute("type");
      if (pAttrType)
      {
        const std::string& sType = pAttrType->GetValue();

        const DataType* pType = m_rDataSource.FindType(sType);

        if (pType)
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


      const xml::Attribute* pAttrValue = rScript.FindAttribute("value");
      if (pAttrValue)
      {
        rVar.sValue = Eval(rdoContext, pAttrValue->GetValue());
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
          if (rVar.tType.sType != "string")
          {
            StringTrim(rVar.sValue);
          }
        }
      }
    }


    void ProcessForeach(const DataObject& rdoContext, const xml::Element& rScript,
                        const DataType& rReturnType, DataObject& rdoResult)
    {
      DataObject tdoElement = rdoContext;

      const xml::Attribute* pAttrElementName = rScript.FindAttribute("element");
      if (pAttrElementName)
      {
        GetChild(tdoElement, pAttrElementName->GetValue(), tdoElement);
      }

      for (DataObject::Iterator itChild = tdoElement.Begin(); itChild != tdoElement.End(); ++itChild)
      {
        ProcessSequence(*itChild, rScript, rReturnType, rdoResult);
      }
    }

    void ProcessIfeq(const DataObject& rdoContext, const xml::Element& rScript,
                     const DataType& rReturnType, DataObject& rdoResult, bool bEqual)
    {
      if ((Eval(rdoContext, rScript.GetAttributeValue("expr1")) ==
           Eval(rdoContext, rScript.GetAttributeValue("expr2"))) == bEqual)
      {
        ProcessSequence(rdoContext, rScript, rReturnType, rdoResult);
      }
    }


    void ProcessReturn(const DataObject& rdoContext, const xml::Element& rScript,
                       const DataType& rReturnType, DataObject& rdoResult)
    {
      const xml::Attribute* pVar = rScript.FindAttribute("var");
      if (pVar)
      {
        const std::string& sVarName = pVar->GetValue();
        STAFF_ASSERT(!sVarName.empty(), "variable name is empty");
        if (sVarName[0] == '$')
        {
          const std::string& sValue = Eval(rdoContext, sVarName);
          rdoResult.SetText(sValue);
        }
        else
        {
          VarMap::const_iterator itVar = m_mVars.find(sVarName);
          STAFF_ASSERT(itVar != m_mVars.end(), "Variable [" + sVarName + "] is undefined");
          const Var& rVar = itVar->second;
          STAFF_ASSERT(rVar.tType.eType == rReturnType.eType, "Types mismatch in [ return " + sVarName + "]");

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
      else
      {
        rdoResult.SetText(Eval(rdoContext, rScript.GetValue()));
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
        if (nBegin > 0 && sResult[nBegin - 1] == '\\')
        {
          sResult.erase(nBegin - 1, 1);
          nEnd = nBegin;
          continue;
        }

        std::string::size_type nSize = sResult.size();
        bool bIsRequest = false;

        STAFF_ASSERT((nBegin + 1) < nSize, "Unexpected '$' at end in [" + sResult + "]");
        if (sResult[nBegin + 1] == '(') // request field
        {
          nEnd = sResult.find(')', nBegin);
          STAFF_ASSERT(nEnd != std::string::npos, "Unexpected end of expression declaration: [" + sResult
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
          STAFF_ASSERT(nEnd != std::string::npos, "Unexpected end of expression declaration: [" + sResult
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
          STAFF_ASSERT(!tdoResult.IsNull(), "Node not found while processing eval. NodeName: [" + sPath + "]");
          sValue = tdoResult.GetText();
        }
        else
        { // variable
          // get var name
          std::string sVarName;
          std::string::size_type nPos = sPath.find('.');
          if (nPos != std::string::npos)
          {
            sVarName = sPath.substr(0, nPos);
          }
          else
          {
            sVarName = sPath;
          }

          STAFF_ASSERT(!sVarName.empty(), "Invalid var name: [" + sPath + "]");

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
            STAFF_ASSERT(itVar != m_mVars.end(), "Variable [" + sVarName + "] is undefined");

            const Var& rVar = itVar->second;
            STAFF_ASSERT(rVar.tType.eType != DataType::Void, "found void type variable [" + sVarName
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
                STAFF_ASSERT(!tdoResult.IsNull(), "Node not found while processing eval. NodeName: [" + sPath + "]");
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

    void ProcessLog(const DataObject& rdoContext, const xml::Element& rScript)
    {
      LogInfo() << m_rDataSource.GetName() << ": " << Eval(rdoContext, rScript.GetValue());
    }

    void ProcessFault(const DataObject& rdoContext, const xml::Element& rScript)
    {
      STAFF_THROW_ASSERT("Error while invoking Operation [" + rdoContext.GetLocalName() +
                  "] in datasource [" + m_rDataSource.GetName() +
                  "]: " + Eval(rdoContext, rScript.GetValue()));
    }

    void ProcessTryCatch(const DataObject& rdoContext, const xml::Element& rScript,
                         const DataType& rReturnType, DataObject& rdoResult)
    {
      const xml::Element& rTry = rScript.GetChildElementByName("try");
      const xml::Element& rCatch = rScript.GetChildElementByName("catch");

      try
      {
        ProcessSequence(rdoContext, rTry, rReturnType, rdoResult);
      }
      catch (const std::exception& rEx)
      {
        const xml::Attribute* pAttrVar = rCatch.FindAttribute("var");
        if (pAttrVar)
        {
          Var& rVar = m_mVars[pAttrVar->GetValue()];
          rVar.tType.eType = DataType::Generic;
          rVar.sValue = rEx.what();
        }
        ProcessSequence(rdoContext, rCatch, rReturnType, rdoResult);
      }
    }


    void GetChild(const DataObject& rdoContext, const std::string& sChildPath, DataObject& rdoResult)
    {
      rdoResult = rdoContext;

      if (!sChildPath.empty())
      {
        std::string::size_type nSize = sChildPath.size();
        STAFF_ASSERT(nSize > 1, "invalid node name: [" + sChildPath + "]");

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
            STAFF_ASSERT(!rdoResult.IsNull(), "parent of root element reached while GetChild[" + sChildPath + "]");
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
    bool m_bReturn;
  };


  ScriptExecuter::ScriptExecuter(const DataSource& rDataSource, Providers& rstProviders)
  {
    m_pImpl = new ScriptExecuterImpl(rDataSource, rstProviders);
  }

  ScriptExecuter::~ScriptExecuter()
  {
    delete m_pImpl;
  }

  void ScriptExecuter::Process(const DataObject& rdoOperation, DataObject& rdoResult)
  {
    const Operation* pOperation = &m_pImpl->m_rDataSource.GetOperation(rdoOperation.GetLocalName());

    m_pImpl->ProcessSequence(rdoOperation, pOperation->tScript, pOperation->stReturn, rdoResult);

    if (pOperation->stReturn.eType == DataType::Generic)
    {
      STAFF_ASSERT(!rdoResult.IsTextNull(), "Empty result for generic type!");
    }
  }

  void ScriptExecuter::Process(const DataObject& rdoContext, const xml::Element& rScript,
                               const DataType& rReturnType, DataObject& rdoResult)
  {
    m_pImpl->ProcessSequence(rdoContext, rScript, rReturnType, rdoResult);
    if (rReturnType.eType == DataType::Generic)
    {
      STAFF_ASSERT(!rdoResult.IsTextNull(), "Empty result for generic type!");
    }
  }

  void ScriptExecuter::Process(const xml::Element& rScript)
  {
    staff::DataObject doResult;
    m_pImpl->ProcessSequence(staff::DataObject(), rScript, DataType(), doResult);
  }


} // namespace das
} // namespace staff
