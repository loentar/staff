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


axis2_status_t GetOperationName(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv,
                                const axis2_char_t** pszOperationName)
{
  axis2_op_t* pOperation = NULL;
  const axutil_qname_t* pOperationQName = NULL;
  axiom_soap_envelope_t* pSoapEnv = NULL;
  axiom_soap_body_t* pSoapBody = NULL;
  axiom_node_t* pNode = NULL;
  axiom_element_t* pElemOperation = NULL;


  /* try to get operation name from message context: for REST */
  pOperation = axis2_msg_ctx_get_op(pMsgCtx, pEnv);
  if (pOperation)
  {
    pOperationQName = axis2_op_get_qname(pOperation, pEnv);
    if (pOperationQName)
    {
      *pszOperationName = axutil_qname_get_localpart(pOperationQName, pEnv);
      return AXIS2_SUCCESS;
    }
  }

  /* try to get operation name from soap request */
  pSoapEnv = axis2_msg_ctx_get_soap_envelope(pMsgCtx, pEnv);
  AXIS2_UTILS_ASSERT(pEnv, pSoapEnv, "Failed to get soap envelope");

  pSoapBody = axiom_soap_envelope_get_body(pSoapEnv, pEnv);
  AXIS2_UTILS_ASSERT(pEnv, pSoapBody, "Failed to get soap body");

  pNode = axiom_soap_body_get_base_node(pSoapBody, pEnv);
  AXIS2_UTILS_ASSERT(pEnv, pNode, "Failed to get soap base node");

  pNode = axiom_node_get_first_element(pNode, pEnv);
  AXIS2_UTILS_ASSERT(pEnv, pNode, "Failed to get soap first element");
  AXIS2_UTILS_ASSERT(pEnv, axiom_node_get_node_type(pNode, pEnv) == AXIOM_ELEMENT,
                     "Invalid node type");

  pElemOperation = (axiom_element_t*)axiom_node_get_data_element(pNode, pEnv);
  AXIS2_UTILS_ASSERT(pEnv, pElemOperation, "Failed to get operation element");

  *pszOperationName = axiom_element_get_localname(pElemOperation, pEnv);
  AXIS2_UTILS_ASSERT(pEnv, *pszOperationName, "Failed to get operation name");

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
      } /* for (axutil_hash_index_t* pSoapHeadersHashIndex =... */
    } /* if(pHeaderHash != NULL) */
  } /* if(pSoapHeader != NULL) */
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
