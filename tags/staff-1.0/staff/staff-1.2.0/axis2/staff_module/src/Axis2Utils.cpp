#include <staff/common/DataObject.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include "Axis2Utils.h"

axiom_node_t* GetSoapOpNode(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv)
{
  AXIS2_ENV_CHECK(pEnv, NULL);

  axiom_soap_envelope_t* pSoapEnv = axis2_msg_ctx_get_soap_envelope(pMsgCtx, pEnv);
  if (pSoapEnv == NULL)
    return NULL;

  axiom_soap_body_t* pSoapBody = axiom_soap_envelope_get_body(pSoapEnv, pEnv);
  if (pSoapBody == NULL)
    return NULL;

  axiom_node_t* pBaseNode = axiom_soap_body_get_base_node(pSoapBody, pEnv);

  if(pBaseNode == NULL)
    return NULL;

  return axiom_node_get_first_element(pBaseNode, pEnv);
}

axiom_namespace_t* GetNs(axiom_node_t* pNode, const axutil_env_t* pEnv)
{
  AXIS2_ENV_CHECK(pEnv, NULL);

  if (axiom_node_get_node_type(pNode, pEnv) != AXIOM_ELEMENT)
    return NULL;

  axiom_element_t* pElement = (axiom_element_t*) axiom_node_get_data_element(pNode, pEnv);
  if (pElement == NULL)
    return NULL;

  return axiom_element_get_namespace(pElement, pEnv, pNode);
}

axis2_char_t* GetServiceUriFromMsgCtx(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv)
{
  AXIS2_ENV_CHECK(pEnv, NULL);

  axiom_node_t* pSoapOpNode = GetSoapOpNode(pMsgCtx, pEnv);
  if (pSoapOpNode == NULL)
    return NULL;

  axiom_namespace_t* pNamespace = GetNs(pSoapOpNode, pEnv);
  if (pNamespace == NULL)
    return NULL;

  return axiom_namespace_get_uri(pNamespace, pEnv);
}

bool ParseServiceName(const axis2_char_t* szServiceURI, const axutil_env_t* pEnv, std::string& sServiceName)
{
  axis2_char_t **url_tokens = axutil_parse_request_url_for_svc_and_op(pEnv, szServiceURI);
  if (url_tokens == NULL)
    return false;

  if (url_tokens[0] == NULL)
  {
    AXIS2_FREE(pEnv->allocator, url_tokens);
    return false;
  }

  sServiceName = url_tokens[0];
  AXIS2_FREE(pEnv->allocator, url_tokens[0]);
  AXIS2_FREE(pEnv->allocator, url_tokens);

  return true;
}

bool GetServiceName(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv, std::string& sServiceName)
{
  const axis2_char_t* szServiceUri = NULL;

  // HTTP-request header
  axis2_endpoint_ref_t* pEndPoint = axis2_msg_ctx_get_to(pMsgCtx, pEnv);
  if (pEndPoint != NULL)
    szServiceUri = axis2_endpoint_ref_get_address(pEndPoint, pEnv);

  if (szServiceUri == NULL || szServiceUri[0] == '\0')
  {
    // не смогли определить к какому сервису HTTP-запрос, попробуем по SOAP namesapce
    szServiceUri = GetServiceUriFromMsgCtx(pMsgCtx, pEnv);
    if (szServiceUri == NULL || szServiceUri[0] == '\0')
      return false; // не смогли определаить URI сервиса
  } else // в SOAP operation необходимо заменить namespace URI, 
         // чтобы сервис смог понять откуда пришел запрос
  {
    printf("HTTP URI: %s\n", szServiceUri);

    axiom_node_t* pSoapOpNode = GetSoapOpNode(pMsgCtx, pEnv);
    if (pSoapOpNode == NULL)
    {
      printf("!GetSoapOpNode\n");
      return false;
    }

    staff::CDataObject do1(pSoapOpNode);
    printf("%s\n\n", do1.ToString().c_str()      );


    axiom_namespace_t* pNamespace = GetNs(pSoapOpNode, pEnv);
    if (pNamespace == NULL)
      return false;

    const axis2_char_t* szSoapUri = axiom_namespace_get_uri(pNamespace, pEnv);

    printf("SOAP URI: %s\n", szSoapUri);

    // если неймспейсы не совпадают
    if (strcmp(szSoapUri, szServiceUri) != 0)
    { // изменить неймспейс
      axutil_string_t* psServiceUri = axutil_string_create(pEnv, szServiceUri);
      axiom_namespace_set_uri_str(pNamespace, pEnv, psServiceUri);
    }
  }

  if(!ParseServiceName(szServiceUri, pEnv, sServiceName))
  {
    printf("can't get service name form uri: %s\n", szServiceUri);
    return false;
  }

  return true;
}
