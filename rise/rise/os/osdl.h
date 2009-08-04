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

#ifndef _OSDL_H_
#define _OSDL_H_

#include <rise/common/types.h>
#include <rise/os/oscommon.h>
#include <rise/string/String.h>

namespace rise
{
#ifdef WIN32
  //! dynamic library handle
  typedef HMODULE HDynamicLib;
#else
#include <dlfcn.h>
  //! dynamic library handle
  typedef void* HDynamicLib;
#endif

  //! library symbol
  typedef void* PLibSymbol;

  //!         load library
  /*! \param  sLibraryName - library name
      \param  bRawName - use system-specific library name
      \return dynamic library handle, NULL if error
  */
  HDynamicLib osLoadLibrary(const CString& sLibraryName, bool bRawName);

  //!         get library symbol
  /*! \param  hDynamicLib - dynamic lib handle
      \param  sSymName - symbol name
      \return pointer to symbol, NULL if error
  */
  PLibSymbol osGetSymbol(HDynamicLib hDynamicLib, const CStringA& sSymName);

  //!         unload and free library
  /*! \param  hDynamicLib - dynamic lib handle
      \return true, if library was successfully unloaded
  */
  bool osFreeLibrary(HDynamicLib hDynamicLib);

  //!         get last error code
  /*! \return last error code
  */
  long osGetLastLibraryError();

  //!        get last error description
  /*! \return last error description
  */
  const TChar* osGetLastLibraryErrorStr();
}

#endif  // _OSDL_H_
