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

#ifndef _REMOTESERVICEWRAPPER_H_
#define _REMOTESERVICEWRAPPER_H_

#include <map>
#include <rise/common/MutablePtr.h>
#include <rise/common/containertypes.h>
#include <staff/component/Service.h>

namespace staff
{
  class CComponent;
  class COperation;
  class CDataObject;
  class CRemoteService;

  //! remote service type
  typedef rise::CMutablePtr<CRemoteService> PRemoteService;

  //! remote service map
  typedef std::map<std::string, PRemoteService> TRemoteServiceMap;

  //!  remote service wrapper
  class CRemoteServiceWrapper: public CService
  {
  public:
    //!         constructor
    /*! \param  pComponent - pointer to component
        */
    CRemoteServiceWrapper(CComponent* pComponent);
    
    //!         destructor
    virtual ~CRemoteServiceWrapper();

    //!         get service name
    /*! \return service name
    */
    const std::string& GetName() const;

    //!         get service description
    /*! \return service description
    */
    const std::string& GetDescr() const;

    //!         get service operations
    /*! \return service operations
    */
    CDataObject GetOperations() const;

    //!         invoke operation
    /*! \param  rOperation - operation
        \param  sID - session id
        */
    void Invoke(COperation& rOperation, const std::string& sID);

    //!         get component, bound to service
    /*! \return component
    */
    const CComponent* GetComponent() const;

    //!         get component, bound to service
    /*! \return component
    */
    CComponent* GetComponent();

    //!         get service implementation pointer
    /*! \param  sID - session id
        \return pointer to service implementation
        */
    void* GetImpl(const std::string& sID);

    //!         get services map
    /*! \return services map
    */
    TRemoteServiceMap& GetServices();

    //!         get services ids
    /*! \return service ids
        */
    virtual rise::CStringList GetServiceIds() const;

  private:
    TRemoteServiceMap m_mServices; //!< services map
    CComponent* m_pComponent;      //!< component
  };
}

#endif // _REMOTESERVICEWRAPPER_H_
