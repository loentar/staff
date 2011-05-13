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

#include <rise/string/String.h>
#include <rise/common/ExceptionTemplate.h>
#include "Encoding.h"

namespace rise
{
  SEncodingType::SEncodingType(int nIDInit, const CString& sNameInit, const CString& sAltNameInit, const CStringA& asTableInit, const CStringW& wsTableInit /*= NULL*/, const CStringA& asMagicInit /*= NULL*/):
    nID(nIDInit),
    sName(sNameInit),
    sAltName(sAltNameInit),
    asTable(asTableInit),
    wsTable(wsTableInit),
    asMagic(asMagicInit)
  {
  }

  void CEncoding::AddUserEncodingType( const SEncodingType& rstEncType )
  {
    EncMap().insert(TEncMap::value_type(rstEncType.nID, rstEncType));
  }

  const CEncoding::TEncMap& CEncoding::GetEncMap()
  {
    return EncMap();
  }

  const SEncodingType& CEncoding::FindEncodingTypeByID( int nID )
  {
    const TEncMap& rmEncMap = EncMap();
    TEncMap::const_iterator itFind = rmEncMap.find(nID);
    RISE_ASSERTES(itFind != rmEncMap.end(), CLogicNoItemException, "Encoding type #" + ToStr(nID) + " not found");
    return itFind->second;
  }

  const SEncodingType& CEncoding::FindEncodingTypeByName( const CString& sEncName )
  {
    const TEncMap& rmEncMap = EncMap();

    for(TEncMap::const_iterator itFind = rmEncMap.begin(); itFind != rmEncMap.end(); ++itFind)
    {
      if (itFind->second.sName == sEncName)
      {
        return itFind->second;
      }
    }

    RISE_THROWS(CLogicNoItemException, "Encoding type #" + sEncName + " not found");
  }

  int CEncoding::FindEncodingIdByName(const CString& sEncName)
  {
    const TEncMap& rmEncMap = EncMap();

    for(TEncMap::const_iterator itFind = rmEncMap.begin(); itFind != rmEncMap.end(); ++itFind)
    {
      if (itFind->second.sName == sEncName)
      {
        return itFind->first;
      }
    }

    return ET_UNKNOWN;
  }

  void CEncoding::Convert( const CString& sFrom, CString& sTo, int nIdFrom, int nIdTo,
                           bool bInsertMagicBytes /*= false*/ )
  {
    if (nIdFrom == nIdTo)
    {
      sTo = sFrom;
      return;
    }

    TCharA chTmp = '\0';
    TCharW wchTmp = L'\0';
    CString::size_type nCharPos = CString::npos;

    const SEncodingType& rstEncFrom = FindEncodingTypeByID(nIdFrom);
    const SEncodingType& rstEncTo = FindEncodingTypeByID(nIdTo);

    sTo.erase();
    if (bInsertMagicBytes)
    {
      sTo += rstEncTo.asMagic;
    }

    sTo.reserve(sFrom.size() * (rstEncTo.wsTable.empty() ? 1 : 2));

    for(CString::const_iterator itFrom = sFrom.begin(); itFrom != sFrom.end(); ++itFrom)
    {
      // source encoding
      if (!rstEncFrom.wsTable.empty()) // unicode
      {
        chTmp = *itFrom;
        if (nIdFrom == ET_UTF_8 && (chTmp & 0x80) == 0)
        {
          nCharPos = CString::npos; // take as single byte char for utf-8
        }
        else
        {
          wchTmp = (wchar_t)chTmp & 0xff;
          ++itFrom;

          if(itFrom == sFrom.end())
            nCharPos = CString::npos;
          else
          {
            chTmp = *itFrom;
            wchTmp = (wchTmp << 8) | (((wchar_t)chTmp) & 0xff);

            nCharPos = rstEncFrom.wsTable.find(wchTmp);
          }
        }
      } else
      { // single-byte encoding
        chTmp = *itFrom;
        wchTmp = (wchar_t)chTmp & 0xff;
        nCharPos = rstEncFrom.asTable.find(chTmp);
      }

      // destination encoding
      if (!rstEncTo.wsTable.empty()) // unicode
      {
        if (nCharPos == CString::npos)
        { // if given char is not found, assume it as generic latin symbol(<128)
          if (nIdTo == ET_UTF_16BE)
          {
            sTo += '\0';
          }
          sTo += chTmp;
          if (nIdTo == ET_UTF_16LE)
          {
            sTo += '\0';
          }
        } else
        { // found
          sTo += (char)((rstEncTo.wsTable[nCharPos] >> 8) & 0xff);
          sTo += (char)(rstEncTo.wsTable[nCharPos] & 0xff);
        }
      } else
      { // single-byte encoding
        if (nCharPos == CString::npos)
        { // if not found, there is a generic symbol(<128)
          sTo += chTmp;
        } else // found
          sTo += rstEncTo.asTable[nCharPos];
      }
    }
  }

  CString CEncoding::Convert(const CString& sFrom, int nIdFrom, int nIdTo, bool bInsertMagicBytes /*= false*/)
  {
    CString sResult;
    Convert(sFrom, sResult, nIdFrom, nIdTo, bInsertMagicBytes);
    return sResult;
  }

  void CEncoding::Convert(const CString& sFrom, CString& sTo, const CString& sFromName, const CString& sToName,
                      bool bInsertMagicBytes /*= false*/)
  {
    int nIdFrom = FindEncodingIdByName(sFromName);
    int nIdTo = FindEncodingIdByName(sToName);

    RISE_ASSERTES(nIdFrom != ET_UNKNOWN, CLogicNoItemException, "Encoding type #" + sFromName + " not found");
    RISE_ASSERTES(nIdTo != ET_UNKNOWN, CLogicNoItemException, "Encoding type #" + sToName + " not found");

    Convert(sFrom, sTo, nIdFrom, nIdTo, bInsertMagicBytes);
  }

  CString CEncoding::Convert(const CString& sFrom, const CString& sFromName, const CString& sToName,
                         bool bInsertMagicBytes /*= false*/)
  {
    CString sResult;
    Convert(sFrom, sResult, sFromName, sToName, bInsertMagicBytes);
    return sResult;
  }


   CEncoding::TEncMap& CEncoding::EncMap()
   {
     static TEncMap mEncMap;
     if (mEncMap.empty())
     {
       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_UTF_8, "UTF-8", "UTF-8",
           "",
           L"\xD0B0\xD0B1\xD0B2\xD0B3\xD0B4\xD0B5\xD191\xD0B6\xD0B7\xD0B8\xD0B9\xD0BA\xD0BB\xD0BC\xD0BD\xD0BE\xD0BF"
           L"\xD180\xD181\xD182\xD183\xD184\xD185\xD186\xD187\xD188\xD189\xD18A\xD18B\xD18C\xD18D\xD18E\xD18F"
           L"\xD090\xD091\xD092\xD093\xD094\xD095\xD081\xD096\xD097\xD098\xD099\xD09A\xD09B\xD09C\xD09D\xD09E\xD09F"
           L"\xD0A0\xD0A1\xD0A2\xD0A3\xD0A4\xD0A5\xD0A6\xD0A7\xD0A8\xD0A9\xD0AA\xD0AB\xD0AC\xD0AD\xD0AE\xD0AF\x0000",
           "\xEF\xBB\xBF"
           ));

       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_KOI8R, "koi8-r", "koi8-r",
           "\xc1\xc2\xd7\xc7\xc4\xc5\xa3\xd6\xda\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd2\xd3\xd4\xd5\xc6\xc8\xc3\xde\xdb\xdd\xdf\xd9\xd8\xdc\xc0\xd1"
           "\xe1\xe2\xf7\xe7\xe4\xe5\xb3\xf6\xfa\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf2\xf3\xf4\xf5\xe6\xe8\xe3\xfe\xfb\xfd\xff\xf9\xf8\xfc\xe0\xf1\x0"
           ));

       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_WIN1251, "windows-1251", "windows-1251",
           "\xe0\xe1\xe2\xe3\xe4\xe5\xb8\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
           "\xc0\xc1\xc2\xc3\xc4\xc5\xa8\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\x0"
           ));

       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_CP866, "cp866", "cp866",
           "\xa0\xa1\xa2\xa3\xa4\xa5\xf1\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
           "\x80\x81\x82\x83\x84\x85\xf0\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\x0"
           ));

       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_MACINTOSH, "macintosh", "macintosh",
           "\xe0\xe1\xe2\xe3\xe4\xe5\xde\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xdf"
           "\x80\x81\x82\x83\x84\x85\xdd\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\x0"
           ));

       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_ISO_8859_5, "ISO-8859-5", "ISO-8859-5",
           "\xd0\xd1\xd2\xd3\xd4\xd5\xf1\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
           "\xb0\xb1\xb2\xb3\xb4\xb5\xa1\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\x0"
           ));


       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_UTF_16LE, "UTF-16LE", "UTF-16",
           "",
           L"\x3004\x3104\x3204\x3304\x3404\x3504\x5104\x3604"
           L"\x3704\x3804\x3904\x3a04\x3b04\x3c04\x3d04\x3e04"
           L"\x3f04\x4004\x4104\x4204\x4304\x4404\x4504\x4604"
           L"\x4704\x4804\x4904\x4a04\x4b04\x4c04\x4d04\x4e04"
           L"\x4f04"
           L"\x1004\x1104\x1204\x1304\x1404\x1504\x0104\x1604"
           L"\x1704\x1804\x1904\x1a04\x1b04\x1c04\x1d04\x1e04"
           L"\x1f04\x2004\x2104\x2204\x2304\x2404\x2504\x2604"
           L"\x2704\x2804\x2904\x2a04\x2b04\x2c04\x2d04\x2e04"
           L"\x2f04"
           L"\x0000",
           "\xFF\xFE"
           ));

       CEncoding::AddUserEncodingType(
         SEncodingType(CEncoding::ET_UTF_16BE, "UTF-16BE", "UTF-16",
           "",
           L"\x0430\x0431\x0432\x0433\x0434\x0435\x0451\x0436"
           L"\x0437\x0438\x0439\x043a\x043b\x043c\x043d\x043e"
           L"\x043f\x0440\x0441\x0442\x0443\x0444\x0445\x0446"
           L"\x0447\x0448\x0449\x044a\x044b\x044c\x044d\x044e"
           L"\x044f"
           L"\x0410\x0411\x0412\x0413\x0414\x0415\x0401\x0416"
           L"\x0417\x0418\x0419\x041a\x041b\x041c\x041d\x041e"
           L"\x041f\x0420\x0421\x0422\x0423\x0424\x0425\x0426"
           L"\x0427\x0428\x0429\x042a\x042b\x042c\x042d\x042e"
           L"\x042f"
           L"\x0000",
           "\xFE\xFF"
           ));
     }
     return mEncMap;
   }
}

