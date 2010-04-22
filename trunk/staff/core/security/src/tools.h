/*
 *  Copyright 2010 Utkin Dmitry
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

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "staffsecurityexport.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define STAFF_SECURITY_NOBODY_SESSION_ID "00000000000000000000000000000000"

  STAFF_SECURITY_EXPORT int staff_security_init();

  STAFF_SECURITY_EXPORT void staff_security_free();

  STAFF_SECURITY_EXPORT int staff_security_calculate_access_by_session_id(const char* szObjectPath, const char* szSessionId, int* pnAccess);

#ifdef __cplusplus
}
#endif

#endif // _TOOLS_H_
