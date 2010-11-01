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

#ifndef PROVIDERCOMPONENT_H
#define PROVIDERCOMPONENT_H

#include <string>
#include <staff/component/Component.h>

namespace staff
{
namespace das
{
  class ProviderComponent: public CComponent
  {
  public:
    ProviderComponent(const std::string& sName);

    void AddServiceWrapper(const std::string& sServiceName, PServiceWrapper& rpServiceWrapper);

    //!         get component name
    /*! \return component name
    */
    virtual const std::string& GetName() const;

    //!         get service with given name
    /*! \param  sService - service name
        \return pointer to service or NULL, if no service found
        */
    virtual const CServiceWrapper* GetService(const std::string& sService) const;

    //!         get service with given name
    /*! \param  sService - service name
        \return pointer to service or NULL, if no service found
        */
    virtual CServiceWrapper* GetService(const std::string& sService);

    //!         get component's services map
    /*! \return component's services map
    */
    virtual const TServiceWrapperMap& GetServices() const;

  private:
    staff::TServiceWrapperMap m_mServices;
    std::string m_sName;
  };
}
}

#endif // PROVIDERCOMPONENT_H
