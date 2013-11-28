/*
 *  Copyright 2010 Utkin Dmitry
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

#include <staff/utils/tostring.h>
#include "Interface.h"
#include "tools.h"
#include <set>

namespace staff
{
namespace codegen
{

  std::string::size_type StrIntersect(const std::string& sString1, const std::string& sString2)
  {
    if (!sString1.empty() && !sString2.empty())
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
    }
    return std::string::npos;
  }

  const BaseType* GetBaseType(const std::string& sNsName, const Interface& rstInterface,
                               const int eBaseType /*= BaseType::TypeAny*/,
                               const Struct* pstParent /*= NULL*/)
  {
    const Struct* pstCurr = pstParent;
    const BaseType* pstResult = NULL;

    std::string::size_type nNsNameSize = sNsName.size();

    if ((eBaseType & BaseType::TypeTypedef) != 0)
    {
      std::string sFindNsName = sNsName;
      if (sNsName.substr(0, 2) != "::")
      {
        sFindNsName = "::" + sNsName;
      }

      for (std::list<Typedef>::const_reverse_iterator itTypedef = rstInterface.lsTypedefs.rbegin();
           itTypedef != rstInterface.lsTypedefs.rend(); ++itTypedef)
      {
        if ((itTypedef->sNamespace + itTypedef->sName) == sFindNsName)
        {
          return &*itTypedef;
        }
      }
    }

    // look substructs
    for(;;)
    { // to avod getting extern type with the same name as in current interface
      // we must search backwards, because firstly goes extern types
      const std::list<Struct>& rStructList = !pstCurr ? rstInterface.lsStructs : pstCurr->lsStructs;
      for (std::list<Struct>::const_reverse_iterator itStruct = rStructList.rbegin();
        itStruct != rStructList.rend(); ++itStruct)
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
          // find in sub enums
          if (((eBaseType & BaseType::TypeStruct) != 0) && !nSizeDiff && sCurrNsName == sNsName)
          {
            pstResult = &*itStruct;
            break; // return
          }
          else
          {
            // empty/partial namespace
            if (nSizeDiff >= 2) // size of "::"
            {
              if (((eBaseType & BaseType::TypeStruct) != 0) &&
                  sCurrNsName.substr(nSizeDiff - 2, 2) == "::" &&
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
                    (nCurrNsNameSize >= (nPos + 2) && sCurrNsName.substr(nCurrNsNameSize - nPos - 2, 2) == "::"))
                && (nPos == sNsName.size() || sNsName.substr(nPos, 2) == "::"))
            {
              // go through child structs
              nPos += 2;
              const Struct* pstTmp = &*itStruct;
              std::string::size_type nBegin = nPos;
              std::string::size_type nEnd = 0;
              do
              {
                if (nBegin >= sNsName.size())
                {
                  pstTmp = NULL;
                  break;
                }
                nEnd = sNsName.find("::", nBegin);
                const std::string& sSubName =
                    nEnd != std::string::npos ?
                    sNsName.substr(nBegin, nEnd - nBegin) :
                    sNsName.substr(nBegin);
                bool bFound = false;

                // find in sub enums
                if ((eBaseType & BaseType::TypeEnum) != 0)
                {
                  for (std::list<Enum>::const_reverse_iterator itSubEnum = pstTmp->lsEnums.rbegin();
                    itSubEnum != pstTmp->lsEnums.rend(); ++itSubEnum)
                  {
                    if (itSubEnum->sName == sSubName)
                    {
                      return &*itSubEnum;
                    }
                  }
                }

                // go into sub struct
                for (std::list<Struct>::const_reverse_iterator itSubStruct = pstTmp->lsStructs.rbegin();
                  itSubStruct != pstTmp->lsStructs.rend(); ++itSubStruct)
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

      // find in sub enums
      if (!pstResult && ((eBaseType & BaseType::TypeEnum) != 0))
      {
        const std::list<Enum>& rlsEnums = !pstCurr ? rstInterface.lsEnums : pstCurr->lsEnums;
        for (std::list<Enum>::const_reverse_iterator itEnum = rlsEnums.rbegin();
          itEnum != rlsEnums.rend(); ++itEnum)
        {
          std::string sCurrNsName = itEnum->sNamespace;
          if (!itEnum->sOwnerName.empty())
          {
            sCurrNsName += itEnum->sOwnerName + "::";
          }
          sCurrNsName += itEnum->sName;
          std::string::size_type nCurrNsNameSize = sCurrNsName.size();

          std::string::size_type nPos = StrIntersect(sCurrNsName, sNsName);
          if (nPos != std::string::npos
              && (nPos == nCurrNsNameSize ||
                  (nCurrNsNameSize >= (nPos + 2) && sCurrNsName.substr(nCurrNsNameSize - nPos - 2, 2) == "::"))
              && (nPos == sNsName.size() || sNsName.substr(nPos, 2) == "::"))
          {
            pstResult = &*itEnum;
            break;
          }
        }
      }

      if (pstResult || !pstCurr)
      {
        break;
      }

      // find in parent owner struct
      pstCurr = pstCurr->sOwnerName.empty() ? NULL :
                GetStruct(pstCurr->sNamespace + pstCurr->sOwnerName, rstInterface);
    }

    return pstResult;
  }

  const Struct* GetStruct(const std::string& sNsName, const Interface& rstInterface,
                           const Struct* pstParent /*= NULL*/)
  {
    return static_cast<const Struct*>(GetBaseType(sNsName, rstInterface, BaseType::TypeStruct, pstParent));
  }


  void OptimizeCppNs(std::string& sOptimizeNs, const std::string& sCurrentNs)
  {
    // sOptimizeNs: ::samples::ticket::Ticket
    // sCurrentNs: ::samples::sharedtypes::
    // result: ticket::

    std::string::size_type nPos = 0;
    for (; nPos < sOptimizeNs.size() &&
           nPos < sCurrentNs.size() &&
           sOptimizeNs[nPos] == sCurrentNs[nPos]; ++nPos);

    if (nPos >= 2 && sOptimizeNs.substr(nPos - 2, 2) == "::")
    {
      sOptimizeNs.erase(0, nPos);
    }
  }

  void MangleCharInString(std::string::size_type& rnPos, std::string& sResult)
  {
    static std::map<char, std::string> mapChars;
    if (mapChars.empty())
    {
      mapChars['!'] = "_exclam_";
      mapChars['"'] = "_dquote_";
      mapChars['#'] = "_number_";
      mapChars['$'] = "_dollar_";
      mapChars['%'] = "_percent_";
      mapChars['&'] = "_amp_";
      mapChars['\''] = "_quote_";
      mapChars['('] = "_lpar_";
      mapChars[')'] = "_rpar_";
      mapChars['*'] = "_asterisk_";
      mapChars['+'] = "_plus_";
      mapChars[','] = "_comma_";
      mapChars['-'] = "_minus_";
      mapChars['.'] = "_period_";
      mapChars['/'] = "_slash_";
      mapChars[':'] = "_colon_";
      mapChars[';'] = "_semicolon_";
      mapChars['<'] = "_lt_";
      mapChars['='] = "_equal_";
      mapChars['>'] = "_gt_";
      mapChars['?'] = "_question_";
      mapChars['@'] = "_at_";
      mapChars['['] = "_lsbracket_";
      mapChars['\\'] = "_backslash_";
      mapChars[']'] = "_rsbracket_";
      mapChars['^'] = "_caret_";
      mapChars['`'] = "_grave_";
      mapChars['{'] = "_lcbrace_";
      mapChars['|'] = "_vbar_";
      mapChars['}'] = "_rcbrace_";
      mapChars['~'] = "_tilde_";
    }

    std::map<char, std::string>::const_iterator itSym = mapChars.find(sResult[rnPos]);
    if (itSym != mapChars.end())
    {
      sResult.replace(rnPos, 1, itSym->second);
      rnPos += itSym->second.size();
    }
    else
    {
      const std::string& sHex = '_' + ToHexString(static_cast<unsignedByte>(sResult[rnPos]));
      sResult.replace(rnPos, 1, sHex);
      rnPos += sHex.size();
    }
  }

  bool FixFileName(std::string& sFileName)
  {
    static const char* szInvalidChars = "\\/:*?\"<>|";
    bool bChanged = false;

    std::string::size_type nPos = 0;
    while ((nPos = sFileName.find_first_of(szInvalidChars, nPos)) != std::string::npos)
    {
      sFileName[nPos] = '_';
      bChanged = true;
    }

    return bChanged;
  }

  bool FixId(std::string& sId, bool bIgnoreBool /*= false*/)
  {
    static const char* szIdChars = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::set<std::string> setCppReservedWords;
    bool bChanged = false;

    if (sId.empty())
    {
      return false;
    }

    std::string::size_type nPos = 0;
    while ((nPos = sId.find_first_not_of(szIdChars, nPos)) != std::string::npos)
    {
      MangleCharInString(nPos, sId);
      bChanged = true;
    }

    const char chFirst = sId[0];
    if (chFirst >= '0' && chFirst <= '9')
    {
      sId.insert(0, "_", 1);
      return true;
    }

    if (setCppReservedWords.empty())
    {
      const unsigned nCppReservedWordsCount = 79;
      const char* aszCppReservedWords[nCppReservedWordsCount] =
      {
        "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "_Bool", "break", "case", "catch",
        "char", "class", "compl", "_Complex", "const", "const_cast", "continue", "default", "delete",
        "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
        "for", "friend", "goto", "if", "_Imaginary", "inline", "int", "long", "mutable", "namespace",
        "new", "not", "not_eq", "operator", "or", "or_eq", "private", "protected", "public", "register",
        "reinterpret_cast", "restrict", "return", "short", "signed", "sizeof", "static", "static_cast",
        "struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename",
        "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq",
        "typeid"
      };
      for (unsigned nIndex = 0; nIndex < nCppReservedWordsCount; ++nIndex)
      {
        setCppReservedWords.insert(aszCppReservedWords[nIndex]);
      }
    }

    if (setCppReservedWords.count(sId))
    {
      if (!bIgnoreBool || (sId != "true" && sId != "false"))
      {
        sId += '_';
        bChanged = true;
      }
    }

    return bChanged;
  }

  const std::string& StringMapValue(const StringMap& rmMap, const std::string& sName,
                                    const std::string& sDefaultValue /*= ""*/)
  {
    StringMap::const_iterator itValue = rmMap.find(sName);
    return (itValue != rmMap.end()) ? itValue->second : sDefaultValue;
  }

}
}
