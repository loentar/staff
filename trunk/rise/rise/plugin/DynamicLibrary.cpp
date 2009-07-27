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

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CDynamicLibrary
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CDynamicLibrary::CDynamicLibrary():
      m_hDynLib(NULL)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     CDynamicLibrary
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Load
    //    DESCRIPTION:    подгрузка динамической библиотеки
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CDynamicLibrary::Load( const CString& sLibName, bool bRawName /*= false*/ )
    {
      RISE_ASSERTE(!m_hDynLib, CLogicAlreadyExistsException);
      m_hDynLib = osLoadLibrary(sLibName, bRawName);
      RISE_ASSERTES(m_hDynLib, CFileOpenException, osGetLastLibraryErrorStr());
      m_sName = sLibName;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetName()
    //    DESCRIPTION:    получение имени
    //    PARAMETRS:      none
    //    RETURN:         имя библиотеки
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    const CString& CDynamicLibrary::GetName() const
    {
      RISE_ASSERTE(m_hDynLib, CLogicNoInitException);
      return m_sName;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetSymbol
    //    DESCRIPTION:    получение указателя на символ в библиотеке
    //    PARAMETRS:      sSymName - имя символа
    //    RETURN:         указатель на функцию, NULL при ошибке
    //    EXCEPTIONS:     CLogicNoItemException - указанный символ не найден
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    PLibSymbol CDynamicLibrary::GetSymbol(const CString& sSymName) const
    {
      PLibSymbol pSym;
      RISE_ASSERTE(m_hDynLib, CLogicNoInitException);
      pSym = osGetSymbol(m_hDynLib, sSymName);
      RISE_ASSERTE(pSym, CLogicNoItemException);
      return pSym;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       UnloadLibrary
    //    DESCRIPTION:    выгрузка библиотеки
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     CFileCloseException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CDynamicLibrary::UnloadLibrary()
    {
      RISE_ASSERTE(m_hDynLib, CLogicNoInitException);
      osFreeLibrary(m_hDynLib);
      m_hDynLib = NULL;
    }

  } // namespace plugin
} // namespace rise
