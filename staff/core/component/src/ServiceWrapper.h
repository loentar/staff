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

#include "staffcomponentexport.h"
#include <string>

namespace rise
{
  template<typename Type> class CSharedPtr;
}

namespace staff
{
  class DataObject;
  class Operation;
  class Component;
  class IService;
  typedef rise::CSharedPtr<IService> PIService;

  //!  base class for service wrappers
  class STAFF_COMPONENT_EXPORT ServiceWrapper
  {
  public:
    //!        destructor
    virtual ~ServiceWrapper();

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
    virtual DataObject GetOperations() const = 0;

    //!         invoke service operation
    /*! \param  rOperation - operation
        \param  sSessionId - service session id
        \param  sInstanceId - service instance id
        */
    virtual void Invoke(Operation& rOperation, const std::string& sSessionId, const std::string& sInstanceId) = 0;

    //!         get service's component
    /*! \return service's component
    */
    virtual const Component* GetComponent() const = 0;

    //!         get service's component
    /*! \return service's component
    */
    virtual Component* GetComponent() = 0;

    //!         get pointer to service implementation
    /*! \param  sSessionId - service session id
        \param  sInstanceId - service instance id
        \return pointer to service implementation or NULL, if service non-local
        */
    virtual PIService& GetImpl(const std::string& sSessionId, const std::string& sInstanceId) = 0;

    //!         create new service impl
    /*! \return resulting service impl
      */
    virtual PIService NewImpl() = 0;

    //!         load service at startup
    /*! \return true, if service needed to be loaded at startup
      */
    virtual bool IsLoadAtStartup() const = 0;

    //!         get comma-delimeted list of dependecies
    /*! \return comma-delimeted list of dependecies
      */
    virtual std::string GetDependencies() const = 0;
  };

#ifndef STAFF_NO_DEPRECATED
  STAFF_DEPRECATED(ServiceWrapper) typedef ServiceWrapper CServiceWrapper;
  class CDataObject;
#endif
}

#endif // _SERVICEWRAPPER_H_
