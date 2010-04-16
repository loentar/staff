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

#include <string.h>
#include "oserror.h"

namespace rise
{
  RISE_EXPORT CString osGetErrorStr( int nError )
  {
#ifdef WIN32
    CString sMsg;

    sMsg.resize(1024);

    DWORD dwChars = ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
              0, nError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
              const_cast<TChar*>(sMsg.c_str()), static_cast<DWORD>(sMsg.size()), NULL );

    if (dwChars == 0)
      return "<Unknown error>";

    sMsg.erase(dwChars);

    return sMsg;
#else
//    return CStringA(strerror(nError)).ToStr();
    return strerror(nError);
#endif
  }
}
