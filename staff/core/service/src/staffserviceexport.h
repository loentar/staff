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

#ifndef _STAFFSERVICEEXPORT_H_
#define _STAFFSERVICEEXPORT_H_

#if defined(WIN32) || defined (_WINDOWS)
  #pragma warning(disable: 4786)

  #ifdef STAFF_SERVICE_DLL_EXPORTS
    #define STAFF_SERVICE_EXPORT __declspec(dllexport)
  #else
    #define STAFF_SERVICE_EXPORT __declspec(dllimport)
  #endif

#else
  #define STAFF_SERVICE_EXPORT 
#endif

#endif // _STAFFSERVICEEXPORT_H_

/** \mainpage Документация на модуль сервисных функций распределенных приложений
<a href="annotated.html">Список</a> классов, реализуемых модулем.
*/
