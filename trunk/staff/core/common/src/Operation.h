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

#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "staffcommonexport.h"
#include <string>
#include "DataObject.h"

// AXIS2/C typedefs
typedef struct axiom_node axiom_node_t;

namespace staff
{
  class Value;
  class MessageContext;

  //!  Service Operation
  class STAFF_COMMON_EXPORT Operation
  {
  public:
    //! constructor
    Operation(const std::string& sName = "", const std::string& sResponseName = "", const std::string& sResultName = "");

    //! destructor
    virtual ~Operation();

    //! set operation name
    /*! \param  sName - operation name
        */
    void SetName(const std::string& sName);

    //! get operation name
    /*! \return operation name
    */
    const std::string GetName() const;

    //! set response node name
    /*! \param  sResponseName - response node name
        */
    void SetResponseName(const std::string& sResponseName);

    //! get response node name
    /*! \return response node name
    */
    const std::string GetResponseName() const;

    //! set result node name
    /*! \param  sResultName - result node name
        */
    void SetResultName(const std::string& sResultName);

    //! get result node name
    /*! \return result node name
    */
    const std::string& GetResultName() const;

    //! set SOAP action
    /*! \param  sSoapAction - SOAP action
        */
    void SetSoapAction(const std::string& sSoapAction);

    //! get SOAP action
    /*! \return SOAP action
        */
    const std::string& GetSoapAction() const;

    //! add operation parameter
    /*! \param  sName - parameter name
        \param  tValue - parameter value
        */
    void AddParameter(const std::string& sName, const Value& tValue);
    
    //! add operation parameter as DataObject
    /*! \param  rDataObject - DataObject
        */
    void AddParameter(DataObject& rDataObject);

    //! create/get request DataObject
    /*! \return request DataObject
    */
    const DataObject& Request() const;

    //! create/get request DataObject
    /*! \return request DataObject(mutable)
    */
    DataObject& Request();

    //! set request DataObject
    /*! \param  rdoRequest - request DataObject
    */
    void SetRequest(DataObject& rdoRequest);

    //! set request DataObject
    /*! \param  pRequest - request
    */
    void SetRequest(axiom_node_t* pRequest);

    //! get result DataObject optional
    /*! \return result DataObject optional
    */
    const DataObject& ResultOpt() const;

    //! get result DataObject optional
    /*! \return result DataObject(mutable)
    */
    DataObject& ResultOpt();

    //! get result DataObject
    /*! \return result DataObject
    */
    const DataObject& Result() const;

    //! get result DataObject
    /*! \return result DataObject(mutable)
    */
    DataObject& Result();

    //! prepare Operation for sending result
    void PrepareResult();

    //! create/get response DataObject
    /*! \return response DataObject
        */
    DataObject& GetResponse();

    //! create/get response DataObject
    /*! \return response DataObject
        */
    const DataObject& GetResponse() const;

    //! set response DataObject
    /*! \param  rdoResponse - response DataObject
        */
    void SetResponse(staff::DataObject& rdoResponse);

    //! set response DataObject
    /*! \param  pResponse - response
        */
    void SetResponse(axiom_node_t* pResponse);

    //! set result
    /*! \param  rDataObject - DataObject with results
        */
    void SetResult(DataObject& rDataObject);

    //! set result value
    /*! \param  rValue - result value
        */
    void SetResultValue(const Value& rValue);

    //! check an operation is fault or not
    /*! \return true - operation is fault
        */
    bool IsFault() const;

    //! get fault description
    /*! \return fault description or empty string
        */
    std::string GetFaultDescr() const;

    //! get fault string
    /*! \return fault string or empty string
    */
    std::string GetFaultString() const;

    //! get fault code
    /*! \return fault code or empty string
        */
    std::string GetFaultCode() const;

    //! get fault detail
    /*! \return fault detail or empty string
    */
    std::string GetFaultDetail() const;

    //! get fault DataObject
    /*! \return fault DataObject
        */
    const DataObject GetFault() const;

    //! set fault
    /*! \param  sFaultCode - fault code
        \param  sFaultString - fault string
        \param  sDetail - fault detail
        */
    void SetFault(const std::string& sFaultCode, const std::string& sFaultString, const std::string& sDetail = "");

    //! reset fault
    void ResetFault();

    //! set fault as DataObject in user-defined format
    /*! \param  rDataObjectFault - DataObject with fault description
        */
    void SetUserFault(DataObject& rDataObjectFault);

    //! get is fault in user-defined format
    /*! \return true if fault is in user-defined format
        */
    bool IsUserFault() const;

    //! set message context
    /*! \param  rMessageContext - message context
      */
    void SetMessageContext(MessageContext& rMessageContext);

    //! get message context
    /*! \return message context
      */
    MessageContext& GetMessageContext();

  private:
    mutable DataObject m_tdoRequest;   //!<  request DataObject
    mutable DataObject m_tdoResponse;  //!<  response DataObject
    mutable DataObject m_tdoResult;    //!<  result DataObject
    std::string m_sRequestName;        //!<  request name
    std::string m_sResponseName;       //!<  response name
    std::string m_sResultName;         //!<  result name
    std::string m_sSoapAction;         //!<  SOAP action
    MessageContext* m_pMessageContext; //!<  message context
    bool m_bIsUserFault;               //!<  true if SetUserFault has been called
  };

}

#endif // _OPERATION_H_
