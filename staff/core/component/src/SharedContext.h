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

#ifndef _SHAREDCONTEXT_H_
#define _SHAREDCONTEXT_H_

#include "staffcomponentexport.h"
#include <string>
#include <map>
#include "CompositeComponent.h"

namespace staff
{
  class ServiceWrapper;
  class Component;
  
  //! Pointer to componsite component
  typedef rise::CMutablePtr<CompositeComponent> PCompositeComponent;

  //! Composite components map
  typedef std::map<std::string, PCompositeComponent> CompositeComponentMap;


  //!  Shared context
  class STAFF_COMPONENT_EXPORT SharedContext
  {
  public:
    //!         get shared context instance
    /*! \return shared context instance
    */
    static SharedContext& Inst();
    
    //!         add component
    /*! \param  pComponent - component
        */
    void AddComponent(Component* pComponent);

    //!         get composite component by name
    /*! \param  sName - component name
        \return pointer to composite component, NULL, if no component found
        */
    const CompositeComponent* GetComponent(const std::string& sName) const;
    
    //!         get composite component by name
    /*! \param  sName - component name
        \return pointer to composite component, NULL, if no component found
        */
    CompositeComponent* GetComponent(const std::string& sName);
    
    //!         get available composite components map
    /*! \return available composite components map
    */
    const CompositeComponentMap& GetComponents() const;

    //!         get service by name
    /*! \param  sName - full service name(including component name)
        \return pointer to service, NULL, if no service found
        */
    const ServiceWrapper* GetService(const std::string& sName) const;

    //!         get service by name
    /*! \param  sName - full service name(including component name)
        \return pointer to service, NULL, if no service found
        */
    ServiceWrapper* GetService(const std::string& sName);

    //!         get services list
    /*! \return services list
        */
    const ServiceWrapperMap& GetServices() const;

    //!         clear component list
    void Clear();

  private:
    SharedContext();
    ~SharedContext();
    SharedContext(const SharedContext&);
    SharedContext& operator=(const SharedContext&);

  private:
    CompositeComponentMap m_mComponents;
    ServiceWrapperMap m_mServiceWrappers;
  };

#ifndef STAFF_NO_DEPRECATED
  typedef SharedContext CSharedContext STAFF_DEPRECATED(SharedContext);
#endif

}

#endif // _SHAREDCONTEXT_H_
