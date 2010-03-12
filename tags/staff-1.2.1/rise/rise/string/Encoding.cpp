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
  CEncoding::TEncMap CEncoding::m_mEncMap;

  struct SInitEncoding
  {
    SInitEncoding()
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
    }
  };

  static SInitEncoding stInitEncoding;

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
    m_mEncMap.insert(TEncMap::value_type(rstEncType.nID, rstEncType));
  }

  const CEncoding::TEncMap& CEncoding::GetEncMap()
  {
    return m_mEncMap;
  }

  const SEncodingType& CEncoding::FindEncodingTypeByID( int nID )
  {
    TEncMap::const_iterator itFind = m_mEncMap.find(nID);
    RISE_ASSERTES(itFind != m_mEncMap.end(), CLogicNoItemException, "Encoding type #" + ToStr(nID) + "not found");
    return itFind->second;
  }

  const SEncodingType& CEncoding::FindEncodingTypeByName( const CString& sEncName )
  {
    for(TEncMap::const_iterator itFind = m_mEncMap.begin(); itFind != m_mEncMap.end(); ++itFind)
      if (itFind->second.sName == sEncName)
        return itFind->second;

    RISE_THROWS(CLogicNoItemException, "Encoding type #" + sEncName + "not found");
  }

  void CEncoding::Convert( const CString& sFrom, CString& sTo, EType nIdFrom, EType nIdTo )
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

    if(nIdTo == ET_UTF_8)
      sTo.reserve(sFrom.size() * 2);
    else
      sTo.reserve(sFrom.size());
    
    for(CString::const_iterator itFrom = sFrom.begin(); itFrom != sFrom.end(); ++itFrom)
    {
      if (nIdFrom == ET_UTF_8) // utf-8
      {
        chTmp = *itFrom;

        if ((chTmp & 0x80) != 0) // not latin
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
        } else
          nCharPos = CString::npos; // take as single byte char
      } else
      { // single-byte encoding
        chTmp = *itFrom;
        wchTmp = (wchar_t)chTmp & 0xff;
        nCharPos = rstEncFrom.asTable.find(chTmp);
      }

      // destination encoding
      if (nIdTo == ET_UTF_8)
      { // utf-8
        if (nCharPos == CString::npos)
        { // if not found, there is a generic symbol(<128)
          sTo += chTmp;
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
  
}

