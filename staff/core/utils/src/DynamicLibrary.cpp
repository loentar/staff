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

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include "Exception.h"
#include "Error.h"
#include "DynamicLibrary.h"

namespace staff
{

    DynamicLibrary::DynamicLibrary():
      m_pDynLib(NULL)
    {
    }

    DynamicLibrary::~DynamicLibrary()
    {
      // do not unload library here to avoid random crashes
      // caused by inconsistent order of symbols destruction
    }

    void DynamicLibrary::Load(const std::string& sLibName, bool bRawName /*= false*/)
    {
      STAFF_ASSERT(!m_pDynLib, "Library already loaded");

      if (bRawName)
      {
#ifdef WIN32
        m_pDynLib = LoadLibraryA(sLibName.c_str());
#else
        m_pDynLib = dlopen(sLibName.c_str(), RTLD_LAZY);
#endif
      }
      else
      {
#ifdef WIN32
        m_pDynLib = LoadLibraryA((sLibName + STAFF_LIBRARY_EXT).c_str());
#else
        std::string::size_type nPos = sLibName.find_last_of('/');
        if (nPos == std::string::npos)
        {
          m_pDynLib = dlopen((STAFF_LIBRARY_PREFIX + sLibName + STAFF_LIBRARY_EXT).c_str(), RTLD_LAZY);
        }
        else
        {
          m_pDynLib = dlopen((sLibName.substr(nPos) + STAFF_LIBRARY_PREFIX +
                              sLibName.substr(nPos + 1) + STAFF_LIBRARY_EXT).c_str(), RTLD_LAZY);
        }
#endif
      }

      STAFF_ASSERT(m_pDynLib, "Failed load library [" + sLibName + "]: " + Error::GetLastLibraryErrorStr());
      m_sName = sLibName;
    }

    const std::string& DynamicLibrary::GetName() const
    {
      STAFF_ASSERT(m_pDynLib, "Library is not loaded");
      return m_sName;
    }

    void* DynamicLibrary::GetSymbol(const std::string& sSymName) const
    {
      STAFF_ASSERT(m_pDynLib, "Library is not loaded");

      void* pSym =
#ifdef WIN32
#ifdef __MINGW32__
        reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(m_pDynLib), sSymName.c_str()));
#else
        GetProcAddress(reinterpret_cast<HMODULE>(m_pDynLib), sSymName.c_str());
#endif
#else
        dlsym(m_pDynLib, sSymName.c_str());
#endif

      STAFF_ASSERT(pSym, "Failed to get symbol [" + sSymName + "]: " + Error::GetLastLibraryErrorStr());
      return pSym;
    }

    void DynamicLibrary::UnloadLibrary()
    {
      STAFF_ASSERT(m_pDynLib, "Library is not loaded");
#ifdef WIN32
      FreeLibrary(reinterpret_cast<HMODULE>(m_pDynLib));
#else
      dlclose(m_pDynLib);
#endif
      m_pDynLib = NULL;
    }

}
