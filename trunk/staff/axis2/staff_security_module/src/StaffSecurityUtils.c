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

#include <axiom_soap_envelope.h>
#include <axiom_soap_body.h>
#include <axiom_soap_header.h>
#include <axiom_soap_header_block.h>
#include <axutil_param_container.h>
#include <axis2_module_desc.h>
#include "StaffSecurityUtils.h"

axis2_status_t GetServiceOperationPath( axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv,
                                        axis2_char_t** pszServiceOperationPath, axis2_char_t** pszServiceName )
{
  const axis2_char_t* szServiceUri = NULL;
  axis2_endpoint_ref_t* pEndPoint = NULL;
  axiom_node_t* pNodeOperation = NULL;
  axiom_element_t* pElementOperation = NULL;

  axiom_soap_envelope_t* pSoapEnv = NULL;
  axiom_soap_body_t* pSoapBody = NULL;
  axiom_node_t* pBaseNode = NULL;

  const axis2_char_t* szServiceName = NULL;
  const axis2_char_t* szServiceOperation = NULL;
  axis2_char_t* szServiceNameParam = NULL;

  AXIS2_UTILS_CHECK(pMsgCtx);
  AXIS2_UTILS_CHECK(pEnv);
  AXIS2_UTILS_CHECK(pszServiceOperationPath);

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

  if (szServiceUri == NULL || szServiceUri[0] == '\0')
  {
    axiom_namespace_t* pNamespace = NULL;
    pNamespace = axiom_element_get_namespace(pElementOperation, pEnv, pNodeOperation);
    AXIS2_UTILS_CHECK(pNamespace);

    szServiceUri = axiom_namespace_get_uri(pNamespace, pEnv);
  }

  AXIS2_UTILS_CHECK(szServiceUri);

  dprintf("Service URI: %s\n", szServiceUri);

  AXIS2_UTILS_CHECK(axiom_node_get_node_type(pNodeOperation, pEnv) == AXIOM_ELEMENT);

  szServiceOperation = axiom_element_get_localname(pElementOperation, pEnv);
  AXIS2_UTILS_CHECK(szServiceOperation);

  szServiceName = strstr(szServiceUri, "/axis2/services/");

  if (szServiceName == NULL)
  {
    // target namespace does not contain "axis2/services"
    // trying to guess service name
    szServiceName = strrchr(szServiceUri, '/');
    if (szServiceName == NULL)
    {
      szServiceName = szServiceUri;
    }
    else
    {
      ++szServiceName;
    }

    szServiceNameParam = axutil_strdup(pEnv, szServiceName);
  } else
  {
    const char* szServiceNameEnd = NULL;
    szServiceName += 16;

    // cut off REST request params from service name
    szServiceNameEnd = strchr(szServiceName, '/');
    if (szServiceNameEnd)
    {
      unsigned uSize = (unsigned)(szServiceNameEnd - szServiceName);
      szServiceNameParam = AXIS2_MALLOC(pEnv->allocator, uSize + 1);
      AXIS2_UTILS_CHECK(szServiceNameParam);
      strncpy(szServiceNameParam, szServiceName, uSize);
      szServiceNameParam[uSize] = '\0';
    }
    else
    {
      szServiceNameParam = axutil_strdup(pEnv, szServiceName);
    }
  }

  *pszServiceName = szServiceNameParam;
  *pszServiceOperationPath = axutil_strcat(pEnv, "component.", szServiceNameParam, ".", szServiceOperation, NULL);

  return AXIS2_SUCCESS;
}

void GetSessionAndInstanceId( axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv,
                              const axis2_char_t** pszSessionId, const axis2_char_t** pszInstanceId )
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
                if (szHeaderLocalName != NULL)
                {
                  if (axutil_strcmp(szHeaderLocalName, "SessionId") == 0)
                  {
                    *pszSessionId = axiom_element_get_text(pSoapHeaderBlockElement, pEnv, pHeaderBlockNode);
                    if (*pszInstanceId != NULL)
                    {
                      break;
                    }
                  }
                  else
                  if (axutil_strcmp(szHeaderLocalName, "InstanceId") == 0)
                  {
                    *pszInstanceId = axiom_element_get_text(pSoapHeaderBlockElement, pEnv, pHeaderBlockNode);
                    if (*pszSessionId != NULL)
                    {
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      } // for (axutil_hash_index_t* pSoapHeadersHashIndex =...
    } // if(pHeaderHash != NULL)
  } // if(pSoapHeader != NULL)
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
