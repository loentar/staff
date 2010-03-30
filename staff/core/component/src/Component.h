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

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "staffcomponentexport.h"
#include <map>
#include <string>

namespace rise
{
  template<typename Type> class CMutablePtr;
}

namespace staff
{
  class CServiceWrapper;
  
  //! Pointer to service wrapper
  typedef rise::CMutablePtr<CServiceWrapper> PServiceWrapper;
  //! Pointer to service wrapper map
  typedef std::map<std::string, PServiceWrapper> TServiceWrapperMap;


  //!  Component
  class STAFF_COMPONENT_EXPORT CComponent
  {
  public:
    //!        destructor
    virtual ~CComponent() {}

    //!         get component name
    /*! \return component name
    */
    virtual const std::string& GetName() const = 0;

    //!         get service with given name
    /*! \param  sService - service name
        \return pointer to service or NULL, if no service found
        */
    virtual const CServiceWrapper* GetService(const std::string& sService) const = 0;

    //!         get service with given name
    /*! \param  sService - service name
        \return pointer to service or NULL, if no service found
        */
    virtual CServiceWrapper* GetService(const std::string& sService) = 0;

    //!         get component's services map
    /*! \return component's services map
    */
    virtual const TServiceWrapperMap& GetServices() const = 0;
  };
}

#endif // _COMPONENT_H_
