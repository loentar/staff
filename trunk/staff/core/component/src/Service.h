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

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <rise/string/String.h>
#include "staffcomponentexport.h"

namespace staff
{
  class CDataObject;
  class COperation;
  class CComponent;

  //!  Service
  class STAFF_COMPONENT_EXPORT CService
  {
  public:
    //!        destructor
    virtual ~CService();

    //!         get service name
    /*! \return service name
    */
    virtual const rise::CString& GetName() const = 0;

    //!         get service description
    /*! \return service description
    */
    virtual const rise::CString& GetDescr() const = 0;
    
    //!         get service operations
    /*! \return service operations
    */
    virtual CDataObject GetOperations() const = 0;
    
    //!         invoke service operation
    /*! \param  rOperation - operation
        \param  sID - service session id
        */
    virtual void Invoke(COperation& rOperation, const rise::CString& sID) = 0;

    //!         get service's component
    /*! \return service's component
    */
    virtual const CComponent* GetComponent() const = 0;

    //!         get service's component
    /*! \return service's component
    */
    virtual CComponent* GetComponent() = 0;

    //!         get pointer to service implementation(only for local services)
    /*! \param  sID - service session id
        \return pointer to service implementation or NULL, if service non-local
        */
    virtual void* GetImpl(const rise::CString& sID) = 0;
  };
}

#endif // _SERVICE_H_
