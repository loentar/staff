#if defined(WIN32)  || defined (_WINDOWS)
#pragma warning(disable: 4786)
#pragma warning(disable: 4091)
#endif

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
#include <rise/common/Log.h>
#include <staff/component/SharedContext.h>
#include "Axis2Utils.h"

extern "C"
{                                                                                   

struct axis2_handler
{
  axis2_handler_desc_t *handler_desc;

  axis2_status_t(AXIS2_CALL * invoke) 
      (axis2_handler_t * handler,
       const axutil_env_t * env,
       struct axis2_msg_ctx * msg_ctx);
};

axis2_status_t AXIS2_CALL 
Axis2Dispatcher_invoke (
    axis2_handler_t * handler, 
    const axutil_env_t *env,
    struct axis2_msg_ctx *msg_ctx);
                           
axis2_svc_t *AXIS2_CALL 
Axis2Dispatcher_find_svc(
    axis2_msg_ctx_t *msg_ctx,
    const axutil_env_t *env);
                            
axis2_op_t *AXIS2_CALL 
Axis2Dispatcher_find_op(
    axis2_msg_ctx_t *msg_ctx, 
    const axutil_env_t *env,
    axis2_svc_t *svc);


AXIS2_EXPORT axis2_handler_t* AXIS2_CALL
Axis2Dispatcher_create(const axutil_env_t *pEnv, axutil_qname_t* /*qname*/) 
{
  axis2_handler_t *handler = NULL;
    
  handler = axis2_handler_create(pEnv);
  if (!handler)
        return NULL;
   
  /* set the base struct's invoke pOp */
  handler->invoke = Axis2Dispatcher_invoke;

  return handler;
}

axis2_svc_t *AXIS2_CALL 
Axis2Dispatcher_find_svc(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv) 
{    
rise::LogEntry();
  std::string sRequestedServiceName;
  const axis2_char_t* szServiceName = "StaffService";
  
  AXIS2_ENV_CHECK(pEnv, NULL);    

  if (pMsgCtx == NULL)
    return NULL;  

  if(!GetServiceName(pMsgCtx, pEnv, sRequestedServiceName))
    return NULL;

  if (staff::CSharedContext::Inst().GetService(sRequestedServiceName) == NULL)
  {
    printf("Service \"%s\" not found in Staff\n", sRequestedServiceName.c_str());
    return NULL;
  }

  axis2_conf_ctx_t* pContCtx = axis2_msg_ctx_get_conf_ctx(pMsgCtx, pEnv);
  if (pContCtx == NULL)
  {
    printf("(!axis2_msg_ctx_get_conf_ctx)\n");
    return NULL;
  }

  axis2_conf_t* pConf = axis2_conf_ctx_get_conf(pContCtx, pEnv);
  if (pConf == NULL)
  {
    printf("(!axis2_conf_ctx_get_conf)\n");
    return NULL;
  }

  axis2_svc_t* pService = axis2_conf_get_svc(pConf, pEnv, szServiceName);
  if (pService == NULL)
  {
    printf("Can't find service \"%s\"\n", szServiceName);
    return NULL;
  }

#ifdef _DEBUG
  {
    axiom_soap_envelope_t* pSoapEnv = axis2_msg_ctx_get_soap_envelope(pMsgCtx, pEnv);
    if (pSoapEnv == NULL)
      return false;

    axiom_soap_body_t* pSoapBody = axiom_soap_envelope_get_body(pSoapEnv, pEnv);
    if (pSoapBody == NULL)
      return false;

    axiom_node_t* pBaseNode = axiom_soap_body_get_base_node(pSoapBody, pEnv);
    axiom_node_t* panBody = axiom_node_get_parent(pBaseNode, pEnv);
    axis2_char_t* szXml = axiom_node_to_string(panBody, pEnv);
    if (szXml)
    {
      printf("request SOAP Envelope: \n%s\n", szXml);
      AXIS2_FREE(pEnv->allocator, szXml);
    }
  }
#endif

  printf("Staff service \"%s\" found\n", sRequestedServiceName.c_str());
  return pService;
}

axis2_op_t *AXIS2_CALL 
Axis2Dispatcher_find_op(
    axis2_msg_ctx_t* /*msg_ctx*/, 
    const axutil_env_t* env,
    axis2_svc_t* svc)
{
  AXIS2_ENV_CHECK(env, NULL); 
  if (svc == NULL)
    return NULL;  

  return axis2_svc_get_op_with_name(svc, env, "Invoke");
}
            
axis2_status_t AXIS2_CALL 
Axis2Dispatcher_invoke(axis2_handler_t* /*handler*/, const axutil_env_t *pEnv, struct axis2_msg_ctx* pMsgCtx)
{
rise::LogEntry();
    AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);    

    if (!(axis2_msg_ctx_get_server_side(pMsgCtx, pEnv)))
        return AXIS2_SUCCESS;
    
    axis2_msg_ctx_set_find_svc(pMsgCtx, pEnv, Axis2Dispatcher_find_svc);
    axis2_msg_ctx_set_find_op(pMsgCtx, pEnv, Axis2Dispatcher_find_op);
    
    axis2_svc_t *pService = NULL;
    axis2_op_t *pOperation = NULL;
    
    pService = axis2_msg_ctx_get_svc(pMsgCtx, pEnv);

    // axis service??
    if (!pService) 
    {
        pService = axis2_msg_ctx_find_svc(pMsgCtx, pEnv);
        if (pService) 
        {
            axis2_msg_ctx_set_svc(pMsgCtx, pEnv, pService);
            /*TODO Set the Service Group Context to the message Context*/
        }
    } else
    { 
      axutil_param_t* pParam = axis2_svc_get_param(pService, pEnv, "IsStaffVirtualService");
      if (pParam != NULL) // Staff virtual service? 
      {
        bool* pbIsStaffVirtualService = (bool*)axutil_param_get_value(pParam, pEnv);
        if (pbIsStaffVirtualService != NULL)
        {
          if(*pbIsStaffVirtualService)
          {
            axis2_svc_t* pStaffService = Axis2Dispatcher_find_svc(pMsgCtx, pEnv);
            if (pStaffService != NULL)
            {
              pOperation = axis2_svc_get_op_with_name(pStaffService, pEnv, "Invoke");
              if (pOperation != NULL)
              {
                axis2_msg_ctx_set_op(pMsgCtx, pEnv, pOperation);
                axis2_msg_ctx_set_svc(pMsgCtx, pEnv, pStaffService);
              }
            }
          }

          return AXIS2_SUCCESS;
        }
      }
    }

    pOperation = axis2_msg_ctx_get_op(pMsgCtx, pEnv);
    if (!pOperation)
    {
        pOperation = axis2_msg_ctx_find_op(pMsgCtx, pEnv, pService);
        
        if (pOperation)
            axis2_msg_ctx_set_op(pMsgCtx, pEnv, pOperation);
    }

    return AXIS2_SUCCESS;
}

}
