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
      return LoadLibrary((sLibraryName + ".dll").c_str());
#else
    if(bRawName)
      return dlopen(sLibraryName.c_str(), RTLD_LAZY);
    else
    {
      rise::CString::size_type nPos = sLibraryName.find_last_of('/');
      if(nPos == rise::CString::npos)
        return dlopen(("lib" + sLibraryName + ".so").c_str(), RTLD_LAZY);
      else
        return dlopen((sLibraryName.substr(nPos) + "lib" + sLibraryName.substr(nPos + 1) + ".so").c_str(), RTLD_LAZY);
    }
#endif
  }

  rise::PLibSymbol osGetSymbol( HDynamicLib hDynamicLib, const CStringA& sSymName )
  {
    return
#ifdef WIN32
      GetProcAddress(hDynamicLib, sSymName.c_str());
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
