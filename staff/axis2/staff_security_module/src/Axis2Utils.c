#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include <axiom_soap_header.h>
#include <axiom_soap_header_block.h>
#include <axutil_param_container.h>
#include <axis2_module_desc.h>
#include "Axis2Utils.h"

axiom_node_t* GetSoapOpNode(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv)
{
  axiom_soap_envelope_t* pSoapEnv = NULL;
  axiom_soap_body_t* pSoapBody = NULL;
  axiom_node_t* pBaseNode = NULL;

  pSoapEnv = axis2_msg_ctx_get_soap_envelope(pMsgCtx, pEnv);
  if (pSoapEnv == NULL)
    return NULL;

  pSoapBody = axiom_soap_envelope_get_body(pSoapEnv, pEnv);
  if (pSoapBody == NULL)
    return NULL;

  pBaseNode = axiom_soap_body_get_base_node(pSoapBody, pEnv);
  if(pBaseNode == NULL)
    return NULL;

  return axiom_node_get_first_element(pBaseNode, pEnv);
}

axis2_status_t GetServiceNameAndOperation( axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv, const axis2_char_t** pszServiceName, const axis2_char_t** pszServiceOperation )
{
  const axis2_char_t* szServiceUri = NULL;
  axis2_endpoint_ref_t* pEndPoint = NULL;
  axiom_node_t* pNodeOperation = NULL;
  axiom_element_t* pElementOperation = NULL;
  axiom_namespace_t* pNamespace = NULL;

  axiom_soap_envelope_t* pSoapEnv = NULL;
  axiom_soap_body_t* pSoapBody = NULL;
  axiom_node_t* pBaseNode = NULL;


  AXIS2_UTILS_CHECK(pMsgCtx);
  AXIS2_UTILS_CHECK(pEnv);
  AXIS2_UTILS_CHECK(pszServiceName);
  AXIS2_UTILS_CHECK(pszServiceOperation);

  // HTTP-request header
  pEndPoint = axis2_msg_ctx_get_to(pMsgCtx, pEnv);
  if (pEndPoint != NULL)
  {
    szServiceUri = axis2_endpoint_ref_get_address(pEndPoint, pEnv);
  }

  pSoapEnv = axis2_msg_ctx_get_soap_envelope(pMsgCtx, pEnv);
  AXIS2_UTILS_CHECK(pSoapEnv);

  pSoapBody = axiom_soap_envelope_get_body(pSoapEnv, pEnv);
  AXIS2_UTILS_CHECK(pSoapBody);

  pBaseNode = axiom_soap_body_get_base_node(pSoapBody, pEnv);
  AXIS2_UTILS_CHECK(pBaseNode);

  pNodeOperation = axiom_node_get_first_element(pBaseNode, pEnv);
  AXIS2_UTILS_CHECK(pNodeOperation);

  pElementOperation = (axiom_element_t*)axiom_node_get_data_element(pNodeOperation, pEnv);
  AXIS2_UTILS_CHECK(pElementOperation);

  pNamespace = axiom_element_get_namespace(pElementOperation, pEnv, pNodeOperation);
  AXIS2_UTILS_CHECK(pNamespace);

  if (szServiceUri == NULL || szServiceUri[0] == '\0')
  {
    szServiceUri = axiom_namespace_get_uri(pNamespace, pEnv);
  }

  AXIS2_UTILS_CHECK(szServiceUri);

  printf("Service URI: %s\n", szServiceUri);

  AXIS2_UTILS_CHECK(axiom_node_get_node_type(pNodeOperation, pEnv) == AXIOM_ELEMENT);

  *pszServiceOperation = axiom_element_get_localname(pElementOperation, pEnv);
  AXIS2_UTILS_CHECK(*pszServiceOperation);

  *pszServiceName = strrchr(szServiceUri, '/');
  if(*pszServiceName == NULL)
  {
    *pszServiceName = szServiceUri;
  } else
    ++*pszServiceName;

  return AXIS2_SUCCESS;
}

axis2_status_t GetSessionId( axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv, const axis2_char_t** pszSessionId )
{
  axiom_soap_envelope_t* pSoapEnv = axis2_msg_ctx_get_soap_envelope(pMsgCtx, pEnv);
  axiom_node_t* pHeaderBlockNode = NULL;  
  axiom_soap_header_block_t* pSoapHeaderBlock = NULL;

  axiom_soap_header_t* pSoapHeader = axiom_soap_envelope_get_header(pSoapEnv, pEnv);
  if(pSoapHeader != NULL)
  {
    axiom_element_t* pSoapHeaderBlockElement = NULL;
    axutil_hash_t* pHeaderHash = axiom_soap_header_get_all_header_blocks(pSoapHeader, pEnv);

    if(pHeaderHash != NULL)
    {
      axutil_hash_index_t* pSoapHeadersHashIndex = NULL;
      for (pSoapHeadersHashIndex = axutil_hash_first(pHeaderHash, pEnv); 
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
                if (szHeaderLocalName != NULL && axutil_strcmp(szHeaderLocalName, "SID") == 0)
                {
                  *pszSessionId = axiom_element_get_text(pSoapHeaderBlockElement, pEnv, pHeaderBlockNode);
                  return AXIS2_SUCCESS;
                }
              }
            }
          }
        }
      } // for (axutil_hash_index_t* pSoapHeadersHashIndex =...
    } // if(pHeaderHash != NULL)
  } // if(pSoapHeader != NULL)
  return AXIS2_FAILURE;
}


const axis2_char_t* GetParamValue(const axis2_char_t* szParam, axis2_module_desc_t* pModuleDesc, const axutil_env_t* pEnv)
{
  axutil_param_t* pParam = NULL;

  pParam = axis2_module_desc_get_param(pModuleDesc, pEnv, szParam);
  if (!pParam)
  {
    printf("param %s is not found\n", szParam);
    return NULL;
  }

  return (const axis2_char_t*)axutil_param_get_value(pParam, pEnv);
}

const char* GetBaseFile( const char* szFilePath )
{
  const char* szFile = strrchr(szFilePath, 
#ifdef WIN32
    '\\'
#else
    '/'
#endif
    );
  
  if (szFile)
  {
    return szFile + 1;
  }
  else
  {
    return szFilePath;
  }
}
