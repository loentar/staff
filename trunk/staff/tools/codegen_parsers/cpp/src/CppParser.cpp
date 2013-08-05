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

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <staff/utils/Log.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/PluginExport.h>
#include <staff/utils/Exception.h>
#include <staff/codegen/tools.h>
#include "CppParser.h"

STAFF_DECLARE_PLUGIN(staff::codegen::CppParser)

namespace staff
{
namespace codegen
{
  class CppHeaderParser
  {
  public:
    CppHeaderParser(const std::string& sRootNs):
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

    void ReadDescrComment(std::string& sDescr)
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
            m_tFile.ignore();
            if (m_tFile.peek() == '!')
            {
              m_tFile.ignore();
              if (m_tFile.peek() == '<') // description
              {
                m_tFile.ignore();
                ReadBefore(sDescr, "\n\r");
                StringTrim(sDescr);
              }
            }
            m_tFile.ignore(INT_MAX, '\n');
            ++m_nLine;
          } else
          {
            m_tFile.unget();
          }
        }
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
      }

      return !sComment.empty();
    }

    bool CheckIgnoreMetacomment(const std::string& sText)
    {
      bool bResult = false;
      if (sText.substr(1, 11) == "ignoreBegin")
      {
        // start ignore block
        std::string sLine;
        do
        {
          ReadBefore(sLine, "\n");
          m_tFile.ignore();
          StringTrim(sLine);
          if (sLine.size() >= 12 && sLine[0] == '/' && sLine[1] == '/')
          {
            sLine.erase(0, 2);
            StringTrim(sLine);
            if (sLine.substr(0, 10) == "*ignoreEnd")
            {
              bResult = true;
              break;
            }
          }
        }
        while (m_tFile.good());
      }

      return bResult;
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

      if (sDataTypeName == "staff::DataObject" ||
          (sDataTypeName == "DataObject" && m_sCurrentNamespace.substr(0, 9) == "::staff::"))
      {
        rDataType.eType = DataType::TypeDataObject;
      }
      else
      if (sDataTypeName == "staff::MessageContext" || sDataTypeName == "staff::Operation" ||
          sDataTypeName == "staff::Optional" ||
          ((sDataTypeName == "MessageContext" || sDataTypeName == "Operation" ||
            sDataTypeName == "Optional")
              && m_sCurrentNamespace.substr(0, 9) == "::staff::"))
      {
        rDataType.eType = DataType::TypeGeneric; // supress unknown datatype warning
      }
      else
      if (
          sDataTypeName == "bool" ||
          sDataTypeName == "char" ||
          sDataTypeName == "int" ||
          sDataTypeName == "short" ||
          sDataTypeName == "long" ||
          sDataTypeName == "float" ||
          sDataTypeName == "double" ||
          sDataTypeName == "void" ||

          // XML types
          sDataTypeName == "staff::boolean" ||
          sDataTypeName == "staff::float" ||
          sDataTypeName == "staff::double" ||
          sDataTypeName == "staff::decimal" ||

          sDataTypeName == "staff::integer" ||
          sDataTypeName == "staff::nonPositiveInteger" ||
          sDataTypeName == "staff::negativeInteger" ||
          sDataTypeName == "staff::long" ||
          sDataTypeName == "staff::int" ||
          sDataTypeName == "staff::short" ||
          sDataTypeName == "staff::byte" ||
          sDataTypeName == "staff::nonNegativeInteger" ||
          sDataTypeName == "staff::unsignedLong" ||
          sDataTypeName == "staff::unsignedInt" ||
          sDataTypeName == "staff::unsignedShort" ||
          sDataTypeName == "staff::unsignedByte" ||
          sDataTypeName == "staff::positiveInteger" ||
          sDataTypeName == "staff::anyAttribute"
        )
      {
        rDataType.eType = DataType::TypeGeneric;
      }
      else
      if (
        sDataTypeName == "std::string" ||
        sDataTypeName == "std::wstring" ||

        sDataTypeName == "staff::string" ||

        sDataTypeName == "staff::duration" ||
        sDataTypeName == "staff::dateTime" ||
        sDataTypeName == "staff::time" ||
        sDataTypeName == "staff::date" ||
        sDataTypeName == "staff::gYearMonth" ||
        sDataTypeName == "staff::gYear" ||
        sDataTypeName == "staff::gMonthDay" ||
        sDataTypeName == "staff::gDay" ||
        sDataTypeName == "staff::gMonth" ||
        sDataTypeName == "staff::hexBinary" ||
        sDataTypeName == "staff::base64Binary" ||
        sDataTypeName == "staff::anyURI" ||
        sDataTypeName == "staff::QName" ||
        sDataTypeName == "staff::NOTATION" ||
        sDataTypeName == "staff::normalizedString" ||
        sDataTypeName == "staff::token" ||
        sDataTypeName == "staff::language" ||
        sDataTypeName == "staff::IDREFS" ||
        sDataTypeName == "staff::ENTITIES" ||
        sDataTypeName == "staff::NMTOKEN" ||
        sDataTypeName == "staff::NMTOKENS" ||
        sDataTypeName == "staff::Name" ||
        sDataTypeName == "staff::NCName" ||
        sDataTypeName == "staff::ID" ||
        sDataTypeName == "staff::IDREF" ||
        sDataTypeName == "staff::ENTITY" ||
        sDataTypeName == "staff::anySimpleType" ||
        sDataTypeName == "staff::anyType"
      )
      {
        rDataType.eType = DataType::TypeString;
      }
      else
      if(ParseCompositeDataType(m_stInterface.lsTypedefs, rDataType))
      {
        rDataType.eType = DataType::TypeTypedef;
      }
      else
      {
        // long type names detection. example: unsigned long int
        bool bGeneric = true;
        {
          bool bIsLong = false;
          bool bIsShort = false;
          bool bIsSigned = false;
          bool bIsUnsigned = false;
          bool bIsChar = false;
          bool bIsInt = false;
          bool bIsFloat = false;

          for (std::string::size_type nBegin = 0, nEnd = 0;
               nEnd != std::string::npos; nBegin = nEnd + 1)
          {
            nEnd = sDataTypeName.find_first_of(" \t\n\r", nBegin);
            std::string sType = sDataTypeName.substr(nBegin, nEnd - nBegin);
            StringTrim(sType);

            if (sType == "long")
            {
              CSP_ASSERT(!bIsShort, "short and long type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsChar, "char and long type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsFloat, "float and long type", m_stInterface.sFileName, m_nLine);
              bIsLong = true;
            }
            else
            if (sType == "short")
            {
              CSP_ASSERT(!bIsLong, "long and short type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsChar, "char and short type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsFloat, "float and short type", m_stInterface.sFileName, m_nLine);
              bIsShort = true;
            }
            else
            if (sType == "signed")
            {
              CSP_ASSERT(!bIsUnsigned, "signed and unsigned type", m_stInterface.sFileName, m_nLine);
              bIsSigned = true;
            }
            else
            if (sType == "unsigned")
            {
              CSP_ASSERT(!bIsSigned, "signed and unsigned type", m_stInterface.sFileName, m_nLine);
              bIsUnsigned = true;
            }
            else
            if (sType == "char")
            {
              CSP_ASSERT(!bIsShort, "short and char type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsLong, "long and char type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsFloat, "char and float type", m_stInterface.sFileName, m_nLine);
              bIsChar = true;
            }
            else
            if (sType == "double" || sType == "float")
            {
              CSP_ASSERT(!bIsShort, "short and float type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsLong, "long and float type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsSigned, "signed and float type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsUnsigned, "unsigned and float type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsLong, "long and float type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsChar, "char and float type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsInt, "int and float type", m_stInterface.sFileName, m_nLine);
              bIsFloat = true;
            }
            else
            if (sType == "int")
            {
              CSP_ASSERT(!bIsChar, "char and int type", m_stInterface.sFileName, m_nLine);
              CSP_ASSERT(!bIsFloat, "float and int type", m_stInterface.sFileName, m_nLine);
              bIsInt = true;
            }
            else
            {
              bGeneric = false;
            }
          }

          rDataType.sName = sDataTypeName;
        }
        rDataType.eType = bGeneric ? DataType::TypeGeneric : DataType::TypeUnknown;
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

    std::string::size_type ParseTemplate(const std::string& sTemplate, DataType& rDataType, const Struct* pstParent)
    {
      const std::string::size_type nTemplateSize = sTemplate.size();
      std::string::size_type nResult = 0;
      std::string::size_type nTmp = 0;
      std::string sToken;
      for (std::string::size_type nBegin = 0, nEnd = 0;
           nEnd != std::string::npos; nBegin = nEnd + 1)
      {
        nEnd = sTemplate.find_first_of(",<>&", nBegin);
        if (nEnd == std::string::npos)
        {
          sToken = sTemplate.substr(nBegin);
        }
        else
        {
          if (sTemplate[nEnd] == '<')
          {
            int nRecurse = 1;

            ++nEnd;
            for (; nRecurse > 0; ++nEnd)
            {
              nEnd = sTemplate.find_first_of("<>", nEnd);
              CSP_ASSERT(nEnd != std::string::npos,
                         " error while parsing template params: \"<" + sTemplate + ">\" unmatched open/close",
                         m_stInterface.sFileName, m_nLine);

              if (sTemplate[nEnd] == '<')
              {
                ++nRecurse;
              }
              else
              if (sTemplate[nEnd] == '>')
              {
                --nRecurse;
              }

            }

          }

          // detect '&'
          if ((nEnd + 1) < nTemplateSize)
          {
            nTmp = sTemplate.find_first_of(",>", nEnd + 1);
            if (nTmp != std::string::npos)
            {
              nEnd = nTmp;
            }
          }

          sToken = sTemplate.substr(nBegin, nEnd - nBegin);
        }

        StringTrim(sToken);
        if (!sToken.empty())
        {
          DataType stTemplParam;
          ParseDataType(sToken, stTemplParam, pstParent);
          rDataType.lsParams.push_back(stTemplParam);
        }

        if (sTemplate[nEnd] == '>')
        {
          nResult = nEnd;
          break;
        }
      }
      return nResult;
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
        nEnd = sDataType.find_first_of(" \n\r\t&<*", nBegin);
        if (nEnd != std::string::npos)
        {
          sTmp = sDataType.substr(nBegin, nEnd  - nBegin);
          nEnd = sDataType.find_first_not_of(" \n\r\t", nEnd); // skip whitespaces
        }
        else
        {
          sTmp = sDataType.substr(nBegin);
        }

        if (sTmp == "static")
        {
          LogWarning() << "members must be non-static. Line:" << m_nLine;
          CSP_ASSERT(sTypeName.empty(), "static after typename", m_stInterface.sFileName, m_nLine);
        }
        else
        if (sTmp == "struct")
        {
          rDataType.sPrefix = sTmp;
          sTmp = sDataType.substr(sTmp.size());
          StringTrim(sTmp);
          nEnd = std::string::npos;
        }

        if (nEnd != std::string::npos)
        {
          char chTmp = sDataType[nEnd];
          if (chTmp == '<')
          {
            std::string::size_type nPos = sTmp.find_last_of("::");
            if (nPos != std::string::npos)
            {
              ++nPos;
              rDataType.sName = sTmp.substr(nPos);
              rDataType.sNamespace = sTmp.substr(0, nPos);
            }
            else
            {
              rDataType.sName = sTmp;
              rDataType.sNamespace.erase();
            }

            rDataType.eType = DataType::TypeTemplate;

            ++nEnd;

  //          std::string::size_type nPos = sDataType.find_last_of('>');
  //          CSP_ASSERT(nPos != std::string::npos, "Can't find end of template declaration: [" + sDataType + "]",
  //                     m_stInterface.sFileName, m_nLine);
             std::string::size_type nTemplateSize = ParseTemplate(sDataType.substr(nEnd), rDataType, pstParent);
             nEnd += nTemplateSize;
             continue;
          }
          else
          if(chTmp == '&')
          {
            CSP_ASSERT(sTypeName.empty(), "reference before typename: [" + sDataType + "]",
                       m_stInterface.sFileName, m_nLine);
            bIsRef = true;
            ++nEnd;
          }
          else
          if(chTmp == '*')
          {
            CSP_THROW("pointers is not supported. in: [" + sDataType + "]", m_stInterface.sFileName, m_nLine);
          }
        }

        if(sTmp == "const")
        {
          CSP_ASSERT(sTypeName.empty(), "const after type declaration: ["
                     + sDataType + "]", m_stInterface.sFileName, m_nLine);
          rDataType.bIsConst = true;
        }
        else
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
        else
        {
          LogWarning() << "Can't find type declaration: " << sDataType;
        }
      }
    }

    // parameter
    void ParseParam(Param& rParameter)
    {
      rParameter.sDescr.erase();

      // read param type and name
      std::string sParamAndType;
      ReadBefore(sParamAndType, ",)");
      StringTrim(sParamAndType);
      std::string::size_type nPos = sParamAndType.find_last_of(" \n\r\t");
      CSP_ASSERT(nPos != std::string::npos, "Can't get param name: [" + sParamAndType + "]", m_stInterface.sFileName, m_nLine);
      std::string sDataType = sParamAndType.substr(0, nPos);
      StringTrim(sDataType);

      ParseDataType(sDataType, rParameter.stDataType);
      rParameter.sName = sParamAndType.substr(nPos + 1);
    }

    // member
    void ParseMember(Member& rMember)
    {
      Param stParam;
      char chData;
      std::string sTmp;

      rMember.bIsConst = false;
      rMember.bIsAsynch = false;

      SkipWs();

      // read return type and operation name
      std::string sOperationAndType;
      ReadBefore(sOperationAndType, "(");
      StringTrim(sOperationAndType);
      std::string::size_type nPos = sOperationAndType.find_last_of(" \n\r\t");
      CSP_ASSERT(nPos != std::string::npos, "Can't get operation name: [" + sOperationAndType + "]",
                 m_stInterface.sFileName, m_nLine);
      std::string sDataType = sOperationAndType.substr(0, nPos);
      StringTrim(sDataType);

      ParseDataType(sDataType, rMember.stReturn.stDataType);

      CSP_ASSERT(!rMember.stReturn.stDataType.bIsRef, "return value cannot be reference",
                 m_stInterface.sFileName, m_nLine);

      rMember.sName = sOperationAndType.substr(nPos + 1);

      SkipWs();
      m_tFile >> chData;
      CSP_ASSERT(chData == '(', "'(' expected after function name", m_stInterface.sFileName, m_nLine);

      SkipWs(); // arguments?
      chData = m_tFile.peek();
      if (chData == ')')
        m_tFile.ignore();
      else
        while (m_tFile.good())
        {
          CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after member name)", m_stInterface.sFileName, m_nLine);

          ParseParam(stParam); // reading param
          if (stParam.stDataType.sName == "ICallback")
          {
            if (!stParam.stDataType.bIsRef)
            {
              LogWarning() << "Callback must defined as reference: \n"
                  << "staff::ICallback<ReturnType>& rCallback\n"
                  << " in member: " << rMember.sName
                  << " in " << m_stInterface.sFileName << ":" << m_nLine;
            }

            CSP_ASSERT(!stParam.stDataType.lsParams.empty(), "Callback must define asynchronous return type: "
                        "staff::ICallback<ReturnType>& rCallback", m_stInterface.sFileName, m_nLine);

            CSP_ASSERT(rMember.stReturn.stDataType.sName == "void",
                       "Asynchronous operation must have void return type", m_stInterface.sFileName, m_nLine);

            rMember.bIsAsynch = true;
          }
          else
          if (stParam.stDataType.bIsRef && !stParam.stDataType.bIsConst &&
              stParam.stDataType.sName != "Operation")
          {
            LogWarning() << "Non-const reference to " << stParam.stDataType.sName
                << " in member: " << rMember.sName
                << " in " << m_stInterface.sFileName << ":" << m_nLine
                << " \n(return value cannot be passed over argument)";
          }

          SkipWs();
          m_tFile >> chData; // more arguments?
          rMember.lsParams.push_back(stParam);

          if (chData == ')')
          {
            m_tFile.ignore();
            break;
          }

          CSP_ASSERT(chData == ',', "error parsing param", m_stInterface.sFileName, m_nLine);
        }

      SkipWs();
      m_tFile >> chData;
      if (chData == 'c')
      {
        ReadStr(sTmp, false);
        if (sTmp == "onst") // const
          rMember.bIsConst = true;

        SkipWs();
        m_tFile >> chData;
      }
      CSP_ASSERT(chData == '=', "members must be pure virtual!", m_stInterface.sFileName, m_nLine);

      SkipWs();
      m_tFile >> chData;
      CSP_ASSERT(chData == '0', "members must be pure virtual!", m_stInterface.sFileName, m_nLine);

      SkipWs();
      m_tFile >> chData;
      CSP_ASSERT(chData == ';', "';' expected", m_stInterface.sFileName, m_nLine);

      SkipSingleLineComment();
    }

    // class
    void ParseClass(Class& rClass)
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
        Member stMember;
        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          StringTrim(sTmp);
          if (sTmp.size() != 0)
          {
            if (sTmp[0] == '*') // codegen metacomment
            {
              if (!CheckIgnoreMetacomment(sTmp))
              {
                std::string::size_type nPos = sTmp.find(':', 1);
                if (nPos != std::string::npos)
                {  // add an option
                  std::string sName = sTmp.substr(1, nPos - 1);
                  std::string sValue = sTmp.substr(nPos + 1);
                  StringTrim(sName);
                  StringTrim(sValue);
                  stMember.mOptions[sName] = sValue;
                }
              }
            }
            else
            if (sTmp[0] == '!')
            {
              std::string sDescrTmp = sTmp.substr(1);
              StringTrim(sDescrTmp);
              if (stMember.sDescr.size() != 0)
              {
                if (stMember.sDetail.size() != 0)
                {
                  stMember.sDetail += '\n';
                }
                stMember.sDetail += sDescrTmp;
              }
              else
              {
                stMember.sDescr = sDescrTmp;
              }
            }
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

        if (sTmp == "public:")
        {
        } else
        if (sTmp == "private:" || sTmp == "protected:")
        {
          LogWarning() << "all members of interface class must be only public!";
        }
        else
        if (sTmp.substr(0, rClass.sName.size()) == rClass.sName)   // constructor-ignore it
        {
          IgnoreFunction();
        }
        else
        if (sTmp.substr(0, rClass.sName.size() + 1) == "~" + rClass.sName)   // non virtual destructor
        {
          CSP_THROW("destructor must be virtual!", m_stInterface.sFileName, m_nLine);
        }
        else
        if (sTmp == "enum")   // enum -ignore
        {
          LogWarning() << "Enum in service definition: ignored";
          IgnoreFunction();
        }
        else
        if (sTmp == "virtual")
        {
          SkipWs();
          if (m_tFile.peek() == '~')
          {
            m_tFile.ignore();
            ReadBefore(sTmp);
            if (sTmp == rClass.sName)   // destructor-ignore it
              IgnoreFunction();
            else
              CSP_THROW("Non-valid destructor: ~" + sTmp + " but expected ~" + rClass.sName,
                        m_stInterface.sFileName, m_nLine);
          } else
          {
            ParseMember(stMember);
            for (StringMap::const_iterator itOption = stMember.mOptions.begin();
                 itOption != stMember.mOptions.end(); ++itOption)
            {
              const std::string& sOptionName = itOption->first;
              if (sOptionName.substr(0, 6) == "param-")
              {
                std::string::size_type nPos = sOptionName.find_last_of('-');
                if (nPos == std::string::npos)
                {
                  LogError() << "Invalid param option: " << sOptionName;
                }
                else
                {
                  const std::string& sParamName = sOptionName.substr(6, nPos - 6);


                  for (std::list<Param>::iterator itParam = stMember.lsParams.begin();
                       itParam != stMember.lsParams.end(); ++itParam)
                  {
                    if (itParam->sName == sParamName)
                    {
                      const std::string& sName = sOptionName.substr(nPos + 1);
                      // add an option
                      itParam->mOptions[sName] = itOption->second;
                      break;
                    }
                  }
                }
              }
            }
            rClass.lsMembers.push_back(stMember);
          }
        }
        else
        {
          CSP_THROW("all members must be pure virtual!", m_stInterface.sFileName, m_nLine);
        }
      }
    }

    void ParseStruct(Struct& rStruct)
    {
      bool bHasVirtualDtor = false;
      char chTmp = '\0';
      std::string sTmp;
      bool bFunction = false;
      const std::string& sOwnerName = (!rStruct.sOwnerName.empty() ?
                                        (rStruct.sOwnerName + "::") : "") + rStruct.sName;

      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after struct name): " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);

      ReadStr(sTmp);
      CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(after structname): " + rStruct.sName,
                 m_stInterface.sFileName, m_nLine);

      if (sTmp == ";")
      {
        m_tFile.unget();
        return;
      }
      else
      {
        CSP_ASSERT(rStruct.bForward, "Duplicating struct " + rStruct.sName, m_stInterface.sFileName, m_nLine);
      }

      if (sTmp == ":")
      { // inheritance
        ReadStr(sTmp);
        CSP_ASSERT(! m_tFile.eof(), "unexpected EOF(after structname and inheritance sign): " + rStruct.sName,
                   m_stInterface.sFileName, m_nLine);

        if (sTmp != "public")
        {
          LogWarning() << "non-public inheritance: " << rStruct.sName << " => " << sTmp;
        }
        else
        {
          ReadStr(sTmp);
          CSP_ASSERT(!m_tFile.eof(), "unexpected EOF(while reading parent struct name): " + rStruct.sName,
                     m_stInterface.sFileName, m_nLine);
        }

        rStruct.sParentName = sTmp;
        ReadStr(sTmp);
      }

      rStruct.bForward = false;

      CSP_ASSERT(sTmp == "{", "'{' or ';' after structname expected: " + rStruct.sName,
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
            if (sTmp[0] == '*') // codegen metacomment
            {
              if (!CheckIgnoreMetacomment(sTmp))
              {
                std::string::size_type nPos = sTmp.find(':', 1);
                if (nPos != std::string::npos)
                {  // add an option
                  std::string sName = sTmp.substr(1, nPos - 1);
                  std::string sValue = sTmp.substr(nPos + 1);
                  StringTrim(sName);
                  StringTrim(sValue);
                  stParamTmp.mOptions[sName] = sValue;
                }
              }
            }
            else
            if (sTmp[0] == '!') // doxygen metacomment
            {
              stParamTmp.sDescr = sTmp.substr(1);
              StringTrim(stParamTmp.sDescr);
            }
          }
          SkipWsOnly();
        }

        bFunction = false;
        std::string sToken;
        ReadBefore(sToken);
        StringTrim(sToken);

        if (sToken == "enum")
        {
          std::string sName;
          SkipWs();
          ReadBefore(sName, " \r\n\t;{}");

          Enum& rstEnum = TypeInList(rStruct.lsEnums, sName, m_sCurrentNamespace, sOwnerName);

          ParseEnum(rstEnum);
          if (!rstEnum.bForward)
          {
            rstEnum.sDescr = stParamTmp.sDescr;
            rstEnum.mOptions = stParamTmp.mOptions;
          }

          continue;
        }

        if (sToken == "struct")
        {
          std::string sName;
          SkipWs();
          ReadBefore(sName, " \r\n\t;{}");

          bool bNestedStruct = false;

          if (!sName.empty() && *sName.rbegin() == ':')
          {
            sName.resize(sName.size() - 1);
            bNestedStruct = true;
            m_tFile.unget();
          }
          else
          {
            SkipWs();
            char chData = m_tFile.peek();
            bNestedStruct = chData == ';' || chData == ':' || chData == '{';
          }

          if (bNestedStruct)
          { // nested struct
            Struct& rstStruct = TypeInList(rStruct.lsStructs, sName, m_sCurrentNamespace, sOwnerName);

            rstStruct.mOptions.insert(stParamTmp.mOptions.begin(), stParamTmp.mOptions.end());

            ParseStruct(rstStruct);
            if (!rstStruct.bForward)
            {
              rstStruct.sDescr = stParamTmp.sDescr;
            }

            continue;
          }
          else
          { // struct ::ns::Struct1 struct1;
            stParamTmp.stDataType.sPrefix = sToken;
            sToken += " " + sName;
          }
        }

        ReadBefore(sTmp, ";}(");
        StringTrim(sTmp);

        chTmp = m_tFile.peek();
        if (chTmp == '}')
        {
          CSP_ASSERT(sTmp.empty(), "\";\" expected while parsing field. in struct: " + rStruct.sName,
                     m_stInterface.sFileName, m_nLine);
          m_tFile.ignore();
          break;
        }

        sTmp = sToken + ' ' + sTmp;
        StringTrim(sTmp);

        if ((chTmp == '(') || // function
            (sToken == rStruct.sName) ||  // constructor
            (sToken == ("~" + rStruct.sName)))  // destructor
        {
          bFunction = true;
        }

        if (sTmp == ("virtual ~" + rStruct.sName))  // virtual destructor
        {
          bHasVirtualDtor = true;
        }

        // struct member
        if (!bFunction)
        {
          std::string::size_type nNameBegin = sTmp.find_last_of(" \n\r\t");
          CSP_ASSERT(nNameBegin != std::string::npos, "Can't detect field name. in struct: "
                     + rStruct.sName + " [" + sTmp + "]", m_stInterface.sFileName, m_nLine);

          stParamTmp.sName = sTmp.substr(nNameBegin + 1);

          sTmp.erase(nNameBegin);
          StringTrim(sTmp);

          ParseDataType(sTmp, stParamTmp.stDataType, &rStruct);

          SkipWs();
          m_tFile.get(chTmp);
          if (chTmp == ';')
          {
            CSP_ASSERT(!stParamTmp.stDataType.bIsConst, "Struct members must be non-const: " +
                       rStruct.sName, m_stInterface.sFileName, m_nLine);
            CSP_ASSERT(!stParamTmp.stDataType.bIsRef, "Struct members must be non-ref: " +
                       rStruct.sName, m_stInterface.sFileName, m_nLine);

            ReadDescrComment(stParamTmp.sDescr);

            rStruct.lsMembers.push_back(stParamTmp);
            //m_tStream >> SkipSingleLineComment;
          } else
          {
            bFunction = true;
          }
        }

        if (bFunction)
        {
          IgnoreFunction();
        }
      }

      SkipWsInLine();
      m_tFile >> chTmp;
      CSP_ASSERT(chTmp == ';', "missing ';' after struct definition", m_stInterface.sFileName, m_nLine);

      SkipSingleLineComment();

      StringMap::const_iterator itAbstract = rStruct.mOptions.find("abstract");
      if (itAbstract != rStruct.mOptions.end() &&
          (itAbstract->second == "true" || itAbstract->second == "1"))
      {
        CSP_ASSERT(bHasVirtualDtor, "\nStructure [" + rStruct.sNamespace + rStruct.sName +
                   "] marked as abstract, but it does not have virtual destructor.\n",
                   m_stInterface.sFileName, m_nLine);
      }
    }

    void ParseEnum(Enum& rEnum)
    {
      char chTmp = '\0';
      std::string sTmp;
      std::string sDescr;

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
      else
      {
        CSP_ASSERT(rEnum.bForward, "Duplicating enum " + rEnum.sName, m_stInterface.sFileName, m_nLine);
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
            if (sTmp[0] == '*') // codegen metacomment
            {
              if (!CheckIgnoreMetacomment(sTmp))
              {
                std::string::size_type nPos = sTmp.find(':', 1);
                if (nPos != std::string::npos)
                {
                  std::string sName = sTmp.substr(1, nPos - 1);
                  StringTrim(sName);
                  if (sName == "value")
                  {
                    stMember.sValue = sTmp.substr(nPos + 1);
                    StringTrim(stMember.sValue);
                  }
                }
              }
            }
          }
          SkipWsOnly();
        }

        ReadBefore(stMember.sName);
        SkipWs();

        chTmp = m_tFile.peek();
        if (chTmp == '=')
        { // read value
          m_tFile.ignore();
          SkipWs();
          ReadBefore(stMember.sValue);
          SkipWsOnly();
          if (!rEnum.lsMembers.empty())
          {
            ReadDescrComment(rEnum.lsMembers.back().sDescr);
          }
          SkipWs();
          chTmp = m_tFile.peek();
        }

        if (!stMember.sName.empty())
        {
          rEnum.lsMembers.push_back(stMember);
        }

        if (chTmp == '}')
        {
          m_tFile.ignore();
          break;
        }

        CSP_ASSERT(chTmp == ',', "Expected \",\" or \"}\" after enum member but \"" +
                   std::string(1, chTmp) + "\" found.", m_stInterface.sFileName, m_nLine);

        m_tFile.ignore();

        ReadDescrComment(rEnum.lsMembers.back().sDescr);
      }

      SkipWsInLine();
      m_tFile >> chTmp;
      CSP_ASSERT(chTmp == ';', "missing ';' after enum definition", m_stInterface.sFileName, m_nLine);

      SkipSingleLineComment();
    }

    void ParseTypedef(Typedef& rTypedef)
    {
      std::string sTmp;
      ReadBefore(sTmp, ";");
      StringTrim(sTmp);

      std::string::size_type nNameBegin = sTmp.find_last_of(" \n\r\t");
      CSP_ASSERT(nNameBegin != std::string::npos, "Can't detect typedef name: [" + sTmp + "]",
                 m_stInterface.sFileName, m_nLine);

      rTypedef.sName = sTmp.substr(nNameBegin + 1);

      sTmp.erase(nNameBegin);
      StringTrim(sTmp);

      ParseDataType(sTmp, rTypedef.stDataType);
      rTypedef.sNamespace = m_sCurrentNamespace;
    }

    void ImportEnums(const std::list<Enum>& rlsSrc, std::list<Enum>& rlsDst,
                     const StringMap& rmOptions)
    {
      for (std::list<Enum>::const_iterator itEnum = rlsSrc.begin();
          itEnum != rlsSrc.end(); ++itEnum)
      {
        rlsDst.push_back(*itEnum);
        Enum& rstEnum = rlsDst.back();
        rstEnum.bExtern = true;
        // does not overwrite existing keys and values
        rstEnum.mOptions.insert(rmOptions.begin(), rmOptions.end());
      }
    }

    void ImportStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst,
                      const StringMap& rmOptions)
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
        rstStruct.mOptions = itStruct->mOptions;
        // does not overwrite existing keys and values
        rstStruct.mOptions.insert(rmOptions.begin(), rmOptions.end());
        ImportEnums(itStruct->lsEnums, rstStruct.lsEnums, rmOptions);
        ImportStruct(itStruct->lsStructs, rstStruct.lsStructs, rmOptions);
      }
    }

    void ParsePreprocessorBlock()
    {
      std::string sTmp;
      m_tFile.ignore();
      ReadStr(sTmp, false);
      if (sTmp == "include")
      {
        SkipWs();
        char chData = m_tFile.peek();
        if (chData == '\"')
        {
          std::stringbuf sbTmp;
          m_tFile.ignore();
          m_tFile.get(sbTmp, chData);

          CppHeaderParser tCppHeaderParser(m_sRootNamespace);
          const Interface& rInterface = tCppHeaderParser.Parse(m_sInDir, sbTmp.str(), *m_pProject);

          // import extern enums
          ImportEnums(rInterface.lsEnums, m_stInterface.lsEnums, rInterface.mOptions);

          // import extern structs
          ImportStruct(rInterface.lsStructs, m_stInterface.lsStructs, rInterface.mOptions);

          // use extern typedefs
          for (std::list<Typedef>::const_iterator itTypedef = rInterface.lsTypedefs.begin();
              itTypedef != rInterface.lsTypedefs.end(); ++itTypedef)
          {
            Typedef stTypedef = *itTypedef;
            stTypedef.sName = itTypedef->sName;
            stTypedef.sNamespace = itTypedef->sNamespace;
            stTypedef.sDescr = itTypedef->sDescr;
            stTypedef.bExtern = true;
            m_stInterface.lsTypedefs.push_back(stTypedef);
          }

          Include stInclude;
          stInclude.sInterfaceName = rInterface.sName;
          stInclude.sNamespace = rInterface.sNamespace;
          stInclude.sFileName = rInterface.sFileName;
          stInclude.sFilePath = rInterface.sFilePath;
          stInclude.sTargetNs = StringMapValue(rInterface.mOptions, "targetNamespace");
          m_stInterface.lsIncludes.push_back(stInclude);
        }
      }

      m_tFile.ignore(INT_MAX, '\n');
      ++m_nLine;
    }

    void ParseHeaderBlock(Interface& rInterface)
    {
      char chData = 0;
      std::string sDescr;
      std::string sDetail;
      std::string sTmp;
      StringMap mOptions;
      StringList lsModules;

      SkipWsOnly();
      while (ReadComment(sTmp))
      {
        StringTrim(sTmp);
        if (sTmp.size() != 0)
        {
          if (sTmp[0] == '*') // codegen metacomment
          {
            if (!CheckIgnoreMetacomment(sTmp))
            {
              std::string::size_type nPos = sTmp.find(':', 1);
              if (nPos != std::string::npos)
              {  // add an option
                std::string sName = sTmp.substr(1, nPos - 1);
                std::string sValue = sTmp.substr(nPos + 1);
                StringTrim(sName);
                StringTrim(sValue);
                if (sName == "engageModule")
                {
                  lsModules.push_back(sValue);
                }
                else
                {
                  if (sName.substr(0, 10) == "interface.")
                  {
                    rInterface.mOptions[sName.substr(10)] = sValue;
                  }
                  else
                  {
                    mOptions[sName] = sValue;
                  }
                }
              }
            }
          }
          else
          if (sTmp[0] == '!')
          {
            std::string sDescrTmp = sTmp.substr(1);
            StringTrimLeft(sDescrTmp);
            if (sDescr.size() != 0)
            {
              if (sDetail.size() != 0)
              {
                sDetail += '\n';
              }
              sDetail += sDescrTmp;
            }
            else
            {
              sDescr = sDescrTmp;
            }
          }
        }
        SkipWsOnly();
      }

      chData = m_tFile.peek();
      if (chData == '#') // preprocessor
      {
        ParsePreprocessorBlock();
        return;
      }
      else
      if (chData == '}') // end of namespace
      {
        return;
      }

      ReadStr(sTmp);

      if (sTmp.size() == 0)
      {
        return; // eof
      }

      if (sTmp == "class")
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
        if (chTmp == '{')
        {
          IgnoreFunction(); // ignore non-service class
          return;
        }

        if (chTmp == ':') // inheritance
        {
          m_tFile.ignore();
        }

        ReadStr(sTmp);
        if (sTmp != "public")
        { // not our class
          ReadBefore(sTmp, "{;");
          IgnoreFunction(); // ignore non-service class
          return;
        }

        ReadStr(sTmp);
        if (sTmp != "IService" && sTmp != "staff::IService")
        { // not our class
          ReadBefore(sTmp, "{;");
          IgnoreFunction(); // ignore non-service class
          return;
        }

        LogDebug() << "Using [" << stClass.sName << "] as service class";

        ParseClass(stClass);
        stClass.sDescr = sDescr;
        stClass.sDetail = sDetail;
        stClass.lsModules = lsModules;
        stClass.mOptions = mOptions;
        rInterface.lsClasses.push_back(stClass);

        SkipWs();
        m_tFile >> chData;
        CSP_ASSERT(chData == ';', "missing ';' after class definition", m_stInterface.sFileName, m_nLine);

        SkipSingleLineComment();
        lsModules.clear();
        mOptions.clear();
        sDescr.erase();
        sDetail.erase();
      } else
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
          rstEnum.sDetail = sDetail;
          rstEnum.mOptions = mOptions;
        }

        sDescr.erase();
        sDetail.erase();
      } else
      if (sTmp == "struct")
      {
        std::string sName;
        SkipWs();
        ReadBefore(sName, " \r\n\t:;{}");

        Struct& rstStruct = TypeInList(rInterface.lsStructs, sName, m_sCurrentNamespace);

        rstStruct.mOptions.insert(mOptions.begin(), mOptions.end());

        ParseStruct(rstStruct);
        if (!rstStruct.bForward)
        {
          rstStruct.sDescr = sDescr;
          rstStruct.sDetail = sDetail;
        }

        sDescr.erase();
        sDetail.erase();
      } else
      if (sTmp == "typedef")
      {
        Typedef stTypedef;
        ParseTypedef(stTypedef);

        SkipWs();
        m_tFile >> chData;
        CSP_ASSERT(chData == ';', "missing ';' after typedef definition", m_stInterface.sFileName, m_nLine);

        ReadDescrComment(stTypedef.sDescr);

        stTypedef.mOptions = mOptions;
        rInterface.lsTypedefs.push_back(stTypedef);

        sDescr.erase();
        sDetail.erase();

  //      m_tStream >> SkipSingleLineComment;
      } else
      if (sTmp == "namespace")
      {
        std::string::size_type nNsSize = m_sCurrentNamespace.size();
        std::string sNamespace;
        m_tFile >> sNamespace;
        m_sCurrentNamespace += sNamespace + "::";

        ParseBracketedBlock(rInterface);

        m_sCurrentNamespace.erase(nNsSize);

      } else
      if (sTmp == ";")
      {
        SkipSingleLineComment();
      }
      else
      if (chData == '#') // preprocessor
      {
        ParsePreprocessorBlock();
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

        if (chData == '#') // preprocessor
        {
          ParsePreprocessorBlock();
        } else // text
        {
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
          return *itInterface;
        }
      }

      m_stInterface.sFileName = sInterfaceFileName;
      m_stInterface.sFilePath = sInterfaceFilePath;

      rProject.lsInterfaces.push_back(m_stInterface);
      Interface& rProjectThisInterface = rProject.lsInterfaces.back();

      m_tFile.open((m_sInDir + sFileName).c_str());
      CSP_ASSERT(m_tFile.good(), std::string("can't open file: ") + sFileName + ": "
                 + std::string(strerror(errno)), m_stInterface.sFileName, m_nLine);
      try
      {
        m_stInterface.sName = m_stInterface.sFileName.substr(m_stInterface.sFileName.size() - 2, 2) == ".h" ?
            m_stInterface.sFileName.substr(0, m_stInterface.sFileName.size() - 2) :
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
            if (!itStruct->sNamespace.empty() && !itStruct->bExtern)
            {
              m_stInterface.sNamespace = itStruct->sNamespace;
              break;
            }
          }
        }

        if (m_stInterface.sNamespace.empty())
        {
          for (std::list<Typedef>::const_iterator itTypedef = m_stInterface.lsTypedefs.begin();
              itTypedef != m_stInterface.lsTypedefs.end(); ++itTypedef)
          {
            if (!itTypedef->sNamespace.empty() && !itTypedef->bExtern)
            {
              m_stInterface.sNamespace = itTypedef->sNamespace;
              break;
            }
          }
        }

        if (m_stInterface.sNamespace.empty())
        {
          for (std::list<Enum>::const_iterator itEnum = m_stInterface.lsEnums.begin();
               itEnum != m_stInterface.lsEnums.end(); ++itEnum)
          {
            if (!itEnum->sNamespace.empty() && !itEnum->bExtern)
            {
              m_stInterface.sNamespace = itEnum->sNamespace;
              break;
            }
          }
        }

        rProjectThisInterface = m_stInterface;
        m_tFile.close();
      }
      catch (ParseException& rParseException)
      {
        std::stringbuf sbData;
        m_tFile.get(sbData, '\n');
        m_tFile.ignore();
        m_tFile.get(sbData, '\n');
        std::string sBefore = sbData.str();
        StringTrim(sBefore);

        if (!sBefore.empty())
        {
          rParseException.Message() += ": before\n-----------------\n" + sBefore + "\n-----------------\n";
        }

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
        // skip structs without parent
        if (sNsParent.empty())
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


  const std::string& CppParser::GetId()
  {
    return m_sId;
  }

  void CppParser::Process(const ParseSettings& rParseSettings, Project& rProject)
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
      CppHeaderParser tCppHeaderParser(sRootNs);
      const Interface& rInterface = tCppHeaderParser.Parse(rParseSettings.sInDir, *itFile, rProject);
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

    if (!uServicesCount && !rParseSettings.bNoServiceWarn && !rProject.lsInterfaces.empty())
    {
       LogWarning() <<
           "No staff service interfaces found. Staff services must inherited from staff::IService.\n"
            "Example:\n----\n#include <staff/common/IService.h>\n\n  class Calc: public staff::IService\n"
            "  {\n  public:\n    virtual int Add(int nA, int nB) = 0;\n  };\n----\n\n";
    }
  }

  const std::string CppParser::m_sId = "cpp";
}
}
