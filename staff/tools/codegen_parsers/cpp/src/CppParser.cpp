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
              continue;
            } else
            if (m_tFile.peek() == '*') // multiline comment
            {
              while (m_tFile.good() && !m_tFile.eof())
              {
                m_tFile.ignore(INT_MAX, '*');
                m_tFile.get(chData);
                if(chData == '/')
                  break;
              }
            } else
            {
              m_tFile.unget();
              break;
            }
          } else
            break;
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
          } else
          {
            m_tFile.unget();
          }
        }
      }
    }

    void ReadStr(std::string& sString, bool bSkipWS = true)
    {
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
        if(sDelim.find(chData) != std::string::npos)
        {
          break;
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
          if(chData == '*')
          {
            if (m_tFile.peek() == '/')
            {
              m_tFile.ignore();
              break;
            }
            sComment += chData;
          }
        }
      }
      else
      {
        m_tFile.unget();
      }

      return sComment.size() != 0;
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

    void ParseDataType(const std::string& sDataTypeName, SDataType& rDataType)
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

      if (sDataTypeName == "staff::CDataObject" ||
          (sDataTypeName == "CDataObject" && m_sCurrentNamespace.substr(0, 9) == "::staff::"))
      {
        rDataType.eType = SDataType::EDataObject;
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
      if(ParseCompositeDataType(m_stInterface.lsStruct, rDataType))
      {
        rDataType.eType = SDataType::EStruct;
      }
      else
      if(ParseCompositeDataType(m_stInterface.lsTypedef, rDataType))
      {
        rDataType.eType = SDataType::ETypedef;
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
          if (nRecursion < 0)
          {
            CSP_THROW("mismatch {}", m_stInterface.sFileName, m_nLine);
          }

          if (nRecursion == 0)
          {
            while (m_tFile.peek() == ';')
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
    void ParseDataType( SDataType& rDataType )
    {
      char chTmp = '\0';
      std::string sTmp;

      rDataType.bIsConst = false;
      rDataType.bIsRef = false;
      rDataType.eType = SDataType::EUnknown;

      while (m_tFile.good())
      {
        SkipWs();
        ReadBefore(sTmp);
        if(m_tFile.eof())
        {
          CSP_THROW("unexpected EOF(after type parsing)", m_stInterface.sFileName, m_nLine);
        }

        if (sTmp == "static")
        {
          rise::LogWarning() << "members must be non-static: Line:" << m_nLine;
        }

        chTmp = m_tFile.peek();
        if (chTmp == '<')
        {
          m_tFile.ignore();
          rDataType.sName = sTmp;
          rDataType.sNamespace = "";
          rDataType.eType = SDataType::ETemplate;
          while(m_tFile.good())
          {
            SDataType stTemplParam;
            ParseDataType(stTemplParam);
            SkipWs();
            chTmp = m_tFile.peek();
            rDataType.lsParams.push_back(stTemplParam);
            if (chTmp == '>')
            {
              m_tFile.ignore();

              SkipWs();
              chTmp = m_tFile.peek();
              if(chTmp == '&')
              {
                rDataType.bIsRef = true;
                m_tFile.ignore();
              }

              return;
            } else
            if (chTmp == ',')
            {
              m_tFile.ignore();
            }
            else
            {
              CSP_THROW(" \",\" or \">\" expected while parsing template ", m_stInterface.sFileName, m_nLine);
            }
          }
          break;
        } else
        if(chTmp == '&')
        {
          rDataType.bIsRef = true;
          m_tFile.ignore();
        }
        else
        if (sTmp == "")
        {
          return;
        }

        if(sTmp == "const")
        {
          rDataType.bIsConst = true;
        }
        else // name of type
        {
          if (sTmp == "unsigned")
          {
            ReadStr(sTmp);
            if(m_tFile.eof())
              CSP_THROW("unexpected EOF(after type parsing)", m_stInterface.sFileName, m_nLine);

            ParseDataType(sTmp, rDataType);
            rDataType.sName = "unsigned " + sTmp;
          } else
          {
            ParseDataType(sTmp, rDataType);
          }
          break;
        }
      }

      if (rDataType.bIsRef && !rDataType.bIsConst)
      {
        rise::LogWarning() << "Non-const reference to " << rDataType.sName << " at line " << m_nLine << " \n(return value cannot be passed over argument)";
      }
    }

    // parameter
    void ParseParam( SParam& rParameter )
    {
      ParseDataType(rParameter.stDataType);
      SkipWs();
      ReadBefore(rParameter.sName);
      rParameter.stDataType.sNodeName = rParameter.sName;
    }

    // member
    void ParseMember( SMember& rMember )
    {
      SParam stParam;
      char chData;
      std::string sTmp;
      std::stringbuf tStreamBuff;

      rMember.bIsConst = false;

      ParseDataType(rMember.stReturn.stDataType);

      if (rMember.stReturn.stDataType.bIsRef)
        CSP_THROW("return value cannot be reference", m_stInterface.sFileName, m_nLine);

      SkipWs();

      m_tFile.get(tStreamBuff, '('); // parameters begin

      rMember.sName = tStreamBuff.str();
      rise::StrTrim(rMember.sName);

      SkipWs();
      m_tFile >> chData;
      if (chData != '(')
        CSP_THROW("'(' expected after function name", m_stInterface.sFileName, m_nLine);

      SkipWs(); // arguments?
      chData = m_tFile.peek();
      if (chData == ')')
        m_tFile.ignore();
      else
        while (m_tFile.good())
        {
          if(m_tFile.eof())
            CSP_THROW("unexpected EOF(after member name)", m_stInterface.sFileName, m_nLine);

          ParseParam(stParam); // reading param

          SkipWs();
          m_tFile >> chData; // more arguments?
          rMember.lsParamList.push_back(stParam);

          if (chData == ')')
          {
            m_tFile.ignore();
            break;
          }

          if (chData != ',')
            CSP_THROW("error parsing param", m_stInterface.sFileName, m_nLine);
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
      if (chData != '=')
        CSP_THROW("members must be pure virtual!", m_stInterface.sFileName, m_nLine);

      SkipWs();
      m_tFile >> chData;
      if (chData != '0')
        CSP_THROW("members must be pure virtual!", m_stInterface.sFileName, m_nLine);

      SkipWs();
      m_tFile >> chData;
      if (chData != ';')
        CSP_THROW("';' expected", m_stInterface.sFileName, m_nLine);

      SkipSingleLineComment();
    }

    // class
    void ParseClass( SClass& rClass )
    {
      char chTmp = '\0';
      std::string sTmp;
      std::string sSoapAction;
      std::string sDescr;
      std::string sDetail;
      std::string sRequestElement;
      std::string sResponseElement;
      std::string sResultElement;
      std::string sRestMethod;
      std::string sRestLocation;

      SkipWs();
      rClass.sNamespace = m_sCurrentNamespace;

      ReadStr(sTmp);
      if(m_tFile.eof())
      {
        CSP_THROW("unexpected EOF(after classname and '{')", m_stInterface.sFileName, m_nLine);
      }

      if (sTmp != "{")
      {
        CSP_THROW("'{' after classname expected ", m_stInterface.sFileName, m_nLine);
      }

      while (m_tFile.good() && !m_tFile.eof())
      {
        sSoapAction.erase();
        sDescr.erase();
        sDetail.erase();
        sRequestElement.erase();
        sResponseElement.erase();
        sResultElement.erase();
        sRestMethod.erase();
        sRestLocation.erase();

        SkipWsOnly();
        while (ReadComment(sTmp))
        {
          rise::StrTrim(sTmp);
          if (sTmp.size() != 0)
          {
            if (sTmp.substr(0, 11) == "soapAction:")
            {
              sSoapAction = sTmp.substr(12);
              rise::StrTrimLeft(sSoapAction);
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
            else
            if (sTmp.substr(0, 12) == "description:")
            {
              std::string sDescrTmp = sTmp.substr(13);
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
            else
            if (sTmp.substr(0, 15) == "requestElement:")
            {
              sRequestElement= sTmp.substr(15);
              rise::StrTrimLeft(sRequestElement);
            }
            else
            if (sTmp.substr(0, 16) == "responseElement:")
            {
              sResponseElement = sTmp.substr(16);
              rise::StrTrimLeft(sResponseElement);
            }
            else
            if (sTmp.substr(0, 14) == "resultElement:")
            {
              sResultElement = sTmp.substr(14);
              rise::StrTrimLeft(sResultElement);
            }
            else
            if (sTmp.substr(0, 11) == "restMethod:")
            {
              sRestMethod = sTmp.substr(11);
              rise::StrTrimLeft(sRestMethod);
            }
            else
            if (sTmp.substr(0, 13) == "restLocation:")
            {
              sRestLocation = sTmp.substr(13);
              rise::StrTrimLeft(sRestLocation);
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
              CSP_THROW("Non-valid destructor: ~" + sTmp + " but expected ~" + rClass.sName, m_stInterface.sFileName, m_nLine);
          } else
          {
            SMember stMember;

            ParseMember(stMember);
            stMember.sDescr = sDescr;
            stMember.sDetail = sDetail;
            stMember.sSoapAction = sSoapAction;
            stMember.sNodeName = sRequestElement;
            stMember.stReturn.sName = sResponseElement;
            stMember.stReturn.stDataType.sNodeName = sResultElement;
            stMember.sRestMethod = sRestMethod;
            stMember.sRestLocation = sRestLocation;
            rClass.lsMember.push_back(stMember);
          }
        }
        else
          CSP_THROW("all members must be pure virtual!", m_stInterface.sFileName, m_nLine);
      }
    }

    void ParseStruct( SStruct& rStruct )
    {
      char chTmp = '\0';
      std::string sTmp;
      bool bFunction = false;

      SkipWs();
      ReadBefore(rStruct.sName, " \r\n\t:;{}");
      rStruct.sNamespace = m_sCurrentNamespace;
      if(m_tFile.eof())
      {
        CSP_THROW("unexpected EOF(after struct name): " + rStruct.sName, m_stInterface.sFileName, m_nLine);
      }

      ReadStr(sTmp);
      if(m_tFile.eof())
      {
        CSP_THROW("unexpected EOF(after structname): " + rStruct.sName, m_stInterface.sFileName, m_nLine);
      }

      if (sTmp == ";")
      {
        m_tFile.unget();
        return;
      }

      if (sTmp == ":")
      { // inheritance
        ReadStr(sTmp);
        if(m_tFile.eof())
        {
          CSP_THROW("unexpected EOF(after structname and inheritance sign): " + rStruct.sName, m_stInterface.sFileName, m_nLine);
        }

        if (sTmp != "public")
        {
          rise::LogWarning() << "non-public inheritance: " << rStruct.sName << " => " << sTmp;
        }
        else
        {
          ReadStr(sTmp);
          if(m_tFile.eof())
          {
            CSP_THROW("unexpected EOF(while reading parent struct name): " + rStruct.sName, m_stInterface.sFileName, m_nLine);
          }
        }

        rStruct.sParentName = sTmp;
        ReadStr(sTmp);
      }

      rStruct.bForward = false;

      if (sTmp != "{")
      {
        CSP_THROW("'{' or ';' after structname expected: " + rStruct.sName, m_stInterface.sFileName, m_nLine);
      }

      while (m_tFile.good() && !m_tFile.eof())
      {
        SParam stParamTmp;

        bFunction = false;
        SkipWs();
        chTmp = m_tFile.peek();
        if (chTmp == '}')
        {
          m_tFile.ignore();
          break;
        }

        ParseDataType(stParamTmp.stDataType);

        if (stParamTmp.stDataType.sName == "enum")   // enum -ignore
        {
          IgnoreFunction();
          continue;
        }

        if ((stParamTmp.stDataType.sName.find('(') != std::string::npos) ||
            (stParamTmp.stDataType.sName == rStruct.sName) ||  // constructor
            (stParamTmp.stDataType.sName == ("~" + rStruct.sName)) ||  // destructor
            (stParamTmp.stDataType.sName == "operator"))
        {
          bFunction = true;
        }

        if (!bFunction)
        {
          SkipWs();
          ReadBefore(stParamTmp.sName);
          SkipWs();
          m_tFile.get(chTmp);
          if (chTmp == ';')
          {
            if (stParamTmp.stDataType.bIsConst)
              CSP_THROW("Struct members must be non-const: " + rStruct.sName, m_stInterface.sFileName, m_nLine);
            if (stParamTmp.stDataType.bIsRef)
              CSP_THROW("Struct members must be non-ref: " + rStruct.sName, m_stInterface.sFileName, m_nLine);

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
      }
    }

    void ParseTypedef( STypedef& rTypedef )
    {
      ParseDataType(rTypedef.stDataType);
      SkipWs();
      ReadBefore(rTypedef.sName);
      rTypedef.sNamespace = m_sCurrentNamespace;
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

          // use extern structs
          for (std::list<SStruct>::const_iterator itStruct = rInterface.lsStruct.begin();
              itStruct != rInterface.lsStruct.end(); ++itStruct)
          {
            SStruct stStruct;
            stStruct.sName = itStruct->sName;
            stStruct.sNamespace = itStruct->sNamespace;
            stStruct.sParentName = itStruct->sParentName;
            stStruct.sDescr = itStruct->sDescr;
            stStruct.sDetail = itStruct->sDetail;
            stStruct.bExtern = true;
            m_stInterface.lsStruct.push_back(stStruct);
          }

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
    }

    void ParseHeaderBlock( SInterface& rInterface )
    {
      char chData = 0;
      std::string sServiceUri;
      std::string sDescr;
      std::string sDetail;
      std::string sTmp;
      bool bLoadAtStartup = false;

      SkipWsOnly();
      while (ReadComment(sTmp))
      {
        rise::StrTrim(sTmp);
        if (sTmp.size() != 0)
        {
          if (sTmp.substr(0, 4) == "uri:")
          {
            sServiceUri = sTmp.substr(5);
            rise::StrTrimLeft(sServiceUri);
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
          else
          if (sTmp.substr(0, 12) == "description:")
          {
            std::string sDescrTmp = sTmp.substr(13);
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
          else
          if (sTmp.substr(0, 14) == "loadAtStartup:")
          {
            std::string sLoadAtStartup = sTmp.substr(15);
            rise::StrTrim(sLoadAtStartup);
            bLoadAtStartup = sLoadAtStartup == "true";
          }
          else
          if (sTmp.substr(0, 16) == "targetNamespace:")
          {
            rInterface.sTargetNs = sTmp.substr(17);
            rise::StrTrimLeft(rInterface.sTargetNs);
          }
        }
        SkipWsOnly();
      }

      if (m_tFile.peek() == '#') // preprocessor
      {
        ParsePreprocessorBlock();
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
        stClass.sServiceUri = sServiceUri;
        stClass.bLoadAtStartup = bLoadAtStartup;
        rInterface.lsClass.push_back(stClass);

        SkipWs();
        m_tFile >> chData;
        if (chData != ';')
        {
          CSP_THROW("missing ';' after class definition", m_stInterface.sFileName, m_nLine);
        }

        SkipSingleLineComment();

        sServiceUri.erase();
        sDescr.erase();
        sDetail.erase();
        bLoadAtStartup = false;
      } else
      if (sTmp == "struct")
      {
        SStruct stStruct;
        ParseStruct(stStruct);

        stStruct.sDescr = sDescr;
        stStruct.sDetail = sDetail;

        // check for forward declaration
        std::list<SStruct>::iterator itStruct = rInterface.lsStruct.begin();
        for (; itStruct != rInterface.lsStruct.end(); ++itStruct)
        {
          if (itStruct->sName == stStruct.sName)
          {
            break;
          }
        }

        if (itStruct == rInterface.lsStruct.end())
        {
          rInterface.lsStruct.push_back(stStruct);
        }
        else
        {
          if (!stStruct.bForward)
          {
            if (itStruct->bForward)
            {
              *itStruct = stStruct;
            }
            else
            {
              CSP_THROW("Duplicating struct " + stStruct.sName, m_stInterface.sFileName, m_nLine);
            }
          }
        }

        SkipWs();
        m_tFile >> chData;
        if (chData != ';')
        {
          CSP_THROW("missing ';' after struct definition", m_stInterface.sFileName, m_nLine);
        }

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
        if (chData != ';')
        {
          CSP_THROW("missing ';' after typedef definition", m_stInterface.sFileName, m_nLine);
        }

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
      if (chData != '{')
      {
        CSP_THROW("ParseBracketedBlock: \"{\" is not found!", m_stInterface.sFileName, m_nLine);
      }

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
      if(m_tFile.good())
      {
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
      }
      else
      {
        CSP_THROW(std::string("can't open file: ") + sFileName + ": " + std::string(strerror(errno)), m_stInterface.sFileName, m_nLine);
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

        const std::string& sNamespace = itStruct->sNamespace;
        bool bFound = false;
        // search in current interface, all extern structs is included
        for (std::list<SStruct>::iterator itParentStruct = m_stInterface.lsStruct.begin();
            itParentStruct != m_stInterface.lsStruct.end() && !bFound; ++itParentStruct)
        {
          std::string sParentStructNsName = itParentStruct->sNamespace + itParentStruct->sName;
          std::string::size_type nPos = sNamespace.size();
          do
          {
            std::string sStructNsName = sNamespace.substr(0, nPos) + itStruct->sParentName;

            if (sStructNsName == sParentStructNsName)
            {
              itStruct->sParentNamespace = itParentStruct->sNamespace;
              bFound = true;
              break;
            }

            nPos = sNamespace.substr(0, nPos - 2).find_last_of("::", nPos);
            if (nPos != std::string::npos)
            {
              ++nPos;
            }
          }
          while (nPos != std::string::npos);
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

    if (uServicesCount == 0 && rParseSettings.bNoServiceError)
    {
      CSP_THROW("No staff service interfaces found. Staff services must inherited from staff::IService.\n"
                "Example:\n----\n#include <staff/common/IService.h>\n\n  class Calc: public staff::IService\n"
                "  {\n  public:\n    virtual int Add(int nA, int nB) = 0;\n  };\n----\n\n",
                "", 0);
    }
  }

  const std::string CCppParser::m_sId = "cpp";
}
