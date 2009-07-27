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

#ifndef _STAFFCOMMONEXPORT_H_
#define _STAFFCOMMONEXPORT_H_

#if defined(WIN32) || defined (_WINDOWS)
  #pragma warning(disable: 4786)

  #ifdef STAFF_DLL_EXPORTS
    #define STAFF_COMMON_EXPORT __declspec(dllexport)
  #else
    #define STAFF_COMMON_EXPORT __declspec(dllimport)
  #endif

  #define STAFF_DEPRECATED(Replacement) _CRT_INSECURE_DEPRECATE(Replacement)

#else
  #define STAFF_COMMON_EXPORT 
  #define STAFF_DEPRECATED(Replacement)
#endif

#endif // _STAFFCOMMONEXPORT_H_


/** \mainpage Документация на модуль общих функций распределенных приложений
<a href="annotated.html">Список</a> классов, реализуемых модулем.
*/
