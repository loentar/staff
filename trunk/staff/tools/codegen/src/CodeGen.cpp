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
#include <rise/common/exmacros.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/xml/XMLNode.h>
#include <rise/tools/FileFind.h>
#include "CodeGen.h"

namespace staff
{
  using rise::xml::CXMLNode;

  class CTemplateParser
  {
  public:
    const CXMLNode& GetNode(const std::string& sVariableName, const CXMLNode& rNode) const
    {
      const CXMLNode* pNode = &rNode;
      std::string::size_type nPos = sVariableName.find('.');
      std::string sVariable;

      if(nPos != std::string::npos)
      {
        std::string sClass = sVariableName.substr(0, nPos);
        sVariable = sVariableName.substr(nPos + 1);

        while (pNode != NULL && pNode->NodeName() != sClass)
          pNode = pNode->GetParent();

        RISE_ASSERTS(pNode != NULL, "Can't find node which match current class: \"" + sClass + "\", context: " + rNode.NodeName() + ", Variable: " + sVariableName);
        RISE_ASSERTS(pNode->NodeName() == sClass, "node name does not match current class: \"" + pNode->NodeName() + "\" <=> \"" + sClass + "\", current context: " + rNode.NodeName());

        while ((nPos = sVariable.find('.')) != std::string::npos)
        {
          std::string sSubClass = sVariable.substr(0, nPos);
          pNode = &pNode->Subnode(sSubClass);
          sVariable.erase(0, nPos + 1);
        }
      } else
        sVariable = sVariableName;

      if (sVariable == "$Num")
      {
        static CXMLNode tNodeCount;
        const CXMLNode* pNodeParent = pNode->GetParent();
        RISE_ASSERTS(pNodeParent != NULL, "can't get number for node: " + pNode->NodeName());

        int nNum = 0;

        for (CXMLNode::TXMLNodeConstIterator itNode = pNodeParent->NodeBegin();
                  itNode != pNodeParent->NodeEnd(); ++itNode)
          if(itNode->NodeType() == CXMLNode::ENTGENERIC)
          {
            if(&*itNode == pNode)
              break;
            ++nNum;
          }

        tNodeCount.NodeContent() = nNum;
        return tNodeCount;
      }

      if (sVariable == "$Count")
      {
        static CXMLNode tSubNodeCount;
        int nNum = 0;

        for (CXMLNode::TXMLNodeConstIterator itNode = pNode->NodeBegin();
          itNode != pNode->NodeEnd(); ++itNode)
        {
          if(itNode->NodeType() == CXMLNode::ENTGENERIC)
          {
            ++nNum;
          }
        }

        tSubNodeCount.NodeContent() = nNum;
        return tSubNodeCount;
      }

      return pNode->Subnode(sVariable);
    }

    const std::string& GetValue(const std::string& sVariableName, const CXMLNode& rNode) const
    {
      return GetNode(sVariableName, rNode).NodeContent().AsString();
    }

    void ReplaceToValue(std::string& sString, const CXMLNode& rNode) const
    {
      std::string::size_type nPosStart = 0;
      std::string::size_type nPosEnd = 0;

      while((nPosStart = sString.find("$(", nPosEnd)) != std::string::npos)
      {
        nPosEnd = sString.find(')', nPosStart);
        RISE_ASSERTS(nPosEnd != std::string::npos, "end of variable name expected");
        const std::string& sName = sString.substr(nPosStart + 2, nPosEnd - nPosStart - 2);
        std::string sValue;
        if (sName[0] == '$')
          sValue = m_tVariables[sName.substr(1)];
        else
          sValue = GetValue(sName, rNode);
        sString.replace(nPosStart, nPosEnd - nPosStart + 1, sValue);
        nPosEnd = nPosStart + sValue.size();
      }
    }

    void Init(const std::string& sInDir)
    {
      std::list<std::string> tFileList;
      rise::CFileFind::Find(sInDir, tFileList, "*.*", rise::CFileFind::EFA_FILE);
      for (std::list<std::string>::const_iterator it = tFileList.begin(); it != tFileList.end(); ++it)
      {
        if(it->find('$') != std::string::npos)
          m_tTemplateFileList.push_back(*it);
        else
          m_tConstFileList.push_back(*it);
      }

      m_sInDir = sInDir;
    }

    void Start(const std::string& sOutDir, const CXMLNode& rRootNode, bool bUpdateOnly)
    {
      bool bNeedUpdate = false;
      const CXMLNode& rNodeInterfaces = rRootNode.Subnode("Interfaces");

      for (CXMLNode::TXMLNodeConstIterator itNode = rNodeInterfaces.NodeBegin();
                itNode != rNodeInterfaces.NodeEnd(); ++itNode)
      {
        if(itNode->NodeType() == CXMLNode::ENTGENERIC)
        {
          const CXMLNode& rNodeInterface = *itNode;

          for (std::list<std::string>::const_iterator itTemplateFile = m_tTemplateFileList.begin();
              itTemplateFile != m_tTemplateFileList.end(); ++itTemplateFile)
          {
            std::string sFile = *itTemplateFile;
            try
            {
              ReplaceToValue(sFile, rNodeInterface);
            }
            catch(rise::CLogicNoItemException& )
            {
              rise::LogDebug() << "Skipping template file " << sFile;
              continue;
            }

            ProcessFile(m_sInDir + '/' + *itTemplateFile, sOutDir + '/' + sFile,
                        rNodeInterface, bUpdateOnly, bNeedUpdate);
          }
        }
      }

      for (std::list<std::string>::const_iterator itTemplateFile = m_tConstFileList.begin();
          itTemplateFile != m_tConstFileList.end(); ++itTemplateFile)
      {
        ProcessFile(m_sInDir + '/' + *itTemplateFile, sOutDir + '/' + *itTemplateFile,
                    rRootNode, bUpdateOnly, bNeedUpdate);
      }
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

        RISE_ASSERTS(nPosStart != std::string::npos, "#ifeq expression is invalid!");
        nPosEnd = sLine.find(')', nPosStart);
        RISE_ASSERTS(nPosEnd != std::string::npos, "#ifeq expression is invalid!");

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

            if(sLeftCmp == sRightCmp)
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
        } else
        if (bCurrentBlock == bEq)
          sLines += sLine;
      }

      RISE_ASSERTS(nRecursion == 0, "Unexpected EOF");

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
          sLine = sbData.str() + "\n";
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

      RISE_ASSERTS(nRecursion == 0, "Unexpected EOF");

      const CXMLNode& rSubNode = GetNode(sForEachExpr, rNode);

      for (CXMLNode::TXMLNodeConstIterator itNode = rSubNode.NodeBegin();
        itNode != rSubNode.NodeEnd(); ++itNode)
        if(itNode->NodeType() == CXMLNode::ENTGENERIC)
        {
          std::istringstream ssStream(sLines);
          Process(ssStream, fsOut, *itNode);
        }
    }

    void ProcessInclude(std::istream& fsIn, std::ostream& fsOut, const CXMLNode& rNode, std::string& sLine)
    {
      std::string sIncludeFileName;

      std::string::size_type nPosStart = sLine.find('<', 9);
      std::string::size_type nPosEnd = 0;

      RISE_ASSERTS(nPosStart != std::string::npos, "cginclude expression is invalid!");
      nPosEnd = sLine.find('>', nPosStart);
      RISE_ASSERTS(nPosEnd != std::string::npos, "cginclude expression is invalid!");
      sIncludeFileName = m_sInDir + "/../" + sLine.substr(nPosStart + 1, nPosEnd - nPosStart - 1);

      std::ifstream fsIncFile;
      fsIncFile.open(sIncludeFileName.c_str());

      if (!fsIncFile.good())
        throw std::string("can't include file: " + sIncludeFileName);

      while (!fsIncFile.eof() && fsIncFile.good())
      {
        Process(fsIncFile, fsOut, rNode);
      }

      fsIncFile.close();
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
        } else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str();
          if (sLine.size() > 0 && *sLine.rbegin() == '\\')
            sLine.erase(sLine.size() - 1);
          else
            sLine += "\n";
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
          }
          else
          {
            sVariable = sLine.substr(5, nPos - 5);
            sValue = sLine.substr(nPos + 1, sLine.size() - nPos - 2);
          }

          if (sVariable.size() == 0)
          {
            throw "invalid var declaration: " + sLine;
          }

          ReplaceToValue(sValue, rNode);
          m_tVariables[sVariable] = sValue;
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
        } else
        if (sLine.substr(0, 11) == "#cginclude ")
        {
          ProcessInclude(fsIn, fsOut, rNode, sLine);
        } else
        if (sLine.substr(0, 11) == "#cgwarning ")
        {
          ReplaceToValue(sLine, rNode);
          std::cerr << "Warning: " << sLine.substr(11) << std::endl;
        }
        else
        if (sLine.substr(0, 9) == "#cgerror ")
        {
          ReplaceToValue(sLine, rNode);
          throw sLine.substr(9);
        }
        else
        {
          ReplaceToValue(sLine, rNode);
          fsOut << sLine;
        }
        ++m_nLine;
      }
    }

    void ProcessFile(const std::string& sIn, const std::string& sOut, const CXMLNode& rNodeInterface, bool bUpdateOnly, bool& bNeedUpdate)
    {
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

            stat(rNodeInterface["FileName"].AsString().c_str(), &stInterface);
            if (stOut.st_mtime > stInterface.st_mtime)
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
        if (!fsIn.good())
          throw std::string("can't open input file: " + sIn);

        fsOut.open(sOut.c_str());
        if (!fsOut.good())
        {
          fsIn.close();
          throw std::string("can't open output file: " + sOut);
        }

        std::cout << "Generating " << sOut << std::endl;

        m_nLine = 0;
        Process(fsIn, fsOut, rNodeInterface);

        fsIn.close();
        fsOut.close();
      }
    }

    void SetEnv(const TStringMap& rmEnv)
    {
      m_tVariables = rmEnv;
    }

  private:
    std::list<std::string> m_tTemplateFileList;
    std::list<std::string> m_tConstFileList;
    mutable std::map<std::string, std::string> m_tVariables;
    std::string m_sInDir;
    int m_nLine;
  };


  void CCodeGen::Start( const std::string& sTemplateDir, const std::string& sOutDir, const rise::xml::CXMLNode& rRootNode, bool bUpdateOnly, const TStringMap& rmEnv )
  {
    CTemplateParser tTemplateParser;

    tTemplateParser.Init(sTemplateDir);
    tTemplateParser.SetEnv(rmEnv);
    tTemplateParser.Start(sOutDir, rRootNode, bUpdateOnly);
  }
}
