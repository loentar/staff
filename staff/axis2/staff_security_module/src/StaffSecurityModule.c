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
#include <staff/security/tools.h>
#include "StaffSecurityUtils.h"

struct axis2_handler
{
  axis2_handler_desc_t* handler_desc;
  axis2_status_t (AXIS2_CALL *invoke) (axis2_handler_t* handler, 
                                       const axutil_env_t* env, 
                                       struct axis2_msg_ctx* msg_ctx);
};

axis2_status_t AXIS2_CALL StaffSecurity_invoke(axis2_handler_t* pHandler, 
                                                 const axutil_env_t *pEnv, 
                                                 struct axis2_msg_ctx* pMsgCtx)
{
  AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);

  if (axis2_msg_ctx_get_server_side(pMsgCtx, pEnv))
  {
    axis2_char_t* szServiceOperationPath = NULL;
    const axis2_char_t* szSessionId = NULL;
    const axis2_char_t* szInstanceId = NULL;
    axis2_char_t* szServiceName = NULL;
    int nAccess = 0;

    AXIS2_UTILS_CHECK(GetServiceOperationPath(pMsgCtx, pEnv, &szServiceOperationPath, &szServiceName));

    GetSessionAndInstanceId(pMsgCtx, pEnv, &szSessionId, &szInstanceId);
    if (szSessionId == NULL)
    {
      dprintf("Session ID is not set, identifying as guest\n");

      szSessionId = STAFF_SECURITY_NOBODY_SESSION_ID;
    }

    if (!staff_security_calculate_access_by_session_id(szServiceOperationPath, szSessionId, &nAccess))
    {
      dprintf("Access denied to user with unknown or expired session id [%s] while accessing to operation %s\n",
        szSessionId, szServiceOperationPath);

      AXIS2_FREE(pEnv->allocator, szServiceOperationPath);
      AXIS2_FREE(pEnv->allocator, szServiceName);

      AXIS2_ERROR_SET_MESSAGE(pEnv->error, "Access denied: sessionid unknown or expired");
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, AXUTIL_ERROR_MAX + 2);
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return AXIS2_FAILURE;
    }

    if(!nAccess)
    {
      dprintf("Access denied to user with session id [%s] while accessing to operation %s\n",
              szSessionId, szServiceOperationPath);

      AXIS2_FREE(pEnv->allocator, szServiceOperationPath);
      AXIS2_FREE(pEnv->allocator, szServiceName);

      AXIS2_ERROR_SET_MESSAGE(pEnv->error, "Access denied");
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, AXUTIL_ERROR_MAX + 2);
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return AXIS2_FAILURE;
    }

    AXIS2_FREE(pEnv->allocator, szServiceOperationPath);

    { // remember service name, sessionid instanceid in message context
      axutil_property_t* pProp = axutil_property_create(pEnv);
      if (!pProp)
      {
        dprintf("WARNING: failed to create property to save service name");
        AXIS2_FREE(pEnv->allocator, szServiceName);
        return AXIS2_FAILURE;
      }

      axutil_property_set_value(pProp, pEnv, szServiceName);
      axis2_msg_ctx_set_property(pMsgCtx, pEnv, "ServiceName", pProp);

      pProp = axutil_property_create(pEnv);
      if (!pProp)
      {
        dprintf("WARNING: failed to create property to save session id");
        return AXIS2_FAILURE;
      }

      axutil_property_set_value(pProp, pEnv, axutil_strdup(pEnv, szSessionId));
      axis2_msg_ctx_set_property(pMsgCtx, pEnv, "SessionId", pProp);

      pProp = axutil_property_create(pEnv);
      if (!pProp)
      {
        dprintf("WARNING: failed to create property to save instance id");
        return AXIS2_FAILURE;
      }

      axutil_property_set_value(pProp, pEnv, axutil_strdup(pEnv, szInstanceId ? szInstanceId : ""));
      axis2_msg_ctx_set_property(pMsgCtx, pEnv, "InstanceId", pProp);
    }
  }

  return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL StaffSecurityModule_init( axis2_module_t* pModule, 
                                                      const axutil_env_t* pEnv,
                                                      axis2_conf_ctx_t* pConfCtx, 
                                                      axis2_module_desc_t* pModuleDesc)
{
  if (!staff_security_init())
  {
    fprintf(stderr, "Failed to initialize staff::security.\n");
    exit(1);
  }
  return AXIS2_SUCCESS;
}

AXIS2_EXPORT axis2_handler_t* AXIS2_CALL StaffSecurity_create(const axutil_env_t *pEnv, 
                                                                axutil_qname_t* pQName) 
{
  axis2_handler_t* pHandler = NULL;

  pHandler = axis2_handler_create(pEnv);
  if (!pHandler)
  {
    return NULL;
  }

  pHandler->invoke = StaffSecurity_invoke;

  return pHandler;
}

axis2_status_t AXIS2_CALL StaffSecurityModule_shutdown(axis2_module_t* pModule, 
                                                         const axutil_env_t* pEnv)
{
  staff_security_free();

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

axis2_status_t AXIS2_CALL StaffSecurityModule_fill_handler_create_func_map(axis2_module_t* pModule, 
                                                                             const axutil_env_t* pEnv)
{
  AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);

  pModule->handler_create_func_map = axutil_hash_make(pEnv);
  axutil_hash_set(pModule->handler_create_func_map, "StaffSecurity", 
    (axis2_ssize_t)AXIS2_HASH_KEY_STRING, StaffSecurity_create);

  return AXIS2_SUCCESS;
}

static const axis2_module_ops_t staff_security_module_ops_var = 
{
  StaffSecurityModule_init,
  StaffSecurityModule_shutdown,
  StaffSecurityModule_fill_handler_create_func_map
};

axis2_module_t* StaffSecurityModule_create(const axutil_env_t* pEnv)
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
  *ppInst = StaffSecurityModule_create(pEnv);
  AXIS2_UTILS_CHECK(*ppInst);

  return AXIS2_SUCCESS;
}

AXIS2_EXPORT int axis2_remove_instance(axis2_module_t* pInst, const axutil_env_t* pEnv)
{
  AXIS2_UTILS_CHECK(pInst);

  return StaffSecurityModule_shutdown(pInst, pEnv);
}
