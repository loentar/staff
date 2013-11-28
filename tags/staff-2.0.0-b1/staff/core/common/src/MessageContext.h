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

#include "staffcommonexport.h"
#include <string>

typedef struct axis2_msg_ctx axis2_msg_ctx_t;
typedef struct axutil_env axutil_env_t;

namespace staff
{
  //! message context class
  class STAFF_COMMON_EXPORT MessageContext
  {
  public:
    MessageContext(const axutil_env_t* pEnv, axis2_msg_ctx_t* pMsgCtx);

    //! get axis2 message context
    /*! \return axis2 message context
      */
    operator axis2_msg_ctx_t*();

    //! get fault to address
    /*! \return fault to address
      */
    std::string GetFaultToAddress() const;

    //! get from address
    /*! \return from address
      */
    std::string GetFromAddress() const;

    //! get to address
    /*! \return to address
      */
    std::string GetToAddress() const;

    //! get in fault flow
    /*! \return in fault flow
      */
    bool GetInFaultFlow() const;

    //! get msg id
    /*! \return msg id
      */
    std::string GetMsgId() const;

    //! get process fault
    /*! \return process fault
      */
    bool GetProcessFault() const;

    //! get server side
    /*! \return server side
      */
    bool GetServerSide() const;

    //! get new thread required
    /*! \return new thread required
      */
    bool GetNewThreadRequired() const;

    //! get wsa action
    /*! \return wsa action
      */
    std::string GetWsaAction() const;

    //! get wsa message id
    /*! \return wsa message id
      */
    std::string GetWsaMessageId() const;

    //! is keep alive
    /*! \return keep alive
      */
    bool IsKeepAlive() const;

    //! get rest http method
    /*! \return rest http method
      */
    std::string GetRestHttpMethod() const;

    //! get service context id
    /*! \return service context id
      */
    std::string GetServiceContextId() const;

    //! get property value
    /*! \param  sName - name
        \return property value
      */
    std::string GetPropertyValue(const std::string& sName) const;

    //! get paused handler name
    /*! \return paused handler name
      */
    std::string GetPausedHandlerName() const;

    //! get soap action
    /*! \return soap action
      */
    std::string GetSoapAction() const;

    //! get doing mtom
    /*! \return doing mtom
      */
    bool GetDoingMtom() const;

    //! get doing rest
    /*! \return doing rest
      */
    bool GetDoingRest() const;

    //! get do rest through post
    /*! \return do rest through post
      */
    bool GetDoRestThroughPost() const;

    //! get manage session
    /*! \return manage session
      */
    bool GetManageSession() const;

    //! get is soap11
    /*! \return is soap11
      */
    bool GetIsSoap11() const;

    //! is paused
    /*! \return
      */
    bool IsPaused() const;

    //! get flow
    /*! \return flow
      */
    int GetFlow() const;

    //! get charset encoding
    /*! \return charset encoding
      */
    std::string GetCharsetEncoding() const;

    //! get status code
    /*! \return status code
      */
    int GetStatusCode() const;

    //! get transport header value
    /*! \param  sHeader - header
        \param  sValue - value
        \return transport header value
      */
    bool GetTransportHeaderValue(const std::string& sHeader, std::string& sValue) const;

    //! get transport url
    /*! \return transport url
      */
    std::string GetTransportUrl() const;

    //! get no content
    /*! \return no content
      */
    bool GetNoContent() const;

  private:
    const axutil_env_t* m_pEnv; //!< axis2 environment
    mutable axis2_msg_ctx_t* m_pMsgCtx; //!< message context
  };

} // namespace staff

#endif // _MESSAGECONTEXT_H_


