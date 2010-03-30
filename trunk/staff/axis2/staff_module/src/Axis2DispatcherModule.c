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

#include <axis2_module.h>
#include <axis2_addr_mod.h>
#include <axis2_conf_ctx.h>
#include <axis2_disp.h>

/* forward decl */
axis2_status_t AXIS2_CALL Axis2DispatcherModule_shutdown(axis2_module_t *module, const axutil_env_t *env);

axis2_status_t AXIS2_CALL Axis2DispatcherModule_init(axis2_module_t* pModule, const axutil_env_t* pEnv,
                                                     axis2_conf_ctx_t *conf_ctx, axis2_module_desc_t *module_desc);

axis2_status_t AXIS2_CALL Axis2DispatcherModule_fill_handler_create_func_map(axis2_module_t *module,
                                                                             const axutil_env_t *env);

AXIS2_EXTERN axis2_handler_t* AXIS2_CALL Axis2Dispatcher_create(const axutil_env_t* pEnv, axutil_qname_t* pQname);



static const axis2_module_ops_t staff_module_ops_var =
{
  Axis2DispatcherModule_init,
  Axis2DispatcherModule_shutdown,
  Axis2DispatcherModule_fill_handler_create_func_map
};

axis2_module_t* Axis2DispatcherModule_create(const axutil_env_t* pEnv)
{
  axis2_module_t *module = (axis2_module_t*)AXIS2_MALLOC(pEnv->allocator, sizeof(axis2_module_t));
  module->ops = &staff_module_ops_var;
  return module;
}

axis2_status_t AXIS2_CALL Axis2DispatcherModule_init(axis2_module_t* pModule, const axutil_env_t* pEnv,
                                                     axis2_conf_ctx_t* pConfCtx, axis2_module_desc_t* pModuleDesc)
{
  return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL Axis2DispatcherModule_shutdown(axis2_module_t* pModule, const axutil_env_t* pEnv)
{
  if (pModule->handler_create_func_map)
  {
    axutil_hash_free(pModule->handler_create_func_map, pEnv);
    pModule->handler_create_func_map = NULL;
  }

  if (pModule)
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
  axutil_hash_set(pModule->handler_create_func_map, "StaffDispatcher",
      (axis2_ssize_t)AXIS2_HASH_KEY_STRING, (const void *)Axis2Dispatcher_create);

  return AXIS2_SUCCESS;
}

/**
 * Following block distinguish the exposed part of the dll.
 */

AXIS2_EXPORT int axis2_get_instance(axis2_module_t** ppInst, const axutil_env_t* pEnv)
{
  *ppInst = Axis2DispatcherModule_create(pEnv);
  if(!(*ppInst))
  {
    return AXIS2_FAILURE;
  }

  return AXIS2_SUCCESS;
}

AXIS2_EXPORT int axis2_remove_instance(axis2_module_t* pInst, const axutil_env_t* pEnv)
{
  axis2_status_t status = AXIS2_FAILURE;
  if (pInst)
  {
    status = Axis2DispatcherModule_shutdown(pInst, pEnv);
  }
  return status;
}
