#include <rise/common/MutablePtr.h>
#include <staff/security/tools.h>
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
    for (OperationsList::const_iterator itOperation = rlsOperations .begin();
        itOperation != rlsOperations.end(); ++itOperation)
    {
      const Operation& rOperation = *itOperation;
      CDataObject tdoOperation = tdoResult.CreateChild("Operation");

      tdoOperation.CreateChild("Name").SetText(rOperation.sName);
      tdoOperation.CreateChild("IsConst", 0);

      CDataObject tdoParams = tdoOperation.CreateChild("Parameters");

      for (TypesList::const_iterator itParam = rOperation.lsParams.begin();
            itParam != rOperation.lsParams.end(); ++itParam)
      {
        const Type& rParam = *itParam;

        CDataObject tdoParam = tdoParams.CreateChild("Parameter");

        tdoParam.CreateChild("Name").SetText(rParam.sName);
        tdoParam.CreateChild("Type").SetText(rParam.sType);
        tdoParam.CreateChild("IsConst", 0);
        tdoParam.CreateChild("IsRef", 0);
      }

      std::string sReturnType;

      if (rOperation.stReturn.eType == Type::List || rOperation.stReturn.eType == Type::Struct)
      {
        sReturnType = rOperation.stReturn.sName; // using "<struct name>", not "struct"
      }
      else
      {
        sReturnType = rOperation.stReturn.sType;
      }

      CDataObject tdoReturn = tdoOperation.CreateChild("Return");
      tdoReturn.CreateChild("Type").SetText(sReturnType);
      tdoReturn.CreateChild("IsConst", 0);
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
      ProviderService* pService = static_cast<ProviderService*>(GetImpl(sSessionId, sInstanceId));
      RISE_ASSERT(pService);
      CDataObject tdoResponse = pService->Invoke(rRequest);
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

  IService* ProviderServiceWrapper::GetImpl(const std::string& sSessionId, const std::string& sInstanceId)
  {
    return staff::CServiceInstanceManager::Inst().GetServiceInstance(sSessionId, m_sName, sInstanceId).Get();
  }

  IService* ProviderServiceWrapper::NewImpl()
  {
    return new ProviderService;
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
