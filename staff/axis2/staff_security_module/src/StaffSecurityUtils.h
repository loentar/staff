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

#ifndef _STAFF_SECURITY_UTILS_H_
#define _STAFF_SECURITY_UTILS_H_

#include <axis2_module_desc.h>
#include <axis2_msg_ctx.h>

#define AXIS2_UTILS_ASSERT(env, expression, description) \
  if(!(expression)) { \
    fprintf(stderr, "Assertion failed: %s[%d]: %s (%s)\n", GetBaseFile(__FILE__), __LINE__, \
           description, #expression); \
    AXIS2_ERROR_SET_MESSAGE(env->error, description); \
    AXIS2_ERROR_SET_ERROR_NUMBER(env->error, AXUTIL_ERROR_MAX + 2); \
    AXIS2_ERROR_SET_STATUS_CODE(env->error, AXIS2_FAILURE); \
    return AXIS2_FAILURE; }

#ifdef _DEBUG
#define dprintf printf("%s[%d]: staff_security: ", GetBaseFile(__FILE__), __LINE__); printf
#else
#define dprintf(...)
#endif

axis2_status_t GetOperationName(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv,
                                const axis2_char_t** pszOperationName);

void GetSessionAndInstanceId(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv,
                             const axis2_char_t** pszSessionId, const axis2_char_t** pszInstanceId);
const char* GetBaseFile(const char* szFilePath);

#endif /* _STAFF_SECURITY_UTILS_H_ */


