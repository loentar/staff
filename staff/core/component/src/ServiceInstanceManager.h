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

#include <string>

namespace rise
{
  template<typename Type> class CMutablePtr;
}

namespace staff
{
  class IService;
  class CServiceWrapper;

  typedef rise::CMutablePtr<IService> PIService;

  class CServiceInstanceManager
  {
  public:
    static CServiceInstanceManager& Inst();
    static void FreeInst();

    void CreateSession(const std::string& sSessionId);
    void FreeSession(const std::string& sSessionId);

    PIService& CreateServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId);
    void FreeServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId);

    PIService& GetServiceInstance(const std::string& sSessionId, const std::string& sServiceName, const std::string& sInstanceId);

  protected:
    CServiceInstanceManager();
    ~CServiceInstanceManager();

  private:
    static CServiceInstanceManager* m_pInst;
    class CServiceInstanceManagerImpl;
    CServiceInstanceManagerImpl* m_pImpl;
  };
}

#endif // SERVICEINSTANCEMANAGER_H
