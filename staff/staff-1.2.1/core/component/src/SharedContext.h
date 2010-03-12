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
#include <rise/string/String.h>
#include <rise/common/MutablePtr.h>
#include <map>
#include "CompositeComponent.h"

namespace staff
{
  class CService;
  class CComponent;
  
  //! Pointer to componsite component
  typedef rise::CMutablePtr<CCompositeComponent> PCompositeComponent;

  //! Composite components map
  typedef std::map<rise::CString, PCompositeComponent> TCompositeComponentMap;


  //!  Shared context
  class STAFF_COMPONENT_EXPORT CSharedContext
  {
  public:
    //!         get shared context instance
    /*! \return shared context instance
    */
    static CSharedContext& Inst();
    
    //!         add component
    /*! \param  pComponent - component
        */
    void AddComponent(CComponent* pComponent);

    //!         get composite component by name
    /*! \param  sName - component name
        \return pointer to composite component, NULL, if no component found
        */
    const CCompositeComponent* GetComponent(const rise::CString& sName) const;
    
    //!         get composite component by name
    /*! \param  sName - component name
        \return pointer to composite component, NULL, if no component found
        */
    CCompositeComponent* GetComponent(const rise::CString& sName);
    
    //!         get available composite components map
    /*! \return available composite components map
    */
    const TCompositeComponentMap& GetComponents() const;

    //!         get service by name
    /*! \param  sName - full service name(including component name)
        \return pointer to service, NULL, if no service found
        */
    const CService* GetService(const rise::CString& sName) const;

    //!         get service by name
    /*! \param  sName - full service name(including component name)
        \return pointer to service, NULL, if no service found
        */
    CService* GetService(const rise::CString& sName);

    //!         get services list
    /*! \return services list
        */
    TServiceMap GetServices() const;

    //!         clear component list
    void Clear();

  protected:
    CSharedContext();
    virtual ~CSharedContext();

  private:
    TCompositeComponentMap m_mComponents;
    static CSharedContext* m_pInst;
  };
}

#endif // _SHAREDCONTEXT_H_
