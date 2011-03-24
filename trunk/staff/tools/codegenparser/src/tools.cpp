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

#include "Interface.h"
#include "tools.h"

namespace staff
{
namespace codegen
{

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

  const SBaseType* GetBaseType(const std::string& sNsName, const SInterface& rstInterface,
                               const int eBaseType /*= SBaseType::EAny*/,
                               const SStruct* pstParent /*= NULL*/)
  {
    const SStruct* pstCurr = pstParent;
    const SBaseType* pstResult = NULL;

    std::string::size_type nNsNameSize = sNsName.size();

    // look substructs
    for(;;)
    { // to avod getting extern type with the same name as in current interface
      // we must search backwards, because firstly goes extern types
      const std::list<SStruct>& rStructList = !pstCurr ? rstInterface.lsStructs : pstCurr->lsStructs;
      for (std::list<SStruct>::const_reverse_iterator itStruct = rStructList.rbegin();
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
          if (((eBaseType & SBaseType::EStruct) != 0) && !nSizeDiff && sCurrNsName == sNsName)
          {
            pstResult = &*itStruct;
            break; // return
          }
          else
          {
            // empty/partial namespace
            if (nSizeDiff >= 2) // size of "::"
            {
              if (((eBaseType & SBaseType::EStruct) != 0) &&
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

                // find in sub enums
                if ((eBaseType & SBaseType::EEnum) != 0)
                {
                  for (std::list<SEnum>::const_reverse_iterator itSubEnum = pstTmp->lsEnums.rbegin();
                    itSubEnum != pstTmp->lsEnums.rend(); ++itSubEnum)
                  {
                    if (itSubEnum->sName == sSubName)
                    {
                      return &*itSubEnum;
                    }
                  }
                }

                // go into sub struct
                for (std::list<SStruct>::const_reverse_iterator itSubStruct = pstTmp->lsStructs.rbegin();
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
      if (!pstResult && ((eBaseType & SBaseType::EEnum) != 0))
      {
        const std::list<SEnum>& rlsEnums = !pstCurr ? rstInterface.lsEnums : pstCurr->lsEnums;
        for (std::list<SEnum>::const_reverse_iterator itEnum = rlsEnums.rbegin();
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

  const SStruct* GetStruct(const std::string& sNsName, const SInterface& rstInterface,
                           const SStruct* pstParent /*= NULL*/)
  {
    return static_cast<const SStruct*>(GetBaseType(sNsName, rstInterface, SBaseType::EStruct, pstParent));
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

    if (nPos > 2 && sOptimizeNs.substr(nPos - 2, 2) == "::")
    {
      sOptimizeNs.erase(0, nPos);
    }
  }

}
}
