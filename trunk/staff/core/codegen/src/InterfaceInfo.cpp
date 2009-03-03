#include <limits.h>
#include <list>
#include <iostream>
#include <sstream>
#include "InterfaceInfo.h"

SInterface& Interface(SInterface* pInterface = NULL)
{
  static SInterface* pCurrentInterface = NULL;
  if (pInterface != NULL)
    pCurrentInterface = pInterface;

  if (pCurrentInterface == NULL)
    throw "(pCurrentInterface == NULL)";

  return *pCurrentInterface;
}

template<typename TType>
std::ostream& operator<<(std::ostream& rStream, const std::list<TType>& rList)
{
  for(typename std::list<TType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
    rStream << *it;
  return rStream;
}

std::istream& SkipWs(std::istream& rStream)
{
  char chData = 0;
  while (rStream.good() && !rStream.eof())
  {
    chData = rStream.peek();
    if (chData == '\n')
      ++GetLine();

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

void ReadStr(std::istream& rStream, std::string& sString, bool bSkipWS = true)
{
  if (bSkipWS)
    rStream >> SkipWs;
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
      break;
    sOut += chData;
    rStream.ignore();
  }

  return rStream;
}

template<typename TStructType>
bool IsExistsInList(const std::list<TStructType>& rList, const std::string& sName)
{
  for (typename std::list<TStructType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
    if (it->sName == sName)
      return true;
  
  return false;
}


SDataType::EDataType DetectType(const std::string& sName)
{
  if (sName == "staff::CDataObject")
    return SDataType::EDataObject;
  
  if (sName == "bool" ||
      sName == "char" ||
      sName == "int" ||
      sName == "short" ||
      sName == "long" ||
      sName == "float" ||
      sName == "double" ||
      sName == "void" ||
      
      sName == "std::string" ||
      sName == "rise::CString" ||
      sName == "rise::CStringA" ||
      sName == "rise::CStringW" ||
      sName == "std::wstring" ||

      sName == "rise::byte" ||
      sName == "rise::word" ||
      sName == "rise::ushort" ||
      sName == "rise::dword" ||
      sName == "rise::ulong" ||
      sName == "rise::uint" ||
      sName == "rise::TSize"
      )
    return SDataType::EGeneric;
  
  if(IsExistsInList(Interface().lsStruct, sName))
    return SDataType::EStruct;
  if(IsExistsInList(Interface().lsTypedef, sName))
    return SDataType::ETypedef;
  return SDataType::EUnknown;
}

void IgnoreFunction(std::istream& rStream)
{
  char chTmp = '\0';
  int nRecursion = 0;

  while (rStream.good() && !rStream.eof())
  {
    rStream >> chTmp;

    if (chTmp == ';' && nRecursion == 0)
      break;
    else
    if (chTmp == '}')
    {
      --nRecursion;
      if (nRecursion < 0)
        throw "mismatch {}";
      if (nRecursion == 0)
      {
        while (rStream.peek() == ';')
          rStream.ignore();
        break;
      }
    }
    else
    if (chTmp == '{')
      ++nRecursion;
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
      throw "unexpected EOF(after type parsing)";
    if (sTmp == "static")
      std::cout << "Warning: members must be non-static: Line:" << GetLine();

    chTmp = rStream.peek();
    if (chTmp == '<')
    {
      rStream.ignore();
      rDataType.sName = sTmp;
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
      return rStream;

    if(sTmp == "const")
      rDataType.bIsConst = true;
    else // name of type
    {
      if (sTmp == "unsigned")
      {
        ReadStr(rStream, sTmp);
        if(rStream.eof())
          throw "unexpected EOF(after type parsing)";

        rDataType.eType = DetectType(sTmp);
        rDataType.sName = "unsigned " + sTmp;
      } else
      {
        rDataType.eType = DetectType(sTmp);
        rDataType.sName = sTmp;
      }
      break;
    }
  }

  if (rDataType.bIsRef && !rDataType.bIsConst)
  {
    std::cerr << "Warning: Non-const reference to " << rDataType.sName << " at line " << GetLine() << " \n(return value cannot be passed over argument)"<< std::endl;
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

  rStream >> rMember.stReturn;

  if (rMember.stReturn.bIsRef)
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

  return rStream;
}

// class
std::istream& operator>>( std::istream& rStream, SClass& rClass )
{
  char chTmp = '\0';
  std::string sTmp;

  rStream >> SkipWs;
  ReadBefore(rStream, rClass.sName);
  if(rStream.eof())
    throw "unexpected EOF(after classname)";
  ReadStr(rStream, sTmp);
  if(rStream.eof())
    throw "unexpected EOF(after classname and '{')";
  if (sTmp != "{")
    throw "'{' after classname expected ";

  while (rStream.good() && !rStream.eof())
  {
    rStream >> SkipWs;
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
      std::cerr << "warn: all members of interface class must be only public!" << std::endl;
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

  SParam stParamTmp;

  rStream >> SkipWs;
  ReadBefore(rStream, rStruct.sName);
  if(rStream.eof())
    throw "unexpected EOF(after struct name): " + rStruct.sName;
  ReadStr(rStream, sTmp);
  if(rStream.eof())
    throw "unexpected EOF(after structname and '{'): " + rStruct.sName;
  if (sTmp != "{")
    throw "'{' after structname expected: " + rStruct.sName;

  while (rStream.good() && !rStream.eof())
  {
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

    if (stParamTmp.stDataType.sName.find('(') != std::string::npos)
      bFunction = true;

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
      } else
        bFunction = true;
    }

    if (bFunction)
      IgnoreFunction(rStream);
  }
  return rStream;
}

std::istream& operator>>( std::istream& rStream, STypedef& rTypedef )
{
  rStream >> rTypedef.stDataType;
  rStream >> SkipWs;
  ReadBefore(rStream, rTypedef.sName);

  return rStream;
}

// Interface
std::istream& operator>>( std::istream& rStream, SInterface& rInterface )
{
  char chData = 0;
  std::string sTmp;

  Interface(&rInterface);

  rInterface.sName = rInterface.sFileName.substr(rInterface.sFileName.size() - 2, 2) == ".h" ?
      rInterface.sFileName.substr(0, rInterface.sFileName.size() - 2) :
      rInterface.sFileName;

  while (rStream.good() && !rStream.eof())
  {
    rStream >> SkipWs;
    rStream.get(chData);

    if (chData == '#') // preprocessor
    {
      rStream.ignore(INT_MAX, '\n');
    } else // text
    {
      sTmp.erase();
      rStream.unget();
      ReadStr(rStream, sTmp);
      
      if (sTmp == "class")
      {
        SClass stClass;
        rStream >> stClass;
        rInterface.lsClass.push_back(stClass);

        rStream >> SkipWs >> chData;
        if (chData != ';')
          throw "missing ';' after class definition";
      } else
      if (sTmp == "struct")
      {
        SStruct stStruct;
        rStream >> stStruct;
        rInterface.lsStruct.push_back(stStruct);

        rStream >> SkipWs >> chData;
        if (chData != ';')
          throw "missing ';' after struct definition";
      } else
      if (sTmp == "typedef")
      {
        STypedef stTypedef;
        rStream >> stTypedef;
        rInterface.lsTypedef.push_back(stTypedef);

        rStream >> SkipWs >> chData;
        if (chData != ';')
          throw "missing ';' after typedef definition";
      } else
      if (sTmp == "namespace")
      {
        rStream >> rInterface.sNamespace;
      } else
      if (sTmp == "enum")   // enum -ignore
      {
        IgnoreFunction(rStream);
      }
      else
      if (sTmp == "{")
      {
      } else
      if (sTmp == "}")
      {
      }
    }
  }
  return rStream;
}

#ifdef DEBUG
std::ostream& operator<<( std::ostream& rStream, const SInterface& rInterface )
{
  return rStream << "//interface: " << rInterface.sFileName << "\nnamespace: " << 
    rInterface.sNamespace << "\n" << rInterface.lsClass << "\n";
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