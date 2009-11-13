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

#include <signal.h>
#include <axis2_svc_skeleton.h>
#include <axis2_conf.h>
#include <axutil_array_list.h>
#include <axutil_log_default.h>
#include <axutil_error_default.h>
#include <axutil_utils.h>
#include <axiom_soap_header.h>
#include <axiom_soap_header_block.h>
#include <axiom_soap_body.h>
#include <axiom.h>
#include <exception>
#include <rise/common/console.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/Log.h>
#include <staff/common/Exception.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObjectHelper.h>
#include <staff/component/Service.h>
#include <staff/component/SharedContext.h>
#include <staff/security/Security.h>
#include "Axis2Utils.h"
#include "ServiceDispatcher.h"

#ifdef OS_MCBC
#define sighandler_t __sighandler_t
#endif

class CAxis2Service
{
public:
  static axis2_svc_skeleton_t* axis2_Axis2Service_create(axutil_env_t *pEnv)
  {
    axis2_svc_skeleton_t *svc_skeleton = NULL;
    svc_skeleton = (axis2_svc_skeleton_t *) AXIS2_MALLOC((pEnv)->allocator, sizeof(axis2_svc_skeleton_t));

    svc_skeleton->ops = &m_stAxis2SkelOps;
    svc_skeleton->func_array = NULL;

    return svc_skeleton;
  }

  static int AXIS2_CALL Axis2Service_init(axis2_svc_skeleton_t *svc_skeleton, const axutil_env_t *pEnv)
  {
    // This method never seems to be called - an old Axis2C artifact?
//    svc_skeleton->func_array = axutil_array_list_create(pEnv, 0);

    return AXIS2_SUCCESS;
  }

  static int	AXIS2_CALL Axis2Service_init_with_conf(
    axis2_svc_skeleton_t * /*pSvcSkeleton*/,
    const axutil_env_t * pEnv, 
    axis2_conf * pConf)
  {
    const axis2_char_t* service_name = "StaffService";
    m_pAxis2Svc = axis2_conf_get_svc(pConf, pEnv, service_name);

    m_pEnv = pEnv;
    m_pConf = pConf;

    staff::CServiceDispatcher::Inst().Init(staff::CServiceDispatcher::SEvents(CAxis2Service::OnConnect, CAxis2Service::OnDisconnect));

    return AXIS2_SUCCESS;
  }

  static int AXIS2_CALL Axis2Service_free(axis2_svc_skeleton_t *svc_skeleton, const axutil_env_t *pEnv)
  {
rise::LogEntry();
    m_bShuttingDown = true;
    staff::CServiceDispatcher::Inst().Deinit();
    staff::CServiceDispatcher::FreeInst();

    if(svc_skeleton)
    {
      AXIS2_FREE((pEnv)->allocator, svc_skeleton);
      svc_skeleton = NULL;
    }

    return AXIS2_SUCCESS; 
  }

  static bool IsNeedReply(axis2_msg_ctx_t* pAxis2MsgCtx, const axutil_env_t* pEnv)
  {
    axis2_op* pAxis2Op = axis2_msg_ctx_get_op(pAxis2MsgCtx, pEnv);
    const axis2_char_t* szExchangePattern = axis2_op_get_msg_exchange_pattern(pAxis2Op, pEnv);

    return !(axutil_strcmp(szExchangePattern, AXIS2_MEP_URI_OUT_ONLY) == 0 ||
             axutil_strcmp(szExchangePattern, AXIS2_MEP_URI_ROBUST_OUT_ONLY) == 0);
  }

  /*
  * This method invokes the target service method 
  */
  static axiom_node_t* AXIS2_CALL
    Axis2Service_invoke(axis2_svc_skeleton_t* /*pServiceSkeleton*/,
    const axutil_env_t* pEnv,
    axiom_node_t* pAxiomNode,
    axis2_msg_ctx_t* pAxis2MsgCtx)
  {
rise::LogEntry();
    if (pAxiomNode == NULL)
    {
      rise::LogError() << "Invalid parameters in Axis2 request OM\n";
      return NULL;
    }

    if (axiom_node_get_node_type(pAxiomNode, pEnv) != AXIOM_ELEMENT)
    {
      rise::LogError() << "!axiom_node_get_node_type\n";
      return NULL;
    }

    axiom_element_t* pAxiomElement = (axiom_element_t*)axiom_node_get_data_element(pAxiomNode, pEnv);
    if (pAxiomElement == NULL)
    {
      rise::LogError() << "!axiom_node_get_data_element\n";
      return NULL;
    }

    const axis2_char_t* szSessionId = NULL;

    {
      axis2_op_ctx_t* pOperationContext = axis2_msg_ctx_get_op_ctx(pAxis2MsgCtx, pEnv);
      axis2_msg_ctx_t* pInMessageContext = axis2_op_ctx_get_msg_ctx(pOperationContext, pEnv, AXIS2_WSDL_MESSAGE_LABEL_IN);
      axiom_soap_envelope_t* pSoapEnv = axis2_msg_ctx_get_soap_envelope(pInMessageContext, pEnv);
      axiom_node_t* pHeaderBlockNode = NULL;  
      axiom_soap_header_block_t* pSoapHeaderBlock = NULL;

      axiom_soap_header* pSoapHeader = axiom_soap_envelope_get_header(pSoapEnv, pEnv);
      if(pSoapHeader != NULL)
      {
        axiom_element_t* pSoapHeaderBlockElement = NULL;
        axutil_hash_t* pHeaderHash = axiom_soap_header_get_all_header_blocks(pSoapHeader, pEnv);

        if(pHeaderHash != NULL)
        {
          for (axutil_hash_index_t* pSoapHeadersHashIndex = axutil_hash_first(pHeaderHash, pEnv); 
               pSoapHeadersHashIndex != NULL;
               pSoapHeadersHashIndex = axutil_hash_next(pEnv, pSoapHeadersHashIndex))
          {
            void* pHeaderBlock = NULL;
            axis2_char_t* szHeaderLocalName = NULL;

            axutil_hash_this(pSoapHeadersHashIndex, NULL, NULL, &pHeaderBlock);
            if (pHeaderBlock != NULL)
            {
              pSoapHeaderBlock = (axiom_soap_header_block_t *)pHeaderBlock;
              if (pSoapHeaderBlock != NULL)
              {
                pHeaderBlockNode = axiom_soap_header_block_get_base_node(pSoapHeaderBlock, pEnv);
                if (pHeaderBlockNode != NULL)
                {
                  pSoapHeaderBlockElement = (axiom_element_t*)axiom_node_get_data_element(pHeaderBlockNode, pEnv);
                  if (pSoapHeaderBlockElement != NULL)
                  {
                    szHeaderLocalName = axiom_element_get_localname(pSoapHeaderBlockElement, pEnv);
                    if (szHeaderLocalName != NULL && axutil_strcmp(szHeaderLocalName, "SessionId") == 0)
                    {
                      szSessionId = axiom_element_get_text(pSoapHeaderBlockElement, pEnv, pHeaderBlockNode);
                      break;
                    }
                  }
                }
              }
            }
          } // for (axutil_hash_index_t* pSoapHeadersHashIndex =...
        } // if(pHeaderHash != NULL)
      } // if(pSoapHeader != NULL)
    }


rise::LogLabel();

#ifdef _DEBUG
    {
      axiom_node_t* panBody = axiom_node_get_parent(pAxiomNode, pEnv);
      axiom_node_t* panEnv = axiom_node_get_parent(panBody, pEnv);

      rise::LogDebug2() << "request SOAP Envelope: \n" << rise::ColorInkBlue << staff::CDataObject(panEnv).ToString() << "\n" << rise::ColorDefault;
    }
#endif
    

    staff::COperation tOperation;

    rise::CString sServiceName;
    rise::CString sID = (szSessionId == NULL || szSessionId[0] == '\0') ? STAFF_SECURITY_GUEST_SESSION_ID : szSessionId;

    try
    {
      tOperation.Request().Attach(pAxiomNode);
      
      rise::CString sUri = tOperation.Request().GetNamespaceUri();

      rise::TSize nPos = sUri.find_last_of('/');
      if (nPos == rise::CString::npos)
        nPos = 0;

      rise::StrMid(sUri, sServiceName, nPos + 1);

      if (sServiceName == "StaffService")
      {
        staff::CServiceDispatcher::Inst().InvokeSelf(tOperation);
      } else
      {
        staff::CService* pService = staff::CSharedContext::Inst().GetService(sServiceName);
        RISE_ASSERTS(pService != NULL, "Service not found!");
        pService->Invoke(tOperation, sID);
      }
    }
    catch(const staff::CRemoteException& rEx)
    {
      tOperation.SetFault("Failed to invoke service " + sServiceName + "(" + sID + ")", rEx.GetString());
    }
    catch(const rise::CException& rEx)
    {
      tOperation.SetFault("Failed to invoke service " + sServiceName + "(" + sID + ")", rEx.GetString());
    }
    catch(const std::exception& rEx)
    {
      tOperation.SetFault("Failed to invoke service " + sServiceName + "(" + sID + ")", rEx.what());
    }
    catch(...)
    {
      tOperation.SetFault("Failed to invoke service " + sServiceName + "(" + sID + ")", "Unknown exception");
    }

    if (tOperation.IsFault())
    {
      m_sLastFaultDetail = tOperation.GetFaultDetail();
      m_sLastFaultReason = tOperation.GetFaultReason();
      rise::LogWarning() << "Fault: \n" << tOperation.GetFaultString() << "\n";
      AXIS2_ERROR_SET_MESSAGE(pEnv->error, const_cast<axis2_char_t*>(m_sLastFaultReason.c_str()));
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, static_cast<axutil_error_codes_t>(AXUTIL_ERROR_MAX + 1));
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return NULL;
    }

    if(!IsNeedReply(pAxis2MsgCtx, pEnv))
    {
      return NULL;
    }

    tOperation.PrepareResult();
    tOperation.GetResponse().SetOwner(false);

#ifdef _DEBUG
    rise::LogDebug2() << "Sending Response: \n" <<  rise::ColorInkBlue << tOperation.GetResponse().ToString() << "\n" << rise::ColorDefault;
#endif

    return tOperation.GetResponse();
  }

  static void OnConnect(const std::string& sServiceName, const staff::CService* pService)
  {
    void* pSvcClass = axis2_svc_get_impl_class(m_pAxis2Svc, m_pEnv);

    Axis2UtilsCreateVirtualService(sServiceName, pService, pSvcClass, m_pEnv, m_pConf);
  }

  static void OnDisconnect(const rise::CString& sServiceName)
  {
    if (!m_bShuttingDown) // double free prevent
    {
      Axis2UtilsRemoveVirtualService(sServiceName, m_pEnv, m_pConf);
    }
  }

  static axiom_node_t * AXIS2_CALL Axis2Service_on_fault(
    axis2_svc_skeleton_t*,
    const axutil_env_t* pEnv,
    axiom_node_t* /*pNode*/)
  {
    axiom_node_t* pErrorNode = NULL;
    axiom_element_t* pErrorElement = axiom_element_create(pEnv, NULL, "Exception", NULL, &pErrorNode);
    axiom_element_set_text(pErrorElement, pEnv, m_sLastFaultDetail.c_str(), pErrorNode);
    return pErrorNode;
  }

  static void OnSignal(int nSignal)
  {
    if(nSignal == SIGSEGV)
    {
      std::string sTracedStack;
      rise::tools::CStackTracer::StackTraceStr(sTracedStack);
      rise::LogError() << "Segmentation fault.\nTraced stack:\n" << sTracedStack;
      exit(1);
    }
  }

  static void Init()
  {
    m_pSigSegvHandler = signal(SIGSEGV, CAxis2Service::OnSignal);
  }

  static void Deinit()
  {
    signal(SIGSEGV, CAxis2Service::m_pSigSegvHandler);
  }

private:
  static axis2_svc_skeleton_ops_t m_stAxis2SkelOps;
  static axis2_svc_t* m_pAxis2Svc;
  static std::string m_sLastFaultDetail;
  static std::string m_sLastFaultReason;
  static const axutil_env_t* m_pEnv; 
  static axis2_conf* m_pConf;
  static bool m_bShuttingDown;
  static sighandler_t m_pSigSegvHandler;
};


axis2_svc_skeleton_ops_t CAxis2Service::m_stAxis2SkelOps =
{
  Axis2Service_init,
  Axis2Service_invoke,
  Axis2Service_on_fault,
  Axis2Service_free,
  Axis2Service_init_with_conf
};

axis2_svc_t* CAxis2Service::m_pAxis2Svc = NULL;
std::string CAxis2Service::m_sLastFaultDetail;
std::string CAxis2Service::m_sLastFaultReason;
const axutil_env_t* CAxis2Service::m_pEnv = NULL; 
axis2_conf* CAxis2Service::m_pConf = NULL;
bool CAxis2Service::m_bShuttingDown = false;
sighandler_t CAxis2Service::m_pSigSegvHandler = NULL;

/**
 * Following block distinguish the exposed part of the dll.
 */

extern "C" AXIS2_EXPORT int axis2_get_instance(axis2_svc_skeleton** ppSvcSkeleton, axutil_env_t* pAxEnv)
{
rise::LogEntry();

#ifdef WIN32
// cp 20866
  setlocale(LC_ALL, "Russian_Russia.20866");
#endif // WIN32

  *ppSvcSkeleton = CAxis2Service::axis2_Axis2Service_create(pAxEnv);
  if(!(*ppSvcSkeleton))
    return AXIS2_FAILURE;

#if defined DEBUG || defined _DEBUG
  rise::LogInfo() << "StaffService started";
#endif

  CAxis2Service::Init();

  return AXIS2_SUCCESS;
}

extern "C" AXIS2_EXPORT int axis2_remove_instance(axis2_svc_skeleton_t* pSvcSkeleton, axutil_env_t* pAxEnv)
{
rise::LogEntry();
#if defined DEBUG || defined _DEBUG
  rise::LogDebug() << "stopping StaffService";
#endif

  CAxis2Service::Deinit();

  axis2_status_t tStatus = AXIS2_FAILURE;
  if (pSvcSkeleton)
    tStatus = AXIS2_SVC_SKELETON_FREE(pSvcSkeleton, pAxEnv);

  return tStatus;
}
