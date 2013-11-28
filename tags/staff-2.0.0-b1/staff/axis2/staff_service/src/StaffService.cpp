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

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4091)
#endif

#ifdef WIN32
#include <windows.h>
#endif
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
#include <iostream>
#include <staff/utils/console.h>
#include <staff/utils/Log.h>
#include <staff/utils/Thread.h>
#include <staff/utils/Mutex.h>
#include <staff/utils/StackTracer.h>
#include <staff/utils/CrashHandler.h>
#include <staff/common/Exception.h>
#include <staff/common/Operation.h>
#include <staff/common/MessageContext.h>
#ifndef WITHOUT_SECURITY
#include <staff/security/tools.h>
#endif
#include <staff/component/ServiceWrapper.h>
#include <staff/component/SharedContext.h>
#include "Axis2Utils.h"
#include "ServiceDispatcher.h"
#if !defined VERSION_FULL && defined _MSC_VER
#include "version.h"
#endif

class StaffService
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

#if defined WIN32
  static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
  {
    if (dwCtrlType == CTRL_CLOSE_EVENT || dwCtrlType == CTRL_C_EVENT)
    {
      m_bShuttingDown = true;
      staff::ServiceDispatcher::Inst().Deinit();
    }
    return FALSE; // pass SIGINT to axis2/c
  }
#endif

  static int AXIS2_CALL Axis2Service_init(axis2_svc_skeleton_t* /*svc_skeleton*/, const axutil_env_t* /*pEnv*/)
  {
    // This method never seems to be called - an old Axis2C artifact?
//    svc_skeleton->func_array = axutil_array_list_create(pEnv, 0);

    return AXIS2_SUCCESS;
  }

  static int AXIS2_CALL Axis2Service_init_with_conf(
    axis2_svc_skeleton_t * /*pSvcSkeleton*/,
    const axutil_env_t * pEnv, 
    axis2_conf* pConf)
  {
    staff::CrashHandler::Enable();
#if defined WIN32
    // installing handler to process Staff deinitialization before dlls are unloaded
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
#endif

    const axis2_char_t* szServiceName = "StaffService";
    m_pAxis2Svc = axis2_conf_get_svc(pConf, pEnv, szServiceName);

    // check for staff module is engaged
    axutil_qname_t* pQName = axutil_qname_create(pEnv, "staff", NULL, NULL);
    axis2_bool_t bEngaged = axis2_svc_is_module_engaged(m_pAxis2Svc, pEnv, pQName);
    axutil_qname_free(pQName, pEnv);

    if (!bEngaged)
    {
      staff::LogError() << "staff module is not engaged. Unable to continue.";
      exit(1);
    }

#ifndef WITHOUT_SECURITY
    // check for staff_security is engaged
    pQName = axutil_qname_create(pEnv, "staff_security", NULL, NULL);
    bEngaged = axis2_svc_is_module_engaged(m_pAxis2Svc, pEnv, pQName);
    axutil_qname_free(pQName, pEnv);

    if (!bEngaged)
    {
      staff::LogError() << "staff_security module is not engaged. Unable to continue.";
      exit(1);
    }

    // initialize security
    if (!staff_security_init())
    {
      staff::LogError() << "Failed to initialize staff::security.";
      exit(1);
    }
#endif

    m_pEnv = pEnv;
    m_pConf = pConf;

    try
    {
      staff::ServiceDispatcher::Inst().Init(staff::ServiceDispatcher::Events(StaffService::OnConnect, StaffService::OnDisconnect));
      std::cout << "StaffService started (version " VERSION_FULL ")" << std::endl;

      return AXIS2_SUCCESS;
    }
    STAFF_CATCH_ALL_DESCR("Failed to start StaffService dispatcher")

    return AXIS2_FAILURE;
  }

  static int AXIS2_CALL Axis2Service_free(axis2_svc_skeleton_t *pSvcSkeleton, const axutil_env_t *pEnv)
  {
    staff::LogDebug() << "stopping StaffService";

#if !defined WIN32
    m_bShuttingDown = true;
    staff::ServiceDispatcher::Inst().Deinit();
#endif

#ifndef WITHOUT_SECURITY
    staff_security_free();
#endif

    if (pSvcSkeleton)
    {
      AXIS2_FREE(pEnv->allocator, pSvcSkeleton);
      pSvcSkeleton = NULL;
    }

    staff::CrashHandler::Disable();

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
    if (pAxiomNode == NULL)
    {
      staff::LogError() << "AxiOM node is NULL\n";
      return NULL;
    }

    if (axiom_node_get_node_type(pAxiomNode, pEnv) != AXIOM_ELEMENT)
    {
      staff::LogError() << "Can't get AxiOM node type";
      return NULL;
    }

    axiom_element_t* pAxiomElement = (axiom_element_t*)axiom_node_get_data_element(pAxiomNode, pEnv);
    if (pAxiomElement == NULL)
    {
      staff::LogError() << "Can't get AxiOM node element\n";
      return NULL;
    }

    const axis2_char_t* szServiceName = reinterpret_cast<const axis2_char_t*>
        (axis2_msg_ctx_get_property_value(pMsgCtx, pEnv, "ServiceName"));
#ifndef WITHOUT_SECURITY
    const axis2_char_t* szSessionId = reinterpret_cast<const axis2_char_t*>
        (axis2_msg_ctx_get_property_value(pMsgCtx, pEnv, "SessionId"));
    const axis2_char_t* szInstanceId = reinterpret_cast<const axis2_char_t*>
        (axis2_msg_ctx_get_property_value(pMsgCtx, pEnv, "InstanceId"));
#endif

    if (szServiceName == NULL)
    {
      staff::LogError() << "Cannot process message: Failed to get service name.";
      return NULL;
    }

#ifndef WITHOUT_SECURITY
    if (szSessionId == NULL)
    {
      staff::LogError() << "Cannot process message: Failed to get session id.";
      return NULL;
    }

    if (szInstanceId == NULL)
    {
      staff::LogError() << "Cannot process message: Failed to get instance id.";
      return NULL;
    }
#endif

#ifdef _DEBUG
    staff::LogDebug1() << "Service name: [" << szServiceName << "]";
#ifndef WITHOUT_SECURITY
    staff::LogDebug1() << "Session id: [" << szSessionId << "]";
    staff::LogDebug1() << "Instance id: [" << szInstanceId << "]";
#endif

    {
      axiom_node_t* panBody = axiom_node_get_parent(pAxiomNode, pEnv);
      axiom_node_t* panEnv = axiom_node_get_parent(panBody, pEnv);

      staff::LogDebug2() << "request SOAP Envelope: \n" << staff::ColorTextBlue
                  << staff::DataObject(panEnv).ToString() << "\n" << staff::ColorDefault;
    }
#endif

    staff::Operation tOperation;
    staff::MessageContext tMessageContext(pEnv, pMsgCtx);

    std::string sServiceName(szServiceName);
#ifndef WITHOUT_SECURITY
    std::string sSessionId(szSessionId);
    std::string sInstanceId(szInstanceId);
#else
    static std::string sSessionId;
    static std::string sInstanceId;
#endif

    try
    {
      tOperation.SetRequest(pAxiomNode);
      tOperation.SetMessageContext(tMessageContext);

      if (axis2_msg_ctx_get_doing_rest(pMsgCtx, pEnv))
      {
        const axis2_char_t* szOperation = reinterpret_cast<const axis2_char_t*>
            (axis2_msg_ctx_get_property_value(pMsgCtx, pEnv, "Operation"));
        if (szOperation != NULL)
        {
          tOperation.SetName(szOperation);
        }
      }

      if (sServiceName == "StaffService")
      {
        staff::ServiceDispatcher::Inst().InvokeSelf(tOperation);
      }
      else
      {
        staff::ServiceWrapper* pServiceWrapper =
            staff::SharedContext::Inst().GetService(sServiceName);
        STAFF_ASSERT(pServiceWrapper, "Service [" + sServiceName + "] is not found: ");
        pServiceWrapper->Invoke(tOperation, sSessionId, sInstanceId);
      }
    }
    catch (const std::exception& rEx)
    {
      tOperation.SetFault("server", rEx.what(), "Failed to invoke service " + sServiceName
                          + "." + tOperation.GetName()
#ifndef WITHOUT_SECURITY
                          + "#" + sInstanceId + "(" + sSessionId + ")"
#endif
                          );
    }
    catch (...)
    {
      tOperation.SetFault("server", "Unknown exception", "Failed to invoke service " + sServiceName
                          + "." + tOperation.GetName()
#ifndef WITHOUT_SECURITY
                          + "#" + sInstanceId + "(" + sSessionId + ")"
#endif
                          );
    }

    if (tOperation.IsFault())
    {
      staff::ScopedLock tLock(m_tFaultDetailsMutex);

      m_mFaultDetails[staff::Thread::GetCurrentId()] = tOperation.GetFaultDetail();
      staff::LogWarning() << "Fault: \n" << tOperation.GetFaultDescr() << "\n";
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
    staff::DataObject& rResponse = tOperation.GetResponse();
    rResponse.SetOwner(false);

#ifdef _DEBUG
    staff::LogDebug2() << "Sending Response: \n" <<  staff::ColorTextBlue << rResponse.ToString() << "\n" << staff::ColorDefault;
#endif

    return rResponse;
  }

  static void OnConnect(const staff::ServiceWrapper* pServiceWrapper)
  {
    Axis2UtilsCreateVirtualService(pServiceWrapper, m_pEnv, m_pConf);
  }

  static void OnDisconnect(const std::string& sServiceName)
  {
    Axis2UtilsRemoveVirtualService(sServiceName, m_pEnv, m_pConf, m_bShuttingDown);
  }

  static axiom_node_t * AXIS2_CALL Axis2Service_on_fault(
    axis2_svc_skeleton_t*,
    const axutil_env_t* pEnv,
    axiom_node_t* /*pNode*/)
  {
    staff::ScopedLock tLock(m_tFaultDetailsMutex);

    std::map<unsigned long, std::string>::iterator itFault =
        m_mFaultDetails.find(staff::Thread::GetCurrentId());

    axiom_node_t* pErrorNode = NULL;
    axiom_element_t* pErrorElement = axiom_element_create(pEnv, NULL, "Exception", NULL, &pErrorNode);

    if (itFault != m_mFaultDetails.end())
    {
      axiom_element_set_text(pErrorElement, pEnv, itFault->second.c_str(), pErrorNode);
      m_mFaultDetails.erase(itFault);
    }
    else
    {
      axiom_element_set_text(pErrorElement, pEnv, "No fault details", pErrorNode);
    }

    return pErrorNode;
  }

private:
  static axis2_svc_skeleton_ops_t m_stAxis2SkelOps;
  static axis2_svc_t* m_pAxis2Svc;
  static staff::Mutex m_tFaultDetailsMutex;
  static std::map<unsigned long, std::string> m_mFaultDetails;
  static const axutil_env_t* m_pEnv; 
  static axis2_conf* m_pConf;
  static bool m_bShuttingDown;
};


axis2_svc_skeleton_ops_t StaffService::m_stAxis2SkelOps =
{
  Axis2Service_init,
  Axis2Service_invoke,
  Axis2Service_on_fault,
  Axis2Service_free,
  Axis2Service_init_with_conf
};

axis2_svc_t* StaffService::m_pAxis2Svc = NULL;
staff::Mutex StaffService::m_tFaultDetailsMutex;
std::map<unsigned long, std::string> StaffService::m_mFaultDetails;
const axutil_env_t* StaffService::m_pEnv = NULL;
axis2_conf* StaffService::m_pConf = NULL;
bool StaffService::m_bShuttingDown = false;

/**
 * Following block distinguish the exposed part of the dll.
 */

extern "C" AXIS2_EXPORT int axis2_get_instance(axis2_svc_skeleton** ppSvcSkeleton, axutil_env_t* pAxEnv)
{
  *ppSvcSkeleton = StaffService::axis2_Axis2Service_create(pAxEnv);
  if(!(*ppSvcSkeleton))
  {
    return AXIS2_FAILURE;
  }

  return AXIS2_SUCCESS;
}

extern "C" AXIS2_EXPORT int axis2_remove_instance(axis2_svc_skeleton_t* pSvcSkeleton, axutil_env_t* pAxEnv)
{
  axis2_status_t tStatus = AXIS2_FAILURE;
  if (pSvcSkeleton)
  {
    tStatus = AXIS2_SVC_SKELETON_FREE(pSvcSkeleton, pAxEnv);
  }

  return tStatus;
}
