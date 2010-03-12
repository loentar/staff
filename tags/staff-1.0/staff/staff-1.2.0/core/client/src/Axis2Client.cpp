#include <axiom.h>
#include <axis2_client.h>
#include <axis2_defines.h>
#include <axutil_env.h>
#include <axiom_soap_const.h>

#include <rise/common/Log.h>
#include <rise/common/StreamBuffer.h>
#include <rise/string/String.h>

#include <staff/common/QName.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObject.h>
#include <staff/common/DataObjectHelper.h>

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
      m_pAxutilEnv(axutil_env_create_all("staff_axis2_client.log", AXIS2_LOG_LEVEL_TRACE)),
      m_pAxis2Options(NULL),
      m_bInit(false)
    {
    }

    virtual ~CAxis2ClientImpl()
    {
      Deinit();
    }

    virtual const rise::CString& GetServiceURI() const
    {
      return m_sServiceURI;
    }

    void Init(const rise::CString& sServiceName, const rise::CString& sHostName, const rise::CString& sHostPort )
    {
//      axutil_string_t* pstSoapAction = axutil_string_create(m_pAxutilEnv, sServiceName.c_str());
      m_sServiceURI = "http://" + sHostName + ":" + sHostPort + "/axis2/services/" + sServiceName;

      axis2_endpoint_ref_t* pEndpointRef = axis2_endpoint_ref_create(m_pAxutilEnv, m_sServiceURI.c_str());

      /* Setup m_pAxis2Options */
      m_pAxis2Options = axis2_options_create(m_pAxutilEnv);
      axis2_options_set_to(m_pAxis2Options, m_pAxutilEnv, pEndpointRef);
      axis2_options_set_soap_version(m_pAxis2Options, m_pAxutilEnv, AXIOM_SOAP11);
      // service name!!!

      rise::LogDebug() << "WS Endpoint szAddress: " << m_sServiceURI;

      /* Create service client */
      axis2_char_t* szAxis2Home = AXIS2_GETENV("AXIS2C_HOME");

      RISE_ASSERTES(szAxis2Home != NULL, rise::CLogicNoInitException, "AXIS2C_HOME not set");

      m_pAxis2ServiceClient = axis2_svc_client_create(m_pAxutilEnv, szAxis2Home);
      if (!m_pAxis2ServiceClient)
      {
        rise::COStringStream msg;
        msg << "Axis2 svc_client_create failed, error: " << m_pAxutilEnv->error->error_number <<
          ", " << AXIS2_ERROR_GET_MESSAGE(m_pAxutilEnv->error);
        RISE_THROWS(rise::CLogicNoInitException, msg.str());
      }

      /* Set service client m_pAxis2Options */
      axis2_svc_client_set_options(m_pAxis2ServiceClient, m_pAxutilEnv, m_pAxis2Options);    

      /* Engage addressing module */
      axis2_svc_client_engage_module(m_pAxis2ServiceClient, m_pAxutilEnv, AXIS2_MODULE_ADDRESSING);

      m_bInit = true;
    }

    void Deinit()
    {
      if (m_pAxis2ServiceClient)
      {
        axis2_svc_client_free(m_pAxis2ServiceClient, m_pAxutilEnv);
        m_pAxis2ServiceClient = NULL;
      }

      if(m_pAxutilEnv)
      {
        axutil_env_free(m_pAxutilEnv);
        m_pAxutilEnv = NULL;
      }

      m_bInit = false;
    }

    virtual void Invoke(COperation& rOperation)
    {
      CDataObject& rdoRequest = rOperation.Request();
      rdoRequest.SetOwner(false);
      
      if (rdoRequest.GetNamespaceUri() == "")
      {
        CQName tTargetQName(rdoRequest.GetLocalName(), m_sServiceURI, "staff");
        rdoRequest.SetQName(tTargetQName);
      }

      // Get the AXIOM node representing the SOAP Body 
      {
        axiom_node_t* pSidNode = NULL;
        axiom_element_t* pSidElement = NULL;

        pSidElement = axiom_element_create(m_pAxutilEnv, NULL, "SessionId", NULL, &pSidNode);
        axiom_element_set_text(pSidElement, m_pAxutilEnv, m_sSessionId.c_str(), pSidNode);
        axis2_svc_client_add_header(m_pAxis2ServiceClient, m_pAxutilEnv, pSidNode);
      }

#if defined _DEBUG || defined DEBUG
      rise::LogDebug2() << "Sending Request: \n" << rdoRequest.ToString();
#endif

      /* Send request */
      axiom_node_t* pAxiomResultNode = axis2_svc_client_send_receive(m_pAxis2ServiceClient, m_pAxutilEnv, rdoRequest);
      if(pAxiomResultNode == NULL)
      {
        rise::COStringStream ssMessage;
        ssMessage << "Axis2 client_send_received failed, error: " << m_pAxutilEnv->error->error_number <<
          ", " << AXIS2_ERROR_GET_MESSAGE(m_pAxutilEnv->error);
        RISE_THROWS(rise::CFileIOException, ssMessage.str());
      }

      // Set return value - now need to detach the node from the Axiom pDocument                     
      // for clean-up
      axiom_document_t* pDocument = axiom_node_get_document(pAxiomResultNode, m_pAxutilEnv);
      if (pDocument)
        axiom_document_build_all (pDocument, m_pAxutilEnv);

//       axiom_node_detach (pAxiomResultNode, m_pAxutilEnv);

      CDataObject& rdoResult = rOperation.Result();
      rdoResult.Attach(pAxiomResultNode, true);
      rdoResult.DetachNode();

#if defined _DEBUG || defined DEBUG
      rise::LogDebug2() << "Received Result: \n" << rdoResult.ToString(); 
#endif
    }

    bool IsInit() const
    {
      return m_bInit;
    }

    void SetSessionId(const rise::CString& sSessionId)
    {
      m_sSessionId = sSessionId;
    }

  private:
    rise::CString         m_sServiceURI;
    rise::CString         m_sSessionId;
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
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  void CAxis2Client::Deinit()
  {
    m_pImpl->Deinit();
  }

  void CAxis2Client::Init(const rise::CString& sServiceName, const rise::CString& sHostName, const rise::CString& sHostPort /*= "9090"*/ )
  {
    m_pImpl->Init(sServiceName, sHostName, sHostPort);
  }

  const rise::CString& CAxis2Client::GetServiceURI() const
  {
    return m_pImpl->GetServiceURI();
  }

  void CAxis2Client::Invoke( COperation& rOperation )
  {
    m_pImpl->Invoke(rOperation);
  }

  bool CAxis2Client::IsInit() const
  {
    return m_pImpl->IsInit();
  }

  void CAxis2Client::SetSessionId( const rise::CString& sSessionId )
  {
    m_pImpl->SetSessionId(sSessionId);
  }

}
