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
#include "Provider.h"
#include "ScriptExecuter.h"

namespace staff
{
namespace das
{
  struct Var
  {
    Type tType;
    std::string sValue;
    staff::CDataObject tdoValue;
  };


  class ScriptExecuter::ScriptExecuterImpl
  {
  public:
    typedef std::map<std::string, Var> VarMap;

  public:
    ScriptExecuterImpl(const CDataObject& rdoOperation, const DataSource& rDataSource, PProvider& rpProvider):
      m_pOperation(NULL), m_rdoOperation(rdoOperation), m_rDataSource(rDataSource), m_rpProvider(rpProvider)
    {
    }

    CDataObject Process()
    {
      m_pOperation = &m_rDataSource.GetOperation(m_rdoOperation.GetLocalName());

      CDataObject tdoResult(m_pOperation->sName + "Result");

      ProcessSequence(m_rdoOperation, m_pOperation->tScript, m_pOperation->stReturn, tdoResult);

      return tdoResult;
    }

    void ProcessSequence(const CDataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                                         const Type& rReturnType, CDataObject& rdoResult)
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
          {
            RISE_THROWS(rise::CLogicNoItemException, "Invalid operator: [" + sOperator + "]");
          }
        }
      }
    }

    void ProcessExecute(const CDataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                         const Type& rReturnType, CDataObject& rdoResult)
    {
      std::string sExecute = rScript.NodeContent().AsString();
      rise::LogDebug1() << "Query is [" + sExecute + "]";

      sExecute = Eval(rdoContext, sExecute);
      rise::LogDebug1() << "Query with params [" + sExecute + "]";

      m_rpProvider->Invoke(sExecute, rReturnType, rdoResult);
    }

    void ProcessVar(const CDataObject& rdoContext, const rise::xml::CXMLNode& rScript)
    {
      const std::string& sVarName = rScript.Attribute("name").AsString();

      Var& rVar = m_mVars[sVarName];

      // get var type
      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrType = rScript.FindAttribute("type");
      if (itAttrType != rScript.AttrEnd())
      {
        const std::string& sType = itAttrType->sAttrValue.AsString();

        const Type* pType = m_rDataSource.FindType(sType);

        if (pType != NULL)
        { // datasource's type
          rVar.tType = *pType;
        }
        else
        { // parse type
          rVar.tType.sType = sType;
          if (sType == "DataObject")
          {
            rVar.tType.eType = Type::DataObject;
            rVar.tdoValue.Create(sVarName);
          }
          else
          { // accept as generic
            rVar.tType.eType = Type::Generic;
          }
        }
      }


      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrValue = rScript.FindAttribute("value");
      if (itAttrValue != rScript.AttrEnd())
      {
        rVar.sValue = Eval(rdoContext, itAttrValue->sAttrValue.AsString());
        if (rVar.tType.eType == Type::Void)
        {
          rVar.tType.eType = Type::Generic;
        }
      }
      else
      {
        ProcessSequence(rdoContext, rScript, rVar.tType, rVar.tdoValue);
        if (rVar.tType.eType == Type::Void)
        {
          rVar.tType.eType = Type::DataObject;
        }
      }
    }


    void ProcessForeach(const CDataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                         const Type& rReturnType, CDataObject& rdoResult)
    {
      CDataObject tdoElement = rdoContext;

      rise::xml::CXMLNode::TXMLAttrConstIterator itAttrElementName = rScript.FindAttribute("element");
      if (itAttrElementName != rScript.AttrEnd())
      {
        GetChild(tdoElement, itAttrElementName->sAttrValue.AsString(), tdoElement);
      }

      for (CDataObject::Iterator itChild = tdoElement.Begin(); itChild != tdoElement.End(); ++itChild)
      {
        ProcessSequence(*itChild, rScript, rReturnType, rdoResult);
      }
    }

    void ProcessIfeq(const CDataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                     const Type& rReturnType, CDataObject& rdoResult, bool bEqual)
    {
      if ((Eval(rdoContext, rScript.Attribute("expr1").AsString()) ==
           Eval(rdoContext, rScript.Attribute("expr2").AsString())) == bEqual)
      {
        ProcessSequence(rdoContext, rScript, rReturnType, rdoResult);
      }
    }


    void ProcessReturn(const rise::xml::CXMLNode& rScript, const Type& rReturnType, CDataObject& rdoResult)
    {
      const std::string& sVarName = rScript.Attribute("var").AsString();
      VarMap::const_iterator itVar = m_mVars.find(sVarName);
      RISE_ASSERTS(itVar != m_mVars.end(), "Variable [" + sVarName + "] is undefined");
      const Var& rVar = itVar->second;
      RISE_ASSERTS(rVar.tType.eType == rReturnType.eType, "Types mismatch in [ return " + sVarName + "]");

      if (rReturnType.eType == Type::Generic)
      {
        rdoResult.SetText(rVar.sValue);
      }
      else
      { // dataobject, list, struct
        rdoResult = rVar.tdoValue;
      }
    }

    std::string Eval(const CDataObject& rdoContext, const std::string& sExpr)
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
          nEnd = sResult.find_first_of(" \n\t\r,.;'\"~`!@#$%^&*()-+=/?", nBegin + 1);

          if (nEnd == std::string::npos)
          {
            nEnd = sResult.size();
          }

          nNameBegin = nBegin + 1;
          nNameEnd = nEnd;
        }

        const std::string& sPath = sResult.substr(nNameBegin, nNameEnd - nNameBegin);

        std::string sValue;
        if (bIsRequest)
        { // request
          CDataObject tdoResult;
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
            RISE_ASSERTS(rVar.tType.eType != Type::Void, "found void type variable [" + sVarName
                         + "] while evaluating expression [" + sExpr + "]");

            if (rVar.tType.eType == Type::Generic)
            {
              sValue = rVar.sValue;
            }
            else
            { // dataobject, list, struct
              CDataObject tdoResult;
              GetChild(rVar.tdoValue, sPath.substr(nPos + 1), tdoResult);
              RISE_ASSERTS(!tdoResult.IsNull(), "Node not found while processing eval. NodeName: [" + sPath + "]");
              sValue = tdoResult.GetText();
            }
          }
        } // if request

        sResult.replace(nBegin, nEnd - nBegin, sValue);
        nEnd = nBegin + sValue.size();
      } // while find '$'

      return sResult;
    }

    void GetChild(const CDataObject& rdoContext, const std::string& sChildPath, CDataObject& rdoResult)
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
        do
        {
          nEnd = sPath.find('.', nBegin);
          std::string sName;
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
            rdoResult = rdoResult.GetChildByLocalName(sName);
          }

          nBegin = nEnd + 1;
        }
        while(nEnd != std::string::npos);
      }
    }

  public:
    const Operation* m_pOperation;
    const CDataObject& m_rdoOperation;
    const DataSource& m_rDataSource;
    PProvider& m_rpProvider;
    VarMap m_mVars;
  };


  ScriptExecuter::ScriptExecuter(const CDataObject& rdoOperation,
                                 const DataSource& rDataSource, PProvider& rpProvider)
  {
    m_pImpl = new ScriptExecuterImpl(rdoOperation, rDataSource, rpProvider);
  }

  ScriptExecuter::~ScriptExecuter()
  {
    delete m_pImpl;
  }

  CDataObject ScriptExecuter::Process()
  {
    return m_pImpl->Process();
  }



} // namespace das
} // namespace staff
