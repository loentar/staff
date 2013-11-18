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


#ifndef _STAFF_UTILS_FROMCSTRING_H_
#define _STAFF_UTILS_FROMCSTRING_H_

#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define staff_strtoll _strtoi64
#define staff_strtoull _strtoui64
#define staff_strtof(str, end) static_cast<float>(strtod(str, end))
#define staff_strtold(str, end) static_cast<long double>(strtod(str, end))
#else
#define staff_strtoll strtoll
#define staff_strtoull strtoull
#define staff_strtof strtof
#define staff_strtold strtold
#endif

namespace staff
{
  // wsdl compat
  typedef char byte;
  typedef unsigned char unsignedByte;

  inline bool FromCString(const char* szString, bool& rbValue)
  {
    if (!strcmp(szString, "true") || !strcmp(szString, "1"))
    {
      rbValue = true;
      return true;
    }

    if (!strcmp(szString, "false") || !strcmp(szString, "0"))
    {
      rbValue = false;
      return true;
    }

    return false;
  }


  inline bool FromCString(const char* szString, byte& rbtValue)
  {
    char* szEndPtr = NULL;
    rbtValue = static_cast<byte>(strtol(szString, &szEndPtr, 10));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, int& rnValue)
  {
    char* szEndPtr = NULL;
    rnValue = static_cast<int>(strtol(szString, &szEndPtr, 10));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, short& rshValue)
  {
    char* szEndPtr = NULL;
    rshValue = static_cast<short>(strtol(szString, &szEndPtr, 10));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, long& rlValue)
  {
    char* szEndPtr = NULL;
    rlValue = strtol(szString, &szEndPtr, 10);
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, long long& rllValue)
  {
    char* szEndPtr = NULL;
    rllValue = staff_strtoll(szString, &szEndPtr, 10);
    return !szEndPtr || *szEndPtr == '\0';
  }


  inline bool FromCString(const char* szString, unsignedByte& rubtValue)
  {
    char* szEndPtr = NULL;
    rubtValue = static_cast<unsignedByte>(strtoul(szString, &szEndPtr, 10));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, unsigned int& runValue)
  {
    char* szEndPtr = NULL;
    runValue = static_cast<unsigned int>(strtoul(szString, &szEndPtr, 10));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, unsigned short& rushValue)
  {
    char* szEndPtr = NULL;
    rushValue = static_cast<unsigned short>(strtoul(szString, &szEndPtr, 10));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, unsigned long& rulValue)
  {
    char* szEndPtr = NULL;
    rulValue = strtoul(szString, &szEndPtr, 10);
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, unsigned long long& rullValue)
  {
    char* szEndPtr = NULL;
    rullValue = staff_strtoull(szString, &szEndPtr, 10);
    return !szEndPtr || *szEndPtr == '\0';
  }


  inline bool FromCString(const char* szString, float& rfValue)
  {
    char* szEndPtr = NULL;
    rfValue = staff_strtof(szString, &szEndPtr);
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, double& rdValue)
  {
    char* szEndPtr = NULL;
    rdValue = strtod(szString, &szEndPtr);
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromCString(const char* szString, long double& rldValue)
  {
    char* szEndPtr = NULL;
    rldValue = staff_strtold(szString, &szEndPtr);
    return !szEndPtr || *szEndPtr == '\0';
  }

  template <typename Type>
  inline Type FromCStringDefault(const char* szString, Type tDefaultValue)
  {
    Type tResult = 0;
    return (szString != NULL && FromCString(szString, tResult)) ? tResult : tDefaultValue;
  }


  inline bool FromHexCString(const char* szString, unsignedByte& rubtValue)
  {
    char* szEndPtr = NULL;
    rubtValue = static_cast<unsignedByte>(strtoul(szString, &szEndPtr, 16));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromHexCString(const char* szString, unsigned int& runValue)
  {
    char* szEndPtr = NULL;
    runValue = static_cast<unsigned int>(strtoul(szString, &szEndPtr, 16));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromHexCString(const char* szString, unsigned short& rushValue)
  {
    char* szEndPtr = NULL;
    rushValue = static_cast<unsigned short>(strtoul(szString, &szEndPtr, 16));
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromHexCString(const char* szString, unsigned long& rulValue)
  {
    char* szEndPtr = NULL;
    rulValue = strtoul(szString, &szEndPtr, 16);
    return !szEndPtr || *szEndPtr == '\0';
  }

  inline bool FromHexCString(const char* szString, unsigned long long& rullValue)
  {
    char* szEndPtr = NULL;
    rullValue = staff_strtoull(szString, &szEndPtr, 16);
    return !szEndPtr || *szEndPtr == '\0';
  }

  template <typename Type>
  inline Type FromHexCStringDefault(const char* szString, Type tDefaultValue)
  {
    Type tResult = 0;
    return (szString != NULL && FromHexCString(szString, tResult)) ? tResult : tDefaultValue;
  }

}

#endif // _STAFF_UTILS_FROMCSTRING_H_
