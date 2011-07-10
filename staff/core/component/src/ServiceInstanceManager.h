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

#ifndef SERVICEINSTANCEMANAGER_H
#define SERVICEINSTANCEMANAGER_H

#include "staffcomponentexport.h"
#include <string>

namespace rise
{
  template<typename Type> class CSharedPtr;
}

namespace staff
{
  class IService;
  class ServiceWrapper;

  typedef rise::CSharedPtr<IService> PIService; //!< thread-safe counted pointer to service

  //! service instance manager
  class STAFF_COMPONENT_EXPORT ServiceInstanceManager
  {
  public:
    //! get manager instance
    /*! \return manager instance
        */
    static ServiceInstanceManager& Inst();

    //! create session
    /*! \param sSessionId - session id
      */
    void CreateSession(const std::string& sSessionId);

    //! free session
    /*! free session and all services in session
        \param sSessionId - session id
      */
    void FreeSession(const std::string& sSessionId);

    //! free all sessions
    void FreeAllSessions();

    //! get existing or create service instance
    /*! \param sSessionId - session id
        \param sServiceName - service name
        \param sInstanceId - instance id
        \return service instance
        */
    PIService& ServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId);

    //! create (allocate) service instance
    /*! \param sSessionId - session id
        \param sServiceName - service name
        \param sInstanceId - instance id
        \return created service instance
        */
    PIService& CreateServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId);

    //! free (deallocate) service instance
    /*! \param sSessionId - session id
        \param sServiceName - service name
        \param sInstanceId - instance id
        */
    void FreeServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId);

    //! get service instance
    /*! \param sSessionId - session id
        \param sServiceName - service name
        \param sInstanceId - instance id
        \return service instance
        */
    PIService& GetServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId);

    //! get existing or create service instance
    /*! more convenient way to get another service instance within service
        \param pService - current service with sSessionId, sInstanceId info
        \param sServiceName - service name
        \return service instance
        */
    PIService& ServiceInstance(const IService* pService, const std::string& sServiceName);

  private:
    ServiceInstanceManager();
    ~ServiceInstanceManager();
    ServiceInstanceManager(const ServiceInstanceManager&);
    ServiceInstanceManager& operator=(const ServiceInstanceManager&);

  private:
    class ServiceInstanceManagerImpl;
    ServiceInstanceManagerImpl* m_pImpl;
  };

#ifndef STAFF_NO_DEPRECATED
  STAFF_DEPRECATED(ServiceInstanceManager) typedef ServiceInstanceManager CServiceInstanceManager;
#endif

}

#endif // SERVICEINSTANCEMANAGER_H
