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

#ifndef _SERVICEWRAPPER_H_
#define _SERVICEWRAPPER_H_

#include <string>
#include "staffcomponentexport.h"

namespace staff
{
  class CDataObject;
  class COperation;
  class CComponent;
  class IService;

  //!  Service
  class STAFF_COMPONENT_EXPORT CServiceWrapper
  {
  public:
    //!        destructor
    virtual ~CServiceWrapper();

    //!         get service name
    /*! \return service name
    */
    virtual const std::string& GetName() const = 0;

    //!         get service description
    /*! \return service description
    */
    virtual const std::string& GetDescr() const = 0;

    //!         get service operations
    /*! \return service operations
    */
    virtual CDataObject GetOperations() const = 0;

    //!         invoke service operation
    /*! \param  rOperation - operation
        \param  sSessionId - service session id
        \param  sInstanceId - service instance id
        */
    virtual void Invoke(COperation& rOperation, const std::string& sSessionId, const std::string& sInstanceId) = 0;

    //!         get service's component
    /*! \return service's component
    */
    virtual const CComponent* GetComponent() const = 0;

    //!         get service's component
    /*! \return service's component
    */
    virtual CComponent* GetComponent() = 0;

    //!         get pointer to service implementation
    /*! \param  sID - service session id
        \return pointer to service implementation or NULL, if service non-local
        */
    virtual IService* GetImpl(const std::string& sSessionId, const std::string& sInstanceId) = 0;

    //!         create new service impl
    /*! \return resulting service impl
      */
    virtual IService* NewImpl() = 0;
  };
}

#endif // _SERVICEWRAPPER_H_
