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


#ifndef _MESSAGECONTEXT_H_
#define _MESSAGECONTEXT_H_

#include <string>

typedef struct axis2_msg_ctx axis2_msg_ctx_t;
typedef struct axutil_env axutil_env_t;

namespace staff
{
  class CMessageContext
  {
  public:
    CMessageContext(const axutil_env_t* pEnv, axis2_msg_ctx_t* pMsgCtx);

    operator axis2_msg_ctx_t*();

    std::string GetFaultToAddress() const;
    std::string GetFromAddress() const;
    std::string GetToAddress() const;
    bool GetInFaultFlow() const;
    //GetSoapEnvelope() const;
    //GetResponseSoapEnvelope() const;
    //GetFaultSoapEnvelope() const;
    std::string GetMsgId() const;
    bool GetProcessFault() const;
    //GetRelatesTo() const;
    //GetReplyTo() const;
    bool GetServerSide() const;
    bool GetNewThreadRequired() const;
    std::string GetWsaAction() const;
    std::string GetWsaMessageId() const;
  //  GetMsgInfoHeaders() const;
    bool IsKeepAlive() const;
    std::string GetRestHttpMethod() const;
    std::string GetServiceContextId() const;
    //GetConfCtx() const;
    //GetSvcCtx() const;
    //GetParameter() const;
    //GetModuleParameter() const;
    //GetProperty() const;
    std::string GetPropertyValue(const std::string& sName) const;
    std::string GetPausedHandlerName() const;
    //std::string GetPausedPhaseName() const;
    std::string GetSoapAction() const;
    bool GetDoingMtom() const;
    bool GetDoingRest() const;
    bool GetDoRestThroughPost() const;
    bool GetManageSession() const;
    bool GetIsSoap11() const;
    //GetOptions() const;
    bool IsPaused() const;
    int GetFlow() const;
    //GetSupportedRestHttpMethods() const;
    std::string GetCharsetEncoding() const;
    int GetStatusCode() const;
    //GetOutTransportInfo() const;
    //ResetOutTransportInfo() const;
    //GetTransportHeaders() const;
    bool GetTransportHeaderValue(const std::string& sHeader, std::string& sValue) const;
    //ExtractTransportHeaders() const;
    std::string GetTransportUrl() const;
    bool GetNoContent() const;

  private:
    const axutil_env_t* m_pEnv;
    mutable axis2_msg_ctx_t* m_pMsgCtx;
  };

} // namespace staff

#endif // _MESSAGECONTEXT_H_


