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

#include <map>
#include <rise/common/MutablePtr.h>
#include "ServiceWrapper.h"
#include "ServiceWrapperFactory.h"

namespace staff
{
  CServiceWrapperFactory& CServiceWrapperFactory::Inst()
  {
    if (!m_pInst)
    {
      m_pInst = new CServiceWrapperFactory;
    }

    return *m_pInst;
  }

  void CServiceWrapperFactory::FreeInst()
  {
    if (m_pInst)
    {
      delete m_pInst;
      m_pInst = NULL;
    }
  }

  CServiceWrapperFactory::CServiceWrapperFactory()
  {
  }

  CServiceWrapperFactory::~CServiceWrapperFactory()
  {
  }

  void CServiceWrapperFactory::RegisterServiceWrapper(const std::string& sServiceName, PServiceWrapper& rpServiceWrapper)
  {
    m_mWrappers[sServiceName] = rpServiceWrapper;
  }

  void CServiceWrapperFactory::UnregisterServiceWrapper(const std::string& sServiceName)
  {
    m_mWrappers.erase(sServiceName);
  }

  PServiceWrapper& CServiceWrapperFactory::GetServiceWrapper(const std::string& sServiceName)
  {
    TServiceWrapperMap::iterator itWrapper = m_mWrappers.find(sServiceName);
    RISE_ASSERTES(itWrapper != m_mWrappers.end(), rise::CLogicNoItemException, "Service Wrapper " +
                  sServiceName + " is not registered");
    return itWrapper->second;
  }

  CServiceWrapperFactory* CServiceWrapperFactory::m_pInst = NULL;

}
