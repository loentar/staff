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

#include <axiom.h>
#include <axis2_client.h>
#include <axis2_defines.h>
#include <axutil_env.h>
#include <axiom_soap_const.h>
#include <string>
#include <rise/common/console.h>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <staff/common/Runtime.h>
#include <staff/common/QName.h>
#include <staff/common/Namespace.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObject.h>
#include "Axis2Client.h"

#ifdef WIN32
#pragma warning ( disable: 4996 )
#endif

namespace staff
{

  class CAxis2Client::CAxis2ClientImpl
  {
  public:
    CAxis2ClientImpl():
      m_pAxis2ServiceClient(NULL),
      m_pAxis2Options(NULL),
      m_bInit(false)
    {
      m_pAxutilEnv = CRuntime::Inst().GetAxis2Env("staff_client");
    }

    ~CAxis2ClientImpl()
    {
      if(m_pAxutilEnv != NULL)
      {
        CRuntime::Inst().FreeAxis2Env("staff_client");
        m_pAxutilEnv = NULL;
      }
    }

    void Init()
    {
      axis2_endpoint_ref_t* pEndpointRef = axis2_endpoint_ref_create(m_pAxutilEnv, m_sServiceUri.c_str());

      // Setup options
      m_pAxis2Options = axis2_options_create(m_pAxutilEnv);
      axis2_options_set_to(m_pAxis2Options, m_pAxutilEnv, pEndpointRef);
      axis2_options_set_soap_version(m_pAxis2Options, m_pAxutilEnv, AXIOM_SOAP11);

      rise::LogDebug() << "Service URI: " << m_sServiceUri;

      // Create service client
      std::string sAxis2Home = CRuntime::Inst().GetAxis2Home();

      m_pAxis2ServiceClient = axis2_svc_client_create(m_pAxutilEnv, sAxis2Home.c_str());
      if (!m_pAxis2ServiceClient)
      {
        rise::COStringStream ssError;
        ssError << "Axis2 svc_client_create failed, error: " << m_pAxutilEnv->error->error_number <<
          ", " << AXIS2_ERROR_GET_MESSAGE(m_pAxutilEnv->error);
        RISE_THROWS(rise::CLogicNoInitException, ssError.str());
      }

      // Set service client options
      axis2_svc_client_set_options(m_pAxis2ServiceClient, m_pAxutilEnv, m_pAxis2Options);

      // Engage addressing module
      axis2_svc_client_engage_module(m_pAxis2ServiceClient, m_pAxutilEnv, AXIS2_MODULE_ADDRESSING);

      m_bInit = true;
    }

  public:
    std::string           m_sServiceUri;
    std::string           m_sTargetNamespace;
    std::string           m_sSessionId;
    std::string           m_sInstanceId;
    axis2_svc_client_t*   m_pAxis2ServiceClient;
    axutil_env_t*         m_pAxutilEnv;
    axis2_options_t*      m_pAxis2Options;
    bool                  m_bInit;
  };

  CAxis2Client::CAxis2Client()
  {
    m_pImpl = new CAxis2ClientImpl;
  }

  CAxis2Client::~CAxis2Client()
  {
    Deinit();

    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  void CAxis2Client::Init(const std::string& sServiceUri, const std::string& sSessionId)
  {
    Deinit();
    m_pImpl->m_sServiceUri = sServiceUri;
    m_pImpl->m_sSessionId = sSessionId;
  }

  void CAxis2Client::Deinit()
  {
    if (m_pImpl->m_pAxis2ServiceClient != NULL)
    {
      axis2_svc_client_free(m_pImpl->m_pAxis2ServiceClient, m_pImpl->m_pAxutilEnv);
      m_pImpl->m_pAxis2ServiceClient = NULL;
    }

    m_pImpl->m_bInit = false;
  }

  void CAxis2Client::Invoke(COperation& rOperation)
  {
    if (!m_pImpl->m_bInit)
    {
      m_pImpl->Init();
    }

    CDataObject& rdoRequest = rOperation.Request();
    rdoRequest.SetOwner(false);

    const std::string& sTargetNamespace = m_pImpl->m_sTargetNamespace.size() != 0 ? m_pImpl->m_sTargetNamespace : m_pImpl->m_sServiceUri;

    rdoRequest.DeclareDefaultNamespace(sTargetNamespace);

    rise::LogDebug1() << "targetNamespace: " << sTargetNamespace;

    // adding session id header
    if (m_pImpl->m_sSessionId.size() != 0)
    {
      axiom_node_t* pNodeSessionId = NULL;
      axiom_element_t* pElemSessionId = NULL;
      axiom_namespace_t* pHeaderNs = NULL;

      pHeaderNs = axiom_namespace_create(m_pImpl->m_pAxutilEnv, "http://tempui.org/staff/sessionid", "sid");
      pElemSessionId = axiom_element_create(m_pImpl->m_pAxutilEnv, NULL, "SessionId", pHeaderNs, &pNodeSessionId);
      axiom_element_set_text(pElemSessionId, m_pImpl->m_pAxutilEnv, m_pImpl->m_sSessionId.c_str(), pNodeSessionId);
      axis2_svc_client_add_header(m_pImpl->m_pAxis2ServiceClient, m_pImpl->m_pAxutilEnv, pNodeSessionId);
    }

    // adding instance id header
    if (m_pImpl->m_sInstanceId.size() != 0)
    {
      axiom_node_t* pNodeInstanceId = NULL;
      axiom_element_t* pElemInstanceId = NULL;
      axiom_namespace_t* pHeaderNs = NULL;

      pHeaderNs = axiom_namespace_create(m_pImpl->m_pAxutilEnv, "http://tempui.org/staff/instanceid", "iid");
      pElemInstanceId = axiom_element_create(m_pImpl->m_pAxutilEnv, NULL, "InstanceId", pHeaderNs, &pNodeInstanceId);
      axiom_element_set_text(pElemInstanceId, m_pImpl->m_pAxutilEnv, m_pImpl->m_sInstanceId.c_str(), pNodeInstanceId);
      axis2_svc_client_add_header(m_pImpl->m_pAxis2ServiceClient, m_pImpl->m_pAxutilEnv, pNodeInstanceId);
    }

    // setting SOAP Action
    {
      axutil_string_t* psSoapAction = axutil_string_create(m_pImpl->m_pAxutilEnv, rOperation.GetSoapAction().c_str());
      axis2_options_set_soap_action(m_pImpl->m_pAxis2Options, m_pImpl->m_pAxutilEnv, psSoapAction);
      axutil_string_free(psSoapAction, m_pImpl->m_pAxutilEnv);
      rise::LogDebug1() << "SoapAction: \"" << rOperation.GetSoapAction() << "\"";
    }

#if defined _DEBUG || defined DEBUG
    rise::LogDebug2() << "Sending Request: \n" << rise::ColorInkBlue << rdoRequest.ToString() << rise::ColorDefault;
#endif

    // Send request
    axiom_node_t* pAxiomResponseNode = axis2_svc_client_send_receive(m_pImpl->m_pAxis2ServiceClient,
                                                                     m_pImpl->m_pAxutilEnv, rdoRequest);

    if (pAxiomResponseNode == NULL)
    {
      rise::COStringStream ssMessage;
      ssMessage << "Axis2 client_send_receive failed, error #" << m_pImpl->m_pAxutilEnv->error->error_number <<
          ": " << AXIS2_ERROR_GET_MESSAGE(m_pImpl->m_pAxutilEnv->error);
      RISE_THROWS(rise::CFileIOException, ssMessage.str());
    }

    // Set return value - now need to detach the node from the Axiom pDocument for clean-up
    axiom_document_t* pDocument = axiom_node_get_document(pAxiomResponseNode, m_pImpl->m_pAxutilEnv);
    if (pDocument != NULL)
    {
      axiom_document_build_all(pDocument, m_pImpl->m_pAxutilEnv);
    }

    axiom_node_detach(pAxiomResponseNode, m_pImpl->m_pAxutilEnv);

    CDataObject tdoResponse(pAxiomResponseNode);
    rOperation.SetResponse(tdoResponse);

#if defined _DEBUG || defined DEBUG
    rise::LogDebug2() << "Received Response: \n" << rise::ColorInkBlue << tdoResponse.ToString() << rise::ColorDefault;
#endif
  }

  void CAxis2Client::SetTargetNamespace(const std::string& sTargetNamespace)
  {
    m_pImpl->m_sTargetNamespace = sTargetNamespace;
  }

  void CAxis2Client::SetInstanceId(const std::string& sIntanceId)
  {
    m_pImpl->m_sInstanceId = sIntanceId;
  }

}
