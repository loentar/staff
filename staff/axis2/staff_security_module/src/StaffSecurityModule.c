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

#if defined WIN32 && !defined __MINGW32__
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
#include <axis2_util.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include <staff/security/tools.h>
#include "StaffSecurityUtils.h"

#define STAFF_PARAM_UNUSED(param) (void)param
#define STAFF_SECURITY_CALC_FN_NAME "staff_security_calculate_access_by_session_id"
#ifdef WIN32
#ifdef _MSC_VER
#define STAFF_SECURITY_DL_NAME "staff_security.dll"
#else
#define STAFF_SECURITY_DL_NAME "staffsecurity.dll"
#endif
#else
#define STAFF_SECURITY_DL_NAME "libstaffsecurity" LIBRARY_VEREXT
#endif

axis2_bool_t g_bIsSvrSide = AXIS2_FALSE;
typedef int (*staff_security_calc_fn_t)(const char*, const char*, int*);
staff_security_calc_fn_t g_pstaff_security_calc_fn = NULL;

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
  STAFF_PARAM_UNUSED(pHandler);
  AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);

  if (g_bIsSvrSide)
  {
    axis2_char_t* szServiceOperationPath = NULL;
    const axis2_char_t* szSessionId = NULL;
    const axis2_char_t* szInstanceId = NULL;
    const axis2_char_t* szServiceName = NULL;
    int nAccess = 0;
    axutil_property_t* pProp = NULL;
    axis2_svc_t* pService = NULL;
    const axis2_char_t* szOperation = NULL;

    AXIS2_UTILS_ASSERT(pEnv, g_pstaff_security_calc_fn, "Failed to get staff_security_calc_fn");

    pService = axis2_msg_ctx_get_svc(pMsgCtx, pEnv);
    AXIS2_UTILS_ASSERT(pEnv, pService, "Failed to get service object");

    szServiceName = axis2_svc_get_name(pService, pEnv);
    AXIS2_UTILS_ASSERT(pEnv, szServiceName, "Failed to get service name");

    GetOperationName(pMsgCtx, pEnv, &szOperation);
    AXIS2_UTILS_ASSERT(pEnv, szOperation, "Failed to get operation name");

    szServiceOperationPath =
        axutil_strcat(pEnv, "component.", szServiceName, ".", szOperation, NULL);


    GetSessionAndInstanceId(pMsgCtx, pEnv, &szSessionId, &szInstanceId);
    if (szSessionId == NULL)
    {
      szSessionId = STAFF_SECURITY_NOBODY_SESSION_ID;
    }

    if (!g_pstaff_security_calc_fn(szServiceOperationPath, szSessionId, &nAccess))
    {
      dprintf("Access denied to user with unknown or expired session id [%s] while accessing to operation %s\n",
        szSessionId, szServiceOperationPath);

      AXIS2_FREE(pEnv->allocator, szServiceOperationPath);

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

      AXIS2_ERROR_SET_MESSAGE(pEnv->error, "Access denied");
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, AXUTIL_ERROR_MAX + 2);
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return AXIS2_FAILURE;
    }

    AXIS2_FREE(pEnv->allocator, szServiceOperationPath);

    /* store session id and instance id within message context */
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

  return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL StaffSecurityModule_init( axis2_module_t* pModule,
                                                      const axutil_env_t* pEnv,
                                                      axis2_conf_ctx_t* pConfCtx,
                                                      axis2_module_desc_t* pModuleDesc)
{
  axis2_ctx_t* pAxis2Ctx = NULL;
  axutil_property_t* pProp = NULL;

  STAFF_PARAM_UNUSED(pModule);
  STAFF_PARAM_UNUSED(pEnv);
  STAFF_PARAM_UNUSED(pConfCtx);
  STAFF_PARAM_UNUSED(pModuleDesc);

  pAxis2Ctx = axis2_conf_ctx_get_base(pConfCtx, pEnv);
  if (!pAxis2Ctx)
  {
    return AXIS2_FAILURE;
  }

  pProp = axis2_ctx_get_property(pAxis2Ctx, pEnv, AXIS2_IS_SVR_SIDE);
  if (pProp)
  {
    const void* pPropValue = axutil_property_get_value(pProp, pEnv);
    if (pProp && !strcmp((const axis2_char_t*)pPropValue, AXIS2_VALUE_TRUE))
    {
      g_bIsSvrSide = AXIS2_TRUE;
    }
  }

  if (g_bIsSvrSide)
  {
    void* pProcAddr = NULL;
    AXIS2_DLHANDLER pDlHandler = AXIS2_PLATFORM_LOADLIB(STAFF_SECURITY_DL_NAME);
    if (!pDlHandler)
    {
#ifndef WIN32
      AXIS2_LOG_ERROR(pEnv->log, AXIS2_LOG_SI, "Loading shared library %s  Failed. DLERROR IS %s",
                      STAFF_SECURITY_DL_NAME, AXIS2_PLATFORM_LOADLIB_ERROR);
#else
      axis2_char_t szBuff[AXUTIL_WIN32_ERROR_BUFSIZE];
      axutil_win32_get_last_error(szBuff, AXUTIL_WIN32_ERROR_BUFSIZE);
      AXIS2_LOG_ERROR(pEnv->log, AXIS2_LOG_SI, "Loading shared library %s  Failed. DLERROR IS %s",
                      STAFF_SECURITY_DL_NAME, szBuff);
#endif
      AXIS2_ERROR_SET(pEnv->error, AXIS2_ERROR_DLL_LOADING_FAILED, AXIS2_FAILURE);

      return AXIS2_FAILURE;
    }

    pProcAddr = AXIS2_PLATFORM_GETPROCADDR(pDlHandler, STAFF_SECURITY_CALC_FN_NAME);
    g_pstaff_security_calc_fn = *(staff_security_calc_fn_t*)&pProcAddr;
    if (!g_pstaff_security_calc_fn)
    {
      AXIS2_LOG_ERROR(pEnv->log, AXIS2_LOG_SI, "Failed to get symbol");
      AXIS2_ERROR_SET(pEnv->error, AXIS2_ERROR_DLL_LOADING_FAILED, AXIS2_FAILURE);
      return AXIS2_FAILURE;
    }
  }

  return AXIS2_SUCCESS;
}

AXIS2_EXPORT axis2_handler_t* AXIS2_CALL StaffSecurity_create(const axutil_env_t *pEnv, 
                                                                axutil_qname_t* pQName)
{
  axis2_handler_t* pHandler = NULL;
  STAFF_PARAM_UNUSED(pQName);

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
  /* avoiding the warning "ISO C forbids conversion of function pointer to object pointer type" */
  axis2_handler_t* (AXIS2_CALL *const pFunction)(const axutil_env_t*, axutil_qname_t*) = StaffSecurity_create;

  AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);

  pModule->handler_create_func_map = axutil_hash_make(pEnv);
  axutil_hash_set(pModule->handler_create_func_map, "StaffSecurity",
    (axis2_ssize_t)AXIS2_HASH_KEY_STRING, *(const void**)&pFunction);

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
  AXIS2_UTILS_ASSERT(pEnv, *ppInst, "Failed to get instance");

  return AXIS2_SUCCESS;
}

AXIS2_EXPORT int axis2_remove_instance(axis2_module_t* pInst, const axutil_env_t* pEnv)
{
  AXIS2_UTILS_ASSERT(pEnv, pInst, "Instance is NULL");

  return StaffSecurityModule_shutdown(pInst, pEnv);
}
