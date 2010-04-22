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

#ifndef SERVICEWRAPPERFACTORY_H
#define SERVICEWRAPPERFACTORY_H

#include "staffcomponentexport.h"
#include <string>
#include "Component.h"

namespace staff
{
  //! service wrapper factory
  class STAFF_COMPONENT_EXPORT CServiceWrapperFactory
  {
  public:
    //! get factory instance
    /*! \return factory instance
        */
    static CServiceWrapperFactory& Inst();

    //! free factory instance
    static void FreeInst();

    //! register service wrapper
    /*! \param sServiceName - wrapper's service name
        \param rpServiceWrapper - service wrapper to register
      */
    void RegisterServiceWrapper(const std::string& sServiceName, PServiceWrapper& rpServiceWrapper);

    //! unregister service wrapper
    /*! \param sServiceName - wrapper's service name
      */
    void UnregisterServiceWrapper(const std::string& sServiceName);

    //! get service wrapper
    /*! \param sServiceName - wrapper's service name
      */
    PServiceWrapper& GetServiceWrapper(const std::string& sServiceName);

  private:
    CServiceWrapperFactory();
    ~CServiceWrapperFactory();

  private:
    static CServiceWrapperFactory* m_pInst;
    TServiceWrapperMap m_mWrappers;
  };
}

#endif // SERVICEWRAPPERFACTORY_H
