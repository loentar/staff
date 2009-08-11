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
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <rise/common/Log.h>
#include <rise/common/exmacros.h>
#include <rise/common/ExceptionTemplate.h>
#include "InterfaceInfo.h"

std::string g_sCurrentNamespace = "::";

SInterface& Interface(SInterface* pInterface = NULL)
{
  static SInterface* pCurrentInterface = NULL;
  if (pInterface != NULL)
  {
    pCurrentInterface = pInterface;
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

  if (sDataTypeName == "staff::CDataObject")
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
        throw "mismatch {}";
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
      throw "unexpected EOF(after type parsing)";
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
          throw " \",\" or \">\" expected while parsing template ";
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
          throw "unexpected EOF(after type parsing)";

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
    throw "return value cannot be reference";

  rStream >> SkipWs;

  rStream.get(tStreamBuff, '('); // parameters begin

  rMember.sName = tStreamBuff.str();

  rStream >> SkipWs >> chData;
  if (chData != '(')
    throw "'(' expected after function name";

  rStream >> SkipWs; // arguments?
  chData = rStream.peek();
  if (chData == ')')
    rStream.ignore();
  else
    while (rStream.good())
    {
      if(rStream.eof())
        throw "unexpected EOF(after member name)";

      rStream >> stParam; // reading param

      rStream >> SkipWs >> chData; // more arguments?
      rMember.lsParamList.push_back(stParam);

      if (chData == ')')
      {
        rStream.ignore();
        break;
      }
      
      if (chData != ',')
        throw "error parsing param";
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
    throw "members must be pure virtual!";

  rStream >> SkipWs >> chData;
  if (chData != '0')
    throw "members must be pure virtual!";

  rStream >> SkipWs >> chData;
  if (chData != ';')
    throw "';' expected";

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
  std::string sResponseElement;
  std::string sResultElement;

  rStream >> SkipWs;
  ReadBefore(rStream, rClass.sName);
  rClass.sNamespace = g_sCurrentNamespace;
  if(rStream.eof())
  {
    throw "unexpected EOF(after classname)";
  }

  ReadStr(rStream, sTmp);
  if(rStream.eof())
  {
    throw "unexpected EOF(after classname and '{')";
  }

  if (sTmp != "{")
  {
    throw "'{' after classname expected ";
  }

  while (rStream.good() && !rStream.eof())
  {
    sSoapAction.erase();
    sDescr.erase();
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
            sDescr += '\n';
          }
          sDescr += sDescrTmp;
        }
        else
        if (sTmp.substr(0, 12) == "description:")
        {
          std::string sDescrTmp = sTmp.substr(13);
          rise::StrTrimLeft(sDescrTmp);
          if (sDescr.size() != 0)
          {
            sDescr += '\n';
          }
          sDescr += sDescrTmp;
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
      throw "destructor must be virtual!";
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
          throw "Non-valid destructor: ~" + sTmp + " but expected ~" + rClass.sName;
      } else
      {
        SMember stMember;

        rStream >> stMember;
        stMember.sDescr = sDescr;
        stMember.sSoapAction = sSoapAction;
        stMember.stReturn.sName = sResponseElement;
        stMember.stReturn.stDataType.sNodeName = sResultElement;
        rClass.lsMember.push_back(stMember);
      }
    } 
    else
      throw "all members must be pure virtual!";
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
    throw "unexpected EOF(after struct name): " + rStruct.sName;
  }

  ReadStr(rStream, sTmp);
  if(rStream.eof())
  {
    throw "unexpected EOF(after structname): " + rStruct.sName;
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
      throw "unexpected EOF(after structname and inheritance sign): " + rStruct.sName;
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
        throw "unexpected EOF(while reading parent struct name): " + rStruct.sName;
      }
    }
    
    rStruct.sParent = sTmp;
    ReadStr(rStream, sTmp);
  }

  rStruct.bForward = false;
  
  if (sTmp != "{")
  {
    throw "'{' or ';' after structname expected: " + rStruct.sName;
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
          throw "Struct members must be non-const: " + rStruct.sName;
        if (stParamTmp.stDataType.bIsRef)
          throw "Struct members must be non-ref: " + rStruct.sName;
        rStruct.lsMember.push_back(stParamTmp);
        rStream >> SkipSingleLineComment;
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
  std::string sServiceDescr;
  std::string sTmp;

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
        if (sServiceDescr.size() != 0)
        {
          sServiceDescr += '\n';
        }
        sServiceDescr += sDescrTmp;
      }
      else
      if (sTmp.substr(0, 12) == "description:")
      {
        std::string sDescrTmp = sTmp.substr(13);
        rise::StrTrimLeft(sDescrTmp);
        if (sServiceDescr.size() != 0)
        {
          sServiceDescr += '\n';
        }
        sServiceDescr += sDescrTmp;
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
    rStream >> stClass;
    stClass.sDescr = sServiceDescr;
    stClass.sServiceUri = sServiceUri;
    rInterface.lsClass.push_back(stClass);

    rStream >> SkipWs >> chData;
    if (chData != ';')
    {
      throw "missing ';' after class definition";
    }

    rStream >> SkipSingleLineComment;
  } else
  if (sTmp == "struct")
  {
    SStruct stStruct;
    rStream >> stStruct;

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
          throw "Duplicating struct " + stStruct.sName;
        }
      }
    }
    
    rStream >> SkipWs >> chData;
    if (chData != ';')
    {
      throw "missing ';' after struct definition";
    }
  
    rStream >> SkipSingleLineComment;
  } else
  if (sTmp == "typedef")
  {
    STypedef stTypedef;
    rStream >> stTypedef;
    rInterface.lsTypedef.push_back(stTypedef);

    rStream >> SkipWs >> chData;
    if (chData != ';')
    {
      throw "missing ';' after typedef definition";
    }
  
    rStream >> SkipSingleLineComment;
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
    throw ("Unknown lexeme: \"" + sTmp + "\"");
  }
}

void ParseBracketedBlock( std::istream& rStream, SInterface& rInterface )
{
  char chData = 0;

  rStream >> SkipWs;
  rStream.get(chData);
  if (chData != '{')
  {
    throw "ParseBracketedBlock: \"{\" is not found!";
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

  throw "ParseBracketedBlock: EOF found!";
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

#ifdef DEBUG
std::ostream& operator<<( std::ostream& rStream, const SInterface& rInterface )
{
  return rStream << "//interface: " << rInterface.sFileName << "\n" << rInterface.lsClass << "\n";
}

std::ostream& operator<<( std::ostream& rStream, const SClass& rClass )
{
  return rStream << "class " << rClass.sName << "\n{\n" << rClass.lsMember << "};\n\n";
}

std::ostream& operator<<( std::ostream& rStream, const SMember& rMember )
{
  return rStream << "virtual " << rMember.stReturn << " " << rMember.sName << " (" << rMember.lsParamList << ")" 
    << (rMember.bIsConst ? " const" : "") << " = 0;\n";
}

std::ostream& operator<<( std::ostream& rStream, const SParam& rParameter )
{
  return rStream << rParameter.stDataType << " " << rParameter.sName << ", ";
}

std::ostream& operator<<( std::ostream& rStream, const SDataType& rDataType )
{
  return rStream << (rDataType.bIsConst ? "const " : "") << rDataType.sName <<
    (rDataType.bIsRef ? "&" : "");
}
#endif


void ResetLine()
{
  GetLine() = 1;
}

int& GetLine()
{
  static int nLine = 1;
  return nLine;
}

std::ostream& operator<<( std::ostream& rStream, const SDataType::EDataType eDataType )
{
  switch (eDataType)
  {
  case SDataType::EGeneric:
    rStream << "generic";
    break;

  case SDataType::EString:
    rStream << "string";
    break;

  case SDataType::EDataObject:
    rStream << "dataobject";
    break;

  case SDataType::EStruct:
    rStream << "struct";
    break;

  case SDataType::ETypedef:
    rStream << "typedef";
    break;

  case SDataType::ETemplate:
    rStream << "template";
    break;

  default:
    rStream << "unknown";
  }

  return rStream;
}

SDataType::SDataType():
  bIsConst(false), bIsRef(false), eType(EGeneric)
{

}

SDataType& SDataType::operator=(const SDataType& stDataType)
{
  bIsConst = stDataType.bIsConst;
  bIsRef = stDataType.bIsRef;
  eType = stDataType.eType;
  sName = stDataType.sName;
  sNodeName = stDataType.sNodeName;
  sNamespace = stDataType.sNamespace;
  lsParams = stDataType.lsParams;

  return *this;
}

SParam& SParam::operator=(const SParam& rParam)
{
  stDataType = rParam.stDataType;
  sName = rParam.sName;
  return *this;
}

SStruct::SStruct():
  bForward(true)
{
}

SMember::SMember():
  bIsConst(false)
{

}
