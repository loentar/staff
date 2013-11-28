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


#ifndef _STAFF_UTILS_TOSTRING_H_
#define _STAFF_UTILS_TOSTRING_H_

#include <string>
#include "tocstring.h"

namespace staff
{

  template<typename Type>
  std::string ToString(Type tValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(tValue, szBuffer, sizeof(szBuffer));
    return szBuffer;
  }

  template<typename Type>
  std::string& ToString(Type tValue, std::string& sResult)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToCString(tValue, szBuffer, sizeof(szBuffer));
    sResult = szBuffer;
    return sResult;
  }

  template<typename Type>
  std::string ToString(Type tValue, bool* pbIsOk)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    bool bIsOk = ToCString(tValue, szBuffer, sizeof(szBuffer));
    if (pbIsOk)
    {
      *pbIsOk = bIsOk;
    }
    return szBuffer;
  }

  template<typename Type>
  std::string& ToString(Type tValue, std::string& sResult, bool* pbIsOk)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    bool bIsOk = ToCString(tValue, szBuffer, sizeof(szBuffer));
    if (pbIsOk)
    {
      sResult = szBuffer;
      *pbIsOk = bIsOk;
    }
    return sResult;
  }


  template<typename Type>
  std::string ToHexString(Type tValue)
  {
    char szBuffer[NUM_TO_STR_BUFF_SIZE];
    ToHexCString(tValue, szBuffer, sizeof(szBuffer));
    return szBuffer;
  }


}

#endif // _STAFF_UTILS_TOSTRING_H_
