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

#ifndef _OSCOMMON_H_
#define _OSCOMMON_H_

#ifdef WIN32
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <windows.h>
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef WIN32
#define RISE_PATH_SEPARATOR "\\"
#define RISE_EXPLICIT_TEMPLATE(RISE_LOCAL_TEMPLATE) RISE_LOCAL_TEMPLATE
#else
#define RISE_PATH_SEPARATOR "/"
#define RISE_EXPLICIT_TEMPLATE(RISE_LOCAL_TEMPLATE)
#endif

#endif // _OSCOMMON_H_
