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
#include <rise/common/Log.h>
#include <rise/common/exmacros.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/plugin/PluginExport.h>
#include "CppParser.h"

RISE_DECLARE_PLUGIN(staff::CCppParser);

namespace staff
{
  class CCppHeaderParser
  {
  public:
    CCppHeaderParser():
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
                rise::StrTrim(sDescr);
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
      for(;;)
      {
        const std::list<SStruct>& rStructList = !pstCurr ? m_stInterface.lsStruct : pstCurr->lsStruct;
        for (std::list<SStruct>::const_iterator itStruct = rStructList.begin();
          itStruct != rStructList.end(); ++itStruct)
        {
//          if (!itStruct->bForward) // skip forward declarations
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
                  && (nPos == nCurrNsNameSize ||
                      (nCurrNsNameSize > (nPos + 2) && sCurrNsName.substr(nCurrNsNameSize - nPos - 2, 2) == "::"))
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
        }

        if (pstResult || !pstCurr)
        {
          break;
        }

        // find in parent owner struct
        pstCurr = pstCurr->sOwnerName.empty() ? NULL :
                  GetStruct(pstCurr->sNamespace + pstCurr->sOwnerName);
      }

      return pstResult;
    }

    void GetDataType(const std::string& sDataTypeName, SDataType& rDataType)
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

      if (sDataTypeName == "staff::CDataObject" ||
          (sDataTypeName == "CDataObject" && m_sCurrentNamespace.substr(0, 9) == "::staff::"))
      {
        rDataType.eType = SDataType::EDataObject;
      }
      else
      if (sDataTypeName == "staff::CMessageContext" || sDataTypeName == "staff::COperation" ||
          ((sDataTypeName == "CMessageContext" || sDataTypeName == "COperation")
              && m_sCurrentNamespace.substr(0, 9) == "::staff::"))
      {
        rDataType.eType = SDataType::EGeneric; // supress unknown datatype warning
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

          sDataTypeName == "rise::byte" ||
          sDataTypeName == "rise::word" ||
          sDataTypeName == "rise::ushort" ||
          sDataTypeName == "rise::dword" ||
          sDataTypeName == "rise::ulong" ||
          sDataTypeName == "rise::uint" ||
          sDataTypeName == "rise::TSize" ||

          sDataTypeName == "staff::CValue" ||

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
          sDataTypeName == "staff::positiveInteger"
        )
      {
        rDataType.eType = SDataType::EGeneric;
      }
      else
      if (
        sDataTypeName == "std::string" ||
        sDataTypeName == "std::wstring" ||

        sDataTypeName == "rise::CString" ||
        sDataTypeName == "rise::CStringA" ||
        sDataTypeName == "rise::CStringW" ||

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
        sDataTypeName == "staff::anySimpleType"
      )
      {
        rDataType.eType = SDataType::EString;
      }
      else
      if(ParseCompositeDataType(m_stInterface.lsTypedef, rDataType))
      {
        rDataType.eType = SDataType::ETypedef;
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
            rise::StrTrim(sType);

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
        rDataType.eType = bGeneric ? SDataType::EGeneric : SDataType::EUnknown;
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

    std::string::size_type ParseTemplate(const std::string& sTemplate, SDataType& rDataType, const SStruct* pstParent)
    {
      std::string::size_type nResult = 0;
      std::string sToken;
      for (std::string::size_type nBegin = 0, nEnd = 0;
           nEnd != std::string::npos; nBegin = nEnd + 1)
      {
        nEnd = sTemplate.find_first_of(",<>", nBegin);
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

          sToken = sTemplate.substr(nBegin, nEnd - nBegin);
        }

        rise::StrTrim(sToken);
        if (!sToken.empty())
        {
          SDataType stTemplParam;
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
          rise::LogWarning() << "members must be non-static. Line:" << m_nLine;
          CSP_ASSERT(sTypeName.empty(), "static after typename", m_stInterface.sFileName, m_nLine);
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

            rDataType.eType = SDataType::ETemplate;

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
          rDataType.sName = (pstStruct->sOwnerName.empty() ? "" : (pstStruct->sOwnerName + "::"))
                            + pstStruct->sName;

          rDataType.sUsedName = sTypeName;
        }
      }
    }

    // parameter
    void ParseParam( SParam& rParameter )
    {
      rParameter.sDescr.erase();

      // read param type and name
      std::string sParamAndType;
      ReadBefore(sParamAndType, ",)");
      rise::StrTrim(sParamAndType);
      std::string::size_type nPos = sParamAndType.find_last_of(" \n\r\t");
      CSP_ASSERT(nPos != std::string::npos, "Can't get param name: [" + sParamAndType + "]", m_stInterface.sFileName, m_nLine);
      std::string sDataType = sParamAndType.substr(0, nPos);
      rise::StrTrim(sDataType);

      ParseDataType(sDataType, rParameter.stDataType);
      rParameter.sName = sParamAndType.substr(nPos + 1);
      // rise::StrTrim(rParameter.sName); // already between spaces

      rParameter.stDataType.sNodeName = rParameter.sName;
    }

    // member
    void ParseMember( SMember& rMember )
    {
      SParam stParam;
      char chData;
      std::string sTmp;

      rMember.bIsConst = false;
      rMember.bIsAsynch = false;

      SkipWs();

      // read return type and operation name
      std::string sOperationAndType;
      ReadBefore(sOperationAndType, "(");
      std::string::size_type nPos = sOperationAndType.find_last_of(" \n\r\t");
      CSP_ASSERT(nPos != std::string::npos, "Can't get operation name: [" + sOperationAndType + "]",
                 m_stInterface.sFileName, m_nLine);
      std::string sDataType = sOperationAndType.substr(0, nPos);
      rise::StrTrim(sDataType);

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
              rise::LogWarning() << "Callback must defined as reference: \n"
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
              stParam.stDataType.sName != "COperation")
          {
            rise::LogWarning() << "Non-const reference to " << stParam.stDataType.sName
                << " in member: " << rMember.sName
                << " in " << m_stInterface.sFileName << ":" << m_nLine
                << " \n(return value cannot be passed over argument)";
          }

          SkipWs();
          m_tFile >> chData; // more arguments?
          rMember.lsParamList.push_back(stParam);

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
            if (sTmp[0] == '*') // codegen metacomment
            {
              std::string::size_type nPos = sTmp.find(':', 1);
              if (nPos != std::string::npos)
              {  // add an option
                std::string sName = sTmp.substr(1, nPos - 1);
                std::string sValue = sTmp.substr(nPos + 1);
                rise::StrTrim(sName);
                rise::StrTrim(sValue);
                stMember.mOptions[sName] = sValue;
              }
            }
            else
            if (sTmp[0] == '!')
            {
              std::string sDescrTmp = sTmp.substr(1);
              rise::StrTrim(sDescrTmp);
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
          rise::LogWarning() << "all members of interface class must be only public!";
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
            rClass.lsMember.push_back(stMember);
          }
        }
        else
        {
          CSP_THROW("all members must be pure virtual!", m_stInterface.sFileName, m_nLine);
        }
      }
    }

    void ParseStruct( SStruct& rStruct )
    {
      char chTmp = '\0';
      std::string sTmp;
      std::string sDescr;
      bool bFunction = false;

      rStruct.sNamespace = m_sCurrentNamespace;
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
          rise::LogWarning() << "non-public inheritance: " << rStruct.sName << " => " << sTmp;
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
        SParam stParamTmp;

        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          rise::StrTrim(sTmp);
          if (sTmp.size() != 0)
          {
            if (sTmp[0] == '*') // codegen metacomment
            {
              std::string::size_type nPos = sTmp.find(':', 1);
              if (nPos != std::string::npos)
              {  // add an option
                std::string sName = sTmp.substr(1, nPos - 1);
                std::string sValue = sTmp.substr(nPos + 1);
                rise::StrTrim(sName);
                rise::StrTrim(sValue);
                stParamTmp.mOptions[sName] = sValue;
              }
            }
            else
            if (sTmp[0] == '!') // doxygen metacomment
            {
              sDescr = sTmp.substr(1);
              rise::StrTrim(sDescr);
            }
          }
          SkipWsOnly();
        }

        bFunction = false;
        std::string sToken;
        ReadBefore(sToken);
        rise::StrTrim(sToken);

        if (sToken == "enum")   // enum in struct -ignore
        {
          IgnoreFunction();
          continue;
        }

        if (sToken == "struct")
        {
          rStruct.lsStruct.push_back(SStruct());
          SStruct& rstSubStruct = rStruct.lsStruct.back();
          rstSubStruct.sOwnerName = (!rStruct.sOwnerName.empty() ?
                                (rStruct.sOwnerName + "::") : "") + rStruct.sName;
          rstSubStruct.sDescr = sDescr;

          SkipWs();
          ReadBefore(rstSubStruct.sName, " \r\n\t:;{}");

          ParseStruct(rstSubStruct);
          m_tFile.get(chTmp);
          CSP_ASSERT(chTmp == ';', "\";\" expected while after substruct. in struct: " + rStruct.sName,
                     m_stInterface.sFileName, m_nLine);
          continue;
        }

        ReadBefore(sTmp, ";}(");
        rise::StrTrim(sTmp);

        chTmp = m_tFile.peek();
        if (chTmp == '}')
        {
          CSP_ASSERT(sTmp.empty(), "\";\" expected while parsing field. in struct: " + rStruct.sName,
                     m_stInterface.sFileName, m_nLine);
          m_tFile.ignore();
          break;
        }

        sTmp = sToken + ' ' + sTmp;
        rise::StrTrim(sTmp);

        if ((chTmp == '(') || // function
            (sToken == rStruct.sName) ||  // constructor
            (sToken == ("~" + rStruct.sName)))  // destructor
        {
          bFunction = true;
        }

        if (!bFunction)
        {
          std::string::size_type nNameBegin = sTmp.find_last_of(" \n\r\t");
          CSP_ASSERT(nNameBegin != std::string::npos, "Can't detect field name. in struct: "
                     + rStruct.sName + " [" + sTmp + "]", m_stInterface.sFileName, m_nLine);

          stParamTmp.sName = sTmp.substr(nNameBegin + 1);

          sTmp.erase(nNameBegin);
          rise::StrTrim(sTmp);

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

            rStruct.lsMember.push_back(stParamTmp);
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

        sDescr.erase();
      }
    }

    void ParseTypedef( STypedef& rTypedef )
    {
      std::string sTmp;
      ReadBefore(sTmp, ";");
      rise::StrTrim(sTmp);

      std::string::size_type nNameBegin = sTmp.find_last_of(" \n\r\t");
      CSP_ASSERT(nNameBegin != std::string::npos, "Can't detect typedef name: [" + sTmp + "]",
                 m_stInterface.sFileName, m_nLine);

      rTypedef.sName = sTmp.substr(nNameBegin + 1);

      sTmp.erase(nNameBegin);
      rise::StrTrim(sTmp);

      ParseDataType(sTmp, rTypedef.stDataType);
      rTypedef.sNamespace = m_sCurrentNamespace;
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

    void ParsePreprocessorBlock()
    {
      char chData = '\0';
      std::string sTmp;
      m_tFile.ignore();
      ReadStr(sTmp, false);
      if (sTmp == "include")
      {
        SkipWs();
        chData = m_tFile.peek();
        if (chData == '\"')
        {
          std::stringbuf sbTmp;
          m_tFile.ignore();
          m_tFile.get(sbTmp, chData);

          CCppHeaderParser tCppHeaderParser;
          const SInterface& rInterface = tCppHeaderParser.Parse(m_sInDir, sbTmp.str(), *m_pProject);

          // import extern structs
          ImportStruct(rInterface.lsStruct, m_stInterface.lsStruct);

          // use extern typedefs
          for (std::list<STypedef>::const_iterator itTypedef = rInterface.lsTypedef.begin();
              itTypedef != rInterface.lsTypedef.end(); ++itTypedef)
          {
            STypedef stTypedef = *itTypedef;
            stTypedef.sName = itTypedef->sName;
            stTypedef.sNamespace = itTypedef->sNamespace;
            stTypedef.sDescr = itTypedef->sDescr;
            stTypedef.bExtern = true;
            m_stInterface.lsTypedef.push_back(stTypedef);
          }

          SInclude stInclude;
          stInclude.sInterfaceName = rInterface.sName;
          stInclude.sNamespace = rInterface.sNamespace;
          stInclude.sFileName = rInterface.sFileName;
          stInclude.sTargetNs = rInterface.sTargetNs;
          m_stInterface.lsInclude.push_back(stInclude);
        }
      }

      m_tFile.ignore(INT_MAX, '\n');
      ++m_nLine;
    }

    void ParseHeaderBlock( SInterface& rInterface )
    {
      char chData = 0;
      std::string sDescr;
      std::string sDetail;
      std::string sTmp;
      TStringMap mOptions;
      TStringList lsModules;

      SkipWsOnly();
      while (ReadComment(sTmp))
      {
        rise::StrTrim(sTmp);
        if (sTmp.size() != 0)
        {
          if (sTmp[0] == '*') // codegen metacomment
          {
            std::string::size_type nPos = sTmp.find(':', 1);
            if (nPos != std::string::npos)
            {  // add an option
              std::string sName = sTmp.substr(1, nPos - 1);
              std::string sValue = sTmp.substr(nPos + 1);
              rise::StrTrim(sName);
              rise::StrTrim(sValue);
              if (sName == "engageModule")
              {
                lsModules.push_back(sValue);
              }
              else
              {
                if (sName == "targetNamespace")
                {
                  rInterface.sTargetNs = sValue;
                }
                mOptions[sName] = sValue;
              }
            }
          }
          else
          if (sTmp[0] == '!')
          {
            std::string sDescrTmp = sTmp.substr(1);
            rise::StrTrimLeft(sDescrTmp);
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

        rise::LogDebug() << "Using [" << stClass.sName << "] as service class";

        ParseClass(stClass);
        stClass.sDescr = sDescr;
        stClass.sDetail = sDetail;
        stClass.lsModules = lsModules;
        stClass.mOptions = mOptions;
        rInterface.lsClass.push_back(stClass);

        SkipWs();
        m_tFile >> chData;
        CSP_ASSERT(chData == ';', "missing ';' after class definition", m_stInterface.sFileName, m_nLine);

        SkipSingleLineComment();
        lsModules.clear();
        mOptions.clear();
        sDescr.erase();
        sDetail.erase();
      } else
      if (sTmp == "struct")
      {
        std::string sName;
        SkipWs();
        ReadBefore(sName, " \r\n\t:;{}");

        std::list<SStruct>::iterator itNewStruct = rInterface.lsStruct.end();

        // check for forward declaration
        for (std::list<SStruct>::iterator itStruct = rInterface.lsStruct.begin();
            itStruct != rInterface.lsStruct.end();)
        {
          if (itStruct != itNewStruct &&
              itStruct->sName == sName &&
              itStruct->sNamespace == m_sCurrentNamespace)
          {
            itNewStruct = itStruct; // use existing struct
            break;
          }
          else
          {
            ++itStruct;
          }
        }

        if (itNewStruct == rInterface.lsStruct.end())
        { // add new
          itNewStruct = rInterface.lsStruct.insert(rInterface.lsStruct.end(), SStruct());
          itNewStruct->sName = sName;
        }

        SStruct& rstStruct = *itNewStruct;
        rstStruct.sDescr = sDescr;
        rstStruct.sDetail = sDetail;
        ParseStruct(rstStruct);


        SkipWs();
        m_tFile >> chData;
        CSP_ASSERT(chData == ';', "missing ';' after struct definition", rInterface.sFileName, m_nLine);

        sDescr.erase();
        sDetail.erase();

        SkipSingleLineComment();
      } else
      if (sTmp == "typedef")
      {
        STypedef stTypedef;
        ParseTypedef(stTypedef);

        SkipWs();
        m_tFile >> chData;
        CSP_ASSERT(chData == ';', "missing ';' after typedef definition", m_stInterface.sFileName, m_nLine);

        ReadDescrComment(stTypedef.sDescr);

        rInterface.lsTypedef.push_back(stTypedef);

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
      if (chData == '#') // preprocessor
      {
        ParsePreprocessorBlock();
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
        m_stInterface.sName = m_stInterface.sFileName.substr(m_stInterface.sFileName.size() - 2, 2) == ".h" ?
            m_stInterface.sFileName.substr(0, m_stInterface.sFileName.size() - 2) :
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

        if (m_stInterface.sNamespace.empty())
        {
          for (std::list<STypedef>::const_iterator itTypedef = m_stInterface.lsTypedef.begin();
              itTypedef != m_stInterface.lsTypedef.end(); ++itTypedef)
          {
            if (!itTypedef->sNamespace.empty())
            {
              m_stInterface.sNamespace = itTypedef->sNamespace;
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

    std::string m_sInDir;
    std::string m_sCurrentNamespace;
    std::ifstream m_tFile;
    int m_nLine;
    SInterface m_stInterface;
    SProject* m_pProject;
  };


  const std::string& CCppParser::GetId()
  {
    return m_sId;
  }

  void CCppParser::Process(const SParseSettings& rParseSettings, SProject& rProject)
  {
    unsigned uServicesCount = 0;
    for (TStringList::const_iterator itFile = rParseSettings.lsFiles.begin();
        itFile != rParseSettings.lsFiles.end(); ++itFile)
    {
      CCppHeaderParser tCppHeaderParser;
      const SInterface& rInterface = tCppHeaderParser.Parse(rParseSettings.sInDir + "/", *itFile, rProject);
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

  const std::string CCppParser::m_sId = "cpp";
}
