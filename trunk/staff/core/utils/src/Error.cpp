/*
 *  Copyright 2012 Utkin Dmitry
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

#ifdef WIN32
#include <windows.h>
#else
#include <errno.h>
#include <string.h>
#include <dlfcn.h>
#endif
#include "Error.h"

namespace staff
{

  std::string Error::GetErrorStr(long lErrorNo)
  {
  #ifdef WIN32
    LPVOID lpMsgBuf = NULL;
    DWORD dwChars = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                     FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lErrorNo, 0,
                                     reinterpret_cast<LPSTR>(&lpMsgBuf), 0, NULL);

    if (!dwChars || !lpMsgBuf)
    {
      return "<Unknown error>";
    }
    else
    {
      std::string sError(reinterpret_cast<LPCSTR>(lpMsgBuf),
                         static_cast<std::string::size_type>(dwChars));
      LocalFree(lpMsgBuf);
      return sError;
    }
  #else
    return strerror(lErrorNo);
  #endif
  }

  std::string Error::GetLastErrorStr()
  {
    return
#ifdef WIN32
        GetErrorStr(GetLastError());
#else
        GetErrorStr(errno);
#endif
  }

  std::string Error::GetLastLibraryErrorStr()
  {
    return
#ifdef WIN32
      GetErrorStr(GetLastError());
#else
      dlerror();
#endif
  }


}
