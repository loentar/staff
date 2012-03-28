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
#include <staff/utils/Log.h>
#include <staff/utils/PluginExport.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/Exception.h>
#include <staff/codegen/tools.h>
#include "ProtobufParser.h"

STAFF_DECLARE_PLUGIN(staff::codegen::ProtobufParser)

namespace staff
{
namespace codegen
{
  class ProtobufHeaderParser
  {
  public:
    ProtobufHeaderParser(const std::string& sRootNs):
      m_sRootNamespace(sRootNs), m_sCurrentNamespace(sRootNs), m_nLine(1), m_pProject(NULL)
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

    void SkipWsInLine()
    {
      char chData = 0;
      while (m_tFile.good() && !m_tFile.eof())
      {
        chData = m_tFile.peek();

        if(chData != ' ' && chData != '\t')
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

      StringTrim(sComment);

      return true;
    }
    
    template<typename StructType>
    bool ParseCompositeDataType(const std::list<StructType>& rList, DataType& rDataType)
    {
      for (typename std::list<StructType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
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

    void GetDataType(const std::string& sDataTypeName, DataType& rDataType)
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
        rDataType.eType = DataType::TypeGeneric;
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
        rDataType.eType = DataType::TypeGeneric;
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
        rDataType.eType = DataType::TypeGeneric;
        rDataType.sName = "long";
        rDataType.sUsedName = rDataType.sName;
      }
      else
      if (
          sDataTypeName == "string" ||
          sDataTypeName == "bytes"
          )
      {
        rDataType.eType = DataType::TypeString;
        rDataType.sName = "string";
        rDataType.sNamespace = "std::";
        rDataType.sUsedName = rDataType.sNamespace + rDataType.sName;
      }
      else
      if(ParseCompositeDataType(m_stInterface.lsStructs, rDataType))
      {
        rDataType.eType = DataType::TypeStruct;
      }
      else
      {
        rDataType.eType = DataType::TypeUnknown;
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
    void ParseDataType(const std::string& sDataType, DataType& rDataType, const Struct* pstParent = NULL)
    {
      std::string sTmp;

      bool bIsRef = false;

      rDataType.bIsConst = false;
      rDataType.bIsRef = false;
      rDataType.eType = DataType::TypeUnknown;
      rDataType.sUsedName.erase();
      rDataType.lsParams.clear();
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

      if (rDataType.eType == DataType::TypeUnknown)
      {
        GetDataType(sTypeName, rDataType);
      }

      if (rDataType.eType == DataType::TypeUnknown)
      {
        const BaseType* pstType = GetBaseType(sTypeName, m_stInterface, BaseType::TypeAny, pstParent);
        if (pstType)
        {
          rDataType.eType = pstType->eType == BaseType::TypeStruct ? DataType::TypeStruct : DataType::TypeEnum;
          rDataType.sNamespace = pstType->sNamespace;
          rDataType.sName = pstType->sName;
          if ((pstType->eType == BaseType::TypeStruct || BaseType::TypeEnum) &&
              !pstType->sOwnerName.empty())
          {
            rDataType.sName = pstType->sOwnerName + "::" + rDataType.sName;
          }

          rDataType.sUsedName = sTypeName;
        }
      }
    }

    // member
    void ParseMember(Member& rMember)
    {
      std::string sTmp;

      rMember.bIsConst = false;
      rMember.bIsAsynch = false;

      SkipWs();

      // read return type and operation name
      std::string sOperation;
      ReadBefore(sOperation);
      CSP_ASSERT(!sOperation.empty(), "Can't get operation name", m_stInterface.sFileName, m_nLine);
      FixId(sOperation);
      rMember.sName = sOperation;

      SkipWs();
      CSP_ASSERT(m_tFile.get() == '(', "Error after operation name: [" + sOperation + "]",
                 m_stInterface.sFileName, m_nLine);

      SkipWs();

      std::string sMessage;
      ReadBefore(sMessage);

      SkipWs();
      CSP_ASSERT(m_tFile.get() == ')', "Error after message name in operation: [" + sOperation + "]",
                 m_stInterface.sFileName, m_nLine);

      if (!sMessage.empty())
      {
        StringReplace(sMessage, ".", "::", true);

        Param stParam;
        ParseDataType(sMessage, stParam.stDataType);
        stParam.sName = "request";
        stParam.stDataType.bIsConst = true;
        stParam.stDataType.bIsRef = true;
        OptimizeCppNs(stParam.stDataType.sUsedName, m_sCurrentNamespace);
        rMember.lsParams.push_back(stParam);

//      expand requests
//        const Struct* pMessage = GetStruct(sMessage, m_stInterface);
//        CSP_ASSERT(pMessage, "Can't find message declaration: [" + sMessage + "]",
//                   m_stInterface.sFileName, m_nLine);

//        // todo: remove const_cast
//        const_cast<Struct*>(pMessage)->bExtern = true; // mark struct as extern to hide it from interface file

//        rMember.lsParamList = pMessage->lsMembers;

//        for (std::list<SParam>::iterator itParam = rMember.lsParamList.begin();
//          itParam != rMember.lsParamList.end(); ++itParam)
//        {
//          DataType& rstDataType = itParam->stDataType;

//          OptimizeCppNs(rstDataType.sUsedName, m_sCurrentNamespace);
//          if (rstDataType.eType == DataType::TypeString ||
//              rstDataType.eType == DataType::TypeTemplate ||
//              rstDataType.eType == DataType::TypeStruct)
//          {
//            rstDataType.bIsConst = true;
//            rstDataType.bIsRef = true;
//          }
//        }
      }

      SkipWs();
      ReadBefore(sTmp);

      CSP_ASSERT(sTmp == "returns", "Missing return message name", m_stInterface.sFileName, m_nLine);

      std::string sReturn;
      SkipWs();
      CSP_ASSERT(m_tFile.get() == '(', "Error after \"returns\" in operation: [" + sOperation + "]",
                 m_stInterface.sFileName, m_nLine);
      SkipWs();
      ReadBefore(sReturn);
      CSP_ASSERT(!sReturn.empty(), "Can't get result message name", m_stInterface.sFileName, m_nLine);
      SkipWs();
      CSP_ASSERT(m_tFile.get() == ')', "Error after message type in operation: [" + sOperation + "]",
                 m_stInterface.sFileName, m_nLine);

      StringReplace(sReturn, ".", "::", true);

      ParseDataType(sReturn, rMember.stReturn.stDataType);
      if (rMember.stReturn.stDataType.eType == DataType::TypeStruct)
      {
        OptimizeCppNs(rMember.stReturn.stDataType.sUsedName, m_sCurrentNamespace);
      }
      else
      {
        LogWarning() << "Return type for " << sOperation << " is not struct";
      }

      SkipWs();

      char chTmp = m_tFile.peek();
      if (chTmp == '{')
      {
        IgnoreFunction();
        SkipWsOnly();
        while ((chTmp = m_tFile.peek()) == ';' && !m_tFile.eof())
        {
          m_tFile.ignore();
        }
      }
      else
      {
        CSP_ASSERT(chTmp == ';', "Missing ';' or '{' after operation declaration",
                 m_stInterface.sFileName, m_nLine);

        do
        {
          m_tFile.ignore();
        }
        while ((chTmp = m_tFile.peek()) == ';' && !m_tFile.eof());
      }


      SkipSingleLineComment();
    }

    // service
    void ParseClass(Class& rClass)
    {
      char chTmp = '\0';
      std::string sTmp;

      FixId(rClass.sName);
      SkipWs();
      rClass.sNamespace = m_sCurrentNamespace;

      ReadStr(sTmp);
      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after classname and '{')", m_stInterface.sFileName, m_nLine);
      CSP_ASSERT(sTmp == "{", "'{' after classname expected ", m_stInterface.sFileName, m_nLine);

      // parsing members
      while (m_tFile.good() && !m_tFile.eof())
      {
        Member stMember;
        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          StringTrim(sTmp);
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
          rClass.lsMembers.push_back(stMember);
        }
        else
        if (sTmp == "option")
        { // skip options
          ReadBefore(sTmp, ";");
          LogDebug() << "Ignoring option" << sTmp;
          m_tFile.ignore();
          continue;
        }
        else
        {
          CSP_THROW("parse error! before: (" + sTmp + ")", m_stInterface.sFileName, m_nLine);
        }
      }
    }

    void ParseEnum(Enum& rEnum)
    {
      char chTmp = '\0';
      std::string sTmp;
      std::string sDescr;

      FixId(rEnum.sName);

      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after enum name): " + rEnum.sName,
                 m_stInterface.sFileName, m_nLine);

      ReadStr(sTmp);
      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after enum name): " + rEnum.sName,
                 m_stInterface.sFileName, m_nLine);

      if (sTmp == ";")
      {
        m_tFile.unget();
        return;
      }

      rEnum.bForward = false;

      CSP_ASSERT(sTmp == "{", "'{' or ';' after enum name expected: " + rEnum.sName,
                 m_stInterface.sFileName, m_nLine);

      while (m_tFile.good() && !m_tFile.eof())
      {
        Enum::EnumMember stMember;

        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          StringTrim(sTmp);
          if (sTmp.size() != 0)
          {
            sDescr = sTmp.substr(1);
            StringTrim(sDescr);
          }
          SkipWsOnly();
        }

        ReadBefore(stMember.sName, "=;}");
        StringTrim(stMember.sName);
        FixId(stMember.sName);

        chTmp = m_tFile.peek();
        if (chTmp == '=')
        { // read value
          m_tFile.ignore();
          ReadBefore(stMember.sValue, ";}");
          StringTrim(stMember.sValue);
          FixId(stMember.sValue);
          chTmp = m_tFile.peek();
        }

        if (!stMember.sName.empty())
        {
          rEnum.lsMembers.push_back(stMember);
        }

        m_tFile.ignore(); // '}' or ';'

        if (chTmp == '}')
        {
          break;
        }

        sDescr.erase();
      }

      SkipWsInLine();
      if (m_tFile.peek() == ';')
      {
        m_tFile.ignore();
      }

      SkipSingleLineComment();
    }

    void ParseStruct(Struct& rStruct)
    {
      char chTmp = '\0';
      std::string sTmp;

      FixId(rStruct.sName);

      const std::string& sOwnerName = (!rStruct.sOwnerName.empty() ?
                                        (rStruct.sOwnerName + "::") : "") + rStruct.sName;

      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after message name): " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);
      SkipWs();
      chTmp = m_tFile.get();
      if (chTmp == ';')
      {
        return; // forward declaration
      }

      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after message name): " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);

      rStruct.bForward = false;

      CSP_ASSERT(chTmp == '{', "'{' or ';' after structname expected: " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);

      while (m_tFile.good() && !m_tFile.eof())
      {
        Param stParamTmp;

        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          StringTrim(sTmp);
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
          m_tFile.ignore();
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
        if (sToken == "enum")
        {
          std::string sName;
          SkipWs();
          ReadBefore(sName, " \r\n\t;{}");
          FixId(sName);

          Enum& rstEnum = TypeInList(rStruct.lsEnums, sName, m_sCurrentNamespace, sOwnerName);

          ParseEnum(rstEnum);
          if (!rstEnum.bForward)
          {
            rstEnum.sDescr = stParamTmp.sDescr;
            rstEnum.mOptions = stParamTmp.mOptions;
          }

          continue;
        }
        else
        if (sToken == "message")
        {
          std::string sName;
          SkipWs();
          ReadBefore(sName, " \r\n\t;{}");
          FixId(sName);

          Struct& rstStruct = TypeInList(rStruct.lsStructs, sName, m_sCurrentNamespace, sOwnerName);

          ParseStruct(rstStruct);
          if (!rstStruct.bForward)
          {
            rstStruct.sDescr = stParamTmp.sDescr;
            rstStruct.mOptions = stParamTmp.mOptions;
          }

          continue;
        }
        else
        if (sToken == "option")
        { // skip options
          ReadBefore(sTmp, ";");
          LogDebug() << "Ignoring option" << sTmp;
          m_tFile.ignore();
          continue;
        }
        else
        if (sToken == "extensions")
        { // skip options
          ReadBefore(sTmp, ";");
          LogDebug() << "Ignoring extensions " << sTmp;
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
        StringReplace(sToken, ".", "::", true);

        if (bRepeated)
        {
          stParamTmp.stDataType.eType = DataType::TypeTemplate;
          stParamTmp.stDataType.sName = "list";
          stParamTmp.stDataType.sNamespace = "std::";
          stParamTmp.stDataType.sUsedName.erase();

          DataType stDataType;
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
        FixId(stParamTmp.sName);

        SkipWs();
        chTmp = m_tFile.peek();
        CSP_ASSERT(chTmp == '=', "missing tag. message: " + rStruct.sName,
                   m_stInterface.sFileName, m_nLine);
        m_tFile.ignore();

        SkipWs();
        ReadBefore(sToken); // tag id
        StringTrim(sToken);
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
            StringTrim(sToken);

            SkipWs();
            chTmp = m_tFile.peek();
            CSP_ASSERT(chTmp == '=', "missing '=' after option name. message: " + rStruct.sName,
                       m_stInterface.sFileName, m_nLine);
            m_tFile.ignore();

            SkipWs();
            ReadBefore(sTmp, ",]");
            StringTrim(sTmp);

            if (sToken == "default")
            {
              if (stParamTmp.stDataType.eType == DataType::TypeEnum)
              {
                // fix namespace
                FixId(sTmp);
                sTmp = stParamTmp.stDataType.sNamespace + sTmp;
                OptimizeCppNs(sTmp, rStruct.sNamespace + sOwnerName);
              }
              else
              {
                FixId(sTmp, true);
              }
              
              stParamTmp.mOptions["defaultValue"] = sTmp;
            }
          }
          m_tFile.ignore();
        }

        SkipWs();
        m_tFile >> chTmp;
        CSP_ASSERT(chTmp == ';', "missing end of message field. message: " + rStruct.sName,
                   m_stInterface.sFileName, m_nLine);

        SkipWsOnly();
        if (ReadComment(sTmp))
        {
          StringTrim(sTmp);
          if (sTmp.size() != 0)
          {
            if (stParamTmp.sDescr.size() != 0)
            {
              stParamTmp.sDescr += ' ';
            }
            stParamTmp.sDescr += sTmp;
          }
        }

        rStruct.lsMembers.push_back(stParamTmp);
      }

      SkipWsInLine();
      if (m_tFile.peek() == ';')
      {
        m_tFile.ignore();
      }

      SkipSingleLineComment();
    }

    void ImportEnums(const std::list<Enum>& rlsSrc, std::list<Enum>& rlsDst)
    {
      for (std::list<Enum>::const_iterator itEnum = rlsSrc.begin();
          itEnum != rlsSrc.end(); ++itEnum)
      {
        rlsDst.push_back(*itEnum);
        rlsDst.back().bExtern = true;
      }
    }

    void ImportStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst)
    {
      for (std::list<Struct>::const_iterator itStruct = rlsSrc.begin();
          itStruct != rlsSrc.end(); ++itStruct)
      {
        rlsDst.push_back(Struct());
        Struct& rstStruct = rlsDst.back();
        rstStruct.sName = itStruct->sName;
        rstStruct.sNamespace = itStruct->sNamespace;
        rstStruct.sParentName = itStruct->sParentName;
        rstStruct.sDescr = itStruct->sDescr;
        rstStruct.sDetail = itStruct->sDetail;
        rstStruct.bExtern = true;
        rstStruct.sOwnerName = itStruct->sOwnerName;
        ImportEnums(itStruct->lsEnums, rstStruct.lsEnums);
        ImportStruct(itStruct->lsStructs, rstStruct.lsStructs);
      }
    }

    void Import(const std::string& sFileName)
    {
      ProtobufHeaderParser tProtobufHeaderParser(m_sRootNamespace);
      
      LogDebug() << "importing file " << sFileName << " from " << m_stInterface.sFileName;
      const Interface& rInterface = tProtobufHeaderParser.Parse(m_sInDir, sFileName, *m_pProject);

      // use extern enums
      ImportEnums(rInterface.lsEnums, m_stInterface.lsEnums);

      // use extern structs
      ImportStruct(rInterface.lsStructs, m_stInterface.lsStructs);

      Include stInclude;
      stInclude.sInterfaceName = rInterface.sName;
      stInclude.sNamespace = rInterface.sNamespace;
      stInclude.sFileName = rInterface.sFileName;
      stInclude.sFilePath = rInterface.sFilePath;
      m_stInterface.lsIncludes.push_back(stInclude);
    }

    void ParseHeaderBlock(Interface& rInterface)
    {
      char chData = 0;
      std::string sDescr;
      std::string sTmp;
      StringMap mOptions;
      StringList lsModules;

      SkipWsOnly();
      while (ReadComment(sTmp))
      {
        StringTrim(sTmp);
        if (sTmp.size() != 0)
        {
          if (sDescr.size() != 0)
          {
            sDescr += ' ';
          }
          sDescr += sTmp;
        }

        // don't get file header comment
        int nLine = m_nLine;
        SkipWsOnly();
        if (m_nLine > (nLine + 1))
        {
          sDescr.erase();
        }
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
        Class stClass;

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

        LogDebug() << "Using [" << stClass.sName << "] as service class";

        ParseClass(stClass);
        stClass.sDescr = sDescr;
        stClass.lsModules = lsModules;
        stClass.mOptions = mOptions;
        rInterface.lsClasses.push_back(stClass);

        SkipWs();

        SkipSingleLineComment();
        lsModules.clear();
        mOptions.clear();
        sDescr.erase();
      }
      else
      if (sTmp == "enum")
      {
        std::string sName;
        SkipWs();
        ReadBefore(sName, " \r\n\t;{}");

        Enum& rstEnum = TypeInList(rInterface.lsEnums, sName, m_sCurrentNamespace);

        ParseEnum(rstEnum);
        if (!rstEnum.bForward)
        {
          rstEnum.sDescr = sDescr;
          rstEnum.mOptions = mOptions;
        }

        sDescr.erase();
      }
      else
      if (sTmp == "message")
      {
        std::string sName;
        SkipWs();
        ReadBefore(sName, " \r\n\t;{}");

        Struct& rstStruct = TypeInList(rInterface.lsStructs, sName, m_sCurrentNamespace);

        ParseStruct(rstStruct);
        if (!rstStruct.bForward)
        {
          rstStruct.sDescr = sDescr;
          rstStruct.mOptions = mOptions;
        }

        sDescr.erase();
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
        StringTrim(sNamespace);
        StringReplace(sNamespace, ".", "::", true);
        m_sCurrentNamespace = m_sRootNamespace + sNamespace + "::";

        SkipWs();
        char chTmp = '\0';
        m_tFile.get(chTmp);
        CSP_ASSERT(chTmp == ';', "';' is not found!", m_stInterface.sFileName, m_nLine);
      }
      else
      if (sTmp == "extend")   // extend -ignore
      {
        ReadBefore(sTmp, "{;");
        LogDebug() << "Ignoring extend" << sTmp;
        IgnoreFunction();
        SkipSingleLineComment();
      }
      else
      if (sTmp == ";")
      {
        SkipSingleLineComment();
      }
      else
      if (sTmp == "option")
      { // skip options
        ReadBefore(sTmp, ";");
        LogDebug() << "Ignoring option" << sTmp;
      }
      else
      {
        CSP_THROW(("Unknown lexeme: \"" + sTmp + "\""), m_stInterface.sFileName, m_nLine);
      }
    }

    void ParseBracketedBlock(Interface& rInterface)
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

    void ParseHeader(Interface& rInterface)
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


    static bool IsStructUsesAnother(const Struct& rstStruct, const Struct& rstOtherStruct)
    {
      bool bResult = false;
      // check the same level
      for (std::list<Param>::const_iterator itMember = rstStruct.lsMembers.begin();
          itMember != rstStruct.lsMembers.end(); ++itMember)
      {
        const DataType& rstDataType = itMember->stDataType;
        if (rstDataType.eType == DataType::TypeStruct &&
            rstDataType.sNamespace == rstOtherStruct.sNamespace &&
            rstDataType.sName == rstOtherStruct.sName)
        {
          bResult = true;
          break;
        }
      }

      if (!bResult) // check nested structs
      {
        for (std::list<Struct>::const_iterator itStruct = rstStruct.lsStructs.begin();
            itStruct != rstStruct.lsStructs.end(); ++itStruct)
        {
          if (IsStructUsesAnother(*itStruct, rstOtherStruct))
          {
            bResult = true;
            break;
          }
        }
      }

      return bResult;
    }

    bool FixStructOrderByUsing(std::list<Struct>& rlsStructs)
    {
      bool bWasChanged = false;
      for (std::list<Struct>::size_type nRetry = rlsStructs.size(); nRetry; --nRetry)
      {
        bWasChanged = false;
        for (std::list<Struct>::iterator itThisStruct = rlsStructs.begin();
            itThisStruct != rlsStructs.end();)
        {
          // check is
          std::list<Struct>::iterator itOtherStruct = itThisStruct;
          ++itOtherStruct;
          for (; itOtherStruct != rlsStructs.end(); ++itOtherStruct)
          {
            if (IsStructUsesAnother(*itThisStruct, *itOtherStruct))
            {
              LogDebug() << "Moving " << itOtherStruct->sName
                  << " to before " << itThisStruct->sName;
              rlsStructs.splice(itThisStruct++, rlsStructs, itOtherStruct);
              // now itThisStruct points to new pos of the itThisStruct
              bWasChanged = true;
              break;
            }
          }

          ++itThisStruct;
        }

        for (std::list<Struct>::iterator itStruct = rlsStructs.begin();
            itStruct != rlsStructs.end(); ++itStruct)
        {
          bWasChanged |= FixStructOrderByUsing(itStruct->lsStructs);
        }

        if (!bWasChanged)
        {
          break;
        }
      }

      if (bWasChanged)
      {
        LogWarning() << "Failed to reorder structures. Build may fail";
      }

      return bWasChanged;
    }


    void CheckAndFixUnknownDataType(DataType& rstDataType, Struct* pstStruct = NULL)
    {
      if (rstDataType.eType == DataType::TypeUnknown)
      {
        LogDebug() << "2nd pass: fixing datatype: " << rstDataType.sName;
        ParseDataType(rstDataType.sNamespace + rstDataType.sName, rstDataType, pstStruct);
      }
      
      if (rstDataType.eType == DataType::TypeTemplate && !rstDataType.lsParams.empty())
      {
        DataType& rstTemplParam = rstDataType.lsParams.front();
        if (rstTemplParam.eType == DataType::TypeUnknown)
        {
          LogDebug() << "2nd pass fixing datatype: " << rstTemplParam.sName;
          ParseDataType(rstTemplParam.sNamespace + rstTemplParam.sName, rstTemplParam, pstStruct);
        }
      }
    }

    void FixUnknownDataTypesInStructs(std::list<Struct>& rlsStructs)
    {
      // search unknown data types in structures
      for (std::list<Struct>::iterator itStruct = rlsStructs.begin();
          itStruct != rlsStructs.end(); ++itStruct)
      {
        for (std::list<Param>::iterator itMember = itStruct->lsMembers.begin();
            itMember != itStruct->lsMembers.end(); ++itMember)
        {
          CheckAndFixUnknownDataType(itMember->stDataType, &*itStruct);
        }
        FixUnknownDataTypesInStructs(itStruct->lsStructs);
      }
    }

    void FixUnknownDataTypesInClasses(std::list<Class>& rlsClasses)
    {
      // search unknown data types in services
      for (std::list<Class>::iterator itClass = rlsClasses.begin();
          itClass != rlsClasses.end(); ++itClass)
      {
        for (std::list<Member>::iterator itMember = itClass->lsMembers.begin();
            itMember != itClass->lsMembers.end(); ++itMember)
        {
          CheckAndFixUnknownDataType(itMember->stReturn.stDataType);

          for (std::list<Param>::iterator itParam = itMember->lsParams.begin();
              itParam != itMember->lsParams.end(); ++itParam)
          {
            CheckAndFixUnknownDataType(itParam->stDataType);
          }

        }
      }
    }


    // Interface
    const Interface& Parse(const std::string& sInDir, const std::string& sFileName, Project& rProject)
    {
      m_pProject = &rProject;
      m_sInDir = sInDir;

      std::string::size_type nPos = sFileName.find_last_of("/\\");
      const std::string& sInterfaceFileName = (nPos != std::string::npos) ?
                                              sFileName.substr(nPos + 1) : sFileName;
      const std::string& sInterfaceFilePath = (nPos != std::string::npos) ?
                                              sFileName.substr(0, nPos + 1) : "";

      for (std::list<Interface>::const_iterator itInterface = rProject.lsInterfaces.begin();
          itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        if (itInterface->sFileName == sInterfaceFileName &&
            itInterface->sFilePath == sInterfaceFilePath)
        {
          return *itInterface; // already parsed
        }
      }

      rProject.lsInterfaces.push_back(m_stInterface);
      Interface& rProjectThisInterface = rProject.lsInterfaces.back();

      m_stInterface.sFileName = sInterfaceFileName;
      m_stInterface.sFilePath = sInterfaceFilePath;

      const std::string& sFilePath = m_sInDir + sFileName;

      LogDebug() << "processing file: " << sFilePath;
      m_tFile.open(sFilePath.c_str());
      CSP_ASSERT(m_tFile.good(), std::string("can't open file: ") + sFilePath + ": "
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
          for (std::list<Class>::const_iterator itClass = m_stInterface.lsClasses.begin();
              itClass != m_stInterface.lsClasses.end(); ++itClass)
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
          for (std::list<Struct>::const_iterator itStruct = m_stInterface.lsStructs.begin();
              itStruct != m_stInterface.lsStructs.end(); ++itStruct)
          {
            if (!itStruct->sNamespace.empty())
            {
              m_stInterface.sNamespace = itStruct->sNamespace;
              break;
            }
          }
        }
        
        FixUnknownDataTypesInStructs(m_stInterface.lsStructs);
        FixUnknownDataTypesInClasses(m_stInterface.lsClasses);
        FixStructOrderByUsing(m_stInterface.lsStructs);

        rProjectThisInterface = m_stInterface;
        m_tFile.close();
      }
      catch (ParseException& rParseException)
      {
        std::stringbuf sbData;
        m_tFile.get(sbData, '\n');
        m_tFile.ignore();
        m_tFile.get(sbData, '\n');

        rParseException.Message() += ": before\n-----------------\n" + sbData.str() + "\n-----------------\n";

        throw;
      }

      return m_stInterface;
    }
    
    void CorrectStuctParentNs()
    {
      // correct structs parent namespaces
      for (std::list<Struct>::iterator itStruct = m_stInterface.lsStructs.begin();
          itStruct != m_stInterface.lsStructs.end(); ++itStruct)
      {
        std::string& sNsParent = itStruct->sParentName;
        // skip structs with no parent and with namespace, declared from global scope
        if (sNsParent.empty() || sNsParent.substr(0, 2) == "::")
        {
          continue;
        }

        const Struct* pstParent = GetStruct(sNsParent, m_stInterface);
        if (pstParent)
        {
          itStruct->sParentName = pstParent->sName;
          itStruct->sParentNamespace = pstParent->sNamespace;
        }
      }
    }

    std::string m_sInDir;
    std::string m_sRootNamespace;
    std::string m_sCurrentNamespace;
    std::ifstream m_tFile;
    int m_nLine;
    Interface m_stInterface;
    Project* m_pProject;
  };


  const std::string& ProtobufParser::GetId()
  {
    return m_sId;
  }

  void ProtobufParser::Process(const ParseSettings& rParseSettings, Project& rProject)
  {
    unsigned uServicesCount = 0;

    std::string sRootNs = "::";
    StringMap::const_iterator itRootNs = rParseSettings.mEnv.find("rootns");
    if (itRootNs != rParseSettings.mEnv.end() && !itRootNs->second.empty())
    {
      sRootNs = "::" + itRootNs->second + "::";
      StringReplace(sRootNs, ".", "::", true);
    }

    for (StringList::const_iterator itFile = rParseSettings.lsFiles.begin();
        itFile != rParseSettings.lsFiles.end(); ++itFile)
    {
      LogDebug() << "---- " << *itFile << " -------------------------------------------------------";
      ProtobufHeaderParser tProtobufHeaderParser(sRootNs);
      const Interface& rInterface = tProtobufHeaderParser.Parse(rParseSettings.sInDir, *itFile, rProject);
      uServicesCount += rInterface.lsClasses.size();
    }

    StringMap::const_iterator itComponentNs = rParseSettings.mEnv.find("componentns");
    if (itComponentNs != rParseSettings.mEnv.end())
    {
      rProject.sNamespace = "::" + itComponentNs->second + "::";
      StringReplace(rProject.sNamespace, ".", "::", true);
    }
    else
    { // autodetect: take first defined namespace
      for (std::list<Interface>::const_iterator itInterface = rProject.lsInterfaces.begin();
        itInterface != rProject.lsInterfaces.end(); ++itInterface)
      {
        for (std::list<Class>::const_iterator itClass = itInterface->lsClasses.begin();
            itClass != itInterface->lsClasses.end(); ++itClass)
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

    CSP_ASSERT(!(uServicesCount == 0 && rParseSettings.bNoServiceWarn),
               "No staff service interfaces found. Staff services must inherited from staff::IService.\n"
                "Example:\n----\n#include <staff/common/IService.h>\n\n  class Calc: public staff::IService\n"
                "  {\n  public:\n    virtual int Add(int nA, int nB) = 0;\n  };\n----\n\n",
                "", 0);
  }

  const std::string ProtobufParser::m_sId = "protobuf";
}
}
