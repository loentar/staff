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

#include <rise/os/oscommon.h>
#include <string>
#include "oserror.h"
#include "osdl.h"

namespace rise
{
  rise::HDynamicLib osLoadLibrary( const CString& sLibraryName, bool bRawName )
  {
#ifdef WIN32
    if(bRawName)
      return LoadLibrary(sLibraryName.c_str());
    else
      return LoadLibrary((sLibraryName + RISE_LIBRARY_EXT).c_str());
#else
    if(bRawName)
      return dlopen(sLibraryName.c_str(), RTLD_LAZY);
    else
    {
      rise::CString::size_type nPos = sLibraryName.find_last_of('/');
      if(nPos == rise::CString::npos)
        return dlopen((RISE_LIBRARY_PREFIX + sLibraryName + RISE_LIBRARY_EXT).c_str(), RTLD_LAZY);
      else
        return dlopen((sLibraryName.substr(nPos) + RISE_LIBRARY_PREFIX + sLibraryName.substr(nPos + 1) + RISE_LIBRARY_EXT).c_str(), RTLD_LAZY);
    }
#endif
  }

  rise::PLibSymbol osGetSymbol( HDynamicLib hDynamicLib, const CStringA& sSymName )
  {
    return
#ifdef WIN32
#ifdef __MINGW32__
      reinterpret_cast<rise::PLibSymbol>(GetProcAddress(hDynamicLib, sSymName.c_str()));
#else
      GetProcAddress(hDynamicLib, sSymName.c_str());
#endif
#else
      dlsym(hDynamicLib, sSymName.c_str());
#endif
  }

  bool osFreeLibrary(HDynamicLib hDynamicLib)
  {
    return
#ifdef WIN32
      FreeLibrary(hDynamicLib) == TRUE;
#else
      dlclose(hDynamicLib);
#endif
  }

  long osGetLastLibraryError()
  {
    return
#ifdef WIN32
      GetLastError();
#else
      dlerror() == NULL ? -1 : 0;
#endif
  }

  CString osGetLastLibraryErrorStr()
  {
    return
#ifdef WIN32
      osGetErrorStr(osGetLastLibraryError());
#else
      dlerror();
#endif
  }

}
