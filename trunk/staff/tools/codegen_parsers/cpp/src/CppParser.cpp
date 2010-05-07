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

  std::string g_sCurrentNamespace = "::";

  std::istream& operator>>(std::istream& rStream, SDataType& rDataType);
  std::istream& operator>>(std::istream& rStream, SParam& rParameter);
  std::istream& operator>>(std::istream& rStream, SMember& rMember);
  std::istream& operator>>(std::istream& rStream, SClass& rClass);
  std::istream& operator>>(std::istream& rStream, SStruct& rStruct);
  std::istream& operator>>(std::istream& rStream, STypedef& rTypedef);
  std::istream& operator>>(std::istream& rStream, SInterface& rInterface);

  std::ostream& operator<<(std::ostream& rStream, const SDataType::EDataType eDataType);

  int& GetLine();

  SInterface& Interface(SInterface* pInterface = NULL)
  {
    static SInterface* pCurrentInterface = NULL;
    if (pInterface != NULL)
    {
      pCurrentInterface = pInterface;
      GetLine() = 1;
    }

    RISE_ASSERTS(pCurrentInterface != NULL, "(pCurrentInterface == NULL)");

    return *pCurrentInterface;
  }

  template<typename TType>
  std::ostream& operator<<(std::ostream& rStream, const std::list<TType>& rList)
  {
    for(typename std::list<TType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
    {
      rStream << *it;
    }

    return rStream;
  }

  std::istream& SkipWs(std::istream& rStream)
  {
    char chData = 0;
    while (rStream.good() && !rStream.eof())
    {
      chData = rStream.peek();
      if (chData == '\n')
      {
        ++GetLine();
      }

      if(chData != ' ' && chData != '\n' && chData != '\r' && chData != '\t')
      {
        if (chData == '/') // comment
        {
          rStream.ignore();
          if (rStream.peek() == '/') // single line comment
          {
            rStream.ignore(INT_MAX, '\n');
            continue;
          } else
          if (rStream.peek() == '*') // multiline comment
          {
            while (rStream.good() && !rStream.eof())
            {
              rStream.ignore(INT_MAX, '*');
              rStream.get(chData);
              if(chData == '/')
                break;
            }
          } else
          {
            rStream.unget();
            break;
          }
        } else
          break;
      }
      rStream.ignore();
    }

    return rStream;
  }

  std::istream& SkipWsOnly(std::istream& rStream)
  {
    char chData = 0;
    while (rStream.good() && !rStream.eof())
    {
      chData = rStream.peek();
      if (chData == '\n')
      {
        ++GetLine();
      }

      if(chData != ' ' && chData != '\n' && chData != '\r' && chData != '\t')
      {
        break;
      }
      rStream.ignore();
    }

    return rStream;
  }

  std::istream& SkipSingleLineComment(std::istream& rStream)
  {
    char chData = '\0';
    while (rStream.good() && !rStream.eof())
    {
      chData = rStream.peek();
      if(chData != ' ' && chData != '\t')
      {
        break;
      }
      rStream.ignore();
    }

    if (rStream.good() && !rStream.eof())
    {
      if (rStream.peek() == '/') // comment
      {
        rStream.ignore();
        if (rStream.peek() == '/') // single line comment
        {
          rStream.ignore(INT_MAX, '\n');
        } else
        {
          rStream.unget();
        }
      }
    }

    return rStream;
  }

  void ReadStr(std::istream& rStream, std::string& sString, bool bSkipWS = true)
  {
    if (bSkipWS)
    {
      rStream >> SkipWs;
    }
    rStream >> sString;
  }

  std::istream& ReadBefore(std::istream& rStream, std::string& sOut, const std::string& sDelim = " \r\n\t,();[]{}<>\\/*-+!@#$%^&*=")
  {
    char chData = 0;

    sOut.erase();

    while (rStream.good() && !rStream.eof())
    {
      chData = rStream.peek();
      if(sDelim.find(chData) != std::string::npos)
      {
        break;
      }

      sOut += chData;
      rStream.ignore();
    }

    return rStream;
  }

  void ReadDescrComment(std::istream& rStream, std::string& sDescr)
  {
    char chData = '\0';
    while (rStream.good() && !rStream.eof())
    {
      chData = rStream.peek();
      if(chData != ' ' && chData != '\t')
      {
        break;
      }
      rStream.ignore();
    }

    if (rStream.good() && !rStream.eof())
    {
      if (rStream.peek() == '/') // comment
      {
        rStream.ignore();
        if (rStream.peek() == '/') // single line comment
        {
          rStream.ignore();
          if (rStream.peek() == '!')
          {
            rStream.ignore();
            if (rStream.peek() == '<') // description
            {
              rStream.ignore();
              ReadBefore(rStream, sDescr, "\n\r");
            }
          }
          rStream.ignore(INT_MAX, '\n');
        } else
        {
          rStream.unget();
        }
      }
    }
  }

  bool ReadComment(std::istream& rStream, std::string& sComment)
  {
    if (rStream.peek() != '/')
    {
      return false;
    }

    sComment.erase();

    rStream.ignore();
    if (rStream.peek() == '/')
    {
      rStream.ignore();
      ReadBefore(rStream, sComment, "\n\r");
    }
    else
    if (rStream.peek() == '*')
    {
      char chData = '\0';
      std::string sTmp;

      rStream.ignore();
      while (rStream.good() && !rStream.eof())
      {
        ReadBefore(rStream, sTmp, "*");

        sComment += sTmp;
        rStream.get(chData);
        if(chData == '*')
        {
          if (rStream.peek() == '/')
          {
            rStream.ignore();
            break;
          }
          sComment += chData;
        }
      }
    }
    else
    {
      rStream.unget();
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

        std::string::size_type nPos = g_sCurrentNamespace.find_last_of("::");

        while (nPos != std::string::npos)
        {
          ++nPos;
          if((g_sCurrentNamespace.substr(0, nPos) + rDataType.sNamespace) == it->sNamespace)
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

          nPos = g_sCurrentNamespace.find_last_of("::", nPos - 3);
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
        (sDataTypeName == "CDataObject" && g_sCurrentNamespace.substr(0, 9) == "::staff::"))
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
    if(ParseCompositeDataType(Interface().lsStruct, rDataType))
    {
      rDataType.eType = SDataType::EStruct;
    }
    else
    if(ParseCompositeDataType(Interface().lsTypedef, rDataType))
    {
      rDataType.eType = SDataType::ETypedef;
    }
    else
    {
      rDataType.eType = SDataType::EUnknown;
    }
  }

  void IgnoreFunction(std::istream& rStream)
  {
    char chTmp = '\0';
    int nRecursion = 0;

    while (rStream.good() && !rStream.eof())
    {
      rStream >> chTmp;

      if (chTmp == ';' && nRecursion == 0)
      {
        rStream >> SkipSingleLineComment;
        break;
      }
      else
      if (chTmp == '}')
      {
        --nRecursion;
        if (nRecursion < 0)
        {
          CSP_THROW("mismatch {}", Interface().sFileName, GetLine());
        }

        if (nRecursion == 0)
        {
          while (rStream.peek() == ';')
          {
            rStream.ignore();
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
  std::istream& operator>>( std::istream& rStream, SDataType& rDataType )
  {
    char chTmp = '\0';
    std::string sTmp;

    rDataType.bIsConst = false;
    rDataType.bIsRef = false;
    rDataType.eType = SDataType::EUnknown;

    while (rStream.good())
    {
      rStream >> SkipWs;
      ReadBefore(rStream, sTmp);
      if(rStream.eof())
      {
        CSP_THROW("unexpected EOF(after type parsing)", Interface().sFileName, GetLine());
      }

      if (sTmp == "static")
      {
        rise::LogWarning() << "members must be non-static: Line:" << GetLine();
      }

      chTmp = rStream.peek();
      if (chTmp == '<')
      {
        rStream.ignore();
        rDataType.sName = sTmp;
        rDataType.sNamespace = "";
        rDataType.eType = SDataType::ETemplate;
        while(rStream.good())
        {
          SDataType stTemplParam;
          rStream >> stTemplParam >> SkipWs;
          chTmp = rStream.peek();
          rDataType.lsParams.push_back(stTemplParam);
          if (chTmp == '>')
          {
            rStream.ignore();

            rStream >> SkipWs;
            chTmp = rStream.peek();
            if(chTmp == '&')
            {
              rDataType.bIsRef = true;
              rStream.ignore();
            }

            return rStream;
          } else
          if (chTmp == ',')
            rStream.ignore();
          else
            CSP_THROW(" \",\" or \">\" expected while parsing template ", Interface().sFileName, GetLine());
        }
        break;
      } else
      if(chTmp == '&')
      {
        rDataType.bIsRef = true;
        rStream.ignore();
      }
      else
      if (sTmp == "")
      {
        return rStream;
      }

      if(sTmp == "const")
      {
        rDataType.bIsConst = true;
      }
      else // name of type
      {
        if (sTmp == "unsigned")
        {
          ReadStr(rStream, sTmp);
          if(rStream.eof())
            CSP_THROW("unexpected EOF(after type parsing)", Interface().sFileName, GetLine());

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
      rise::LogWarning() << "Non-const reference to " << rDataType.sName << " at line " << GetLine() << " \n(return value cannot be passed over argument)";
    }

    return rStream;
  }

  // parameter
  std::istream& operator>>( std::istream& rStream, SParam& rParameter )
  {
    rStream >> rParameter.stDataType >> SkipWs;
    ReadBefore(rStream, rParameter.sName);
    rParameter.stDataType.sNodeName = rParameter.sName;
    return rStream;
  }

  // member
  std::istream& operator>>( std::istream& rStream, SMember& rMember )
  {
    SParam stParam;
    char chData;
    std::string sTmp;
    std::stringbuf tStreamBuff;

    rMember.bIsConst = false;

    rStream >> rMember.stReturn.stDataType;

    if (rMember.stReturn.stDataType.bIsRef)
      CSP_THROW("return value cannot be reference", Interface().sFileName, GetLine());

    rStream >> SkipWs;

    rStream.get(tStreamBuff, '('); // parameters begin

    rMember.sName = tStreamBuff.str();

    rStream >> SkipWs >> chData;
    if (chData != '(')
      CSP_THROW("'(' expected after function name", Interface().sFileName, GetLine());

    rStream >> SkipWs; // arguments?
    chData = rStream.peek();
    if (chData == ')')
      rStream.ignore();
    else
      while (rStream.good())
      {
        if(rStream.eof())
          CSP_THROW("unexpected EOF(after member name)", Interface().sFileName, GetLine());

        rStream >> stParam; // reading param

        rStream >> SkipWs >> chData; // more arguments?
        rMember.lsParamList.push_back(stParam);

        if (chData == ')')
        {
          rStream.ignore();
          break;
        }

        if (chData != ',')
          CSP_THROW("error parsing param", Interface().sFileName, GetLine());
      }

    rStream >> SkipWs >> chData;
    if (chData == 'c')
    {
      ReadStr(rStream, sTmp, false);
      if (sTmp == "onst") // const
        rMember.bIsConst = true;

      rStream >> SkipWs >> chData;
    }
    if (chData != '=')
      CSP_THROW("members must be pure virtual!", Interface().sFileName, GetLine());

    rStream >> SkipWs >> chData;
    if (chData != '0')
      CSP_THROW("members must be pure virtual!", Interface().sFileName, GetLine());

    rStream >> SkipWs >> chData;
    if (chData != ';')
      CSP_THROW("';' expected", Interface().sFileName, GetLine());

    rStream >> SkipSingleLineComment;
    return rStream;
  }

  // class
  std::istream& operator>>( std::istream& rStream, SClass& rClass )
  {
    char chTmp = '\0';
    std::string sTmp;
    std::string sSoapAction;
    std::string sDescr;
    std::string sDetail;
    std::string sRequestElement;
    std::string sResponseElement;
    std::string sResultElement;

    rStream >> SkipWs;
    rClass.sNamespace = g_sCurrentNamespace;

    ReadStr(rStream, sTmp);
    if(rStream.eof())
    {
      CSP_THROW("unexpected EOF(after classname and '{')", Interface().sFileName, GetLine());
    }

    if (sTmp != "{")
    {
      CSP_THROW("'{' after classname expected ", Interface().sFileName, GetLine());
    }

    while (rStream.good() && !rStream.eof())
    {
      sSoapAction.erase();
      sDescr.erase();
      sDetail.erase();
      sRequestElement.erase();
      sResponseElement.erase();
      sResultElement.erase();

      rStream >> SkipWsOnly;
      while (ReadComment(rStream, sTmp))
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
        }
        rStream >> SkipWsOnly;
      }

      chTmp = rStream.peek();
      if (chTmp == '}')
      {
        rStream.ignore();
        break;
      }

      ReadStr(rStream, sTmp);

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
        IgnoreFunction(rStream);
      }
      else
      if (sTmp.substr(0, rClass.sName.size() + 1) == "~" + rClass.sName)   // non virtual destructor
      {
        CSP_THROW("destructor must be virtual!", Interface().sFileName, GetLine());
      }
      else
      if (sTmp == "enum")   // enum -ignore
      {
        IgnoreFunction(rStream);
      }
      else
      if (sTmp == "virtual")
      {
        rStream >> SkipWs;
        if (rStream.peek() == '~')
        {
          rStream.ignore();
          ReadBefore(rStream, sTmp);
          if (sTmp == rClass.sName)   // destructor-ignore it
            IgnoreFunction(rStream);
          else
            CSP_THROW("Non-valid destructor: ~" + sTmp + " but expected ~" + rClass.sName, Interface().sFileName, GetLine());
        } else
        {
          SMember stMember;

          rStream >> stMember;
          stMember.sDescr = sDescr;
          stMember.sDetail = sDetail;
          stMember.sSoapAction = sSoapAction;
          stMember.sNodeName = sRequestElement;
          stMember.stReturn.sName = sResponseElement;
          stMember.stReturn.stDataType.sNodeName = sResultElement;
          rClass.lsMember.push_back(stMember);
        }
      }
      else
        CSP_THROW("all members must be pure virtual!", Interface().sFileName, GetLine());
    }
    return rStream;
  }

  std::istream& operator>>( std::istream& rStream, SStruct& rStruct )
  {
    char chTmp = '\0';
    std::string sTmp;
    bool bFunction = false;

    rStream >> SkipWs;
    ReadBefore(rStream, rStruct.sName, " \r\n\t:;{}");
    rStruct.sNamespace = g_sCurrentNamespace;
    if(rStream.eof())
    {
      CSP_THROW("unexpected EOF(after struct name): " + rStruct.sName, Interface().sFileName, GetLine());
    }

    ReadStr(rStream, sTmp);
    if(rStream.eof())
    {
      CSP_THROW("unexpected EOF(after structname): " + rStruct.sName, Interface().sFileName, GetLine());
    }

    if (sTmp == ";")
    {
      rStream.unget();
      return rStream;
    }

    if (sTmp == ":")
    { // inheritance
      ReadStr(rStream, sTmp);
      if(rStream.eof())
      {
        CSP_THROW("unexpected EOF(after structname and inheritance sign): " + rStruct.sName, Interface().sFileName, GetLine());
      }

      if (sTmp != "public")
      {
        rise::LogWarning() << "non-public inheritance: " << rStruct.sName << " => " << sTmp;
      }
      else
      {
        ReadStr(rStream, sTmp);
        if(rStream.eof())
        {
          CSP_THROW("unexpected EOF(while reading parent struct name): " + rStruct.sName, Interface().sFileName, GetLine());
        }
      }

      rStruct.sParent = sTmp;
      ReadStr(rStream, sTmp);
    }

    rStruct.bForward = false;

    if (sTmp != "{")
    {
      CSP_THROW("'{' or ';' after structname expected: " + rStruct.sName, Interface().sFileName, GetLine());
    }

    while (rStream.good() && !rStream.eof())
    {
      SParam stParamTmp;

      bFunction = false;
      rStream >> SkipWs;
      chTmp = rStream.peek();
      if (chTmp == '}')
      {
        rStream.ignore();
        break;
      }

      rStream >> stParamTmp.stDataType;

      if (stParamTmp.stDataType.sName == "enum")   // enum -ignore
      {
        IgnoreFunction(rStream);
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
        rStream >> SkipWs;
        ReadBefore(rStream, stParamTmp.sName);
        rStream >> SkipWs;
        rStream.get(chTmp);
        if (chTmp == ';')
        {
          if (stParamTmp.stDataType.bIsConst)
            CSP_THROW("Struct members must be non-const: " + rStruct.sName, Interface().sFileName, GetLine());
          if (stParamTmp.stDataType.bIsRef)
            CSP_THROW("Struct members must be non-ref: " + rStruct.sName, Interface().sFileName, GetLine());

          ReadDescrComment(rStream, stParamTmp.sDescr);

          rStruct.lsMember.push_back(stParamTmp);
          //rStream >> SkipSingleLineComment;
        } else
        {
          bFunction = true;
        }
      }

      if (bFunction)
      {
        IgnoreFunction(rStream);
      }
    }

    return rStream;
  }

  std::istream& operator>>( std::istream& rStream, STypedef& rTypedef )
  {
    rStream >> rTypedef.stDataType;
    rStream >> SkipWs;
    ReadBefore(rStream, rTypedef.sName);
    rTypedef.sNamespace = g_sCurrentNamespace;
    return rStream;
  }

  void ParseBracketedBlock( std::istream& rStream, SInterface& rInterface );
  void ParseHeaderBlock( std::istream& rStream, SInterface& rInterface );

  void ParsePreprocessorBlock( std::istream& rStream, SInterface& rInterface )
  {
    char chData = '\0';
    std::string sTmp;
    rStream.ignore();
    ReadStr(rStream, sTmp, false);
    if (sTmp == "include")
    {
      rStream >> SkipWs;
      chData = rStream.peek();
      if (chData == '\"')
      {
        std::stringbuf sbTmp;
        rStream.ignore();
        rStream.get(sbTmp, chData);

        std::string::size_type nPos = rInterface.sFileName.find_last_of('/');
        std::string sFileName;
        std::ifstream isFile;

        if (nPos != std::string::npos)
        {
          sFileName = rInterface.sFileName.substr(0, nPos);
        }

        sFileName += sbTmp.str();

        isFile.open(sFileName.c_str());
        if(isFile.good())
        {
          try
          {
            while (isFile.good() && !isFile.eof())
            {
              isFile >> SkipWs;
              chData = isFile.peek();

              if (chData == '#') // preprocessor
              {
                isFile.ignore(INT_MAX, '\n');
              } else // text
              {
                if (chData == '{')
                {
                  ParseBracketedBlock(isFile, rInterface);
                }
                else // text
                {
                  ParseHeaderBlock(isFile, rInterface);
                }
              }
            }
          }
          catch (...)
          {
            isFile.close();
            throw;
          }

          isFile.close();
        }
        else
        {
          rise::LogWarning() << "cannot include file \"" << sFileName << "\".";
        }
      }
    }

    rStream.ignore(INT_MAX, '\n');
  }

  void ParseHeaderBlock( std::istream& rStream, SInterface& rInterface )
  {
    char chData = 0;
    std::string sServiceUri;
    std::string sDescr;
    std::string sDetail;
    std::string sTmp;
    bool bLoadAtStartup = false;

    rStream >> SkipWsOnly;
    while (ReadComment(rStream, sTmp))
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
      rStream >> SkipWsOnly;
    }

    if (rStream.peek() == '#') // preprocessor
    {
      ParsePreprocessorBlock(rStream, rInterface);
      return;
    }

    ReadStr(rStream, sTmp);

    if (sTmp.size() == 0)
    {
      return; // eof
    }

    if (sTmp == "class")
    {
      SClass stClass;

      SkipWs(rStream);

      // checking for service class
      ReadBefore(rStream, stClass.sName, " \t\n\r:{;");

      SkipWs(rStream);
      char chTmp = rStream.peek();

      if (chTmp == ';')
      {
        rStream.ignore();
        return; // class forward
      }
      if (chTmp == '{')
      {
        IgnoreFunction(rStream); // ignore non-service class
        return;
      }

      if (chTmp == ':') // inheritance
      {
        rStream.ignore();
      }

      ReadStr(rStream, sTmp);
      if (sTmp != "public")
      { // not our class
        ReadBefore(rStream, sTmp, "{;");
        IgnoreFunction(rStream); // ignore non-service class
        return;
      }

      ReadStr(rStream, sTmp);
      if (sTmp != "IService" && sTmp != "staff::IService")
      { // not our class
        ReadBefore(rStream, sTmp, "{;");
        IgnoreFunction(rStream); // ignore non-service class
        return;
      }

      std::cout << "Using [" << stClass.sName << "] as service class\n";

      rStream >> stClass;
      stClass.sDescr = sDescr;
      stClass.sDetail = sDetail;
      stClass.sServiceUri = sServiceUri;
      stClass.bLoadAtStartup = bLoadAtStartup;
      rInterface.lsClass.push_back(stClass);

      rStream >> SkipWs >> chData;
      if (chData != ';')
      {
        CSP_THROW("missing ';' after class definition", Interface().sFileName, GetLine());
      }

      rStream >> SkipSingleLineComment;

      sServiceUri.erase();
      sDescr.erase();
      sDetail.erase();
      bLoadAtStartup = false;
    } else
    if (sTmp == "struct")
    {
      SStruct stStruct;
      rStream >> stStruct;

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
            CSP_THROW("Duplicating struct " + stStruct.sName, Interface().sFileName, GetLine());
          }
        }
      }

      rStream >> SkipWs >> chData;
      if (chData != ';')
      {
        CSP_THROW("missing ';' after struct definition", Interface().sFileName, GetLine());
      }

      sDescr.erase();
      sDetail.erase();

      rStream >> SkipSingleLineComment;
    } else
    if (sTmp == "typedef")
    {
      STypedef stTypedef;
      rStream >> stTypedef;

      rStream >> SkipWs >> chData;
      if (chData != ';')
      {
        CSP_THROW("missing ';' after typedef definition", Interface().sFileName, GetLine());
      }

      ReadDescrComment(rStream, stTypedef.sDescr);

      rInterface.lsTypedef.push_back(stTypedef);

      sDescr.erase();
      sDetail.erase();

//      rStream >> SkipSingleLineComment;
    } else
    if (sTmp == "namespace")
    {
      std::string::size_type nNsSize = g_sCurrentNamespace.size();
      std::string sNamespace;
      rStream >> sNamespace;
      g_sCurrentNamespace += sNamespace + "::";

      ParseBracketedBlock(rStream, rInterface);

      g_sCurrentNamespace.erase(nNsSize);

    } else
    if (sTmp == "enum")   // enum -ignore
    {
      IgnoreFunction(rStream);
      rStream >> SkipSingleLineComment;
    }
    else
    if (sTmp == ";")
    {
      rStream >> SkipSingleLineComment;
    }
    else
    if (chData == '#') // preprocessor
    {
      ParsePreprocessorBlock(rStream, rInterface);
    }
    else
    {
      CSP_THROW(("Unknown lexeme: \"" + sTmp + "\""), Interface().sFileName, GetLine());
    }
  }

  void ParseBracketedBlock( std::istream& rStream, SInterface& rInterface )
  {
    char chData = 0;

    rStream >> SkipWs;
    rStream.get(chData);
    if (chData != '{')
    {
      CSP_THROW("ParseBracketedBlock: \"{\" is not found!", Interface().sFileName, GetLine());
    }

    while (rStream.good() && !rStream.eof())
    {
      rStream >> SkipWsOnly;
      chData = rStream.peek();

      if (chData == '#') // preprocessor
      {
        ParsePreprocessorBlock(rStream, rInterface);
      } else // text
      {
        if (chData == '}')
        {
          rStream.ignore();
          return;
        }
        else
        if (chData == '{')
        {
          ParseBracketedBlock(rStream, rInterface);
        }
        else
        {
          ParseHeaderBlock(rStream, rInterface);
        }
      }
    }

    CSP_THROW("ParseBracketedBlock: EOF found!", Interface().sFileName, GetLine());
  }

  void ParseHeader( std::istream& rStream, SInterface& rInterface )
  {
    char chData = 0;
    std::string sTmp;

    while (rStream.good() && !rStream.eof())
    {
      rStream >> SkipWsOnly;
      chData = rStream.peek();

      if (chData == '{')
      {
        ParseBracketedBlock(rStream, rInterface);
      }
      else // text
      {
        ParseHeaderBlock(rStream, rInterface);
      }
    }
  }

  // Interface
  std::istream& operator>>( std::istream& rStream, SInterface& rInterface )
  {
    Interface(&rInterface);

    rInterface.sName = rInterface.sFileName.substr(rInterface.sFileName.size() - 2, 2) == ".h" ?
        rInterface.sFileName.substr(0, rInterface.sFileName.size() - 2) :
        rInterface.sFileName;

    ParseHeader(rStream, rInterface);

    return rStream;
  }

  int& GetLine()
  {
    static int nLine = 1;
    return nLine;
  }

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
      std::string sFileName = rParseSettings.sInDir + "/" + *itFile;
      std::ifstream isFile;
      SInterface stInterface;

      isFile.open(sFileName.c_str());
      if(isFile.good())
      {
        stInterface.sFileName = *itFile;
        try
        {
          isFile >> stInterface;
          isFile.close();
        }
        catch (CParseException& rParseException)
        {
          std::stringbuf sbData;
          isFile.get(sbData, '\n');
          isFile.ignore();
          isFile.get(sbData, '\n');

          rParseException.Message() += ": before\n-----------------\n" + sbData.str() + "\n-----------------\n";

          throw rParseException;
        }

        uServicesCount += stInterface.lsClass.size();
        rProject.lsInterfaces.push_back(stInterface);
      }
      else
      {
        CSP_THROW(std::string("can't open file: ") + *itFile + ": " + std::string(strerror(errno)), Interface().sFileName, GetLine());
      }
    }

    if (uServicesCount == 0 && rParseSettings.bNoServiceError)
    {
      CSP_THROW("No staff service interfaces found. Staff services must inherited from staff::IService.\n"
                "Example:\n----\n  class Calc: public staff::IService\n"
                "  {\n  public:\n    virtual int Add(int nA, int nB) = 0;\n  };\n----\n\n",
                Interface().sFileName, GetLine());
    }
  }

  const std::string CCppParser::m_sId = "cpp";
}
