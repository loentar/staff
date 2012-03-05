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

#include <rise/common/MutablePtr.h>
#include <rise/common/SharedPtr.h>
#include <staff/common/Operation.h>
#include <staff/common/DataObject.h>
#include <staff/component/ServiceInstanceManager.h>
#include <staff/component/Component.h>
#include "DataSource.h"
#include "ProviderService.h"
#include "ProviderServiceWrapper.h"

namespace staff
{
namespace das
{
  ProviderServiceWrapper::ProviderServiceWrapper(Component* pComponent, const DataSource* pDataSource):
    m_pComponent(pComponent), m_pDataSource(pDataSource)
  {
    m_sName = m_pDataSource->GetNamespace();
    if (m_sName.size() != 0)
    {
      m_sName += '.';
    }
    m_sName += m_pDataSource->GetName();
  }


  const std::string& ProviderServiceWrapper::GetName() const
  {
    return m_sName;
  }

  const std::string& ProviderServiceWrapper::GetDescr() const
  {
    return m_pDataSource->GetDescr();
  }

  DataObject ProviderServiceWrapper::GetOperations() const
  {
    DataObject tdoResult("Operations");

    StringMap::const_iterator itTmp;

    const OperationsList& rlsOperations = m_pDataSource->GetOperations();
    for (OperationsList::const_iterator itOperation = rlsOperations.begin();
        itOperation != rlsOperations.end(); ++itOperation)
    {
      const Operation& rOperation = *itOperation;

      DataObject tdoOperation = tdoResult.CreateChild("Operation");
      tdoOperation.CreateChild("Name", rOperation.sName);

      itTmp = rOperation.mOptions.find("restMethod");
      if (itTmp != rOperation.mOptions.end())
      {
        tdoOperation.CreateChild("RestMethod", itTmp->second);
      }

      itTmp = rOperation.mOptions.find("restLocation");
      if (itTmp != rOperation.mOptions.end())
      {
        tdoOperation.CreateChild("RestLocation", itTmp->second);
      }
    }

    return tdoResult;
  }

  void ProviderServiceWrapper::Invoke(staff::Operation& rOperation, const std::string& sSessionId,
                                      const std::string& sInstanceId)
  {
    const staff::DataObject& rRequest = rOperation.Request();
    const std::string& sOperationName = rOperation.GetName();

    if (sOperationName == "GetServiceDescription")
    {
      rOperation.SetResponse(GetServiceDescription());
    }
    else
    if (sOperationName == "CreateInstance")
    {
      const std::string& sInstanceId = rRequest.GetChildTextByLocalName("sInstanceId");
      staff::ServiceInstanceManager::Inst().CreateServiceInstance(sSessionId, m_sName, sInstanceId);
    }
    else
    if (sOperationName == "FreeInstance")
    {
      const std::string& sInstanceId = rRequest.GetChildTextByLocalName("sInstanceId");
      staff::ServiceInstanceManager::Inst().FreeServiceInstance(sSessionId, m_sName, sInstanceId);
    }
    else
    {
      rise::CSharedPtr<ProviderService> tpService = GetImpl(sSessionId, sInstanceId);
      RISE_ASSERT(tpService);
      tpService->Invoke(rRequest, rOperation.GetResponse());
    }

    rOperation.GetResponse().SetNamespaceUriGenPrefix("http://tempui.org/" + m_pComponent->GetName());
  }

  const Component* ProviderServiceWrapper::GetComponent() const
  {
    return m_pComponent;
  }

  Component* ProviderServiceWrapper::GetComponent()
  {
    return m_pComponent;
  }

  PIService& ProviderServiceWrapper::GetImpl(const std::string& sSessionId, const std::string& sInstanceId)
  {
    return staff::ServiceInstanceManager::Inst().GetServiceInstance(sSessionId, m_sName, sInstanceId);
  }

  PIService ProviderServiceWrapper::NewImpl()
  {
    return new ProviderService;
  }

  bool ProviderServiceWrapper::IsLoadAtStartup() const
  {
    return false;
  }

  std::string ProviderServiceWrapper::GetDependencies() const
  {
    return "";
  }

  DataObject ProviderServiceWrapper::GetServiceDescription() const
  {
    staff::DataObject tServiceDescription;

    tServiceDescription.Create("ServiceDescription");
    tServiceDescription.DeclareDefaultNamespace("http://tempui.org/staff/service-description");

    tServiceDescription.CreateChild("Name", m_sName);
    tServiceDescription.CreateChild("Description", GetDescr());

    tServiceDescription.AppendChild(GetOperations());

    return tServiceDescription;
  }
}
}
