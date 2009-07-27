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
  //! динамическая библиотека
  typedef HMODULE HDynamicLib;
#else
#include <dlfcn.h>
  //! динамическая библиотека
  typedef void* HDynamicLib;
#endif

  //! символ в библиотеке
  typedef void* PLibSymbol;

  //!        подгрузка библиотеки
  /*
     \param  szLibraryName - имя библиотеки
     \return идентификатор библиотеки, NULL при ошибке
  */
  HDynamicLib osLoadLibrary(const CString& sLibraryName, bool bRawName);

  //!        получение указателя на символ в библиотеке
  /*
     \param  hDynamicLib - идентификатор библиотеки
     \param  szSymName - имя символа
     \return указатель на функцию, NULL при ошибке
  */
  PLibSymbol osGetSymbol(HDynamicLib hDynamicLib, const CStringA& sSymName);

  //!        выгрузка библиотеки
  /*
     \param  hDynamicLib - идентификатор библиотеки
     \return true, если библиотека успешно выгружена
  */
  bool osFreeLibrary(HDynamicLib hDynamicLib);

  //!        получение кода последней ошибки работы с библиотекой
  /*
     \return код последней ошибки работы с библиотекой
  */
  long osGetLastLibraryError();

  //!        получение кода последней ошибки работы с библиотекой
  /*
     \return код последней ошибки работы с библиотекой
  */
  const TChar* osGetLastLibraryErrorStr();
}

#endif  // _OSDL_H_
