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
    char szBuffer[32];
    ToCString(tValue, &szBuffer, sizeof(szBuffer));
    return szBuffer;
  }

  template<typename Type>
  bool ToString(Type tValue, std::string& sResult)
  {
    char szBuffer[32];
    bool bResult = ToCString(tValue, &szBuffer, sizeof(szBuffer));
    if (bResult)
    {
      sResult = szBuffer;
    }
    return bResult;
  }

}

#endif // _STAFF_UTILS_TOSTRING_H_
