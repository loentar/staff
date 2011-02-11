/*
 *  Copyright 2011 Utkin Dmitry
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

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <rise/common/Log.h>
#include <rise/common/exmacros.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/plugin/PluginExport.h>
#include "ProtobufParser.h"

RISE_DECLARE_PLUGIN(staff::CProtobufParser);

namespace staff
{
  class CProtobufHeaderParser
  {
  public:
    CProtobufHeaderParser():
      m_sCurrentNamespace("::"), m_nLine(1), m_pProject(NULL)
    {
    }

    void SkipWs()
    {
      char chData = 0;
      while (m_tFile.good() && !m_tFile.eof())
      {
        chData = m_tFile.peek();
        if (chData == '\n')
        {
          ++m_nLine;
        }

        if(chData != ' ' && chData != '\n' && chData != '\r' && chData != '\t')
        {
          if (chData == '/') // comment
          {
            m_tFile.ignore();
            if (m_tFile.peek() == '/') // single line comment
            {
              m_tFile.ignore(INT_MAX, '\n');
              ++m_nLine;
              continue;
            }
            else
            if (m_tFile.peek() == '*') // multiline comment
            {
              m_tFile.ignore();
              while (m_tFile.good() && !m_tFile.eof())
              {
                m_tFile.get(chData);
                if (chData == '*')
                {
                  m_tFile.get(chData);
                  if (chData == '/')
                  {
                    break;
                  }
                }

                if (chData == '\n')
                {
                  ++m_nLine;
                }
              }
            }
            else
            {
              m_tFile.unget();
              break;
            }
          }
          else
          {
            break;
          }
        }
        m_tFile.ignore();
      }
    }

    void SkipWsOnly()
    {
      char chData = 0;
      while (m_tFile.good() && !m_tFile.eof())
      {
        chData = m_tFile.peek();
        if (chData == '\n')
        {
          ++m_nLine;
        }

        if(chData != ' ' && chData != '\n' && chData != '\r' && chData != '\t')
        {
          break;
        }
        m_tFile.ignore();
      }
    }

    void SkipSingleLineComment()
    {
      char chData = '\0';
      while (m_tFile.good() && !m_tFile.eof())
      {
        chData = m_tFile.peek();
        if(chData != ' ' && chData != '\t')
        {
          break;
        }
        m_tFile.ignore();
      }

      if (m_tFile.good() && !m_tFile.eof())
      {
        if (m_tFile.peek() == '/') // comment
        {
          m_tFile.ignore();
          if (m_tFile.peek() == '/') // single line comment
          {
            m_tFile.ignore(INT_MAX, '\n');
            ++m_nLine;
          } else
          {
            m_tFile.unget();
          }
        }
      }
    }

    void ReadStr(std::string& sString, bool bSkipWS = true)
    {
      sString.erase();
      if (bSkipWS)
      {
        SkipWs();
      }
      m_tFile >> sString;
    }

    void ReadBefore(std::string& sOut, const std::string& sDelim = " \r\n\t,();[]{}<>\\/*-+!@#$%^&*=")
    {
      char chData = 0;

      sOut.erase();

      while (m_tFile.good() && !m_tFile.eof())
      {
        chData = m_tFile.peek();
        if (sDelim.find(chData) != std::string::npos)
        {
          break;
        }

        if (chData == '\n')
        {
          ++m_nLine;
        }

        sOut += chData;
        m_tFile.ignore();
      }
    }

    bool ReadComment(std::string& sComment)
    {
      if (m_tFile.peek() != '/')
      {
        return false;
      }

      sComment.erase();

      m_tFile.ignore();
      if (m_tFile.peek() == '/')
      {
        m_tFile.ignore();
        ReadBefore(sComment, "\n\r");
      }
      else
      if (m_tFile.peek() == '*')
      {
        char chData = '\0';
        std::string sTmp;

        m_tFile.ignore();
        while (m_tFile.good() && !m_tFile.eof())
        {
          ReadBefore(sTmp, "*");

          sComment += sTmp;
          m_tFile.get(chData);
          if (chData == '*')
          {
            if (m_tFile.peek() == '/')
            {
              m_tFile.ignore();
              break;
            }
            sComment += chData;
          }

          if (chData == '\n')
          {
            ++m_nLine;
          }
        }
      }
      else
      {
        m_tFile.unget();
        return false;
      }

      rise::StrTrim(sComment);

      return true;
    }

    std::string::size_type StrIntersect(const std::string& sString1, const std::string& sString2)
    {
      std::string::size_type nPosA = sString1.size() - 1;
      std::string::size_type nPosB = sString2.size() - 1;
      std::string::size_type nPosA1 = nPosA;
      std::string::size_type nPosB1 = nPosB;
      const char* szStr1 = sString1.c_str();
      const char* szStr2 = sString2.c_str();

      for (; nPosB; --nPosB)
      {
        if (szStr1[nPosA] == szStr2[nPosB])
        {
          nPosA1 = nPosA - 1;
          nPosB1 = nPosB - 1;
          for(; nPosA1 && nPosB1 && szStr1[nPosA1] == szStr2[nPosB1]; --nPosA1, --nPosB1);
          if (!nPosB1)
          {
            return nPosB + 1;
          }
        }
      }
      return std::string::npos;
    }

    template<typename TStructType>
    bool ParseCompositeDataType(const std::list<TStructType>& rList, SDataType& rDataType)
    {
      for (typename std::list<TStructType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
      {
        if (it->sName == rDataType.sName)
        {
          // namespace match
          if (it->sNamespace == rDataType.sNamespace)
          {
            return true;
          }

          std::string::size_type nPos = m_sCurrentNamespace.find_last_of("::");

          while (nPos != std::string::npos)
          {
            ++nPos;
            if((m_sCurrentNamespace.substr(0, nPos) + rDataType.sNamespace) == it->sNamespace)
            {
              if (it->sNamespace != rDataType.sNamespace) // correct namespace
              {
                rDataType.sNamespace = it->sNamespace;
              }

              return true;
            }

            if (nPos < 4)
            {
              break;
            }

            nPos = m_sCurrentNamespace.find_last_of("::", nPos - 3);
          }
        }
      }

      return false;
    }

    // [[some::]namespace::][[Struct::]SubStruct::]SubSubstruct
    const SStruct* GetStruct(const std::string& sNsName, const SStruct* pstParent = NULL)
    {
      const SStruct* pstCurr = pstParent;
      const SStruct* pstResult = NULL;

      std::string::size_type nNsNameSize = sNsName.size();

      // look substructs
      do
      {
        const std::list<SStruct>& rStructList = !pstCurr ? m_stInterface.lsStruct : pstCurr->lsStruct;
        for (std::list<SStruct>::const_iterator itStruct = rStructList.begin();
          itStruct != rStructList.end(); ++itStruct)
        {
          std::string sCurrNsName = itStruct->sNamespace;
          if (!itStruct->sOwnerName.empty())
          {
            sCurrNsName += itStruct->sOwnerName + "::";
          }
          sCurrNsName += itStruct->sName;

          std::string::size_type nCurrNsNameSize = sCurrNsName.size();

          int nSizeDiff = nCurrNsNameSize - nNsNameSize;

          //  full struct name with namespace
          if (!nSizeDiff && sCurrNsName == sNsName)
          {
            pstResult = &*itStruct;
            break; // return
          }
          else
          {
            // empty/partialy namespace
            if (nSizeDiff >= 2) // size of "::"
            {
              if (sCurrNsName.substr(nSizeDiff - 2, 2) == "::" &&
                  sCurrNsName.substr(nSizeDiff) == sNsName)
              {
                pstResult = &*itStruct;
                break; // return
              }
            }

            // includes substruct name
            // find intersection
            // some::namespace::Struct X namespace::Struct::SubStruct = namespace::Struct
            std::string::size_type nPos = StrIntersect(sCurrNsName, sNsName);
            if (nPos != std::string::npos
                && (nPos == nCurrNsNameSize || sCurrNsName.substr(nCurrNsNameSize - nPos - 2, 2) == "::")
                && sNsName.substr(nPos, 2) == "::")
            {
              // go through child structs
              nPos += 2;
              const SStruct* pstTmp = &*itStruct;
              std::string::size_type nBegin = nPos;
              std::string::size_type nEnd = 0;
              do
              {
                nEnd = sNsName.find("::", nBegin);
                const std::string& sSubName =
                    nEnd != std::string::npos ?
                    sNsName.substr(nBegin, nEnd - nBegin) :
                    sNsName.substr(nBegin);
                bool bFound = false;
                for (std::list<SStruct>::const_iterator itSubStruct = pstTmp->lsStruct.begin();
                  itSubStruct != pstTmp->lsStruct.end(); ++itSubStruct)
                {
                  if (itSubStruct->sName == sSubName)
                  {
                    pstTmp = &*itSubStruct;
                    bFound = true;
                    break;
                  }
                }
                if (!bFound)
                {
                  pstTmp = NULL;
                  break;
                }
                nBegin = nEnd + 2;
              }
              while (nEnd != std::string::npos);

              if (pstTmp)
              {
                pstResult = pstTmp;
                break;
              }
            }
          }
        }

        // find in parent owner struct
        if (!pstResult && pstCurr)
        {
          pstCurr = GetStruct(pstCurr->sNamespace + pstCurr->sOwnerName);
          continue; // even if pstCurr == null we must check interface structs
        }
      }
      while (!pstResult && pstCurr);

      return pstResult;
    }

    void GetDataType(const std::string& sDataTypeName, SDataType& rDataType)
    {
      rDataType.sUsedName = sDataTypeName;
      std::string::size_type nPos = sDataTypeName.find_last_of("::");
      if (nPos != std::string::npos)
      {
        ++nPos;
        rDataType.sName = sDataTypeName.substr(nPos);
        rDataType.sNamespace = sDataTypeName.substr(0, nPos);
      }
      else
      {
        rDataType.sName = sDataTypeName;
        rDataType.sNamespace = ""; //"::";
      }

      if (
          sDataTypeName == "double" ||
          sDataTypeName == "float" ||
          sDataTypeName == "bool"
          )
      {
        rDataType.eType = SDataType::EGeneric;
      }
      else
      if (
          sDataTypeName == "int32" ||
          sDataTypeName == "uint32" ||
          sDataTypeName == "sint32" ||
          sDataTypeName == "fixed32" ||
          sDataTypeName == "sfixed32"
          )
      {
        rDataType.eType = SDataType::EGeneric;
        rDataType.sName = "int";
        rDataType.sUsedName = rDataType.sName;
      }
      else
      if (
          sDataTypeName == "int64" ||
          sDataTypeName == "uint64" ||
          sDataTypeName == "sint64" ||
          sDataTypeName == "fixed64" ||
          sDataTypeName == "sfixed64"
          )
      {
        rDataType.eType = SDataType::EGeneric;
        rDataType.sName = "long";
        rDataType.sUsedName = rDataType.sName;
      }
      else
      if (
          sDataTypeName == "string" ||
          sDataTypeName == "bytes"
          )
      {
        rDataType.eType = SDataType::EString;
        rDataType.sName = "string";
        rDataType.sNamespace = "std::";
        rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
      }
      else
      if(ParseCompositeDataType(m_stInterface.lsStruct, rDataType))
      {
        rDataType.eType = SDataType::EStruct;
      }
      else
      {
        rDataType.eType = SDataType::EUnknown;
      }
    }

    void IgnoreFunction()
    {
      char chTmp = '\0';
      int nRecursion = 0;

      while (m_tFile.good() && !m_tFile.eof())
      {
        SkipWs();
        m_tFile >> chTmp;

        if (chTmp == ';' && nRecursion == 0)
        {
          SkipSingleLineComment();
          break;
        }
        else
        if (chTmp == '}')
        {
          --nRecursion;
          CSP_ASSERT(nRecursion >= 0, "mismatch {}", m_stInterface.sFileName, m_nLine);

          if (nRecursion == 0)
          {
            while (!m_tFile.eof() && m_tFile.peek() == ';')
            {
              m_tFile.ignore();
            }
            break;
          }
        }
        else
        if (chTmp == '{')
        {
          ++nRecursion;
        }
      }
    }

    // datatype
    void ParseDataType( const std::string& sDataType, SDataType& rDataType, const SStruct* pstParent = NULL )
    {
      std::string sTmp;

      bool bIsRef = false;

      rDataType.bIsConst = false;
      rDataType.bIsRef = false;
      rDataType.eType = SDataType::EUnknown;
      rDataType.sUsedName.erase();
      rDataType.lsParams.clear();
      rDataType.sNodeName.erase();
      rDataType.sNamespace.erase();

      std::string sTypeName;

      std::string::size_type nSize = sDataType.size();

      for (std::string::size_type nBegin = 0, nEnd = 0;
           nEnd != std::string::npos && nEnd < nSize; nBegin = nEnd)
      {
        nEnd = sDataType.find_first_of(" \n\r\t", nBegin);
        if (nEnd != std::string::npos)
        {
          sTmp = sDataType.substr(nBegin, nEnd  - nBegin);
          nEnd = sDataType.find_first_not_of(" \n\r\t", nEnd); // skip whitespaces
        }
        else
        {
          sTmp = sDataType.substr(nBegin);
        }

        if (!sTmp.empty())
        { // name of type
          CSP_ASSERT(!rDataType.bIsRef, "type after reference: [" + sDataType + "]",
                     m_stInterface.sFileName, m_nLine);
          if (!sTypeName.empty())
          {
            sTypeName += ' ';
          }

          sTypeName += sTmp;
        }

        rDataType.bIsRef = bIsRef;
      }

      if (rDataType.eType == SDataType::EUnknown)
      {
        GetDataType(sTypeName, rDataType);
      }

      if (rDataType.eType == SDataType::EUnknown)
      {
        const SStruct* pstStruct = GetStruct(sTypeName, pstParent);
        if (pstStruct)
        {
          rDataType.eType = SDataType::EStruct;
          rDataType.sNamespace = pstStruct->sNamespace;
          if (!pstStruct->sOwnerName.empty())
          {
            rDataType.sName = pstStruct->sOwnerName + "::" + pstStruct->sName;
          }
          else
          {
            rDataType.sName = pstStruct->sName;
          }
        }
      }
    }

    // member
    void ParseMember( SMember& rMember )
    {
      std::string sTmp;

      rMember.bIsConst = false;
      rMember.bIsAsynch = false;

      SkipWs();

      // read return type and operation name
      std::string sOperation;
      ReadBefore(sOperation, "(");
      rise::StrTrim(sOperation);
      CSP_ASSERT(!sOperation.empty(), "Can't get operation name", m_stInterface.sFileName, m_nLine);
      rMember.sName = sOperation;

      m_tFile.ignore();
      std::string sMessage;
      ReadBefore(sMessage, ")");
      m_tFile.ignore();
      rise::StrTrim(sMessage);

      if (!sMessage.empty())
      {
        rise::StrReplace(sMessage, ".", "::", true);

        SParam stParam;
        ParseDataType(sMessage, stParam.stDataType);
        stParam.sName = "request";
        stParam.stDataType.bIsConst = true;
        stParam.stDataType.bIsRef = true;
        OptimizeCppNs(stParam.stDataType.sUsedName, m_sCurrentNamespace);
        rMember.lsParamList.push_back(stParam);

//      expand requests
//        const SStruct* pMessage = GetStruct(sMessage);
//        CSP_ASSERT(pMessage, "Can't find message declaration: [" + sMessage + "]",
//                   m_stInterface.sFileName, m_nLine);

//        // todo: remove const_cast
//        const_cast<SStruct*>(pMessage)->bExtern = true; // mark struct as extern to hide it from interface file

//        rMember.lsParamList = pMessage->lsMember;

//        for (std::list<SParam>::iterator itParam = rMember.lsParamList.begin();
//          itParam != rMember.lsParamList.end(); ++itParam)
//        {
//          SDataType& rstDataType = itParam->stDataType;

//          OptimizeCppNs(rstDataType.sUsedName, m_sCurrentNamespace);
//          if (rstDataType.eType == SDataType::EString ||
//              rstDataType.eType == SDataType::ETemplate ||
//              rstDataType.eType == SDataType::EStruct)
//          {
//            rstDataType.bIsConst = true;
//            rstDataType.bIsRef = true;
//          }
//        }
      }

      ReadStr(sTmp);

      if (sTmp == "returns")
      {
        std::string sReturn;
        SkipWs();
        m_tFile.ignore();
        ReadBefore(sReturn, ")");
        m_tFile.ignore();
        rise::StrTrim(sReturn);

        rise::StrReplace(sReturn, ".", "::", true);

        ParseDataType(sReturn, rMember.stReturn.stDataType);
        if (rMember.stReturn.stDataType.eType == SDataType::EStruct)
        {
          OptimizeCppNs(rMember.stReturn.stDataType.sUsedName, m_sCurrentNamespace);
        }

        ReadStr(sTmp);
      }
      else
      {
        rMember.stReturn.stDataType.sName = "void";
        rMember.stReturn.stDataType.eType = SDataType::EGeneric;
      }

      CSP_ASSERT(sTmp == ";", "Missing ';' after operation declaration",
                 m_stInterface.sFileName, m_nLine);

      SkipSingleLineComment();
    }

    // service
    void ParseClass( SClass& rClass )
    {
      char chTmp = '\0';
      std::string sTmp;

      SkipWs();
      rClass.sNamespace = m_sCurrentNamespace;

      ReadStr(sTmp);
      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after classname and '{')", m_stInterface.sFileName, m_nLine);
      CSP_ASSERT(sTmp == "{", "'{' after classname expected ", m_stInterface.sFileName, m_nLine);

      // parsing members
      while (m_tFile.good() && !m_tFile.eof())
      {
        SMember stMember;
        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          rise::StrTrim(sTmp);
          if (sTmp.size() != 0)
          {
            if (stMember.sDescr.size() != 0)
            {
              stMember.sDescr += ' ';
            }
            stMember.sDescr += sTmp;
          }
          SkipWsOnly();
        }

        chTmp = m_tFile.peek();
        if (chTmp == '}')
        {
          m_tFile.ignore();
          break;
        }

        ReadStr(sTmp);

        if (sTmp == "rpc")
        {
          SkipWs();
          ParseMember(stMember);
          rClass.lsMember.push_back(stMember);
        }
        else
        {
          CSP_THROW("parse error! before: (" + sTmp + ")", m_stInterface.sFileName, m_nLine);
        }
      }
    }

    void ParseStruct( SStruct& rStruct )
    {
      char chTmp = '\0';
      std::string sTmp;

      SkipWs();
      ReadBefore(rStruct.sName, " \r\n\t:;{}");
      rStruct.sNamespace = m_sCurrentNamespace;
      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after message name): " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);

      ReadStr(sTmp);
      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after message name): " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);

      if (sTmp == ";")
      {
        m_tFile.unget();
        return;
      }

      rStruct.bForward = false;

      CSP_ASSERT(sTmp == "{", "'{' or ';' after structname expected: " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);

      while (m_tFile.good() && !m_tFile.eof())
      {
        SParam stParamTmp;

        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          rise::StrTrim(sTmp);
          if (sTmp.size() != 0)
          {
            if (stParamTmp.sDescr.size() != 0)
            {
              stParamTmp.sDescr += ' ';
            }
            stParamTmp.sDescr += sTmp;
          }
          SkipWsOnly();
        }

        SkipWs();
        chTmp = m_tFile.peek();
        if (chTmp == '}')
        {
          break;
        }

        std::string sToken;
        ReadBefore(sToken);
        bool bRepeated = false;

        if (sToken == "required")
        {
        }
        else
        if (sToken == "optional")
        {
          stParamTmp.mOptions["optional"] = "true";
        }
        else
        if (sToken == "repeated")
        {
          bRepeated = true;
        }
        else
        if (sToken == "message")
        {
          rStruct.lsStruct.push_back(SStruct());
          SStruct& rstSubStruct = rStruct.lsStruct.back();
          rstSubStruct.sDescr = stParamTmp.sDescr;
          rstSubStruct.sOwnerName = (!rStruct.sOwnerName.empty() ?
                                (rStruct.sOwnerName + "::") : "") + rStruct.sName;
          ParseStruct(rstSubStruct);
          m_tFile.ignore();
          continue;
        }
        else
        {
          CSP_THROW("missing field specificator. message: " + rStruct.sName, m_stInterface.sFileName, m_nLine);
        }

        SkipWs();

        // data type
        ReadBefore(sToken);
        rise::StrReplace(sToken, ".", "::", true);

        if (bRepeated)
        {
          stParamTmp.stDataType.eType = SDataType::ETemplate;
          stParamTmp.stDataType.sName = "list";
          stParamTmp.stDataType.sNamespace = "std::";
          stParamTmp.stDataType.sUsedName.erase();

          SDataType stDataType;
          ParseDataType(sToken, stDataType, &rStruct);
          stParamTmp.stDataType.lsParams.push_back(stDataType);
        }
        else
        {
          ParseDataType(sToken, stParamTmp.stDataType, &rStruct);
        }

        // member name
        SkipWs();
        ReadBefore(sToken);
        stParamTmp.sName = sToken;

        SkipWs();
        chTmp = m_tFile.peek();
        CSP_ASSERT(chTmp == '=', "missing tag. message: " + rStruct.sName,
                   m_stInterface.sFileName, m_nLine);
        m_tFile.ignore();

        SkipWs();
        ReadBefore(sToken); // tag id
        rise::StrTrim(sToken);
        CSP_ASSERT(!sToken.empty(), "missing tag id. message: " + rStruct.sName,
                   m_stInterface.sFileName, m_nLine);

        stParamTmp.mOptions["protobufTag"] = sToken;

        SkipWs();

        // optional parameters block
        chTmp = m_tFile.peek();
        if (chTmp == '[')
        {
          m_tFile.ignore();

          while ((chTmp = m_tFile.peek()) != ']')
          {
            if (chTmp == ',')
            {
              m_tFile.ignore();
            }

            // option name
            SkipWs();
            ReadBefore(sToken, " \r\n\t=");
            rise::StrTrim(sToken);

            SkipWs();
            chTmp = m_tFile.peek();
            CSP_ASSERT(chTmp == '=', "missing '=' after option name. message: " + rStruct.sName,
                       m_stInterface.sFileName, m_nLine);
            m_tFile.ignore();

            SkipWs();
            ReadBefore(sTmp, ",]");
            rise::StrTrim(sTmp);

            if (sToken == "default")
            {
              stParamTmp.mOptions["defaultValue"] = sTmp;
            }
          }
          m_tFile.ignore();
        }

        SkipWs();
        chTmp = m_tFile.peek();
        CSP_ASSERT(chTmp == ';', "missing end of message field. message: " + rStruct.sName,
                   m_stInterface.sFileName, m_nLine);

        m_tFile.ignore();

        SkipWsOnly();
        if (ReadComment(sTmp))
        {
          rise::StrTrim(sTmp);
          if (sTmp.size() != 0)
          {
            if (stParamTmp.sDescr.size() != 0)
            {
              stParamTmp.sDescr += ' ';
            }
            stParamTmp.sDescr += sTmp;
          }
        }

        rStruct.lsMember.push_back(stParamTmp);
      }

    }

    void ImportStruct(const std::list<SStruct>& rlsSrc, std::list<SStruct>& rlsDst)
    {
      for (std::list<SStruct>::const_iterator itStruct = rlsSrc.begin();
          itStruct != rlsSrc.end(); ++itStruct)
      {
        rlsDst.push_back(SStruct());
        SStruct& rstStruct = rlsDst.back();
        rstStruct.sName = itStruct->sName;
        rstStruct.sNamespace = itStruct->sNamespace;
        rstStruct.sParentName = itStruct->sParentName;
        rstStruct.sDescr = itStruct->sDescr;
        rstStruct.sDetail = itStruct->sDetail;
        rstStruct.bExtern = true;
        rstStruct.sOwnerName = itStruct->sOwnerName;
        ImportStruct(itStruct->lsStruct, rstStruct.lsStruct);
      }
    }

    void Import(const std::string& sFileName)
    {
      CProtobufHeaderParser tProtobufHeaderParser;
      const SInterface& rInterface = tProtobufHeaderParser.Parse(m_sInDir, sFileName, *m_pProject);

      // use extern structs
      ImportStruct(rInterface.lsStruct, m_stInterface.lsStruct);

      SInclude stInclude;
      stInclude.sInterfaceName = rInterface.sName;
      stInclude.sNamespace = rInterface.sNamespace;
      stInclude.sFileName = rInterface.sFileName;
      stInclude.sTargetNs = rInterface.sTargetNs;
      m_stInterface.lsInclude.push_back(stInclude);
    }

    void ParseHeaderBlock( SInterface& rInterface )
    {
      char chData = 0;
      std::string sDescr;
      std::string sTmp;
      TStringMap mOptions;
      TStringList lsModules;

      SkipWsOnly();
      while (ReadComment(sTmp))
      {
        rise::StrTrim(sTmp);
        if (sTmp.size() != 0)
        {
          if (sDescr.size() != 0)
          {
            sDescr += ' ';
          }
          sDescr += sTmp;
        }
        SkipWsOnly();
      }

      chData = m_tFile.peek();
      if (chData == '}') // end of namespace
      {
        return;
      }

      ReadStr(sTmp);

      if (sTmp.size() == 0)
      {
        return; // eof
      }

      if (sTmp == "service")
      {
        SClass stClass;

        SkipWs();

        // checking for service class
        ReadBefore(stClass.sName, " \t\n\r:{;");

        SkipWs();
        char chTmp = m_tFile.peek();

        if (chTmp == ';')
        {
          m_tFile.ignore();
          return; // class forward
        }

        rise::LogDebug() << "Using [" << stClass.sName << "] as service class";

        ParseClass(stClass);
        stClass.sDescr = sDescr;
        stClass.lsModules = lsModules;
        stClass.mOptions = mOptions;
        rInterface.lsClass.push_back(stClass);

        SkipWs();

        SkipSingleLineComment();
        lsModules.clear();
        mOptions.clear();
        sDescr.erase();
      }
      else
      if (sTmp == "message")
      {
        std::list<SStruct>::iterator itNewStruct = rInterface.lsStruct.insert(rInterface.lsStruct.end(), SStruct());
        SStruct& rstStruct = *itNewStruct;
        rstStruct.sDescr = sDescr;
        ParseStruct(rstStruct);

        // check for forward declaration
        for (std::list<SStruct>::iterator itStruct = rInterface.lsStruct.begin();
            itStruct != rInterface.lsStruct.end();)
        {
          if (itStruct != itNewStruct &&
              itStruct->sName == rstStruct.sName &&
              itStruct->sNamespace == rstStruct.sNamespace)
          {
            CSP_ASSERT(!rstStruct.bForward, "Duplicating struct " + rstStruct.sName, m_stInterface.sFileName, m_nLine);
            // remove forward declaration
            itStruct = rInterface.lsStruct.erase(itStruct);
          }
          else
          {
            ++itStruct;
          }
        }

        SkipWs();
        m_tFile >> chData;
        if (chData == ';')
        {
          m_tFile.ignore();
        }

        sDescr.erase();

        SkipSingleLineComment();
      }
      else
      if (sTmp == "import")
      {
        std::string sFileName;
        SkipWs();
        char chTmp = '\0';
        m_tFile.get(chTmp);
        CSP_ASSERT(chTmp == '"', "import: filename is not found!", m_stInterface.sFileName, m_nLine);

        ReadBefore(sFileName, "\"");
        m_tFile.get(chTmp);
        CSP_ASSERT(chTmp == '"', "import: filename end is not found!", m_stInterface.sFileName, m_nLine);

        SkipWs();
        m_tFile.get(chTmp);
        CSP_ASSERT(chTmp == ';', "';' is not found!", m_stInterface.sFileName, m_nLine);

        Import(sFileName);

        SkipSingleLineComment();
      }
      else
      if (sTmp == "package")
      {
        std::string sNamespace;
        ReadBefore(sNamespace, ";");
        rise::StrTrim(sNamespace);
        rise::StrReplace(sNamespace, ".", "::", true);
        m_sCurrentNamespace = "::" + sNamespace + "::";

        SkipWs();
        char chTmp = '\0';
        m_tFile.get(chTmp);
        CSP_ASSERT(chTmp == ';', "';' is not found!", m_stInterface.sFileName, m_nLine);
      }
      else
      if (sTmp == "enum")   // enum -ignore
      {
        IgnoreFunction();
        SkipSingleLineComment();
      }
      else
      if (sTmp == ";")
      {
        SkipSingleLineComment();
      }
      else
      {
        CSP_THROW(("Unknown lexeme: \"" + sTmp + "\""), m_stInterface.sFileName, m_nLine);
      }
    }

    void ParseBracketedBlock( SInterface& rInterface )
    {
      char chData = 0;

      SkipWs();
      m_tFile.get(chData);
      CSP_ASSERT(chData == '{', "ParseBracketedBlock: \"{\" is not found!", m_stInterface.sFileName, m_nLine);

      while (m_tFile.good() && !m_tFile.eof())
      {
        SkipWsOnly();
        chData = m_tFile.peek();

        if (chData == '}')
        {
          m_tFile.ignore();
          return;
        }
        else
        if (chData == '{')
        {
          ParseBracketedBlock(rInterface);
        }
        else
        {
          ParseHeaderBlock(rInterface);
        }
      }

      CSP_THROW("ParseBracketedBlock: EOF found!", m_stInterface.sFileName, m_nLine);
    }

    void ParseHeader( SInterface& rInterface )
    {
      char chData = 0;

      while (m_tFile.good() && !m_tFile.eof())
      {
        SkipWsOnly();
        chData = m_tFile.peek();

        if (chData == '{')
        {
          ParseBracketedBlock(rInterface);
        }
        else
        if (chData == '}')
        {
          CSP_THROW("unexpected '}' found", m_stInterface.sFileName, m_nLine);
        }
        else // text
        {
          ParseHeaderBlock(rInterface);
        }
      }
    }

    // Interface
    const SInterface& Parse( const std::string& sInDir, const std::string& sFileName, SProject& rProject )
    {
      m_pProject = &rProject;
      m_sInDir = sInDir;

      for (std::list<SInterface>::const_iterator itInterface = rProject.lsInterfaces.begin();
        itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        if (itInterface->sFileName == sFileName)
        {
          return *itInterface;
        }
      }

      m_stInterface.sFileName = sFileName;

      m_tFile.open((m_sInDir + sFileName).c_str());
      CSP_ASSERT(m_tFile.good(), std::string("can't open file: ") + sFileName + ": "
                 + std::string(strerror(errno)), m_stInterface.sFileName, m_nLine);
      try
      {
        m_stInterface.sName = m_stInterface.sFileName.substr(m_stInterface.sFileName.size() - 6, 6) == ".proto" ?
            m_stInterface.sFileName.substr(0, m_stInterface.sFileName.size() - 6) :
            m_stInterface.sFileName;

        ParseHeader(m_stInterface);
        CorrectStuctParentNs();

        // detect interface main namespace
        if (m_stInterface.sNamespace.empty())
        {
          for (std::list<SClass>::const_iterator itClass = m_stInterface.lsClass.begin();
              itClass != m_stInterface.lsClass.end(); ++itClass)
          {
            if (!itClass->sNamespace.empty())
            {
              m_stInterface.sNamespace = itClass->sNamespace;
              break;
            }
          }
        }

        if (m_stInterface.sNamespace.empty())
        {
          for (std::list<SStruct>::const_iterator itStruct = m_stInterface.lsStruct.begin();
              itStruct != m_stInterface.lsStruct.end(); ++itStruct)
          {
            if (!itStruct->sNamespace.empty())
            {
              m_stInterface.sNamespace = itStruct->sNamespace;
              break;
            }
          }
        }

        rProject.lsInterfaces.push_back(m_stInterface);
        m_tFile.close();
      }
      catch (CParseException& rParseException)
      {
        std::stringbuf sbData;
        m_tFile.get(sbData, '\n');
        m_tFile.ignore();
        m_tFile.get(sbData, '\n');

        rParseException.Message() += ": before\n-----------------\n" + sbData.str() + "\n-----------------\n";

        throw rParseException;
      }

      return m_stInterface;
    }

    void CorrectStuctParentNs()
    {
      // correct structs parent namespaces
      for (std::list<SStruct>::iterator itStruct = m_stInterface.lsStruct.begin();
          itStruct != m_stInterface.lsStruct.end(); ++itStruct)
      {
        std::string& sNsParent = itStruct->sParentName;
        // skip structs with no parent and with namespace, declared from global scope
        if (sNsParent.empty() || sNsParent.substr(0, 2) == "::")
        {
          continue;
        }

        const SStruct* pstParent = GetStruct(sNsParent);
        if (pstParent)
        {
          itStruct->sParentName = pstParent->sName;
          itStruct->sParentNamespace = pstParent->sNamespace;
        }
      }
    }

    void OptimizeCppNs(std::string& sOptimizeNs, const std::string& sCurrentNs)
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

    std::string m_sInDir;
    std::string m_sCurrentNamespace;
    std::ifstream m_tFile;
    int m_nLine;
    SInterface m_stInterface;
    SProject* m_pProject;
  };


  const std::string& CProtobufParser::GetId()
  {
    return m_sId;
  }

  void CProtobufParser::Process(const SParseSettings& rParseSettings, SProject& rProject)
  {
    unsigned uServicesCount = 0;
    for (TStringList::const_iterator itFile = rParseSettings.lsFiles.begin();
        itFile != rParseSettings.lsFiles.end(); ++itFile)
    {
      CProtobufHeaderParser tProtobufHeaderParser;
      const SInterface& rInterface = tProtobufHeaderParser.Parse(rParseSettings.sInDir + "/", *itFile, rProject);
      uServicesCount += rInterface.lsClass.size();
    }

    TStringMap::const_iterator itComponentNs = rParseSettings.mEnv.find("componentns");
    if (itComponentNs != rParseSettings.mEnv.end())
    {
      rProject.sNamespace = itComponentNs->second;
    }
    else
    { // autodetect: take first defined namespace
      for (std::list<SInterface>::const_iterator itInterface = rProject.lsInterfaces.begin();
        itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        for (std::list<SClass>::const_iterator itClass = itInterface->lsClass.begin();
            itClass != itInterface->lsClass.end(); ++itClass)
        {
          if (!itClass->sNamespace.empty())
          {
            rProject.sNamespace = itClass->sNamespace;
            break;
          }
        }

        if (!rProject.sNamespace.empty())
        {
          break;
        }
      }
    }

    CSP_ASSERT(!(uServicesCount == 0 && rParseSettings.bNoServiceError),
               "No staff service interfaces found. Staff services must inherited from staff::IService.\n"
                "Example:\n----\n#include <staff/common/IService.h>\n\n  class Calc: public staff::IService\n"
                "  {\n  public:\n    virtual int Add(int nA, int nB) = 0;\n  };\n----\n\n",
                "", 0);
  }

  const std::string CProtobufParser::m_sId = "protobuf";
}
