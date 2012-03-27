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

#include <axis2_msg_ctx.h>
#include <axis2_http_header.h>
#include <axis2_http_transport.h>
#include <rise/common/ExceptionTemplate.h>
#include "MessageContext.h"

namespace staff
{
  MessageContext::MessageContext(const axutil_env_t* pEnv, axis2_msg_ctx_t* pMsgCtx):
    m_pEnv(pEnv), m_pMsgCtx(pMsgCtx)
  {
  }

  MessageContext::operator axis2_msg_ctx_t*()
  {
    return m_pMsgCtx;
  }


  std::string MessageContext::GetFaultToAddress() const
  {
    axis2_msg_info_headers_t* pHeaders = axis2_msg_ctx_get_msg_info_headers(m_pMsgCtx, m_pEnv);
    RISE_ASSERTS(pHeaders, "Can't get Headers");
    axis2_endpoint_ref_t* pEndpoint = axis2_msg_info_headers_get_fault_to(pHeaders, m_pEnv);
    RISE_ASSERTS(pEndpoint, "Can't get endpoint");
    const axis2_char_t* szAddr = axis2_endpoint_ref_get_address(pEndpoint, m_pEnv);
    RISE_ASSERTS(szAddr, "Can't get address");
    return szAddr;
  }

  std::string MessageContext::GetFromAddress() const
  {
    axis2_msg_info_headers_t* pHeaders = axis2_msg_ctx_get_msg_info_headers(m_pMsgCtx, m_pEnv);
    RISE_ASSERTS(pHeaders, "Can't get Headers");
    axis2_endpoint_ref_t* pEndpoint = axis2_msg_info_headers_get_from(pHeaders, m_pEnv);
    RISE_ASSERTS(pEndpoint, "Can't get endpoint");
    const axis2_char_t* szAddr = axis2_endpoint_ref_get_address(pEndpoint, m_pEnv);
    RISE_ASSERTS(szAddr, "Can't get address");
    return szAddr;


//    axis2_endpoint_ref_t* pEndpoint = axis2_msg_ctx_get_from(m_pMsgCtx, m_pEnv);
//    RISE_ASSERTS(pEndpoint, "Can't get endpoint");
//    const axis2_char_t* szAddr = axis2_endpoint_ref_get_address(pEndpoint, m_pEnv);
//    RISE_ASSERTS(szAddr, "Can't get address");
//    return szAddr;
  }

  std::string MessageContext::GetToAddress() const
  {
    axis2_msg_info_headers_t* pHeaders = axis2_msg_ctx_get_msg_info_headers(m_pMsgCtx, m_pEnv);
    RISE_ASSERTS(pHeaders, "Can't get Headers");
    axis2_endpoint_ref_t* pEndpoint = axis2_msg_info_headers_get_to(pHeaders, m_pEnv);
    RISE_ASSERTS(pEndpoint, "Can't get endpoint");
    const axis2_char_t* szAddr = axis2_endpoint_ref_get_address(pEndpoint, m_pEnv);
    RISE_ASSERTS(szAddr, "Can't get address");
    return szAddr;
  }

  bool MessageContext::GetInFaultFlow() const
  {
    return axis2_msg_ctx_get_in_fault_flow(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  //CMessageContext::GetSoapEnvelope() const
  //{
  //  axis2_msg_ctx_get_soap_envelope(m_pMsgCtx, m_pEnv);
  //}

  //CMessageContext::GetResponseSoapEnvelope() const
  //{
  //  axis2_msg_ctx_get_response_soap_envelope
  //}

  //CMessageContext::GetFaultSoapEnvelope() const
  //{
  //  axis2_msg_ctx_get_fault_soap_envelope
  //}

  std::string MessageContext::GetMsgId() const
  {
    const axis2_char_t* szMsgId = axis2_msg_ctx_get_msg_id(m_pMsgCtx, m_pEnv);
    return szMsgId ? szMsgId : "";
  }

  bool MessageContext::GetProcessFault() const
  {
    return axis2_msg_ctx_get_process_fault(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  //CMessageContext::GetRelatesTo() const
  //{
  //  axis2_msg_ctx_get_relates_to()
  //}

  //CMessageContext::GetReplyTo() const
  //{
  //  axis2_msg_ctx_get_reply_to()
  //}

  bool MessageContext::GetServerSide() const
  {
    return axis2_msg_ctx_get_server_side(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  bool MessageContext::GetNewThreadRequired() const
  {
    return axis2_msg_ctx_get_new_thread_required(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  std::string MessageContext::GetWsaAction() const
  {
    const axis2_char_t* szMsgId = axis2_msg_ctx_get_wsa_action(m_pMsgCtx, m_pEnv);
    RISE_ASSERTS(szMsgId, "Can't get wsa action");
    return szMsgId;
  }

  std::string MessageContext::GetWsaMessageId() const
  {
    const axis2_char_t* szMsgId = axis2_msg_ctx_get_wsa_message_id(m_pMsgCtx, m_pEnv);
    RISE_ASSERTS(szMsgId, "Can't get wsa msgid");
    return szMsgId;
  }

//  MessageContext::GetMsgInfoHeaders() const
//  {
//    axis2_msg_ctx_get_msg_info_headers(m_pMsgCtx, m_pEnv);
//  }

  bool MessageContext::IsKeepAlive() const
  {
    return axis2_msg_ctx_is_keep_alive(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  std::string MessageContext::GetRestHttpMethod() const
  {
    const char* szHttpMethod = static_cast<const char*>(axis2_msg_ctx_get_rest_http_method(m_pMsgCtx, m_pEnv));
    return szHttpMethod ? szHttpMethod : "";
  }

  std::string MessageContext::GetServiceContextId() const
  {
    const char* szSvcCtxId = static_cast<const char*>(axis2_msg_ctx_get_svc_ctx_id(m_pMsgCtx, m_pEnv));
    return szSvcCtxId ? szSvcCtxId : "";
  }

  //CMessageContext::GetConfCtx() const
  //{
  //  axis2_msg_ctx_get_conf_ctx()
  //}

  //CMessageContext::GetSvcCtx() const
  //{
  //  axis2_msg_ctx_get_svc_ctx(m_pMsgCtx, m_pEnv);
  //}

  //CMessageContext::GetParameter() const
  //{
  //  axis2_msg_ctx_get_parameter()
  //}

  //CMessageContext::GetModuleParameter() const
  //{
  //  axis2_msg_ctx_get_module_parameter
  //}

  //CMessageContext::GetProperty() const
  //{
  //  axis2_msg_ctx_get_property
  //}

  std::string MessageContext::GetPropertyValue(const std::string& sName) const
  {
    const axis2_char_t* szValue = reinterpret_cast<const char*>
        (axis2_msg_ctx_get_property_value(m_pMsgCtx, m_pEnv, sName.c_str()));
    return szValue ? szValue : "";
  }

  std::string MessageContext::GetPausedHandlerName() const
  {
    const axutil_string_t* psName = axis2_msg_ctx_get_paused_handler_name(m_pMsgCtx, m_pEnv);
    return std::string(axutil_string_get_buffer(psName, m_pEnv), axutil_string_get_length(psName, m_pEnv));
  }

  //std::string MessageContext::GetPausedPhaseName() const
  //{
  //  const char* szName = static_cast<const char*>(axis2_msg_ctx_get_paused_phase_name(m_pMsgCtx, m_pEnv));
  //  return szName ? szName : "";
  //}

  std::string MessageContext::GetSoapAction() const
  {
    const axutil_string_t* psAction = axis2_msg_ctx_get_soap_action(m_pMsgCtx, m_pEnv);
    return std::string(axutil_string_get_buffer(psAction, m_pEnv), axutil_string_get_length(psAction, m_pEnv));
  }

  bool MessageContext::GetDoingMtom() const
  {
    return axis2_msg_ctx_get_doing_mtom(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  bool MessageContext::GetDoingRest() const
  {
    return axis2_msg_ctx_get_doing_rest(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  bool MessageContext::GetDoRestThroughPost() const
  {
    return axis2_msg_ctx_get_do_rest_through_post(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  bool MessageContext::GetManageSession() const
  {
    return axis2_msg_ctx_get_manage_session(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  bool MessageContext::GetIsSoap11() const
  {
    return axis2_msg_ctx_get_is_soap_11(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  //CMessageContext::GetOptions() const
  //{
  //  axis2_msg_ctx_get_options()
  //}

  bool MessageContext::IsPaused() const
  {
    return axis2_msg_ctx_is_paused(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

  int MessageContext::GetFlow() const
  {
    return axis2_msg_ctx_get_flow(m_pMsgCtx, m_pEnv);
  }

  //CMessageContext::GetSupportedRestHttpMethods() const
  //{
  //  axis2_msg_ctx_get_supported_rest_http_methods
  //}

  std::string MessageContext::GetCharsetEncoding() const
  {
    const axutil_string_t* psEnc = axis2_msg_ctx_get_charset_encoding(m_pMsgCtx, m_pEnv);
    return std::string(axutil_string_get_buffer(psEnc, m_pEnv), axutil_string_get_length(psEnc, m_pEnv));
  }

  int MessageContext::GetStatusCode() const
  {
    return axis2_msg_ctx_get_status_code(m_pMsgCtx, m_pEnv);
  }

  //CMessageContext::GetOutTransportInfo() const
  //{
  //  axis2_msg_ctx_get_out_transport_info
  //}

  //CMessageContext::ResetOutTransportInfo() const
  //{
  //  axis2_msg_ctx_reset_out_transport_info
  //}

  //CMessageContext::GetTransportHeaders() const
  //{
  //  axis2_msg_ctx_get_transport_headers

  //}

  bool MessageContext::GetTransportHeaderValue(const std::string& sHeader, std::string& sValue) const
  {
    axutil_hash_t* pHashHeaders = axis2_msg_ctx_get_transport_headers(m_pMsgCtx, m_pEnv);
    RISE_ASSERTS(pHashHeaders, "Can't get transport headers");

    axis2_http_header_t* pHostHeader = reinterpret_cast<axis2_http_header_t*>
        (axutil_hash_get(pHashHeaders, sHeader.c_str(), AXIS2_HASH_KEY_STRING));

    if (!pHostHeader)
    {
      return false;
    }

    axis2_char_t* szValue = axis2_http_header_get_value(pHostHeader, m_pEnv);
    if (szValue)
    {
      sValue = szValue;
    }
    else
    {
      sValue.erase();
    }

    return true;
  }

  //CMessageContext::ExtractTransportHeaders() const
  //{
  //  axis2_msg_ctx_extract_transport_headers
  //}

  std::string MessageContext::GetTransportUrl() const
  {
    const char* szUrl = static_cast<const char*>(axis2_msg_ctx_get_transport_url(m_pMsgCtx, m_pEnv));
    return szUrl ? szUrl : "";
  }

  bool MessageContext::GetNoContent() const
  {
    return axis2_msg_ctx_get_no_content(m_pMsgCtx, m_pEnv) != AXIS2_FALSE;
  }

} // namespace staff
