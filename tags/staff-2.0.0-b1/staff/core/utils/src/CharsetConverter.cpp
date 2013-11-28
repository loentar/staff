/*
 *  Copyright 2011 Utkin Dmitry
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

#ifdef WITH_ICONV
#include <iconv.h>
#else
#ifdef _MSC_VER
#define STAFF_STRINGIZE(x) #x
#define STAFF_TO_STRING(x) STAFF_STRINGIZE(x)
#pragma message(__FILE__ "(" STAFF_TO_STRING(__LINE__) ") : warning: building without iconv support")
#else
#warning Warning: building without iconv support
#endif
#endif
#include <errno.h>
#include "Error.h"
#include "Log.h"
#include "CharsetConverter.h"

namespace staff
{

  template <typename Type>
  inline Type Min(Type nA, Type nB)
  {
    return nA < nB ? nA : nB;
  }

  CharsetConverter::CharsetConverter(const char* szFrom, const char* szTo)
  {
#ifdef WITH_ICONV
    m_pIconv = iconv_open(szTo, szFrom);
    if (m_pIconv == reinterpret_cast<iconv_t>(-1))
    {
      LogError() << "Error iconv_open: " << Error::GetLastErrorStr();
    }
#endif
  }

  CharsetConverter::CharsetConverter(const std::string& sFrom, const std::string& sTo)
  {
#ifdef WITH_ICONV
    m_pIconv = iconv_open(sTo.c_str(), sFrom.c_str());
    if (m_pIconv == reinterpret_cast<iconv_t>(-1))
    {
      LogError() << "Error iconv_open: " << Error::GetLastErrorStr();
    }
#endif
  }

  CharsetConverter::~CharsetConverter()
  {
#ifdef WITH_ICONV
    iconv_close(m_pIconv);
#endif
  }

  bool CharsetConverter::Convert(const char* szSource, unsigned long nSourceSize,
                                 char* szResult, unsigned long nResultSize,
                                 unsigned long* pnConvertedSize /*= NULL*/)
  {
#ifndef WITH_ICONV
    if (pnConvertedSize)
    {
      *pnConvertedSize = 0;
    }
    return false;
#else
    if (m_pIconv == reinterpret_cast<iconv_t>(-1))
    {
      return false;
    }

#if defined _LIBICONV_VERSION && _LIBICONV_VERSION >= 0x010D
    const char* szSrc = szSource;
#else
    char* szSrc = const_cast<char*>(szSource);
#endif
    size_t nSrcSize = static_cast<size_t>(nSourceSize);
    size_t nOut = nResultSize;

#ifndef sun
    size_t nRet = iconv(m_pIconv, &szSrc, &nSrcSize, &szResult, &nOut);
#else
    size_t nRet = iconv(m_pIconv,const_cast<const char **>(&szSrc), &nSrcSize, &szResult, &nOut);
#endif

    if (pnConvertedSize)
    {
      *pnConvertedSize = static_cast<unsigned long>(nOut);
    }

    if (nRet == static_cast<size_t>(-1))
    {
      return false;
    }

    return true;
#endif
  }

  bool CharsetConverter::Convert(const std::string& sSource, std::string& sResult)
  {
    sResult.erase();
#ifndef WITH_ICONV
    return false;
#else
    if (m_pIconv == reinterpret_cast<iconv_t>(-1))
    {
      return false;
    }

    const static size_t nOutBuffSize = 256;
    char szOutBuff[nOutBuffSize];
    size_t nOut = nOutBuffSize;

#if defined _LIBICONV_VERSION && _LIBICONV_VERSION >= 0x010D
    const char* szSrc = sSource.data();
    const char* szSrcCurr = NULL;
#else
    char* szSrc = const_cast<char*>(sSource.data());
    char* szSrcCurr = NULL;
#endif
    size_t nSrcSize = sSource.size();
    size_t nSrcPos = 0;
    size_t nSrcCurrSize = 0;
    size_t nSrcCurrSizeRead = 0;

    char* szOutTmp = NULL;
    size_t nRet = static_cast<size_t>(-1);

    while (nSrcPos != nSrcSize)
    {
      nSrcCurrSize = Min(nOutBuffSize / 4, nSrcSize - nSrcPos);
      nSrcCurrSizeRead = nSrcCurrSize;
      szSrcCurr = szSrc + nSrcPos;
      szOutTmp = szOutBuff;

#ifndef sun
      nRet = iconv(m_pIconv, &szSrcCurr, &nSrcCurrSizeRead, &szOutTmp, &nOut);
#else
      nRet = iconv(m_pIconv, const_cast<const char **>(&szSrcCurr), &nSrcCurrSizeRead, &szOutTmp, &nOut);
#endif
      if (nRet == static_cast<size_t>(-1))
      {
        switch (errno)
        {
        case EINVAL:
          break;
        case EILSEQ:
          --nSrcCurrSizeRead;
          break;
        default:
          LogError() << "Error iconv: " << Error::GetLastErrorStr();
          iconv_close(m_pIconv);
          return false;
        }
      }

      sResult.append(szOutBuff, nOutBuffSize - nOut);
      nOut = nOutBuffSize;
      nSrcPos += nSrcCurrSize - nSrcCurrSizeRead;
    }

    return true;
#endif
  }

}
