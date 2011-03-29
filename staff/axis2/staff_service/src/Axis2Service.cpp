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
#include <rise/threading/Thread.h>
#include <staff/common/Exception.h>
#include <staff/common/Operation.h>
#include <staff/common/MessageContext.h>
#include <staff/component/ServiceWrapper.h>
#include <staff/component/SharedContext.h>
#include <staff/component/SessionManager.h>
#include <staff/security/Sessions.h>
#include "Axis2Utils.h"
#include "ServiceDispatcher.h"

#if defined LINUX_RELEASE_MCBC
#define sighandler_t __sighandler_t
#else
#if defined OS_FreeBSD || defined OS_Darwin
#define sighandler_t sig_t
#else
#ifdef WIN32
typedef void (*sighandler_t)(int);
#endif
#endif
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

    // check for staff module is engaged
    axutil_qname_t* pQName = axutil_qname_create(pEnv, "staff", NULL, NULL);
    axis2_bool_t bEngaged = axis2_svc_is_module_engaged(m_pAxis2Svc, pEnv, pQName);
    axutil_qname_free(pQName, pEnv);

    if (!bEngaged)    
    {
      rise::LogError() << "staff module is not engaged. Unable to continue.";
      exit(1);
    }

    // check for staff_security is engaged
    pQName = axutil_qname_create(pEnv, "staff_security", NULL, NULL);
    bEngaged = axis2_svc_is_module_engaged(m_pAxis2Svc, pEnv, pQName);
    axutil_qname_free(pQName, pEnv);

    if (!bEngaged)    
    {
      rise::LogError() << "staff_security module is not engaged. Unable to continue.";
      exit(1);
    }


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

  static bool IsNeedReply(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv)
  {
    axis2_op* pAxis2Op = axis2_msg_ctx_get_op(pMsgCtx, pEnv);
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
    axis2_msg_ctx_t* pMsgCtx)
  {
rise::LogEntry();

    if (pAxiomNode == NULL)
    {
      rise::LogError() << "AxiOM node is NULL\n";
      return NULL;
    }

    if (axiom_node_get_node_type(pAxiomNode, pEnv) != AXIOM_ELEMENT)
    {
      rise::LogError() << "Can't get AxiOM node type";
      return NULL;
    }

    axiom_element_t* pAxiomElement = (axiom_element_t*)axiom_node_get_data_element(pAxiomNode, pEnv);
    if (pAxiomElement == NULL)
    {
      rise::LogError() << "Can't get AxiOM node element\n";
      return NULL;
    }

    const axis2_char_t* szServiceName = reinterpret_cast<const axis2_char_t*>
        (axis2_msg_ctx_get_property_value(pMsgCtx, pEnv, "ServiceName"));
    const axis2_char_t* szSessionId = reinterpret_cast<const axis2_char_t*>
        (axis2_msg_ctx_get_property_value(pMsgCtx, pEnv, "SessionId"));
    const axis2_char_t* szInstanceId = reinterpret_cast<const axis2_char_t*>
        (axis2_msg_ctx_get_property_value(pMsgCtx, pEnv, "InstanceId"));

    if (szServiceName == NULL)
    {
      rise::LogError() << "Cannot process message: Service name is not set by security module.";
      return NULL;
    }

    if (szSessionId == NULL)
    {
      rise::LogError() << "Cannot process message: Session id is not set by security module.";
      return NULL;
    }

    if (szInstanceId == NULL)
    {
      rise::LogError() << "Cannot process message: Instance id is not set by security module.";
      return NULL;
    }

#ifdef _DEBUG
    rise::LogDebug1() << "Service name: [" << szServiceName << "]";
    rise::LogDebug1() << "Session id: [" << szSessionId << "]";
    rise::LogDebug1() << "Instance id: [" << szInstanceId << "]";

    {
      axiom_node_t* panBody = axiom_node_get_parent(pAxiomNode, pEnv);
      axiom_node_t* panEnv = axiom_node_get_parent(panBody, pEnv);

      rise::LogDebug2() << "request SOAP Envelope: \n" << rise::ColorInkBlue << staff::CDataObject(panEnv).ToString() << "\n" << rise::ColorDefault;
    }
#endif
    

    staff::COperation tOperation;
    staff::CMessageContext tMessageContext(pEnv, pMsgCtx);

    std::string sServiceName = szServiceName;
    std::string sSessionId = szSessionId;
    std::string sInstanceId = szInstanceId;

    try
    {
      tOperation.SetRequest(pAxiomNode);
      tOperation.SetMessageContext(tMessageContext);

      if (sServiceName == "StaffService")
      {
        staff::CServiceDispatcher::Inst().InvokeSelf(tOperation);
      }
      else
      {
        staff::CServiceWrapper* pServiceWrapper = staff::CSharedContext::Inst().GetService(sServiceName);
        RISE_ASSERTS(pServiceWrapper, "Service [" + sServiceName + "] is not found: ");
        pServiceWrapper->Invoke(tOperation, sSessionId, sInstanceId);
      }
    }
    catch(const staff::CRemoteException& rEx)
    {
      tOperation.SetFault("server", rEx.GetString(), "Failed to invoke service " + sServiceName
                          + "." + tOperation.GetName() + "#" + sInstanceId + "(" + sSessionId + ")");
    }
    catch(const rise::CException& rEx)
    {
      tOperation.SetFault("server", rEx.GetString(), "Failed to invoke service " + sServiceName
                          + "." + tOperation.GetName() + "#" + sInstanceId + "(" + sSessionId + ")");
    }
    catch(const std::exception& rEx)
    {
      tOperation.SetFault("server", rEx.what(), "Failed to invoke service " + sServiceName
                          + "." + tOperation.GetName() + "#" + sInstanceId + "(" + sSessionId + ")");
    }
    catch(...)
    {
      tOperation.SetFault("server", "Unknown exception", "Failed to invoke service " + sServiceName
                          + "." + tOperation.GetName() + "#" + sInstanceId + "(" + sSessionId + ")");
    }

    if (tOperation.IsFault())
    {
      m_sLastFaultDetail = tOperation.GetFaultDetail();
      rise::LogWarning() << "Fault: \n" << tOperation.GetFaultDescr() << "\n";
      AXIS2_ERROR_SET_MESSAGE(pEnv->error, static_cast<axis2_char_t*>(axutil_strdup(pEnv, tOperation.GetFaultString().c_str())));
      AXIS2_ERROR_SET_ERROR_NUMBER(pEnv->error, static_cast<axutil_error_codes_t>(AXUTIL_ERROR_MAX + 1));
      AXIS2_ERROR_SET_STATUS_CODE(pEnv->error, AXIS2_FAILURE);
      return NULL;
    }

    if(!IsNeedReply(pMsgCtx, pEnv))
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

  static void OnConnect(const std::string& sServiceName, const staff::CServiceWrapper* pServiceWrapper)
  {
    void* pSvcClass = axis2_svc_get_impl_class(m_pAxis2Svc, m_pEnv);

    Axis2UtilsCreateVirtualService(sServiceName, pServiceWrapper, pSvcClass, m_pEnv, m_pConf);
  }

  static void OnDisconnect(const std::string& sServiceName)
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
      rise::LogError() << "Segmentation fault in thread " << rise::threading::CThread::GetCurrentId()
          << ".\nTraced stack:\n" << sTracedStack;
      exit(1);
    }
  }

  static void Init()
  {
    m_pPrevSigSegvHandler = signal(SIGSEGV, CAxis2Service::OnSignal);
    staff::CSessionManager::Inst().Start();
  }

  static void Deinit()
  {
    staff::CSessionManager::Inst().Stop();
    staff::CSessionManager::FreeInst();
    signal(SIGSEGV, CAxis2Service::m_pPrevSigSegvHandler);
  }

private:
  static axis2_svc_skeleton_ops_t m_stAxis2SkelOps;
  static axis2_svc_t* m_pAxis2Svc;
  static std::string m_sLastFaultDetail;
  static const axutil_env_t* m_pEnv; 
  static axis2_conf* m_pConf;
  static bool m_bShuttingDown;
  static sighandler_t m_pPrevSigSegvHandler;
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
const axutil_env_t* CAxis2Service::m_pEnv = NULL; 
axis2_conf* CAxis2Service::m_pConf = NULL;
bool CAxis2Service::m_bShuttingDown = false;
sighandler_t CAxis2Service::m_pPrevSigSegvHandler = NULL;

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
