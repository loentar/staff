#include <axiom.h>
#include <axis2_client.h>
#include <axis2_defines.h>
#include <axutil_env.h>
#include <axiom_soap_const.h>

#include <string>

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
    CAxis2ClientImpl(CAxis2Client* pSelf):
      m_pAxis2ServiceClient(NULL),
      m_pAxis2Options(NULL),
      m_bInit(false),
      m_pSelf(pSelf)
    {
      m_pAxutilEnv = CRuntime::Inst().GetAxis2Env("staff_client");
    }

    ~CAxis2ClientImpl()
    {
      m_pSelf->Deinit();

      if(m_pAxutilEnv != NULL)
      {
        CRuntime::Inst().FreeAxis2Env("staff_client");
        m_pAxutilEnv = NULL;
      }
    }
    

  public:
    std::string           m_sServiceUri;
    std::string           m_sSessionId;
    axis2_svc_client_t*   m_pAxis2ServiceClient;
    axutil_env_t*         m_pAxutilEnv;
    axis2_options_t*      m_pAxis2Options;
    bool                  m_bInit;
    CAxis2Client*         m_pSelf;
  }; 
  
  CAxis2Client::CAxis2Client()
  {
    m_pImpl = new CAxis2ClientImpl(this);
  }
  
  CAxis2Client::~CAxis2Client()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  void CAxis2Client::Init(const std::string& sServiceUri, const std::string& sSessionId)
  {
    m_pImpl->m_sServiceUri = sServiceUri;
    m_pImpl->m_sSessionId = sSessionId;

    axis2_endpoint_ref_t* pEndpointRef = axis2_endpoint_ref_create(m_pImpl->m_pAxutilEnv, m_pImpl->m_sServiceUri.c_str());

    // Setup options
    m_pImpl->m_pAxis2Options = axis2_options_create(m_pImpl->m_pAxutilEnv);
    axis2_options_set_to(m_pImpl->m_pAxis2Options, m_pImpl->m_pAxutilEnv, pEndpointRef);
    axis2_options_set_soap_version(m_pImpl->m_pAxis2Options, m_pImpl->m_pAxutilEnv, AXIOM_SOAP11);

    rise::LogDebug() << "Service URI: " << m_pImpl->m_sServiceUri;

    // Create service client
    std::string sAxis2Home = CRuntime::Inst().GetAxis2Home();

    m_pImpl->m_pAxis2ServiceClient = axis2_svc_client_create(m_pImpl->m_pAxutilEnv, sAxis2Home.c_str());
    if (!m_pImpl->m_pAxis2ServiceClient)
    {
      rise::COStringStream ssError;
      ssError << "Axis2 svc_client_create failed, error: " << m_pImpl->m_pAxutilEnv->error->error_number <<
        ", " << AXIS2_ERROR_GET_MESSAGE(m_pImpl->m_pAxutilEnv->error);
      RISE_THROWS(rise::CLogicNoInitException, ssError.str());
    }

    // Set service client options
    axis2_svc_client_set_options(m_pImpl->m_pAxis2ServiceClient, m_pImpl->m_pAxutilEnv, m_pImpl->m_pAxis2Options);    

    // Engage addressing module
    axis2_svc_client_engage_module(m_pImpl->m_pAxis2ServiceClient, m_pImpl->m_pAxutilEnv, AXIS2_MODULE_ADDRESSING);

    m_pImpl->m_bInit = true;
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
    CDataObject& rdoRequest = rOperation.Request();
    rdoRequest.SetOwner(false);
    
    if (rdoRequest.GetNamespaceUri() == "")
    {
      CNamespace tTargetNs(m_pImpl->m_sServiceUri, "staff");
      rdoRequest.SetNamespace(tTargetNs);
    }

    // adding session id header
    {
      axiom_node_t* pSidNode = NULL;
      axiom_element_t* pSidElement = NULL;

      pSidElement = axiom_element_create(m_pImpl->m_pAxutilEnv, NULL, "SessionId", NULL, &pSidNode);
      axiom_element_set_text(pSidElement, m_pImpl->m_pAxutilEnv, m_pImpl->m_sSessionId.c_str(), pSidNode);
      axis2_svc_client_add_header(m_pImpl->m_pAxis2ServiceClient, m_pImpl->m_pAxutilEnv, pSidNode);
    }

    axutil_string_t* psSoapAction = axutil_string_create(m_pImpl->m_pAxutilEnv, rOperation.GetSoapAction().c_str());
    axis2_options_set_soap_action(m_pImpl->m_pAxis2Options, m_pImpl->m_pAxutilEnv, psSoapAction);
    axutil_string_free(psSoapAction, m_pImpl->m_pAxutilEnv);
    rise::LogDebug1() << "SoapAction: " << rOperation.GetSoapAction();

#if defined _DEBUG || defined DEBUG
    rise::LogDebug2() << "Sending Request: \n" << rdoRequest.ToString();
#endif

    // Send request
    axiom_node_t* pAxiomResultNode = axis2_svc_client_send_receive(m_pImpl->m_pAxis2ServiceClient, m_pImpl->m_pAxutilEnv, rdoRequest);
    
    if(pAxiomResultNode == NULL)
    {
      rise::COStringStream ssMessage;
      ssMessage << "Axis2 client_send_received failed, error: " << m_pImpl->m_pAxutilEnv->error->error_number <<
        ", " << AXIS2_ERROR_GET_MESSAGE(m_pImpl->m_pAxutilEnv->error);
      RISE_THROWS(rise::CFileIOException, ssMessage.str());
    }

    // Set return value - now need to detach the node from the Axiom pDocument for clean-up
    axiom_document_t* pDocument = axiom_node_get_document(pAxiomResultNode, m_pImpl->m_pAxutilEnv);
    if (pDocument != NULL)
    {
      axiom_document_build_all(pDocument, m_pImpl->m_pAxutilEnv);
    }

    axiom_node_detach(pAxiomResultNode, m_pImpl->m_pAxutilEnv);

    CDataObject& rdoResult = rOperation.Result();
    rdoResult.Attach(pAxiomResultNode, true);
    rdoResult.DetachNode();

#if defined _DEBUG || defined DEBUG
      rise::LogDebug2() << "Received Result: \n" << rdoResult.ToString(); 
#endif
  }

  const std::string& CAxis2Client::GetServiceURI() const
  {
    return m_pImpl->m_sServiceUri;
  }

  bool CAxis2Client::IsInit() const
  {
    return m_pImpl->m_bInit;
  }

}
