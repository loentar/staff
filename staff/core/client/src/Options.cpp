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

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <axiom_soap_const.h>
#include <axis2_options.h>
#include <staff/common/Runtime.h>
#include "Options.h"

namespace staff
{
  COptions::COptions():
    m_pEnv(CRuntime::Inst().GetAxis2Env("staff_client")), m_bOwner(true)
  {
    m_pOptions = axis2_options_create(m_pEnv);
  }

  COptions::COptions(axis2_options_t* pOptions):
    m_pOptions(pOptions), m_pEnv(CRuntime::Inst().GetAxis2Env("staff_client")), m_bOwner(false)
  {
  }

  COptions::~COptions()
  {
    if (m_bOwner)
    {
      axis2_options_free(m_pOptions, m_pEnv);
    }
    CRuntime::Inst().FreeAxis2Env("staff_client");
  }


  bool COptions::IsOwner() const
  {
    return m_bOwner;
  }

  void COptions::SetOwner(bool bOwner /*= true*/)
  {
    m_bOwner = bOwner;
  }


  std::string COptions::GetAction() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    const axis2_char_t* szAction = axis2_options_get_action(m_pOptions, m_pEnv);
    RISE_ASSERTS(szAction, "Can't get action");

    return reinterpret_cast<const char*>(szAction);
  }

  void COptions::SetAction(const std::string& sAction)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_options_set_action(m_pOptions, m_pEnv, sAction.c_str());
  }


  std::string COptions::GetFromAddress() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_options_get_from(m_pOptions, m_pEnv);
    RISE_ASSERTS(pEndpointRef, "Can't get WSA endpoint");

    const axis2_char_t* szAddress = axis2_endpoint_ref_get_address(pEndpointRef, m_pEnv);
    RISE_ASSERTS(szAddress, "Can't get WSA from address");

    return reinterpret_cast<const char*>(szAddress);
  }

  void COptions::SetFromAddress(const std::string& sAddress)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_endpoint_ref_create(m_pEnv, sAddress.c_str());
    RISE_ASSERTS(pEndpointRef, "Can't create WSA endpoint");

    axis2_options_set_from(m_pOptions, m_pEnv, pEndpointRef);
  }

  std::string COptions::GetToAddress() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_options_get_to(m_pOptions, m_pEnv);
    RISE_ASSERTS(pEndpointRef, "Can't get WSA endpoint");

    const axis2_char_t* szAddress = axis2_endpoint_ref_get_address(pEndpointRef, m_pEnv);
    RISE_ASSERTS(szAddress, "Can't get WSA to address");

    return reinterpret_cast<const char*>(szAddress);
  }

  void COptions::SetToAddress(const std::string& sAddress)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_endpoint_ref_create(m_pEnv, sAddress.c_str());
    RISE_ASSERTS(pEndpointRef, "Can't create WSA endpoint");

    axis2_options_set_to(m_pOptions, m_pEnv, pEndpointRef);
  }

  std::string COptions::GetReplyToAddress() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_options_get_reply_to(m_pOptions, m_pEnv);
    RISE_ASSERTS(pEndpointRef, "Can't get WSA endpoint");

    const axis2_char_t* szAddress = axis2_endpoint_ref_get_address(pEndpointRef, m_pEnv);
    RISE_ASSERTS(szAddress, "Can't get WSA reply to address");

    return reinterpret_cast<const char*>(szAddress);
  }

  void COptions::SetReplyToAddress(const std::string& sAddress)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_endpoint_ref_create(m_pEnv, sAddress.c_str());
    RISE_ASSERTS(pEndpointRef, "Can't create WSA endpoint");

    axis2_options_set_reply_to(m_pOptions, m_pEnv, pEndpointRef);
  }

  std::string COptions::GetFaultToAddress() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_options_get_fault_to(m_pOptions, m_pEnv);
    RISE_ASSERTS(pEndpointRef, "Can't get WSA endpoint");

    const axis2_char_t* szAddress = axis2_endpoint_ref_get_address(pEndpointRef, m_pEnv);
    RISE_ASSERTS(szAddress, "Can't get WSA address");

    return reinterpret_cast<const char*>(szAddress);
  }

  void COptions::SetFaultToAddress(const std::string& sAddress)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_endpoint_ref_t* pEndpointRef = axis2_endpoint_ref_create(m_pEnv, sAddress.c_str());
    RISE_ASSERTS(pEndpointRef, "Can't create WSA endpoint");

    axis2_options_set_fault_to(m_pOptions, m_pEnv, pEndpointRef);
  }

  void COptions::UseSeparateListener(bool bUseSeparateListener)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_options_set_use_separate_listener(m_pOptions, m_pEnv, bUseSeparateListener ? AXIS2_TRUE : AXIS2_FALSE);
  }

  bool COptions::IsUsingSeparateListener()
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    return axis2_options_get_use_separate_listener(m_pOptions, m_pEnv) == AXIS2_TRUE;
  }


  void COptions::SetTimeout(long lTimeout)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_options_set_timeout_in_milli_seconds(m_pOptions, m_pEnv, lTimeout);
  }

  std::string COptions::GetSoapVersionUri() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    const axis2_char_t* szUri = axis2_options_get_soap_version_uri(m_pOptions, m_pEnv);
    RISE_ASSERTS(szUri, "Can't get SOAP version URI");

    return reinterpret_cast<const char*>(szUri);
  }

  void COptions::SetSoapVersionUri(const std::string& sSoapVersionUri)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_options_set_soap_version_uri(m_pOptions, m_pEnv, sSoapVersionUri.c_str());
  }

  COptions::SoapVersion COptions::GetSoapVersion() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    int nVersion = axis2_options_get_soap_version(m_pOptions, m_pEnv);
    switch (nVersion)
    {
    case AXIOM_SOAP11:
      return Soap11;

    case AXIOM_SOAP12:
      return Soap12;

    default:
      return static_cast<SoapVersion>(nVersion);
    }
  }

  void COptions::SetSoapVersion(SoapVersion nSoapVersion)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    int nAxiomSoapVersion = 0;
    switch (nSoapVersion)
    {
    case Soap11:
      nAxiomSoapVersion = AXIOM_SOAP11;
      break;

    case Soap12:
      nAxiomSoapVersion = AXIOM_SOAP12;
      break;

    default:
      nAxiomSoapVersion = static_cast<soap_version>(nSoapVersion);
    }

    axis2_options_set_soap_version(m_pOptions, m_pEnv, nAxiomSoapVersion);
  }

  std::string COptions::GetSoapAction() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    const axutil_string_t* psAction = axis2_options_get_soap_action(m_pOptions, m_pEnv);
    RISE_ASSERTS(psAction, "Can't get SOAP action");

    return reinterpret_cast<const char*>(axutil_string_get_buffer(psAction, m_pEnv));
  }

  void COptions::SetSoapAction(const std::string& sSoapAction)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axutil_string_t* psSoapAction = axutil_string_create(m_pEnv, sSoapAction.c_str());
    axis2_options_set_soap_action(m_pOptions, m_pEnv, psSoapAction);
    axutil_string_free(psSoapAction, m_pEnv);
  }


  void COptions::EnableRest(bool bEnable)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_options_set_enable_rest(m_pOptions, m_pEnv, bEnable ? AXIS2_TRUE : AXIS2_FALSE);
  }

  void COptions::SetHttpMethod(const std::string& sHttpMethod)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_options_set_http_method(m_pOptions, m_pEnv, sHttpMethod.c_str());
  }


  bool COptions::IsMtomEnabled() const
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    return axis2_options_get_enable_mtom(m_pOptions, m_pEnv) == AXIS2_TRUE;
  }

  void COptions::EnableMtom(bool bEnable)
  {
    RISE_ASSERTS(m_pOptions, "Options is not initialized");

    axis2_options_set_enable_mtom(m_pOptions, m_pEnv, bEnable ? AXIS2_TRUE : AXIS2_FALSE);
  }

  COptions& COptions::operator=(axis2_options_t* pOptions)
  {
    if (m_bOwner && m_pOptions)
    {
      axis2_options_free(m_pOptions, m_pEnv);
    }

    m_pOptions = pOptions;
    m_bOwner = false;
    return *this;
  }

  COptions::operator axis2_options_t*()
  {
    return m_pOptions;
  }


  const std::string& COptions::GetSessionId() const
  {
    return m_sSessionId;
  }

  void COptions::SetSessionId(const std::string& sSessionId)
  {
    m_sSessionId = sSessionId;
  }

  const std::string& COptions::GetInstanceId() const
  {
    return m_sInstanceId;
  }

  void COptions::SetInstanceId(const std::string& sInstanceId)
  {
    m_sInstanceId = sInstanceId;
  }

  void COptions::SetDefaultNamespace(const std::string& sUri, const std::string& sPrefix /*= ""*/)
  {
    m_sDefaultNsUri = sUri;
    m_sDefaultNsPrefix = sPrefix;
  }

  const std::string& COptions::GetDefaultNamespaceUri() const
  {
    return m_sDefaultNsUri;
  }

  const std::string& COptions::GetDefaultNamespacePrefix() const
  {
    return m_sDefaultNsPrefix;
  }

}
