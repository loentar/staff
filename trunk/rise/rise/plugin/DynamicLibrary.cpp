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

#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include "DynamicLibrary.h"

namespace rise
{
  namespace plugin
  {

    CDynamicLibrary::CDynamicLibrary():
      m_hDynLib(NULL)
    {
    }

    CDynamicLibrary::~CDynamicLibrary()
    {
      try 
      {
        if ( m_hDynLib )
        {
          LogDebug2() << "Unloading library";
          UnloadLibrary();
        }
      } catch (...) {}
    }

    void CDynamicLibrary::Load( const CString& sLibName, bool bRawName /*= false*/ )
    {
      RISE_ASSERTE(!m_hDynLib, CLogicAlreadyExistsException);
      m_hDynLib = osLoadLibrary(sLibName, bRawName);
      RISE_ASSERTES(m_hDynLib, CFileOpenException, osGetLastLibraryErrorStr());
      m_sName = sLibName;
    }

    const CString& CDynamicLibrary::GetName() const
    {
      RISE_ASSERTE(m_hDynLib, CLogicNoInitException);
      return m_sName;
    }

    PLibSymbol CDynamicLibrary::GetSymbol(const CString& sSymName) const
    {
      PLibSymbol pSym;
      RISE_ASSERTE(m_hDynLib, CLogicNoInitException);
      pSym = osGetSymbol(m_hDynLib, sSymName);
      RISE_ASSERTE(pSym, CLogicNoItemException);
      return pSym;
    }

    void CDynamicLibrary::UnloadLibrary()
    {
      RISE_ASSERTE(m_hDynLib, CLogicNoInitException);
      osFreeLibrary(m_hDynLib);
      m_hDynLib = NULL;
    }

  } // namespace plugin
} // namespace rise
