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

#if defined(WIN32)  || defined (_WINDOWS)
#pragma warning(disable: 4786)
#pragma warning(disable: 4091)
#endif

#include <stdlib.h>
#include <axis2_module.h>
#include <axis2_addr_mod.h>
#include <axis2_disp.h>
#include <axis2_handler.h>
#include <axis2_handler_desc.h>
#include <axutil_qname.h>
#include <axis2_relates_to.h>
#include <axis2_svc.h>
#include <axis2_const.h>
#include <axis2_conf_ctx.h>
#include <axis2_addr.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include <staff/security/Security.h>
#include "Axis2Utils.h"

#define dprintf printf("%s[%d]: staff_security: ", GetBaseFile(__FILE__), __LINE__); printf

struct axis2_handler
{
  axis2_handler_desc_t* handler_desc;
  axis2_status_t (AXIS2_CALL *invoke) (axis2_handler_t* handler, 
                                       const axutil_env_t* env, 
                                       struct axis2_msg_ctx* msg_ctx);
};

axis2_status_t AXIS2_CALL Axis2Dispatcher_invoke(axis2_handler_t* pHandler, 
                                                 const axutil_env_t *pEnv, 
                                                 struct axis2_msg_ctx* pMsgCtx)
{
  AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);    

  if (axis2_msg_ctx_get_server_side(pMsgCtx, pEnv))
  {
    const axis2_char_t* szService = NULL;
    const axis2_char_t* szOperation = NULL;
    const axis2_char_t* szSessionId = NULL;
    TPermission stPermission;
    int nUserId = -1;

    AXIS2_UTILS_CHECK(GetServiceNameAndOperation(pMsgCtx, pEnv, &szService, &szOperation));

    if(!GetSessionId(pMsgCtx, pEnv, &szSessionId) || szSessionId == NULL)
    {
      dprintf("Session ID is not set, identifying as guest\n");

      szSessionId = STAFF_SECURITY_GUEST_SESSION_ID;
    }

    if(!StaffSecurityGetUserIdBySessionId(szSessionId, &nUserId))
    {
      dprintf("Access denied to user with unknown or expired session id: %s: %s.%s\n", 
        szSessionId, szService, szOperation);

      AXIS2_ERROR_SET_MESSAGE(pEnv->error, "Access denied: sessionid unknown or expired");
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, AXUTIL_ERROR_MAX + 2);
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return AXIS2_FAILURE;
    }

    if(!StaffSecurityGetUserPermissionForServiceOperation(szService, szOperation, nUserId, &stPermission))
    {
      dprintf("Can't get permission for service operation for user %d (sid: %s): %s.%s\n", 
        nUserId, szSessionId, szService, szOperation);

      AXIS2_ERROR_SET_MESSAGE(pEnv->error, "Access denied: can't get permission for service operation");
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, AXUTIL_ERROR_MAX + 2);
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return AXIS2_FAILURE;
    }
    
    dprintf("\033[4muser permissions to (%s:%s): %c%c%c\033[0m\n", 
      szService, szOperation,
      stPermission.bRead ? 'r' : '-',
      stPermission.bWrite ? 'w' : '-',
      stPermission.bExecute ? 'x' : '-');

    if(!stPermission.bExecute)
    {
      dprintf("Access denied to user %d(sid %s): %s.%s\n", 
        nUserId, szSessionId, szService, szOperation);

      AXIS2_ERROR_SET_MESSAGE(pEnv->error, "Access denied");
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, AXUTIL_ERROR_MAX + 2);
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return AXIS2_FAILURE;
    }
  }

  return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL Axis2DispatcherModule_init( axis2_module_t* pModule, 
                                                      const axutil_env_t* pEnv,
                                                      axis2_conf_ctx_t* pConfCtx, 
                                                      axis2_module_desc_t* pModuleDesc)
{
  const axis2_char_t* szHost = NULL;
  const axis2_char_t* szPort = NULL;
  const axis2_char_t* szDSN = NULL;
  const axis2_char_t* szUser = NULL;
  const axis2_char_t* szPassword = NULL;
  const axis2_char_t* szTmp = NULL;
  int nSessionExpiration = 0; // never expires
  axis2_status_t nStatus = AXIS2_FAILURE;

  szHost = GetParamValue("pgHost", pModuleDesc, pEnv);
  AXIS2_UTILS_CHECK(szHost);

  szPort = GetParamValue("pgPort", pModuleDesc, pEnv);
  AXIS2_UTILS_CHECK(szPort);

  szDSN = GetParamValue("pgDSN", pModuleDesc, pEnv);
  AXIS2_UTILS_CHECK(szDSN);

  szUser = GetParamValue("pgUser", pModuleDesc, pEnv);
  AXIS2_UTILS_CHECK(szUser);

  szPassword = GetParamValue("pgPassword", pModuleDesc, pEnv);
  AXIS2_UTILS_CHECK(szPassword);

  szTmp = GetParamValue("sessionExpiration", pModuleDesc, pEnv);
  if (szTmp != NULL)
  {
    nSessionExpiration = atoi(szTmp);
  }

  nStatus = StaffSecurityInit( szHost, szPort, szDSN, szUser, szPassword, nSessionExpiration );

  return nStatus;
}

AXIS2_EXPORT axis2_handler_t* AXIS2_CALL Axis2Dispatcher_create(const axutil_env_t *pEnv, 
                                                                axutil_qname_t* pQName) 
{
  axis2_handler_t* pHandler = NULL;

  pHandler = axis2_handler_create(pEnv);
  if (!pHandler)
  {
    return NULL;
  }

  pHandler->invoke = Axis2Dispatcher_invoke;

  return pHandler;
}

axis2_status_t AXIS2_CALL Axis2DispatcherModule_shutdown(axis2_module_t* pModule, 
                                                         const axutil_env_t* pEnv)
{
  StaffSecurityFree();

  if(pModule->handler_create_func_map)
  {
    axutil_hash_free(pModule->handler_create_func_map, pEnv);
    pModule->handler_create_func_map = NULL;
  }

  if(pModule)
  {
    AXIS2_FREE(pEnv->allocator, pModule);
    pModule = NULL;
  }

  return AXIS2_SUCCESS; 
}

axis2_status_t AXIS2_CALL Axis2DispatcherModule_fill_handler_create_func_map(axis2_module_t* pModule, 
                                                                             const axutil_env_t* pEnv)
{
  AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);

  pModule->handler_create_func_map = axutil_hash_make(pEnv);
  axutil_hash_set(pModule->handler_create_func_map, "StaffSecurity", 
    (axis2_ssize_t)AXIS2_HASH_KEY_STRING, Axis2Dispatcher_create);

  return AXIS2_SUCCESS;
}

static const axis2_module_ops_t staff_security_module_ops_var = 
{
  Axis2DispatcherModule_init,
  Axis2DispatcherModule_shutdown,
  Axis2DispatcherModule_fill_handler_create_func_map
};

axis2_module_t* Axis2DispatcherModule_create(const axutil_env_t* pEnv)
{
  axis2_module_t *module = (axis2_module_t*)AXIS2_MALLOC(pEnv->allocator, sizeof(axis2_module_t));
  module->ops = &staff_security_module_ops_var;
  return module;
}

/**
* Following block distinguish the exposed part of the dll.
*/

AXIS2_EXPORT int axis2_get_instance(axis2_module_t** ppInst,
                                    const axutil_env_t* pEnv)
{
  *ppInst = Axis2DispatcherModule_create(pEnv);
  AXIS2_UTILS_CHECK(*ppInst);

  return AXIS2_SUCCESS;
}

AXIS2_EXPORT int axis2_remove_instance(axis2_module_t* pInst, const axutil_env_t* pEnv)
{
  AXIS2_UTILS_CHECK(pInst);

  return Axis2DispatcherModule_shutdown(pInst, pEnv);
}
