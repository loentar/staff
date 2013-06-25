/*
 *  Copyright 2010 Utkin Dmitry
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

#include <axiom.h>
#include <axis2_client.h>
#include <axis2_defines.h>
#include <axis2_callback.h>
#include <axutil_env.h>
#include <axutil_string.h>
#include <axiom_soap_const.h>
#include <string>
#include <staff/utils/Log.h>
#include <staff/utils/console.h>
#include <staff/utils/tostring.h>
#include <staff/common/Exception.h>
#include <staff/common/Exception.h>
#include <staff/common/Runtime.h>
#include <staff/common/QName.h>
#include <staff/common/Namespace.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObject.h>
#include "ICallback.h"
#include "Options.h"
#include "ServiceClient.h"

namespace staff
{
  class CallbackWrapper
  {
  public:
    CallbackWrapper(ICallback<const DataObject&>* pCallback, bool bOwner):
      m_pCallback(pCallback), m_bOwner(bOwner)
    {
    }

    ~CallbackWrapper()
    {
      if (m_bOwner)
      {
        delete m_pCallback;
      }
    }

    ICallback<const DataObject&>* Get()
    {
      return m_pCallback;
    }

  private:
    ICallback<const DataObject&>* m_pCallback;
    bool m_bOwner;
  };

  class ServiceClientTools
  {
  public:
    static axis2_status_t AXIS2_CALL CallbackOnComplete(axis2_callback_t* pAxis2Callback, const axutil_env_t* pEnv)
    {
      if (!pAxis2Callback)
      {
        LogError() << "pCallback is NULL";
        return AXIS2_FAILURE;
      }

      std::auto_ptr<CallbackWrapper> tpCallbackWrapper(
          reinterpret_cast<CallbackWrapper*>(axis2_callback_get_data(pAxis2Callback)));

      if (!tpCallbackWrapper.get())
      {
        LogError() << "pointer to CallbackWrapper is NULL";
        return AXIS2_FAILURE;
      }

      axis2_callback_set_data(pAxis2Callback, NULL);  // avoid axis2/c to destroy C++ data

      ICallback<const DataObject&>* pCallback = tpCallbackWrapper->Get();

      if (!pCallback)
      {
        LogError() << "pointer to ICallback is NULL";
        return AXIS2_FAILURE;
      }

      axiom_soap_envelope_t* pSoapEnvelope = axis2_callback_get_envelope(pAxis2Callback, pEnv);
      if (!pSoapEnvelope)
      {
        const char* szError = reinterpret_cast<const char*>(AXIS2_ERROR_GET_MESSAGE(pEnv->error));
        CreateFault(*pCallback, "client", szError ? szError : "UNKNOWN");
        return AXIS2_FAILURE;
      }

      axiom_soap_body_t* pSoapBody = axiom_soap_envelope_get_body(pSoapEnvelope, pEnv);
      if (!pSoapBody)
      {
        CreateFault(*pCallback, "client", "Failed to get soap body while parsing response");
        return AXIS2_FAILURE;
      }

      axiom_node_t* pAxiomBaseNode = axiom_soap_body_get_base_node(pSoapBody, pEnv);
      if (!pAxiomBaseNode)
      {
        CreateFault(*pCallback, "client", "Failed to get base node while parsing response");
        return AXIS2_FAILURE;
      }

      axiom_node_t* pAxiomResponseNode = axiom_node_get_first_element(pAxiomBaseNode, pEnv);
      if (!pAxiomResponseNode)
      {
        CreateFault(*pCallback, "client", "Failed to get response node while parsing response");
        return AXIS2_FAILURE;
      }

#ifdef _DEBUG
      LogDebug2() << "Received Response: \n" << ColorTextBlue
          << DataObject(pAxiomResponseNode).ToString() << ColorDefault;
#endif

      axiom_element_t* pAxiomResponseElement =
          reinterpret_cast<axiom_element_t*>(axiom_node_get_data_element(pAxiomResponseNode, pEnv));
      if (!pAxiomResponseElement)
      {
        CreateFault(*pCallback, "client", "Failed to get data element while parsing response");
        return AXIS2_FAILURE;
      }

      // is soap Fault?
      if (axutil_strcmp(axiom_element_get_localname(pAxiomResponseElement, pEnv), "Fault") == 0)
      {
        axiom_namespace_t* pNamespace =
            axiom_element_get_namespace(pAxiomResponseElement, pEnv, pAxiomResponseNode);
        if (pNamespace)
        {
          const axis2_char_t* szNamespaceUri = axiom_namespace_get_uri(pNamespace, pEnv);

          if (!axutil_strcmp(szNamespaceUri, AXIOM_SOAP11_SOAP_ENVELOPE_NAMESPACE_URI) || // soap 1.1
              !axutil_strcmp(szNamespaceUri, AXIOM_SOAP12_SOAP_ENVELOPE_NAMESPACE_URI)) // soap 1.2
          {
            try
            {
              pCallback->OnFault(pAxiomResponseNode);
            }
            STAFF_CATCH_ALL_DESCR("Error while processing fault response");

            return AXIS2_SUCCESS;
          }
        }
      }

      try
      {
        pCallback->OnComplete(pAxiomResponseNode);
      }
      STAFF_CATCH_ALL_DESCR("Error while processing response");

      return AXIS2_SUCCESS;
    }

    static axis2_status_t AXIS2_CALL CallbackOnFault(axis2_callback_t* pCallback, const axutil_env_t* pEnv,
                                                     int nFaultCode)
    {
      if (!pCallback)
      {
        LogError() << "pCallback is NULL";
        return AXIS2_FAILURE;
      }

      PICallback tpCallback(reinterpret_cast<ICallback<const DataObject&>*>(axis2_callback_get_data(pCallback)));
      axis2_callback_set_data(pCallback, NULL);  // avoid axis2/c to destroy C++ data

      if (!tpCallback.get())
      {
        LogError() << "pointer to ICallback is NULL";
        return AXIS2_FAILURE;
      }

      axiom_node_t* pAxiomResponseNode = NULL;
      {
        axiom_soap_envelope_t* pSoapEnvelope = axis2_callback_get_envelope(pCallback, pEnv);
        if (pSoapEnvelope)
        {
          pAxiomResponseNode = axiom_soap_envelope_get_base_node(pSoapEnvelope, pEnv);
#ifdef _DEBUG
      LogDebug2() << "Received Response: \n" << staff::ColorTextBlue
          << DataObject(pAxiomResponseNode).ToString() << staff::ColorDefault;
#endif
        }
      }

      try
      {
        if (!pAxiomResponseNode)
        {
          CreateFault(*tpCallback, AXIS2_ERROR_GET_MESSAGE(pEnv->error), ToString(nFaultCode));
        }
        else
        {
          tpCallback->OnFault(pAxiomResponseNode);
        }
      }
      STAFF_CATCH_ALL_DESCR("Error while processing response")

      return AXIS2_SUCCESS;
    }

    static void CreateFault(ICallback<const DataObject&>& rCallback, const std::string& sFaultCode,
                            const std::string& sFaultError)
    {
      // generate fault using Operation
      Operation tFault;
      tFault.SetFault(sFaultCode, sFaultError);
      rCallback.OnFault(tFault.GetResponse());
    }
  };

  ServiceClient::ServiceClient():
    m_pSvcClient(NULL),
    m_pEnv(Runtime::Inst().CreateAxis2Env("staff_client.log")),
    m_pOptions(NULL),
    m_bOptOwner(false),
    m_bInit(false)
  {
  }

  ServiceClient::~ServiceClient()
  {
    Deinit();
    if (m_bOptOwner && m_pOptions)
    {
      delete m_pOptions;
    }
    Runtime::Inst().FreeAxis2Env(m_pEnv);
  }

  void ServiceClient::Init(const std::string& sServiceUri /*= ""*/)
  {
    std::auto_ptr<Options> pOptions(new Options(m_pEnv));
    if (!sServiceUri.empty())
    {
      pOptions->SetToAddress(sServiceUri);
    }

    Init(*pOptions);
    m_bOptOwner = true;
    pOptions.release();
  }

  void ServiceClient::Init(Options& rOptions)
  {
    Deinit();

    // Create service client
    m_pSvcClient = axis2_svc_client_create(m_pEnv, Runtime::Inst().GetAxis2Home().c_str());
    STAFF_ASSERT(m_pSvcClient, "Axis2/C client creation failed. error: " + GetLastErrorStr());

    // Set service client options
    SetOptions(rOptions);

    m_bInit = true;
  }

  void ServiceClient::Deinit()
  {
    if (m_pSvcClient != NULL)
    {
      axis2_svc_client_free(m_pSvcClient, m_pEnv);
      m_pSvcClient = NULL;
    }

    m_bInit = false;
  }

  bool ServiceClient::IsInit() const
  {
    return m_bInit;
  }

  void ServiceClient::SetOptions(Options& rOptions)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    rOptions.SetOwner(false);

    if (m_bOptOwner)
    {
      delete m_pOptions;
      m_bOptOwner = false;
    }

    m_pOptions = &rOptions;

    axis2_svc_client_set_options(m_pSvcClient, m_pEnv, *m_pOptions);
  }

  Options& ServiceClient::GetOptions()
  {
    STAFF_ASSERT(m_pOptions, "Options is not initialized");

    return *m_pOptions;
  }

  void ServiceClient::EngageModule(const std::string& sModuleName)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axis2_status_t nResult =
      axis2_svc_client_engage_module(m_pSvcClient, m_pEnv, sModuleName.c_str());
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "Failed to engage module [" + sModuleName + "]: " + GetLastErrorStr());
  }

  void ServiceClient::DisengageModule(const std::string& sModuleName)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axis2_status_t nResult =
      axis2_svc_client_disengage_module(m_pSvcClient, m_pEnv, sModuleName.c_str());
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "Failed to engage module [" + sModuleName + "]: " + GetLastErrorStr());
  }


  void ServiceClient::AddHeader(DataObject& rdoHeader)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axis2_svc_client_add_header(m_pSvcClient, m_pEnv, rdoHeader);
  }

  void ServiceClient::RemoveAllHeaders()
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axis2_svc_client_remove_all_headers(m_pSvcClient, m_pEnv);
  }

  void ServiceClient::SetProxyWithAuth(const std::string& sProxyHost, const std::string& sProxyPort,
                                        const std::string& sUserName, const std::string& sPassword)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axis2_svc_client_set_proxy_with_auth(m_pSvcClient, m_pEnv,
                                         const_cast<axis2_char_t*>(sProxyHost.c_str()),
                                         const_cast<axis2_char_t*>(sProxyPort.c_str()),
                                         const_cast<axis2_char_t*>(sUserName.c_str()),
                                         const_cast<axis2_char_t*>(sPassword.c_str()));
  }

  void ServiceClient::TestAuthRequired()
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axis2_svc_client_send_robust(m_pSvcClient, m_pEnv, NULL);
  }

  bool ServiceClient::GetProxyAuthRequired()
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    return axis2_svc_client_get_proxy_auth_required(m_pSvcClient, m_pEnv) == AXIS2_TRUE;
  }

  bool ServiceClient::GetHttpAuthRequired()
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    return axis2_svc_client_get_http_auth_required(m_pSvcClient, m_pEnv) == AXIS2_TRUE;
  }

  std::string ServiceClient::GetAuthType()
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axis2_char_t* szResult = axis2_svc_client_get_auth_type(m_pSvcClient, m_pEnv);
    STAFF_ASSERT(szResult, "Auth type is NULL");
    return szResult;
  }


  DataObject ServiceClient::Invoke(DataObject& rdoPayload)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    PrepareToSend(rdoPayload);

#ifdef _DEBUG
    LogDebug2() << "Sending: \n" << staff::ColorTextBlue
        << rdoPayload.ToString() << staff::ColorDefault;
#endif

    axiom_node_t* pAxiomResponseNode = axis2_svc_client_send_receive(m_pSvcClient, m_pEnv, rdoPayload);

    if (m_pEnv->error->status_code == AXIS2_ERROR_RESPONSE_TIMED_OUT)
    {
      STAFF_THROW(TimeoutException, "Axis2/C client send receive failed (Timeout). error: " + GetLastErrorStr())
    }

    STAFF_ASSERT(pAxiomResponseNode, "Axis2/C client send receive failed. error: " + GetLastErrorStr());

#ifdef _DEBUG
      LogDebug2() << "Received Response: \n" << staff::ColorTextBlue
          << DataObject(pAxiomResponseNode).ToString() << staff::ColorDefault;
#endif

    return pAxiomResponseNode;
  }

  void ServiceClient::Invoke(DataObject& rdoPayload, ICallback<const DataObject&>& rCallback)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    PrepareToSend(rdoPayload);

    // asynchronous call
    axis2_callback_t* pAxis2Callback = axis2_callback_create(m_pEnv);
    STAFF_ASSERT(pAxis2Callback, "Cannot create callback");

    // store Axis2/C callback
    rCallback.Set(pAxis2Callback, m_pEnv);

    axis2_callback_set_on_complete(pAxis2Callback, ServiceClientTools::CallbackOnComplete);
    axis2_callback_set_on_error(pAxis2Callback, ServiceClientTools::CallbackOnFault);
    axis2_callback_set_data(pAxis2Callback, new CallbackWrapper(&rCallback, false));

#ifdef _DEBUG
    LogDebug2() << "Sending: \n" << staff::ColorTextBlue
        << rdoPayload.ToString() << staff::ColorDefault;
#endif

    axis2_svc_client_send_receive_non_blocking(m_pSvcClient, m_pEnv, rdoPayload, pAxis2Callback);
  }

  void ServiceClient::Invoke(DataObject& rdoPayload, PICallback& rpCallback)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    PrepareToSend(rdoPayload);

    // asynchronous call
    axis2_callback_t* pAxis2Callback = axis2_callback_create(m_pEnv);
    STAFF_ASSERT(pAxis2Callback, "Cannot create callback");

    // store Axis2/C callback
    (rpCallback)->Set(pAxis2Callback, m_pEnv);

    axis2_callback_set_on_complete(pAxis2Callback, ServiceClientTools::CallbackOnComplete);
    axis2_callback_set_on_error(pAxis2Callback, ServiceClientTools::CallbackOnFault);
    axis2_callback_set_data(pAxis2Callback, new CallbackWrapper(rpCallback.release(), true));

#ifdef _DEBUG
    LogDebug2() << "Sending: \n" << staff::ColorTextBlue
        << rdoPayload.ToString() << staff::ColorDefault;
#endif

    axis2_svc_client_send_receive_non_blocking(m_pSvcClient, m_pEnv, rdoPayload, pAxis2Callback);
  }


  void ServiceClient::SendRobust(DataObject& rdoPayload)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    PrepareToSend(rdoPayload);

    axis2_status_t nResult = axis2_svc_client_send_robust(m_pSvcClient, m_pEnv, rdoPayload);
    STAFF_ASSERT(nResult == AXIS2_SUCCESS, "Axis2/C client send robust failed. error: " + GetLastErrorStr());

#ifdef _DEBUG
    LogDebug2() << "Sending: \n" << staff::ColorTextBlue
        << rdoPayload.ToString() << staff::ColorDefault;
#endif

    axis2_bool_t bIsFault = axis2_svc_client_get_last_response_has_fault(m_pSvcClient, m_pEnv);
    STAFF_ASSERT(bIsFault != AXIS2_TRUE, "Axis2/C client send robust fault received.");
  }

  void ServiceClient::Send(DataObject& rdoPayload)
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    PrepareToSend(rdoPayload);

#ifdef _DEBUG
    LogDebug2() << "Sending: \n" << staff::ColorTextBlue
        << rdoPayload.ToString() << staff::ColorDefault;
#endif

    axis2_svc_client_fire_and_forget(m_pSvcClient, m_pEnv, rdoPayload);
  }

  bool ServiceClient::GetLastResponseHasFault()
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    return axis2_svc_client_get_last_response_has_fault(m_pSvcClient, m_pEnv) == AXIS2_TRUE;
  }


  DataObject ServiceClient::GetLastResponse()
  {
    STAFF_ASSERT(m_pSvcClient, "Service client is not initialized");

    axiom_soap_envelope_t* pSoapEnv =
        axis2_svc_client_get_last_response_soap_envelope(m_pSvcClient, m_pEnv);
    if (!pSoapEnv)
    {
      return NULL;
    }

    return axiom_soap_envelope_get_base_node(pSoapEnv, m_pEnv);
  }


  std::string ServiceClient::GetLastErrorStr()
  {
    const char* szError = reinterpret_cast<const char*>(AXIS2_ERROR_GET_MESSAGE(m_pEnv->error));
    return szError ? szError : "UNKNOWN";
  }

  ServiceClient::operator axis2_svc_client_t*()
  {
    return m_pSvcClient;
  }

  void ServiceClient::PrepareToSend(DataObject& rdoPayload)
  {
    STAFF_ASSERT(m_pOptions, "Options is not initialized");
    rdoPayload.SetOwner(false);

    // adding session id header
    if (!m_pOptions->GetSessionId().empty())
    {
      axiom_node_t* pNodeSessionId = NULL;
      axiom_element_t* pElemSessionId = NULL;
      axiom_namespace_t* pHeaderNs = NULL;

      pHeaderNs = axiom_namespace_create(m_pEnv, "http://tempui.org/staff/sessionid", "sid");
      pElemSessionId = axiom_element_create(m_pEnv, NULL, "SessionId", pHeaderNs, &pNodeSessionId);
      axiom_element_set_text(pElemSessionId, m_pEnv, m_pOptions->GetSessionId().c_str(), pNodeSessionId);
      axis2_svc_client_add_header(m_pSvcClient, m_pEnv, pNodeSessionId);
    }

    // adding instance id header
    if (!m_pOptions->GetInstanceId().empty())
    {
      axiom_node_t* pNodeInstanceId = NULL;
      axiom_element_t* pElemInstanceId = NULL;
      axiom_namespace_t* pHeaderNs = NULL;

      pHeaderNs = axiom_namespace_create(m_pEnv, "http://tempui.org/staff/instanceid", "iid");
      pElemInstanceId = axiom_element_create(m_pEnv, NULL, "InstanceId", pHeaderNs, &pNodeInstanceId);
      axiom_element_set_text(pElemInstanceId, m_pEnv, m_pOptions->GetInstanceId().c_str(), pNodeInstanceId);
      axis2_svc_client_add_header(m_pSvcClient, m_pEnv, pNodeInstanceId);
    }

    if (!m_pOptions->GetDefaultNamespaceUri().empty() &&
        !m_pOptions->GetDefaultNamespacePrefix().empty())
    {
      axiom_namespace_t* pNamespace =
          axiom_namespace_create(m_pEnv, m_pOptions->GetDefaultNamespaceUri().c_str(),
                                 m_pOptions->GetDefaultNamespacePrefix().c_str());
      axiom_element_set_namespace(rdoPayload, m_pEnv, pNamespace, rdoPayload);
    }
  }
}
