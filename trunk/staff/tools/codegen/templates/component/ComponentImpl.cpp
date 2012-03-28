// This file generated by staff_codegen
// For more information please visit: http://code.google.com/p/staff/
// DO NOT EDIT

#include <staff/utils/PluginExport.h>
#include <staff/utils/SharedPtr.h>
#foreach $(Project.Interfaces)
#include "$(Interface.FilePath)$(Interface.Name)Wrapper.h"
#end
#include "ComponentImpl.h"

STAFF_DECLARE_PLUGIN($(Project.Namespace)ComponentImpl)

$(Project.OpeningNs)
  ComponentImpl::ComponentImpl()
  {
#foreach $(Project.Interfaces)
#foreach $(Interface.Classes)
    RegisterService("$(Class.ServiceNsName)", new $(Class.NsName)Wrapper(this));
#end
#end
  }

  ComponentImpl::~ComponentImpl()
  {
  }

  const std::string& ComponentImpl::GetName() const
  {
    return m_sName;
  }

  const std::string ComponentImpl::m_sName = "$(Project.Namespace.!dot)";

$(Project.EndingNs)

