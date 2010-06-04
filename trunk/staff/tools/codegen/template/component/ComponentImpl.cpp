// This file generated by staff_codegen
// For more information please visit: http://code.google.com/p/staff/
// DO NOT EDIT

#include <rise/plugin/PluginExport.h>
#include <rise/common/MutablePtr.h>
#include <rise/common/containertypes.h>
#include <staff/component/ServiceWrapperFactory.h>
#include <staff/component/ServiceInstanceManager.h>
#include <staff/security/tools.h>
#foreach $(Project.Interfaces)
#include "$(Interface.Name)Wrapper.h"
#end
#include "ComponentImpl.h"

RISE_DECLARE_PLUGIN($(Project.Namespace)CComponentImpl)

$(Project.OpeningNs)
  CComponentImpl::CComponentImpl()
  {
#foreach $(Project.Interfaces)
#foreach $(Interface.Classes)
    {
      staff::PServiceWrapper tpServiceWrapper(new $(Class.NsName)Wrapper(this));
      m_mServices["$(Class.ServiceNsName)"] = tpServiceWrapper;
      staff::CServiceWrapperFactory::Inst().RegisterServiceWrapper("$(Class.ServiceNsName)", tpServiceWrapper);
#ifeq($(Class.LoadAtStartup),true)
      // load service at startup
      staff::CServiceInstanceManager::Inst().CreateServiceInstance(STAFF_SECURITY_NOBODY_SESSION_ID, "$(Class.ServiceNsName)", "");
#else
\
#ifeqend
    }
#end
#end
  }

  CComponentImpl::~CComponentImpl()
  {
  }

  const std::string& CComponentImpl::GetName() const
  {
    return m_sName;
  }

  const staff::CServiceWrapper* CComponentImpl::GetService( const std::string& sService ) const
  {
    staff::TServiceWrapperMap::const_iterator itService = m_mServices.find(sService);
    if (itService == m_mServices.end())
      return NULL;

    return itService->second;
  }

  staff::CServiceWrapper* CComponentImpl::GetService( const std::string& sService )
  {
    staff::TServiceWrapperMap::iterator itService = m_mServices.find(sService);
    if (itService == m_mServices.end())
    {
      return NULL;
    }

    return itService->second;
  }

  const staff::TServiceWrapperMap& CComponentImpl::GetServices() const
  {
    return m_mServices;
  }

  const std::string CComponentImpl::m_sName = "$(Project.ComponentName)";

$(Project.EndingNs)
