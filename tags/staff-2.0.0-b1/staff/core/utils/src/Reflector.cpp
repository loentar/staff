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


#if defined __GNUC__
#include <cxxabi.h>
#endif
#include <string.h>
#include "Reflector.h"
#include "Exception.h"

namespace staff
{
  
  std::string Reflector::DecodeTypeName(const char* szName)
  {
    std::string sResult;
#if defined __GNUC__
    // "N1a3bbb5ChildE"
    int nStatus = 0;
    char* szDemangled = abi::__cxa_demangle(szName, 0, 0, &nStatus);
    STAFF_ASSERT(!nStatus && szDemangled, "Symbol demangling failed: [" + std::string(szName) + "]");
    char* szEnd = strchr(szDemangled, '*');
    if (szEnd)
    {
      *szEnd = '\0';
    }
    sResult = szDemangled;
    delete szDemangled;
#elif defined _MSC_VER
    // "struct a::bbb::Child", "struct a::bbb::Child *"
    const char* szStart = strchr(szName, ' ');
    STAFF_ASSERT(szStart, "Symbol demangling failed: [" + std::string(szName) + "]");
    ++szStart;
    const char* szEnd = strchr(szStart, ' ');
    if (!szEnd)
    {
      sResult = szStart;
    }
    else
    {
      sResult.assign(szStart, szEnd - szStart);
    }
#else
#error Unsupported compiler
#endif

    for (std::string::size_type nPos = 0;
         (nPos = sResult.find("::", nPos)) != std::string::npos; ++nPos)
    {
      sResult.replace(nPos, 2, 1, '.');
    }
    return sResult;
  }
  
}
