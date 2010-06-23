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

#ifndef _SERVICEFACTORY_H_
#define _SERVICEFACTORY_H_

#include <typeinfo>
#include <string>

namespace staff
{
  class IService;
  class IProxyAllocator;

  //! service factory
  class CServiceFactory
  {
  public:
    //!         get instance of service factory
    static CServiceFactory& Inst();

    //!         free instance of service factory
    static void FreeInst();

    //!         allocate new object for work with service
    /*! examples:
        rise::CSharedPtr<CCalculator> pCalculator = staff::CServiceFactory::Inst().GetService<CCalculator>("http://localhost:9090/axis2/services/Calculator");
        rise::CSharedPtr<CCalculator> pCalculator = staff::CServiceFactory::Inst().GetService<CCalculator>("", "", "mycalc");

        \param  sServiceUri - service URI
        \param  sSessionId - session id
        \param  sInstanceId - instance id
        \return pointer to object for work with service
        */
    template<typename TServiceClientBaseType>
    TServiceClientBaseType* GetService(const std::string& sServiceUri = "",
                                       const std::string& sSessionId = "",
                                       const std::string& sInstanceId = "")
    {
      return static_cast<TServiceClientBaseType*>(AllocateClient(typeid(TServiceClientBaseType).name(),
                                                                      sServiceUri, sSessionId, sInstanceId));
    }

    //!         allocate new object for work with service
    /*! \param  sHost - service host
        \param  nPort - service port
        \param  sProtocol - protocol
        \param  sServiceName - service name
        \param  sSessionId - session id
        \param  sInstanceId - instance id
        \return pointer to object for work with service
        */
    template<typename TServiceClientBaseType>
    TServiceClientBaseType* GetServiceByHost(const std::string& sHost = "localhost",
                                             int nPort = 9090,
                                             const std::string& sProtocol = "http",
                                             const std::string& sServiceName = "",
                                             const std::string& sSessionId = "",
                                             const std::string& sInstanceId = "")
    {
      return static_cast<TServiceClientBaseType*>(AllocateClientByHost(typeid(TServiceClientBaseType).name(),
                                                                      sHost, nPort, sProtocol, sServiceName, sSessionId, sInstanceId));
    }

    //! internal function. Do not use.
    void RegisterProxyAllocator(const std::string& sProxyTypeId, IProxyAllocator& rProxyAllocator);

  private:
    CServiceFactory();
    ~CServiceFactory();

    staff::IService* AllocateClient(const std::string& sClientType, const std::string& sServiceUri,
                                    const std::string& sSessionId, const std::string& sInstanceId);

    staff::IService* AllocateClientByHost(const std::string& sClientType,
                                          const std::string& sHost,
                                          int nPort,
                                          const std::string& sProtocol,
                                          const std::string& sServiceName,
                                          const std::string& sSessionId,
                                          const std::string& sInstanceId);

  private:
    static CServiceFactory* m_pInst;  //!< instance
    class CServiceFactoryImpl;
    CServiceFactoryImpl* m_pImpl;
  };
}

#endif // _SERVICEFACTORY_H_

