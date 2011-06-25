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

#ifndef PROVIDERSERVICEWRAPPER_H
#define PROVIDERSERVICEWRAPPER_H

#include <staff/component/ServiceWrapper.h>
#include "staffdascommonexport.h"

namespace staff
{
namespace das
{
  class DataSource;

  //! DAS virtual service wrapper
  class STAFF_DAS_COMMON_EXPORT ProviderServiceWrapper: public ServiceWrapper
  {
  public:
    ProviderServiceWrapper(Component* pComponent, const DataSource* pDataSource);

    //!         get service name
    /*! \return service name
    */
    virtual const std::string& GetName() const;

    //!         get service description
    /*! \return service description
    */
    virtual const std::string& GetDescr() const;

    //!         get service operations
    /*! \return service operations
    */
    virtual DataObject GetOperations() const;

    //!         invoke service operation
    /*! \param  rOperation - operation
        \param  sSessionId - service session id
        \param  sInstanceId - service instance id
        */
    virtual void Invoke(staff::Operation& rOperation, const std::string& sSessionId, const std::string& sInstanceId);

    //!         get service's component
    /*! \return service's component
    */
    virtual const Component* GetComponent() const;

    //!         get service's component
    /*! \return service's component
    */
    virtual Component* GetComponent();

    //!         get pointer to service implementation
    /*! \param  sSessionId - service session id
        \param  sInstanceId - service instance id
        \return pointer to service implementation or NULL, if service non-local
        */
    virtual PIService& GetImpl(const std::string& sSessionId, const std::string& sInstanceId);

    //!         create new service impl
    /*! \return resulting service impl
      */
    virtual PIService NewImpl();

    //!         load service at startup
    /*! \return true, if service needed to be loaded at startup
      */
    virtual bool IsLoadAtStartup() const;

    //!         get comma-delimeted list of dependecies
    /*! \return comma-delimeted list of dependecies
      */
    virtual std::string GetDependencies() const;

  private:
    //!         get service description
    /*! \return service description
    */
    virtual DataObject GetServiceDescription() const;

  private:
    Component* m_pComponent;
    const DataSource* m_pDataSource;
    std::string m_sName;
  };
}
}

#endif // PROVIDERSERVICEWRAPPER_H
