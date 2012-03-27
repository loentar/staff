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

#include "IService.h"

namespace staff
{
  IService::IService()
  {
  }

  IService::~IService()
  {
  }

  void IService::OnCreate()
  {
  }

  void IService::OnDestroy()
  {
  }

  ServiceClient* IService::GetClient()
  {
    return NULL;
  }

  const std::string& IService::GetServiceName() const
  {
    return m_sServiceName;
  }

  const std::string& IService::GetSessionId() const
  {
    return m_sSessionId;
  }

  const std::string& IService::GetInstanceId() const
  {
    return m_sInstanceId;
  }

  void IService::Init(const std::string& sServiceName, const std::string& sSessionId, const std::string& sInstanceId)
  {
    m_sServiceName = sServiceName;
    m_sSessionId = sSessionId;
    m_sInstanceId = sInstanceId;
  }

}
