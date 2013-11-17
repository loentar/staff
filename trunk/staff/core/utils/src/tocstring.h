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


#ifndef _STAFF_UTILS_TOCSTRING_H_
#define _STAFF_UTILS_TOCSTRING_H_

#include <stdio.h>

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#define staff_snprintf _snprintf
#else
#define staff_snprintf snprintf
#endif

namespace staff
{
  // wsdl compat
  typedef char byte;
  typedef unsigned char unsignedByte;

#ifndef NUM_TO_STR_BUFF_SIZE
  static const int NUM_TO_STR_BUFF_SIZE = 32;
#endif


  inline bool ToCString(bool bValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%s", bValue ? "true" : "false") < nBufferSize;
  }


  inline bool ToCString(byte btValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%d", static_cast<signed char>(btValue)) < nBufferSize;
  }

  inline bool ToCString(int nValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%d", nValue) < nBufferSize;
  }

  inline bool ToCString(short shValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%d", shValue) < nBufferSize;
  }

  inline bool ToCString(long lValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%ld", lValue) < nBufferSize;
  }

  inline bool ToCString(long long llValue, char* szBuffer, int nBufferSize)
  {
#ifdef WIN32
    return staff_snprintf(szBuffer, nBufferSize, "%I64d", llValue) < nBufferSize;
#else
    return staff_snprintf(szBuffer, nBufferSize, "%lld", llValue) < nBufferSize;
#endif
  }


  inline bool ToCString(unsignedByte ubtValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%d", ubtValue) < nBufferSize;
  }

  inline bool ToCString(unsigned int unValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%u", unValue) < nBufferSize;
  }

  inline bool ToCString(unsigned short ushValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%u", ushValue) < nBufferSize;
  }

  inline bool ToCString(unsigned long ulValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%lu", ulValue) < nBufferSize;
  }

  inline bool ToCString(unsigned long long ullValue, char* szBuffer, int nBufferSize)
  {
#ifdef WIN32
    return staff_snprintf(szBuffer, nBufferSize, "%I64u", ullValue) < nBufferSize;
#else
    return staff_snprintf(szBuffer, nBufferSize, "%llu", ullValue) < nBufferSize;
#endif
  }


  inline bool ToCString(float fValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%f", fValue) < nBufferSize;
  }

  inline bool ToCString(double dValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "%f", dValue) < nBufferSize;
  }

  inline bool ToCString(long double ldValue, char* szBuffer, int nBufferSize)
  {
#ifdef WIN32
    return staff_snprintf(szBuffer, nBufferSize, "%Le", ldValue) < nBufferSize;
#else
    return staff_snprintf(szBuffer, nBufferSize, "%Lf", ldValue) < nBufferSize;
#endif
  }

  inline bool ToCString(float fValue, char* szBuffer, int nBufferSize, const char* szPrec)
  {
    char szFormat[NUM_TO_STR_BUFF_SIZE];
    if (staff_snprintf(szFormat, nBufferSize, "%%%sf", szPrec) >= nBufferSize)
      return false;

    return staff_snprintf(szBuffer, nBufferSize, szFormat, fValue) < nBufferSize;
  }

  inline bool ToCString(double dValue, char* szBuffer, int nBufferSize, const char* szPrec)
  {
    char szFormat[NUM_TO_STR_BUFF_SIZE];
    if (staff_snprintf(szFormat, nBufferSize, "%%%sf", szPrec) >= nBufferSize)
      return false;

    return staff_snprintf(szBuffer, nBufferSize, szFormat, dValue) < nBufferSize;
  }

  inline bool ToCString(long double ldValue, char* szBuffer, int nBufferSize, const char* szPrec)
  {
    char szFormat[NUM_TO_STR_BUFF_SIZE];
#ifdef WIN32
    if (staff_snprintf(szFormat, nBufferSize, "%%%sLe", szPrec) >= nBufferSize)
#else
    if (staff_snprintf(szFormat, nBufferSize, "%%%sLf", szPrec) >= nBufferSize)
#endif
      return false;

    return staff_snprintf(szBuffer, nBufferSize, szFormat, ldValue) < nBufferSize;
  }


  inline bool ToHexCString(unsignedByte ubtValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "0x%x", ubtValue) < nBufferSize;
  }

  inline bool ToHexCString(unsigned int unValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "0x%x", unValue) < nBufferSize;
  }

  inline bool ToHexCString(unsigned short ushValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "0x%x", ushValue) < nBufferSize;
  }

  inline bool ToHexCString(unsigned long ulValue, char* szBuffer, int nBufferSize)
  {
    return staff_snprintf(szBuffer, nBufferSize, "0x%lx", ulValue) < nBufferSize;
  }

  inline bool ToHexCString(unsigned long long ullValue, char* szBuffer, int nBufferSize)
  {
#ifdef WIN32
    return staff_snprintf(szBuffer, nBufferSize, "0x%I64x", ullValue) < nBufferSize;
#else
    return staff_snprintf(szBuffer, nBufferSize, "0x%llx", ullValue) < nBufferSize;
#endif
  }

  inline bool ToHexCString(const void* pAddr, char* szBuffer, int nBufferSize)
  {
#if defined __LP64__ || defined _M_X64 || defined __x86_64
    return staff_snprintf(szBuffer, nBufferSize,
                          "0x%016lx", reinterpret_cast<const unsigned long>(pAddr)) < nBufferSize;
#else
    return staff_snprintf(szBuffer, nBufferSize,
                          "0x%08x", reinterpret_cast<const unsigned int>(pAddr)) < nBufferSize;
#endif
  }

}

#endif // _STAFF_UTILS_TOCSTRING_H_
