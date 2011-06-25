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

#ifndef ISERVICE_H
#define ISERVICE_H

#include "staffcommonexport.h"
#include <string>

namespace rise
{
  template<typename Type> class CSharedPtr;
}

namespace staff
{
  class ServiceClient;

  //! base class for services
  class STAFF_COMMON_EXPORT IService
  {
  public:
    //! constructor
    IService();

    //! destructor
    virtual ~IService();

    //! get service name
    /*! \return service name
      */
    const std::string& GetServiceName() const;

    //! get session id
    /*! \return session id
      */
    const std::string& GetSessionId() const;

    //! get instance id
    /*! \return instance id
      */
    const std::string& GetInstanceId() const;

    //! this function called after service construction and initialization
    /*! server side function */
    virtual void OnCreate();

    //! this function called before service destruction
    /*! server side function */
    virtual void OnDestroy();

    //! get client
    /*! client side function, in server side always returns NULL
        \return pointer to service client
      */
    virtual ServiceClient* GetClient();

    //! internal function
    void Init(const std::string& sServiceName, const std::string& sSessionId, const std::string& sInstanceId);

  private:
    std::string m_sServiceName; //!< service name with component name
    std::string m_sSessionId;   //!< session id
    std::string m_sInstanceId;  //!< instance id
  };

  typedef rise::CSharedPtr<IService> PIService; //!< smart pointer to service
}

#endif // ISERVICE_H
