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
#include <staff/common/Value.h>
#include <staff/component/ServiceInstanceManager.h>
#include "DataSource.h"
#include "ProviderService.h"
#include "ProviderServiceWrapper.h"

namespace staff
{
namespace das
{
  ProviderServiceWrapper::ProviderServiceWrapper(CComponent* pComponent, const DataSource* pDataSource):
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

  CDataObject ProviderServiceWrapper::GetOperations() const
  {
    CDataObject tdoResult("Operations");

    const OperationsList& rlsOperations = m_pDataSource->GetOperations();
    for (OperationsList::const_iterator itOperation = rlsOperations.begin();
        itOperation != rlsOperations.end(); ++itOperation)
    {
      tdoResult.CreateChild("Operation").CreateChild("Name").SetText(itOperation->sName);
    }

    return tdoResult;
  }

  void ProviderServiceWrapper::Invoke(COperation& rOperation, const std::string& sSessionId, const std::string& sInstanceId)
  {
    const staff::CDataObject& rRequest = rOperation.Request();
    staff::CDataObject& rResult = rOperation.Result();
    const std::string& sOperationName = rOperation.GetName();

    if (sOperationName == "GetServiceDescription")
    {
      rResult = GetServiceDescription();
    }
    else
    if (sOperationName == "CreateInstance")
    {
      const std::string& sInstanceId = rRequest.GetChildByLocalName("sInstanceId").GetText();
      staff::CServiceInstanceManager::Inst().CreateServiceInstance(sSessionId, m_sName, sInstanceId);
    }
    else
    if (sOperationName == "FreeInstance")
    {
      const std::string& sInstanceId = rRequest.GetChildByLocalName("sInstanceId").GetText();
      staff::CServiceInstanceManager::Inst().FreeServiceInstance(sSessionId, m_sName, sInstanceId);
    }
    else
    {
      rise::CSharedPtr<ProviderService> tpService = GetImpl(sSessionId, sInstanceId);
      RISE_ASSERT(tpService);
      CDataObject tdoResponse = tpService->Invoke(rRequest);
      rOperation.SetResponse(tdoResponse);
    }
  }

  const CComponent* ProviderServiceWrapper::GetComponent() const
  {
    return m_pComponent;
  }

  CComponent* ProviderServiceWrapper::GetComponent()
  {
    return m_pComponent;
  }

  PIService& ProviderServiceWrapper::GetImpl(const std::string& sSessionId, const std::string& sInstanceId)
  {
    return staff::CServiceInstanceManager::Inst().GetServiceInstance(sSessionId, m_sName, sInstanceId);
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

  CDataObject ProviderServiceWrapper::GetServiceDescription() const
  {
    staff::CDataObject tServiceDescription;

    tServiceDescription.Create("ServiceDescription");
    tServiceDescription.DeclareDefaultNamespace("http://tempui.org/staff/service-description");

    tServiceDescription.CreateChild("Name", m_sName);
    tServiceDescription.CreateChild("Description", GetDescr());

    tServiceDescription.AppendChild(GetOperations());

    return tServiceDescription;
  }
}
}
