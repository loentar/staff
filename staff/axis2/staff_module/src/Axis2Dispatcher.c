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

#include <axis2_handler.h>
#include <axis2_handler_desc.h>
#include <axutil_qname.h>
#include <axis2_relates_to.h>
#include <axis2_svc.h>
#include <axis2_const.h>
#include <axis2_conf_ctx.h>
#include <axis2_addr.h>
#include <axis2_http_header.h>
#include <axis2_http_transport.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>

#define STAFF_PARAM_UNUSED(param) (void)param

struct axis2_handler
{
  axis2_handler_desc_t *handler_desc;
  axis2_status_t (AXIS2_CALL* invoke)(axis2_handler_t * handler, const axutil_env_t * env,
                                      struct axis2_msg_ctx * msg_ctx);
};

axis2_status_t AXIS2_CALL Axis2Dispatcher_invoke(axis2_handler_t* pHandler, const axutil_env_t* pEnv,
                                                 struct axis2_msg_ctx* pMsgCtx);

axis2_svc_t* AXIS2_CALL Axis2Dispatcher_find_svc(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv);

axis2_op_t* AXIS2_CALL Axis2Dispatcher_find_op(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv,
                                               axis2_svc_t* pSvc);

AXIS2_EXPORT axis2_handler_t* AXIS2_CALL Axis2Dispatcher_create(const axutil_env_t* pEnv, axutil_qname_t* pqName)
{
  axis2_handler_t* pHandler = NULL;

  STAFF_PARAM_UNUSED(pqName);

  pHandler = axis2_handler_create(pEnv);
  if (!pHandler)
  {
    return NULL;
  }

  /* set the base struct's invoke pOp */
  pHandler->invoke = Axis2Dispatcher_invoke;

  return pHandler;
}

axis2_svc_t* AXIS2_CALL Axis2Dispatcher_find_svc(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv)
{
  static const axis2_char_t* szServiceName = "StaffService";
  axis2_conf_ctx_t* pContCtx = NULL;
  axis2_conf_t* pConf = NULL;
  axis2_svc_t* pService = NULL;

  AXIS2_ENV_CHECK(pEnv, NULL);

  if (!pMsgCtx)
  {
    AXIS2_LOG_ERROR(pEnv->log, AXIS2_LOG_SI, "pMsgCtx == NULL");
    return NULL;
  }

  pContCtx = axis2_msg_ctx_get_conf_ctx(pMsgCtx, pEnv);
  if (!pContCtx)
  {
    AXIS2_LOG_ERROR(pEnv->log, AXIS2_LOG_SI, "axis2_msg_ctx_get_conf_ctx");
    return NULL;
  }

  pConf = axis2_conf_ctx_get_conf(pContCtx, pEnv);
  if (!pConf)
  {
    AXIS2_LOG_ERROR(pEnv->log, AXIS2_LOG_SI, "axis2_conf_ctx_get_conf");
    return NULL;
  }

  pService = axis2_conf_get_svc(pConf, pEnv, szServiceName);
  if (!pService)
  {
    AXIS2_LOG_ERROR(pEnv->log, AXIS2_LOG_SI, "Can't find service \"%s\"", szServiceName);
    return NULL;
  }

  return pService;
}

axis2_op_t* AXIS2_CALL Axis2Dispatcher_find_op(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv,
                                               axis2_svc_t* pSvc)
{
  STAFF_PARAM_UNUSED(pMsgCtx);
  AXIS2_ENV_CHECK(pEnv, NULL);
  if (pSvc == NULL)
  {
    return NULL;
  }

  return axis2_svc_get_op_with_name(pSvc, pEnv, "Invoke");
}

axis2_status_t AXIS2_CALL Axis2Dispatcher_invoke(axis2_handler_t* pHandler, const axutil_env_t* pEnv,
                                                 struct axis2_msg_ctx* pMsgCtx)
{
  axis2_svc_t* pService = NULL;
  axis2_op_t* pOperation = NULL;

  STAFF_PARAM_UNUSED(pHandler);
  AXIS2_ENV_CHECK(pEnv, AXIS2_FAILURE);

  if (!(axis2_msg_ctx_get_server_side(pMsgCtx, pEnv)))
  {
    return AXIS2_SUCCESS;
  }

  axis2_msg_ctx_set_find_svc(pMsgCtx, pEnv, Axis2Dispatcher_find_svc);
  axis2_msg_ctx_set_find_op(pMsgCtx, pEnv, Axis2Dispatcher_find_op);

  pService = axis2_msg_ctx_get_svc(pMsgCtx, pEnv);

  /* is axis2 service ? */
  if (!pService)
  {
    pService = axis2_msg_ctx_find_svc(pMsgCtx, pEnv);
    if (pService)
    {
      axis2_msg_ctx_set_svc(pMsgCtx, pEnv, pService);
      /*TODO Set the Service Group Context to the message Context*/
    }
  }
  else
  { /* is staff service ? */
    axutil_param_t* pParam = axis2_svc_get_param(pService, pEnv, "IsStaffVirtualService");
    if (pParam != NULL) /* Staff virtual service? */
    {
      axutil_hash_t * pHashHeaders = NULL;
      axis2_svc_t* pStaffService = Axis2Dispatcher_find_svc(pMsgCtx, pEnv);

      const axis2_char_t* szServiceName = axis2_svc_get_name(pService, pEnv);
      if (szServiceName != NULL)
      {
        axutil_property_t* pProp = axutil_property_create(pEnv);
        axutil_property_set_value(pProp, pEnv, axutil_strdup(pEnv, szServiceName));
        axis2_msg_ctx_set_property(pMsgCtx, pEnv, "ServiceName", pProp);
      }

      pHashHeaders = axis2_msg_ctx_get_transport_headers(pMsgCtx, pEnv);
      if (pHashHeaders != NULL)
      {
        axis2_http_header_t* pHostHeader = (axis2_http_header_t *) axutil_hash_get(
            pHashHeaders, AXIS2_HTTP_HEADER_HOST, AXIS2_HASH_KEY_STRING);

        if (pHostHeader != NULL)
        {
          axis2_char_t* szHttpHost = axis2_http_header_get_value(pHostHeader, pEnv);
          if (szHttpHost != NULL)
          {
            axutil_property_t* pProp = axutil_property_create(pEnv);
            axutil_property_set_value(pProp, pEnv, axutil_strdup(pEnv, szHttpHost ? szHttpHost : ""));
            axis2_msg_ctx_set_property(pMsgCtx, pEnv, "HttpHost", pProp);
          }
        }
      }


      if (pStaffService != NULL)
      {
        pOperation = axis2_svc_get_op_with_name(pStaffService, pEnv, "Invoke");
        if (pOperation != NULL)
        {
          /* get real operation name */
          axis2_op_t* pRealOperation = axis2_msg_ctx_get_op(pMsgCtx, pEnv);
          if (pRealOperation != NULL)
          {
            const axutil_qname_t* pOperationQName = axis2_op_get_qname(pRealOperation, pEnv);
            if (pOperationQName != NULL)
            {
              axis2_char_t* szOperation = axutil_qname_get_localpart(pOperationQName, pEnv);
              if (szOperation != NULL)
              {
                axutil_property_t* pProp = axutil_property_create(pEnv);
                axutil_property_set_value(pProp, pEnv, axutil_strdup(pEnv, szOperation));
                axis2_msg_ctx_set_property(pMsgCtx, pEnv, "Operation", pProp);
              }
            }
          }

          axis2_msg_ctx_set_op(pMsgCtx, pEnv, pOperation);
          axis2_msg_ctx_set_svc(pMsgCtx, pEnv, pStaffService);
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
    {
      axis2_msg_ctx_set_op(pMsgCtx, pEnv, pOperation);
    }
  }

  return AXIS2_SUCCESS;
}
