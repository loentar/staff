#if defined(WIN32)  || defined (_WINDOWS)
#pragma warning(disable: 4786)
#pragma warning(disable: 4091)
#endif

#include <axis2_module.h>
#include <axis2_addr_mod.h>
#include <axis2_conf_ctx.h>
#include <axis2_disp.h>
#include <rise/common/Log.h>

extern "C"
{

axis2_status_t AXIS2_CALL
Axis2DispatcherModule_shutdown(axis2_module_t *module,
                        const axutil_env_t *env);

axis2_status_t AXIS2_CALL
Axis2DispatcherModule_init(
        axis2_module_t *module,
        const axutil_env_t *env,
        axis2_conf_ctx_t *conf_ctx,
        axis2_module_desc_t *module_desc);

axis2_status_t AXIS2_CALL
Axis2DispatcherModule_fill_handler_create_func_map(axis2_module_t *module,
                                            const axutil_env_t *env);

AXIS2_EXTERN axis2_handler_t* AXIS2_CALL
Axis2Dispatcher_create(const axutil_env_t *env, 
                         axutil_qname_t *qname); 
     
static const axis2_module_ops_t staff_module_ops_var = 
{
	Axis2DispatcherModule_init,
	Axis2DispatcherModule_shutdown,
	Axis2DispatcherModule_fill_handler_create_func_map
};

axis2_module_t *
Axis2DispatcherModule_create(const axutil_env_t *env)
{
    axis2_module_t *module = (axis2_module_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_module_t));
    module->ops = &staff_module_ops_var;
    return module;
}

axis2_status_t AXIS2_CALL
Axis2DispatcherModule_init(
        axis2_module_t* /*module*/,
        const axutil_env_t* /*env*/,
        axis2_conf_ctx_t* /*conf_ctx*/,
        axis2_module_desc_t* /*module_desc*/)
{
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
Axis2DispatcherModule_shutdown(axis2_module_t *module,
                        const axutil_env_t *env)
{
	if(module->handler_create_func_map)
    {
        axutil_hash_free(module->handler_create_func_map, env);
        module->handler_create_func_map = NULL;
    }
    
    if(module)
    {
        AXIS2_FREE(env->allocator, module);
        module = NULL;
    }
    return AXIS2_SUCCESS; 
}

axis2_status_t AXIS2_CALL
Axis2DispatcherModule_fill_handler_create_func_map(axis2_module_t *module,
                                            const axutil_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    
    module->handler_create_func_map = axutil_hash_make(env);
    axutil_hash_set(module->handler_create_func_map, "StaffDispatcher", 
        (axis2_ssize_t)AXIS2_HASH_KEY_STRING, (const void *)Axis2Dispatcher_create);

    return AXIS2_SUCCESS;
}

/**
 * Following block distinguish the exposed part of the dll.
 */

AXIS2_EXPORT int 
axis2_get_instance(axis2_module_t **inst,
                   const axutil_env_t *env)
{
rise::LogEntry();
   *inst = Axis2DispatcherModule_create(env);
    if(!(*inst))
    {
        return AXIS2_FAILURE;
    }

    return AXIS2_SUCCESS;
}

AXIS2_EXPORT int 
axis2_remove_instance(axis2_module_t *inst,
                      const axutil_env_t *env)
{
    axis2_status_t status = AXIS2_FAILURE;
   if (inst)
   {
        status = Axis2DispatcherModule_shutdown(inst, env);
    }
    return status;
}

}
