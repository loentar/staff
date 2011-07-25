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

#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <errno.h>
#else
#include <sys/types.h>
#include <fnmatch.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <stack>
#include <algorithm>
#include <rise/common/exmacros.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/xml/XMLNode.h>
#include <rise/tools/FileFind.h>
#include "CodeGen.h"

namespace staff
{
namespace codegen
{
  using rise::xml::CXMLNode;

  class TemplateParser
  {
  public:
    TemplateParser():
      m_nLine(0), m_nIndent(0), m_bNeedIndent(false), m_bHasConfig(false)
    {
      m_tmVariables.push(StringMap());
    }

    std::string GetNodePath(const CXMLNode* pNode) const
    {
      if (pNode)
      {
        std::string sPath = pNode->NodeName();
        while ((pNode = pNode->GetParent()))
        {
          sPath = pNode->NodeName() + "." + sPath;
        }
        return sPath;
      }
      else
      {
        return "";
      }
    }

    const CXMLNode& GetNode(const std::string& sVariableName, const CXMLNode& rNode) const
    {
      static CXMLNode tEmptyNode;
      const CXMLNode* pNode = &rNode;
      std::string::size_type nPos = sVariableName.find('.');
      std::string sVariable;
      std::string sClass;

      if (nPos != std::string::npos)
      {
        if (sVariableName.size() == 1)
        { // reference to current node
          return rNode;
        }

        if (!nPos)
        {
          sVariable = sVariableName.substr(1);
          RISE_ASSERTS(!sVariable.empty(), "Element name expected in Name: " + sVariableName);
          nPos = sVariable.find('.');
          sClass = sVariable.substr(0, nPos); // next element name

          for (; pNode != NULL; pNode = pNode->GetParent())
          {
            rise::xml::CXMLNode::TXMLNodeConstIterator itNode = pNode->FindSubnode(sClass);
            if (itNode != pNode->NodeEnd())
            {
              if (nPos == std::string::npos)
              {
                return *itNode;
              }

              sVariable.erase(0, nPos + 1);
              pNode = &*itNode;
              break;
            }
          }
        }
        else
        {
          sClass = sVariableName.substr(0, nPos);
          sVariable = sVariableName.substr(nPos + 1);

          while (pNode != NULL && pNode->NodeName() != sClass)
          {
            pNode = pNode->GetParent();
          }
        }

        RISE_ASSERTS(pNode != NULL, "\nCan't find node which match current class: \"" + sClass
                     + "\"\n context: " + GetNodePath(&rNode) + "\n Variable: " + sVariableName + "\n");
        RISE_ASSERTS(pNode->NodeName() == sClass, "\nNode name does not match current class: \""
                     + pNode->NodeName() + "\" <=> \"" + sClass + "\"\n context: " + GetNodePath(&rNode) + "\n");

        while ((nPos = sVariable.find('.')) != std::string::npos)
        {
          const std::string& sSubClass = sVariable.substr(0, nPos);
          if (sSubClass[0] == '!' || sSubClass[0] == '$')
          {
            break;
          }
          try
          {
            if (sSubClass[0] == '*')
            {
              CXMLNode::TXMLNodeConstIterator itNode = pNode->FindSubnode(sSubClass.substr(1));
              if (itNode != pNode->NodeEnd())
              {
                pNode = &*itNode;
              }
              else
              {
                pNode = &tEmptyNode;
              }
            }
            else
            {
              pNode = &pNode->Subnode(sSubClass);
            }
          }
          catch(...)
          {
            rise::LogDebug() << "While parsing variable: [" << sVariableName << "]";
            throw;
          }

          sVariable.erase(0, nPos + 1);
        }
      } else
      {
        RISE_ASSERTS(pNode->NodeName() == sVariableName, "node name does not match current class: \""
                     + pNode->NodeName() + "\" <=> \"" + sVariableName + "\"");
        return rNode;
      }

      if (sVariable == "$Num")
      {
        static CXMLNode tNodeNum;
        const CXMLNode* pNodeParent = pNode->GetParent();
        RISE_ASSERTS(pNodeParent != NULL, "can't get number for node: " + pNode->NodeName());

        int nNum = 0;

        for (CXMLNode::TXMLNodeConstIterator itNode = pNodeParent->NodeBegin();
                  itNode != pNodeParent->NodeEnd(); ++itNode)
        {
          if (itNode->NodeType() == CXMLNode::ENTGENERIC)
          {
            if (&*itNode == pNode)
            {
              break;
            }
            ++nNum;
          }
        }

        tNodeNum.NodeContent() = nNum;
        return tNodeNum;
      }
      else
      if (sVariable == "$Count")
      {
        static CXMLNode tSubNodeCount;
        int nNum = 0;

        for (CXMLNode::TXMLNodeConstIterator itNode = pNode->NodeBegin();
          itNode != pNode->NodeEnd(); ++itNode)
        {
          if (itNode->NodeType() == CXMLNode::ENTGENERIC)
          {
            ++nNum;
          }
        }

        tSubNodeCount.NodeContent() = nNum;
        return tSubNodeCount;
      }
      else
      if (sVariable[0] == '!')
      { // exec function
        sVariable.erase(0, 1);
        pNode = &ExecuteFunction(sVariable, *pNode);
        while (!sVariable.empty()) // .!trimleft/:/.!dot
        {
          RISE_ASSERTS(sVariable.substr(0, 2) == ".!", "Junk [" + sVariable +  "] in variable: ["
              + sVariableName + "] at pos " + rise::ToStr(sVariableName.size() - sVariable.size()));
          sVariable.erase(0, 2);
          pNode = &ExecuteFunction(sVariable, *pNode);
        }
        return *pNode;
      }
      else
      if (sVariable[0] == '*') // optional node
      {
        CXMLNode::TXMLNodeConstIterator itNode = pNode->FindSubnode(sVariable.substr(1));
        if (itNode != pNode->NodeEnd())
        {
          return *itNode;
        }
        else
        {
          return tEmptyNode;
        }
      }

      return pNode->Subnode(sVariable);
    }

    std::string::size_type ParseParam(std::string& sParamBegin) const
    {
      std::string::size_type nPos = sParamBegin.find_first_of("/\\");
      // slash demasking
      for (;;)
      {
        RISE_ASSERTS(nPos != std::string::npos, "Can't get param");

        char chFound = sParamBegin[nPos];
        if (chFound == '\\')
        {
          // demask
          if (nPos < sParamBegin.size())
          {
            switch (sParamBegin[nPos + 1])
            {
            case '/': sParamBegin.replace(nPos, 2, 1, '/'); break;
            case 'r': sParamBegin.replace(nPos, 2, 1, '\r'); break;
            case 'n': sParamBegin.replace(nPos, 2, 1, '\n'); break;
            case 't': sParamBegin.replace(nPos, 2, 1, '\t'); break;
            case '\\': sParamBegin.erase(nPos, 1); break;
            }
          }
        }
        else
        { // '/'
          break;
        }

        nPos = sParamBegin.find_first_of("/\\", nPos + 1);
      }
      return nPos;
    }

    const CXMLNode& ExecuteFunction(std::string& sFunction, const CXMLNode& rNode) const
    {
      static rise::xml::CXMLNode tResult("Result");
      std::string& sResult = tResult.NodeContent().AsString();
      tResult.Clear();

      if (sFunction.substr(0, 9) == "mangledot")
      {
        sResult = rNode.NodeContent().AsString();
        rise::StrReplace(sResult, ".", "_", true);
        sFunction.erase(0, 9);
      }
      else
      if (sFunction.substr(0, 6) == "mangle")
      {
        sResult = rNode.NodeContent().AsString();
        if (sResult.size() >= 2 && sResult.substr(0, 2) == "::")
        {
          sResult.erase(0, 2);
        }

        if (sResult.size() >= 2 && sResult.substr(sResult.size() - 2, 2) == "::")
        {
          sResult.erase(sResult.size() - 2, 2);
        }

        rise::StrReplace(sResult, "::", "_", true);
        sFunction.erase(0, 6);
      }
      else
      if (sFunction.substr(0, 3) == "dot")
      {
        sResult = rNode.NodeContent().AsString();
        if (sResult.size() >= 2 && sResult.substr(0, 2) == "::")
        {
          sResult.erase(0, 2);
        }

        if (sResult.size() >= 2 && sResult.substr(sResult.size() - 2, 2) == "::")
        {
          sResult.erase(sResult.size() - 2, 2);
        }

        rise::StrReplace(sResult, "::", ".", true);
        sFunction.erase(0, 3);
      }
      else
      if (sFunction.substr(0, 6) == "match/")
      {
        sFunction.erase(0, 6);

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        sResult = (rNode.NodeContent().AsString().find(sWhat) != std::string::npos) ? "true" : "false";
      }
      else
      if (sFunction.substr(0, 8) == "replace/")
      {
        sFunction.erase(0, 8);
        sResult = rNode.NodeContent().AsString();

        // what replace
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        // replace with
        nPosEnd = ParseParam(sFunction);
        std::string sWith = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWith, rNode);

        rise::StrReplace(sResult, sWhat, sWith, true);
      }
      else
      if (sFunction.substr(0, 5) == "trim/")
      {
        sFunction.erase(0, 5);
        sResult = rNode.NodeContent().AsString();

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        rise::StrTrim(sResult, sWhat.c_str());
      }
      else
      if (sFunction.substr(0, 9) == "trimleft/")
      {
        sFunction.erase(0, 9);
        sResult = rNode.NodeContent().AsString();

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        rise::StrTrimLeft(sResult, sWhat.c_str());
      }
      else
      if (sFunction.substr(0, 10) == "trimright/")
      {
        sFunction.erase(0, 10);
        sResult = rNode.NodeContent().AsString();

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        rise::StrTrimRight(sResult, sWhat.c_str());
      }
      else
      if (sFunction.substr(0, 4) == "trim")
      {
        sResult = rNode.NodeContent().AsString();
        rise::StrTrim(sResult);
        sFunction.erase(0, 4);
      }
      else
      if (sFunction.substr(0, 8) == "trimleft")
      {
        sResult = rNode.NodeContent().AsString();
        rise::StrTrimLeft(sResult);
        sFunction.erase(0, 8);
      }
      else
      if (sFunction.substr(0, 7) == "tolower")
      {
        sResult = rNode.NodeContent().AsString();
        std::transform(sResult.begin(), sResult.end(), sResult.begin(), ::tolower);
        sFunction.erase(0, 7);
      }
      else
      if (sFunction.substr(0, 7) == "toupper")
      {
        sResult = rNode.NodeContent().AsString();
        std::transform(sResult.begin(), sResult.end(), sResult.begin(), ::toupper);
        sFunction.erase(0, 7);
      }
      else
      if (sFunction.substr(0, 9) == "trimright")
      {
        sResult = rNode.NodeContent().AsString();
        rise::StrTrimRight(sResult);
        sFunction.erase(0, 9);
      }
      else
      if (sFunction.substr(0, 7) == "append/")
      {
        sFunction.erase(0, 7);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        sResult = rNode.NodeContent().AsString() + sWhat;
      }
      else
      if (sFunction.substr(0, 8) == "prepend/")
      {
        sFunction.erase(0, 8);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        sResult = sWhat + rNode.NodeContent().AsString();
      }
      else
      if (sFunction.substr(0, 9) == "deprefix/")
      {
        sFunction.erase(0, 9);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        sResult = rNode.NodeContent().AsString();

        if (sResult.substr(0, sWhat.size()) == sWhat)
        {
          sResult.erase(0, sWhat.size());
        }
      }
      else
      if (sFunction.substr(0, 10) == "depostfix/")
      {
        sFunction.erase(0, 10);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rNode);

        sResult = rNode.NodeContent().AsString();
        std::string::size_type nResSize = sResult.size();
        std::string::size_type nWhatSize = sWhat.size();

        if (nResSize > nWhatSize &&
            sResult.substr(nResSize - nWhatSize) == sWhat)
        {
          sResult.erase(nResSize - nWhatSize);
        }
      }
      else
      if (sFunction.substr(0, 3) == "inc")
      {
        sResult = rNode.NodeContent().AsString();
        double dTmp = 0;
        rise::FromStr(sResult, dTmp);
        rise::ToStr(dTmp + 1, sResult);
        sFunction.erase(0, 3);
      }
      else
      if (sFunction.substr(0, 3) == "dec")
      {
        sResult = rNode.NodeContent().AsString();
        double dTmp = 0;
        rise::FromStr(sResult, dTmp);
        rise::ToStr(dTmp - 1, sResult);
        sFunction.erase(0, 3);
      }
      else
      if (sFunction.substr(0, 4) == "add/")
      {
        std::string::size_type nPosWhat = sFunction.find('/', 4);

        sResult = rNode.NodeContent().AsString();

        RISE_ASSERTS(nPosWhat != std::string::npos, "Can't get operand for add");

        const std::string& sWhat = sFunction.substr(4, nPosWhat - 4);
        double dOp1 = 0;
        double dOp2 = 0;
        rise::FromStr(sResult, dOp1);
        rise::FromStr(sWhat, dOp2);
        rise::ToStr(dOp1 + dOp2, sResult);
        sFunction.erase(0, nPosWhat + 1);
      }
      else
      if (sFunction.substr(0, 4) == "sub/")
      {
        std::string::size_type nPosWhat = sFunction.find('/', 4);

        sResult = rNode.NodeContent().AsString();

        RISE_ASSERTS(nPosWhat != std::string::npos, "Can't get operand for sub");
        const std::string& sWhat = sFunction.substr(4, nPosWhat - 4);
        double dOp1 = 0;
        double dOp2 = 0;
        rise::FromStr(sResult, dOp1);
        rise::FromStr(sWhat, dOp2);
        rise::ToStr(dOp1 - dOp2, sResult);
        sFunction.erase(0, nPosWhat + 1);
      }
      else
      {
        RISE_THROWS(rise::CLogicNoItemException, "function " + sFunction + " is undefined");
      }

      return tResult;
    }


    const std::string& GetValue(const std::string& sVariableName, const CXMLNode& rNode) const
    {
      return GetNode(sVariableName, rNode).NodeContent().AsString();
    }

    std::string::size_type ReplaceToValueFindBracketMatch(std::string& sString,
                                                          std::string::size_type nPosStart,
                                                          const CXMLNode& rNode) const
    {
      int nRecursion = 1;
      std::string::size_type nPosEnd = nPosStart;
      while ((nPosEnd = sString.find_first_of("()", nPosEnd)) != std::string::npos)
      {
        if (sString[nPosEnd] == ')')
        {
          --nRecursion;
          if (nRecursion == 0)
          {
            break;
          }
        }
        else
        { // == '('
          // check for inline $()
          if (sString[nPosEnd - 1] == '$')
          {
            std::string::size_type nInlineEnd = ReplaceToValueFindBracketMatch(sString, nPosEnd + 1, rNode);
            RISE_ASSERTS(nInlineEnd != std::string::npos, "end of inline variable name expected: [" + sString + "]");
            std::string sInline = sString.substr(nPosEnd - 1, nInlineEnd - nPosEnd + 2);
            ReplaceToValue(sInline, rNode);
            sString.replace(nPosEnd - 1, nInlineEnd - nPosEnd + 2, sInline);
          }
          else
          {
            ++nRecursion;
          }
        }
        ++nPosEnd;
      }
      return nPosEnd;
    }


    void ProcessValue(const std::string& sName, std::string& sValue, const CXMLNode& rNode) const
    {
      if (sName[0] == '$')
      {
        if (sName.substr(0, 13) == "$ThisNodeName")
        {
          sValue = rNode.NodeName();
          if (sName.size() == 13)
          {
            return;
          }
          rise::xml::CXMLNode tVarNode(sValue);
          tVarNode.NodeContent() = sValue;
          sValue = GetValue(sValue + sName.substr(13), tVarNode);
        }
        else
        if (sName.substr(0, 14) == "$ThisNodeValue")
        {
          sValue = rNode.NodeContent().AsString();
          if (sName.size() == 14)
          {
            return;
          }
          rise::xml::CXMLNode tVarNode(sValue);
          tVarNode.NodeContent() = sValue;
          sValue = GetValue(sValue + sName.substr(14), tVarNode);
        }
        else
        {
          std::string::size_type nPos = sName.find('.');
          if (nPos != std::string::npos) // variable + functions
          {
            const std::string& sVarName = sName.substr(1, nPos - 1);
            sValue = m_tmVariables.top()[sVarName];
            // process functions and other
            rise::xml::CXMLNode tVarNode(sVarName);
            tVarNode.NodeContent() = sValue;
            sValue = GetValue(sName.substr(1), tVarNode);
          }
          else // variable only
          {
            sValue = m_tmVariables.top()[sName.substr(1)];
          }
        }
      }
      else // node value
      {
        sValue = GetValue(sName, rNode);
      }
    }

    void ReplaceToValue(std::string& sString, const CXMLNode& rNode) const
    {
      std::string::size_type nPosStart = 0;
      std::string::size_type nPosEnd = 0;

      while((nPosStart = sString.find("$(", nPosEnd)) != std::string::npos)
      {
        nPosEnd = ReplaceToValueFindBracketMatch(sString, nPosStart + 2, rNode);

        RISE_ASSERTS(nPosEnd != std::string::npos, "end of variable name expected: [" + sString + "]");
        const std::string& sExpression = sString.substr(nPosStart + 2, nPosEnd - nPosStart - 2);
        std::string sValue;

        { // parse "node.name||$var.!func||some.other"
          std::string::size_type nNamePosBegin = 0;
          std::string::size_type nNamePosEnd = 0;

          // use first nonempty value
          for(;;)
          {
            nNamePosEnd = sExpression.find("||", nNamePosBegin);

            const std::string& sName = sExpression.substr(nNamePosBegin,
                           (nNamePosEnd != std::string::npos) ? nNamePosEnd - nNamePosBegin : nNamePosEnd);
            std::string::size_type nNameSize = sName.size();

            if (nNameSize > 0 && sName[0] == '"' && sName[nNameSize - 1] == '"')
            {
              sValue = sName.substr(1, nNameSize - 2);
              ReplaceToValue(sValue, rNode);
            }
            else
            {
              ProcessValue(sName, sValue, rNode);
            }

            if (!sValue.empty() || nNamePosEnd == std::string::npos)
            {
              break;
            }

            nNamePosBegin = nNamePosEnd + 2;
          }
        }

        sString.replace(nPosStart, nPosEnd - nPosStart + 1, sValue);
        nPosEnd = nPosStart + sValue.size();
      }
    }

    void Init(const std::string& sInDir)
    {
      std::list<std::string> tFileList;

      rise::CFileFind::Find(sInDir, tFileList, "codegen.config", rise::CFileFind::EFA_FILE);
      m_bHasConfig = !tFileList.empty();
      if (!m_bHasConfig)
      {
        rise::CFileFind::Find(sInDir, tFileList, "*.*", rise::CFileFind::EFA_FILE);
        for (std::list<std::string>::const_iterator it = tFileList.begin(); it != tFileList.end(); ++it)
        {
          if (it->find('$') != std::string::npos)
            m_tTemplateFileList.push_back(*it);
          else
            m_tConstFileList.push_back(*it);
        }
      }

      m_sInDir = sInDir;
    }

    void Start(const std::string& sOutDir, const CXMLNode& rRootNode, bool bUpdateOnly)
    {
      bool bNeedUpdate = false;

      m_sOutDir = sOutDir;

      if (m_bHasConfig)
      {
        const std::string& sIn = m_sInDir + "codegen.config";
        std::ostringstream fsOut;
        std::ifstream fsIn;

        fsIn.open(sIn.c_str());
        RISE_ASSERTS(fsIn.good(), "can't open input file: " + sIn);

        m_nIndent = 0;
        m_nLine = 0;
        Process(fsIn, fsOut, rRootNode);

        fsIn.close();
      }
      else
      {
        const CXMLNode& rNodeInterfaces = rRootNode.Subnode("Interfaces");

        for (CXMLNode::TXMLNodeConstIterator itNode = rNodeInterfaces.NodeBegin();
                  itNode != rNodeInterfaces.NodeEnd(); ++itNode)
        {
          if (itNode->NodeType() == CXMLNode::ENTGENERIC)
          {
            const CXMLNode& rNodeInterface = *itNode;

            for (std::list<std::string>::const_iterator itTemplateFile = m_tTemplateFileList.begin();
                itTemplateFile != m_tTemplateFileList.end(); ++itTemplateFile)
            {
              std::string sFile = *itTemplateFile;
              bool bProcessFile = false;
              try
              {
                ReplaceToValue(sFile, rNodeInterface);
                bProcessFile = true;
              }
              catch(rise::CLogicNoItemException& )
              {
                rise::LogDebug1() << "Skipping template file " << sFile
                    << " for interface " << rNodeInterface["NsName"].AsString();
                continue;
              }

              if (bProcessFile)
              {
                // erase input path
                std::string::size_type nPos = sFile.find_last_of(RISE_PATH_SEPARATOR);
                if (nPos != std::string::npos)
                {
                  sFile.erase(0, nPos + 1);
                }

                ProcessFile(m_sInDir + *itTemplateFile, 
                            sOutDir, sFile,
                            rNodeInterface, bUpdateOnly, bNeedUpdate);
              }
            }
          }
        } // for interface

        for (std::list<std::string>::const_iterator itTemplateFile = m_tConstFileList.begin();
            itTemplateFile != m_tConstFileList.end(); ++itTemplateFile)
        {
          std::string sFile = *itTemplateFile;

          // erase input path
          std::string::size_type nPos = sFile.find_last_of(RISE_PATH_SEPARATOR);
          if (nPos != std::string::npos)
          {
            sFile.erase(0, nPos + 1);
          }

          ProcessFile(m_sInDir + *itTemplateFile, sOutDir, sFile, rRootNode, bUpdateOnly, bNeedUpdate);
        }
      } // has config
    }

    void ProcessIfeq(std::istream& fsIn, std::ostream& fsOut, const CXMLNode& rNode, std::string& sLine, bool bNotEq = false)
    {
      bool bCurrentBlock = true;
      bool bEq = false;
      std::stringbuf sbData;
      std::string sLines;
      int nRecursion = 1;

      ReplaceToValue(sLine, rNode);

      { //#ifeq(123,321)
        int nOffsetPos = bNotEq ? 7 : 6;
        std::string::size_type nPosStart = sLine.find(",", 6);
        std::string::size_type nPosEnd = 0;

        RISE_ASSERTS(nPosStart != std::string::npos, "#ifeq expression is invalid!: \n----\n" + sLine + "\n----\n");
        nPosEnd = sLine.find(')', nPosStart);
        RISE_ASSERTS(nPosEnd != std::string::npos, "#ifeq expression is invalid!: \n----\n" + sLine + "\n----\n");

        std::string sLeft = sLine.substr(nOffsetPos, nPosStart - nOffsetPos);
        std::string sRight = sLine.substr(nPosStart + 1, nPosEnd - nPosStart - 1);

        bEq = false;

        std::string::size_type nPosStartLeft = 0;
        std::string::size_type nPosStartRight = 0;
        std::string::size_type nPosEndLeft = 0;
        std::string::size_type nPosEndRight = 0;
        do
        {
          nPosEndLeft = sLeft.find("||", nPosStartLeft);
          const std::string& sLeftCmp = sLeft.substr(nPosStartLeft, nPosEndLeft - nPosStartLeft);

          nPosStartRight = 0;
          nPosEndRight = 0;
          do
          {
            nPosEndRight = sRight.find("||", nPosStartRight);
            const std::string& sRightCmp = sRight.substr(nPosStartRight, nPosEndRight - nPosStartRight);

            if (sLeftCmp == sRightCmp)
            {
              bEq = true;
              break;
            }
            nPosStartRight = nPosEndRight + 2;
          } while (nPosEndRight != std::string::npos);
          nPosStartLeft = nPosEndLeft + 2;
        } while (nPosEndLeft != std::string::npos && !bEq);

        if (bNotEq)
        {
          bEq = !bEq;
        }
      }

      while (!fsIn.eof() && fsIn.good() && nRecursion > 0)
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        } else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str() + "\n";
          sbData.str("");
        }
        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 6) == "#ifeq(" || sLine.substr(0, 7) == "#ifneq(")
        {
          ++nRecursion;
          if (bCurrentBlock == bEq && nRecursion > 1)
            sLines += sLine;
        }
        else
        if (sLine.substr(0, 5) == "#else")
        {
          if (bCurrentBlock == bEq && nRecursion > 1)
            sLines += sLine;
          if (nRecursion == 1)
            bEq = !bEq;
        }
        else
        if (sLine.substr(0, 8) == "#ifeqend")
        {
          if (bCurrentBlock == bEq && nRecursion > 1)
            sLines += sLine;
          --nRecursion;
        }
        else
        if (bCurrentBlock == bEq)
        {
          sLines += sLine;
        }
      }

      RISE_ASSERTS(nRecursion == 0, "Unexpected EOF while parsing: \n---------\n" + sLines + "\n------------\n");

      {
        std::istringstream ssStream(sLines);
        Process(ssStream, fsOut, rNode);
      }
    }

    void ProcessForEach(std::istream& fsIn, std::ostream& fsOut, const CXMLNode& rNode, std::string& sLine)
    {
      std::stringbuf sbData;
      std::string sForEachExpr;
      std::string sLines;
      int nRecursion = 1;

      std::string::size_type nPosStart = sLine.find("$(", 9);
      std::string::size_type nPosEnd = 0;

      RISE_ASSERTS(nPosStart != std::string::npos, "foreach expression is invalid!");
      nPosEnd = sLine.find(')', nPosStart);
      RISE_ASSERTS(nPosEnd != std::string::npos, "foreach expression is invalid!");
      sForEachExpr = sLine.substr(nPosStart + 2, nPosEnd - nPosStart - 2);

      while (!fsIn.eof() && fsIn.good())
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        } else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str();
          if (fsIn.peek() == '\n')
          {
            sLine += "\n";
          }
          sbData.str("");
        }
        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 9) == "#foreach ")
          ++nRecursion;
        else
        if (sLine.substr(0, 4) == "#end")
        {
          --nRecursion;
          if (nRecursion == 0)
            break;
        }

          sLines += sLine;
      }

      RISE_ASSERTS(nRecursion == 0, "Unexpected EOF while parsing: \n---------\n" + sLines + "\n------------\n");

      const CXMLNode& rSubNode = GetNode(sForEachExpr, rNode);

      for (CXMLNode::TXMLNodeConstIterator itNode = rSubNode.NodeBegin();
        itNode != rSubNode.NodeEnd(); ++itNode)
      {
        if (itNode->NodeType() == CXMLNode::ENTGENERIC)
        {
          std::istringstream ssStream(sLines);
          Process(ssStream, fsOut, *itNode);
        }
      }
    }

    void ProcessContext(std::istream& fsIn, std::ostream& fsOut, const CXMLNode& rNode, std::string& sLine)
    {
      std::stringbuf sbData;
      std::string sContextExpr;
      std::string sLines;
      int nRecursion = 1;

      std::string::size_type nPosStart = sLine.find("$(", 9);
      std::string::size_type nPosEnd = 0;

      RISE_ASSERTS(nPosStart != std::string::npos, "context expression is invalid!");
      nPosEnd = sLine.find(')', nPosStart);
      RISE_ASSERTS(nPosEnd != std::string::npos, "context expression is invalid!");
      sContextExpr = sLine.substr(nPosStart, nPosEnd - nPosStart + 1);

      while (!fsIn.eof() && fsIn.good())
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        }
        else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str();
          if (fsIn.peek() == '\n')
          {
            sLine += "\n";
          }
          sbData.str("");
        }
        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 9) == "#context ")
          ++nRecursion;
        else
        if (sLine.substr(0, 11) == "#contextend")
        {
          --nRecursion;
          if (nRecursion == 0)
            break;
        }

        sLines += sLine;
      }

      RISE_ASSERTS(nRecursion == 0, "Unexpected EOF while parsing: \n---------\n" + sLines + "\n------------\n");

      if (sContextExpr[2] == '$') // variable
      {
        ReplaceToValue(sContextExpr, rNode);
      }
      else
      {
        sContextExpr.erase(0, 2);
        sContextExpr.erase(sContextExpr.size() - 1);
      }

      const CXMLNode& rSubNode = GetNode(sContextExpr, rNode);

      std::istringstream ssStream(sLines);
      Process(ssStream, fsOut, rSubNode);
    }

    void ProcessInclude(std::ostream& fsOut, const CXMLNode& rNode, const std::string& sLine)
    {
      std::string sIncludeFileName;

      char chQuote = *sLine.begin();
      std::string::size_type nPos = 0;

      if (chQuote == '<')
      {
        nPos = sLine.find('>', 1);
        RISE_ASSERTS(nPos != std::string::npos, "cginclude expression is invalid!");
        sIncludeFileName = m_sInDir + "../" + sLine.substr(1, nPos - 1);
      }
      else
      if (chQuote == '"')
      {
        nPos = sLine.find('"', 1);
        RISE_ASSERTS(nPos != std::string::npos, "cginclude expression is invalid!");
        sIncludeFileName = m_sInDir + sLine.substr(1, nPos - 1);
      }
      else
      {
        RISE_THROWS(rise::CInternalAssertException, "cginclude expression is invalid!");
      }

#ifdef WIN32
      rise::StrReplace(sIncludeFileName, "/", "\\", true);
#endif

      std::ifstream fsIncFile;
      fsIncFile.open(sIncludeFileName.c_str());

      RISE_ASSERTS(fsIncFile.good(), "can't include file: " + sIncludeFileName);

      std::string sCurrInDir = m_sInDir;
      m_sInDir = sIncludeFileName.substr(0, sIncludeFileName.find_last_of("/\\") + 1);
      while (!fsIncFile.eof() && fsIncFile.good())
      {
        Process(fsIncFile, fsOut, rNode);
      }
      m_sInDir = sCurrInDir;

      fsIncFile.close();
    }

    void ProcessIndent(const std::string& sLine)
    {
      std::string sValue = sLine.substr(8);
      rise::StrTrim(sValue);
      if (sValue == "+")
      {
        RISE_ASSERTS(m_nIndent < 1024, "Invalid indentation: " + rise::ToStr(m_nIndent + 1)
                     + " while processing line: \n" + sLine);
        ++m_nIndent;
      }
      else
      if (sValue == "-")
      {
        RISE_ASSERTS(m_nIndent > 0, "Invalid indentation: " + rise::ToStr(m_nIndent - 1)
                     + " while processing line: \n" + sLine);
        --m_nIndent;
      }
      else
      {
        int nSign = 0;
        int nIndent = m_nIndent;
        if (sValue[0] == '+')
        {
          sValue.erase(0, 1);
          nSign = 1;
        }
        else
        if (sValue[0] == '-')
        {
          sValue.erase(0, 1);
          nSign = -1;
        }

        rise::FromStr(sValue, nIndent);
        if (nSign != 0)
        {
          nIndent = m_nIndent + nSign * nIndent;
        }
        RISE_ASSERTS(nIndent < 1024 && nIndent >= 0, "Invalid indentation: " + rise::ToStr(nIndent)
                     + " while processing line: \n" + sLine);
        m_nIndent = nIndent;
      }
    }

    void Process(std::istream& fsIn, std::ostream& fsOut, const CXMLNode& rNode)
    {
      std::string sLine;
      std::stringbuf sbData;

      while (!fsIn.eof() && fsIn.good())
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        }
        else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str();
          if (sLine.size() > 0 && *sLine.rbegin() == '\\')
            sLine.erase(sLine.size() - 1);
          else
          {
            if (fsIn.peek() == '\n')
            {
              sLine += "\n";
            }
          }
          sbData.str("");
        }

        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 5) == "#var ")
        {
          std::string::size_type nPos = sLine.find_first_of(" \t", 5);
          std::string sVariable;
          std::string sValue;

          if (nPos == std::string::npos)
          {
            sVariable = sLine.substr(5);
            rise::StrTrimRight(sVariable);
          }
          else
          {
            sVariable = sLine.substr(5, nPos - 5);
            sValue = sLine.substr(nPos + 1, sLine.size() - nPos - 2);
            ReplaceToValue(sValue, rNode);
          }

          RISE_ASSERTS(!sVariable.empty(), "invalid var declaration: " + sLine);

          m_tmVariables.top()[sVariable] = sValue;
        }
        else
        if (sLine.substr(0, 6) == "#ifeq(")
        {
          ProcessIfeq(fsIn, fsOut, rNode, sLine);
        }
        else
        if (sLine.substr(0, 7) == "#ifneq(")
        {
          ProcessIfeq(fsIn, fsOut, rNode, sLine, true);
        }
        else
        if (sLine.substr(0, 9) == "#foreach ")
        {
          ProcessForEach(fsIn, fsOut, rNode, sLine);
        }
        else
        if (sLine.substr(0, 9) == "#context ")
        {
          ProcessContext(fsIn, fsOut, rNode, sLine);
        }
        else
        if (sLine.substr(0, 10) == "#fileopen ")
        {
          std::string sFileName = sLine.substr(10);
          ReplaceToValue(sFileName, rNode);
          rise::StrTrim(sFileName);

          RISE_ASSERTS(!sFileName.empty(), "#fileopen: Filename is empty");

          sFileName = m_sOutDir + sFileName;

          std::ofstream ofsFile(sFileName.c_str());
          RISE_ASSERTS(ofsFile.good(), "can't open output file: " + sFileName);

          std::cout << "Generating " << sFileName << std::endl;
          m_nIndent = 0;
          Process(fsIn, ofsFile, rNode);
          ofsFile.close();
        }
        else
        if (sLine.substr(0, 10) == "#fileclose")
        {
          return;
        }
        else
        if (sLine.substr(0, 7) == "#mkdir ")
        {
          std::string sDirName = sLine.substr(7);
          ReplaceToValue(sDirName, rNode);
          rise::StrTrim(sDirName);

          sDirName = m_sOutDir + sDirName;

          for (std::string::size_type nPos = 0; nPos != std::string::npos;)
          {
            nPos = sDirName.find('/', nPos + 1);
            const std::string& sCurrDir = sDirName.substr(0, nPos);
            if (!sCurrDir.empty())
            {
              int nRes =
#ifndef WIN32
              mkdir(sCurrDir.c_str(), 0755);
#else
              _mkdir(sCurrDir.c_str());
#endif
              RISE_ASSERTS(nRes != -1 || errno == EEXIST, "Failed to create dir ["
                           + sCurrDir + "]: " + strerror(errno));
            }
          }
        }
        else
        if (sLine.substr(0, 11) == "#cginclude ")
        {
          sLine.erase(0, 11);
          rise::StrTrim(sLine);
          ProcessInclude(fsOut, rNode, sLine);
        }
        else
        if (sLine.substr(0, 11) == "#cgwarning ")
        {
          ReplaceToValue(sLine, rNode);
          rise::StrTrimRight(sLine, "\n\r");
          std::cerr << "Warning: " << sLine.substr(11) << std::endl;
        }
        else
        if (sLine.substr(0, 9) == "#cgerror ")
        {
          ReplaceToValue(sLine, rNode);
          RISE_THROWS(rise::CInternalAssertException, sLine.substr(9));
        }
        else
        if (sLine.substr(0, 8) == "#indent ")
        {
          ProcessIndent(sLine);
        }
        else
        if (sLine.substr(0, 11) == "#cgpushvars")
        {
          m_tmVariables.push(m_tmVariables.top());
        }
        else
        if (sLine.substr(0, 10) == "#cgpopvars")
        {
          m_tmVariables.pop();
        }
        else
        {
          if (!sLine.empty())
          {
            std::string sIndent;
            RISE_ASSERTS(m_nIndent < 1024, "Invalid indentation: " + rise::ToStr(m_nIndent));
            for (int nIndent = 0; nIndent < m_nIndent; ++nIndent)
            {
              sIndent += "  ";
            }
            if (m_bNeedIndent && sLine[0] != '\n')
            {
              sLine = sIndent + sLine;
              m_bNeedIndent = false;
            }

            std::string::size_type nBegin = 0;
            std::string::size_type nEnd = 0;
            while ((nEnd = sLine.find('\n', nBegin)) != std::string::npos)
            {
              if (m_bNeedIndent && nEnd > nBegin) // line is not empty
              {
                sLine.insert(nBegin, sIndent);
                nEnd += sIndent.size();
              }

              nBegin = nEnd + 1;

              m_bNeedIndent = true;
            }

            ReplaceToValue(sLine, rNode);

            fsOut << sLine;
          }
        }
        ++m_nLine;
      }
    }

    void ProcessFile(const std::string& sIn, const std::string& sOutDir, const std::string& sOutFile,
                     const CXMLNode& rNodeInterface, bool bUpdateOnly, bool& bNeedUpdate)
    {
      const std::string& sOut = sOutDir + sOutFile;
      if (bUpdateOnly)
      {
        bool bIsStaticTemplate = rNodeInterface.NodeName() == "Project";
        struct stat stIn;
        struct stat stOut;

        stat(sIn.c_str(), &stIn);
        int nResOut = stat(sOut.c_str(), &stOut);

        if (bIsStaticTemplate)
        {
          if (!bNeedUpdate && nResOut == 0 && stOut.st_mtime > stIn.st_mtime)
          {
            std::cout << "Skipping " << sOut << std::endl;
            return;
          }
        }
        else
        {
          if (nResOut == 0 && stOut.st_mtime > stIn.st_mtime)
          {
            struct stat stInterface;

            nResOut = stat((sOutDir + rNodeInterface["FileName"].AsString()).c_str(), &stInterface);
            if (nResOut == 0 && stOut.st_mtime > stInterface.st_mtime)
            {
              std::cout << "Skipping " << sOut << std::endl;
              return;
            }
            else
            {
              bNeedUpdate = true;
            }
          }
        }
      }

      {
        std::ifstream fsIn;
        std::ofstream fsOut;

        fsIn.open(sIn.c_str());
        RISE_ASSERTS(fsIn.good(), "can't open input file: " + sIn);

        fsOut.open(sOut.c_str());
        if (!fsOut.good())
        {
          fsIn.close();
          RISE_THROWS(rise::CInternalAssertException, "can't open output file: " + sOut);
        }

        std::cout << "Generating " << sOut << std::endl;

        m_nLine = 0;
        m_nIndent = 0;
        m_bNeedIndent = true;
        Process(fsIn, fsOut, rNodeInterface);

        fsIn.close();
        fsOut.close();
      }
    }

    void SetEnv(const StringMap& rmEnv)
    {
      m_tmVariables.top() = rmEnv;
    }

  private:
    std::list<std::string> m_tTemplateFileList;
    std::list<std::string> m_tConstFileList;
    mutable std::stack<StringMap> m_tmVariables;
    std::string m_sInDir;
    std::string m_sOutDir;
    int m_nLine;
    int m_nIndent;
    bool m_bNeedIndent;
    bool m_bHasConfig;
  };


  void CodeGen::Start( const std::string& sTemplateDir, const std::string& sOutDir, const rise::xml::CXMLNode& rRootNode, bool bUpdateOnly, const StringMap& rmEnv )
  {
    TemplateParser tTemplateParser;

    tTemplateParser.Init(sTemplateDir);
    tTemplateParser.SetEnv(rmEnv);
    tTemplateParser.Start(sOutDir, rRootNode, bUpdateOnly);
  }
}
}
