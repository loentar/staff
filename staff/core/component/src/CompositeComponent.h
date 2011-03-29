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

#ifndef _COMPOSITECOMPONENT_H_
#define _COMPOSITECOMPONENT_H_

#include "staffcomponentexport.h"
#include <string>
#include <staff/common/DataObject.h>
#include "Component.h"

namespace staff
{
  //!  Composite component
  class STAFF_COMPONENT_EXPORT CCompositeComponent: public CComponent
  {
  public:
    //!        constructor
    CCompositeComponent();

    //!        destructor
    virtual ~CCompositeComponent();

    //!         compose component(add partial component to composite)
    /*! \param  pComponent - component
        */
    void Compose(CComponent* pComponent);

    //!         get component name
    /*! \return component name
    */
    const std::string& GetName() const;

    //!         get component service
    /*! \param  sName - service name
        \return pointer to service, NULL if no service found
        */
    const CServiceWrapper* GetService(const std::string& sName) const;

    //!         get component service
    /*! \param  sName - service name
        \return pointer to service, NULL if no service found
        */
    CServiceWrapper* GetService(const std::string& sName);

    //!         add service into component
    /*! \param  pService - service
        */
    void AddService(CServiceWrapper* pService);

    //!         remove service from component
    /*! \param  sName - service name
        */
    void RemoveService(const std::string& sName);

    //!         get services map
    /*! \return services map
    */
    const TServiceWrapperMap& GetServices() const;

  private:
    std::string m_sName;        //!<  component name
    TServiceWrapperMap m_mServices;      //!<  services map
  };
}

#endif // _COMPOSITECOMPONENT_H_
