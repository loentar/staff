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

namespace staff
{
  // wsdl compat
  typedef char byte;
  typedef unsigned char unsignedByte;


  inline bool ToCString(bool bValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%s", bValue ? "true" : "false") < nBufferSize;
  }


  inline bool ToCString(byte btValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%d", static_cast<signed char>(btValue)) < nBufferSize;
  }

  inline bool ToCString(int nValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%d", nValue) < nBufferSize;
  }

  inline bool ToCString(short shValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%d", shValue) < nBufferSize;
  }

  inline bool ToCString(long lValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%ld", lValue) < nBufferSize;
  }

  inline bool ToCString(long long llValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%lld", llValue) < nBufferSize;
  }


  inline bool ToCString(unsignedByte ubtValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%d", ubtValue) < nBufferSize;
  }

  inline bool ToCString(unsigned int unValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%u", unValue) < nBufferSize;
  }

  inline bool ToCString(unsigned short ushValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%u", ushValue) < nBufferSize;
  }

  inline bool ToCString(unsigned long ulValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%lu", ulValue) < nBufferSize;
  }

  inline bool ToCString(unsigned long long ullValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%llu", ullValue) < nBufferSize;
  }


  inline bool ToCString(float fValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%f", fValue) < nBufferSize;
  }

  inline bool ToCString(double dValue, char* szBuffer, int nBufferSize)
  {
    return snprintf(szBuffer, nBufferSize, "%f", dValue) < nBufferSize;
  }

}

#endif // _STAFF_UTILS_TOCSTRING_H_
