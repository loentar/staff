// This file generated by staff_codegen
// For more information please visit: http://code.google.com/p/staff/
// DO NOT EDIT

#ifneq($(Interface.Classes.$Count),0)
#include <rise/common/SharedPtr.h>
#ifeqend
#include <staff/utils/fromstring.h>
#include <staff/utils/tostring.h>
#include <staff/utils/HexBinary.h>
#include <staff/utils/Base64Binary.h>
#include <staff/common/Attribute.h>
#include <staff/common/Exception.h>
#include <staff/common/DataObject.h>
#ifneq($(Interface.Classes.$Count),0)
#include <staff/common/Operation.h>
#include <staff/common/IService.h>
#include <staff/component/ServiceInstanceManager.h>
#include "$(Interface.FilePath)$(Interface.Name)Impl.h"
#ifeqend // #ifneq($(Interface.Classes.$Count),0)
#foreach $(Interface.Includes)
#include "$(Include.FilePath)$(Include.Name)Wrapper.h"
#end
#include "$(Interface.FilePath)$(Interface.Name)Wrapper.h"

#cginclude <common/Serialization.cpp>

#ifneq($(Interface.Classes.$Count),0)
#foreach $(Interface.Classes)

$(Class.OpeningNs)

$(Class.Name)Wrapper::$(Class.Name)Wrapper(staff::Component* pComponent):
  m_pComponent(pComponent)
{
}

$(Class.Name)Wrapper::~$(Class.Name)Wrapper()
{
}

void $(Class.Name)Wrapper::Invoke(staff::Operation& rOperation, const std::string& sSessionId, const std::string& sInstanceId)
{
  const staff::DataObject& rRequest = rOperation.Request();
  const std::string& sOperationName = rOperation.GetName();

  if (sOperationName == "GetServiceDescription")
  {
    rOperation.SetResponse(GetServiceDescription());
  } else
  if (sOperationName == "CreateInstance")
  {
    staff::ServiceInstanceManager::Inst().CreateServiceInstance(sSessionId, m_sName,
                                                                rRequest.GetChildByLocalName("sInstanceId").GetText());
  } else
  if (sOperationName == "FreeInstance")
  {
    staff::ServiceInstanceManager::Inst().FreeServiceInstance(sSessionId, m_sName,
                                                              rRequest.GetChildByLocalName("sInstanceId").GetText());
  } else
  {
    rise::CSharedPtr<$(Class.Name)Impl> tpServiceImpl;
    tpServiceImpl = GetImpl(sSessionId, sInstanceId);
#foreach $(Class.Members)
    if (sOperationName == "$(Member.Name)")
    {
#ifneq($(Member.Options.*responseElement),)
      rOperation.SetResponseName("$(Member.Options.*responseElement)");
#else
#ifneq($(Member.Return.ResponseName),)
      rOperation.SetResponseName("$(Member.Return.ResponseName)");
#ifeqend
#ifeqend
#ifneq($(Member.Options.*resultElement),)
      rOperation.SetResultName("$(Member.Options.*resultElement)");
#ifeqend
#foreach $(Member.Params)
#ifeq($(Param.DataType.Type),struct||typedef||template||generic||enum)
#ifneq($(Param.DataType.Name),COperation||Operation)
      $(Param.DataType.NsName) $(Param.Name);
#ifeqend
#ifeqend
#end
\
#foreach $(Member.Params)  // ------------------- params ------------------------
\
#ifeq($(Param.DataType.Name),Optional||Nillable)
#ifeq($(Param.DataType.Name),Nillable)
      const staff::DataObject& rdoParam$(Param.Name) = rRequest.GetChildByLocalName("$(Param.Name)");
      if (!rdoParam$(Param.Name).IsNil())
#else
      const staff::DataObject& rdoParam$(Param.Name) = rRequest.GetChildByLocalNameOpt("$(Param.Name)");
      if (!rdoParam$(Param.Name).IsNull())
#ifeqend
      {
#var sParamNode rdoParam$(Param.Name)
#var sContext Param.DataType.TemplateParams.TemplateParam1
#var sOptMod *
#indent +
#else
#var sParamNode rRequest.GetChildByLocalName("$(Param.Name)")
#var sContext Param.DataType
#var sOptMod
#ifeqend
#context $($sContext)
\
#ifeq($(.Name),Abstract) // abstract type

#ifeq($(Param.Options.*isAttribute),true||1) // attribute
#cgerror Can't serialize abstract member into attribute. In struct $(Struct.NsName), member $(Param.Name)
#ifeqend
#ifneq($(.TemplateParams.TemplateParam1.Type),struct)
#cgerror Abstract template type is not struct. In struct $(Struct.NsName), member $(Param.Name)
#ifeqend
      $($sParamNode) >> $($sOptMod)$(Param.Name);
#else // not abstract
\
#ifeq($(.Type),struct||enum)
      $($sParamNode) >> $($sOptMod)$(Param.Name);
#else
#ifeq($(.Type),typedef)
      DeserializeTypedef_$(.NsName.!mangle)($($sParamNode), $($sOptMod)$(Param.Name));
#else
#ifeq($(.Type),generic)
#ifneq($(.Name),COperation||Operation)
      $($sParamNode).GetValue($($sOptMod)$(Param.Name));
#ifeqend
#else
#ifeq($(.Type),template)
\
      for (::staff::DataObject tdoItem = $($sParamNode).FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
      {
#ifeq($(.Name),map||multimap)  // -------- map ----------
        $(.TemplateParams.TemplateParam1) tKey\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
        tdoItem.GetChildByLocalName("Key").GetValue(tKey);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
        DataObject tdoKey = tdoItem.GetChildByLocalName("Key");
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
        tdoKey >> tKey;
#else
        DeserializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoKey, tKey);
#ifeqend
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend

        $(.TemplateParams.TemplateParam2)& rValue = ($($sOptMod)$(Param.Name))[tKey];
#ifeq($(.TemplateParams.TemplateParam2.Type),generic||string)
        tdoItem.GetChildByLocalName("Value").GetValue(rValue);
#else
#ifeq($(.TemplateParams.TemplateParam2.Type),struct||typedef||enum)
        DataObject tdoValue = tdoItem.GetChildByLocalName("Value");
#ifneq($(.TemplateParams.TemplateParam2.Type),typedef)
        tdoValue >> rValue;
#else
        DeserializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoValue, rValue);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam2.Type),dataobject)
        rValue = tdoItem.GetChildByLocalName("Value").FirstChild();
#else
#cgerror key element type $(.TemplateParams.TemplateParam2.Type) is not supported
#ifeqend
#ifeqend
#ifeqend

#else // -------- list ----------
\
#ifeq($(.Name),list||vector)
#ifeq($(.TemplateParams.TemplateParam1.Type),generic)
        $(.TemplateParams.TemplateParam1) tItem = 0;
        tdoItem.GetValue(tItem);
        ($($sOptMod)$(Param.Name)).push_back(tItem);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),string)
        ($($sOptMod)$(Param.Name)).push_back(tdoItem.GetText());
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
        $(.TemplateParams.TemplateParam1)& rItem = *($($sOptMod)$(Param.Name))\
.insert(($($sOptMod)$(Param.Name)).end(), $(.TemplateParams.TemplateParam1)());
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
        tdoItem >> rItem;
#else
        DeserializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoItem, rItem);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),dataobject)
        ($($sOptMod)$(Param.Name)).push_back(tdoItem);
#else
#cgerror item element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#else
#cgerror template type $(.NsName) is not supported
#ifeqend
\
#ifeqend
      }
#ifeqend // template
#ifeqend
#ifeqend
#ifeqend
#ifeqend
\
#ifeq($(Param.DataType.Name),Optional||Nillable)
#indent -
      }
#ifeqend
#contextend
\
#end       // ----------------- return -------------------
      \
#ifneq($(Member.Return.Name),void)      // !!not_void!!
#ifeq($(Member.Return.Type),struct||typedef||template||enum)
$(Member.Return.NsName) tResult = \
#else
#ifeq($(Member.Return.Type),generic||string)    // !!generic!!
rOperation.Result().SetValue(\
#else
#ifeq($(Member.Return.Type),dataobject) // !!dataobject!!
rOperation.Result().AppendChild(
#ifeqend
#ifeqend
#ifeqend
#ifeqend // invoke an function
\
tpServiceImpl->$(Member.Name)(\
#foreach $(Member.Params)
#ifneq($(Param.$Num),0) // param splitter
, \
#ifeqend // params
#ifeq($(Param.DataType.Name),CMessageContext||MessageContext)
rOperation.GetMessageContext()\
#else
#ifeq($(Param.DataType.Name),COperation||Operation)
rOperation\
#else
#ifeq($(Param.DataType.Type),string)    // !!string!!
$($sParamNode).GetText()\
#else
#ifeq($(Param.DataType.Type),dataobject) // !!dataobject!! 
$($sParamNode).FirstChild()\
#else
#ifeq($(Param.DataType.Type),struct||typedef||template||generic||enum)
$(Param.Name)\
#else
#cgerror "Param.DataType.Type = $(Param.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end // end of function param list
\
#ifneq($(Member.Return.Name),void)
#ifeq($(Member.Return.Type),generic||string||dataobject)
#ifneq($(Member.Return.Name),Optional||Nillable)
)\
#ifeqend
#ifeqend
#ifeqend
);
\
\
#ifeq($(Member.Return.Name),Optional||Nillable)
      if (!tResult.IsNull())
      {
#indent +
#var sContext Member.Return.TemplateParams.TemplateParam1
#var sOptMod *
#else
#var sContext Member.Return
#var sOptMod
#ifeqend
#context $($sContext)
\
#ifeq($(.Name),Abstract) // abstract type
      staff::DataObject& rdoResult = rOperation.Result();
      rdoResult << $($sOptMod)tResult;
#else // not abstract
\
#ifeq($(.Type),generic||string)
#ifeq($(Member.Return.Name),Optional||Nillable)
      rOperation.Result().SetValue($($sOptMod)tResult);
#ifeqend
#else
#ifeq($(.Type),struct||typedef||enum) // result for structs and types
#ifneq($(.Type),typedef)
      rOperation.Result() << $($sOptMod)tResult;
#else
      SerializeTypedef_$(.NsName.!mangle)(rOperation.Result(), $($sOptMod)tResult);
#ifeqend
#else
#ifeq($(.Type),template)
      staff::DataObject& rdoResult = rOperation.Result();
      for ($(.NsName)::const_iterator it = ($($sOptMod)tResult).begin(); it != ($($sOptMod)tResult).end(); ++it)
      {
#ifeq($(.Name),map||multimap)
        DataObject tdoItem = rdoResult.CreateChild("Item");
\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
        tdoItem.CreateChild("Key").SetValue(it->first);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
        DataObject tdoKey = tdoItem.CreateChild("Key");
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
        tdoKey << it->first;
#else
        SerializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoKey, it->first);
#ifeqend
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
\
#ifeq($(.TemplateParams.TemplateParam2.Type),generic||string)
        tdoItem.CreateChild("Value").SetValue(it->second);
#else
#ifeq($(.TemplateParams.TemplateParam2.Type),struct||typedef||enum)
        DataObject tdoValue = tdoItem.CreateChild("Value");
#ifneq($(.TemplateParams.TemplateParam2.Type),typedef)
        tdoValue << it->second;
#else
        SerializeTypedef_$(.TemplateParams.TemplateParam2.NsName.!mangle)(tdoValue, it->second);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam2.Type),dataobject)
        tdoItem.CreateChild("Value").AppendChild(it->second);
#else
#cgerror key element type $(.TemplateParams.TemplateParam2.Type) is not supported
#ifeqend
#ifeqend
#ifeqend

#else
\
#ifeq($(.Name),list||vector)
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
        rdoResult.CreateChild("Item").SetValue(*it);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
        DataObject tdoItem = rdoResult.CreateChild("Item");
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
        tdoItem << *it;
#else
        SerializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoItem << *it);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),dataobject)
        rdoResult.CreateChild("Item").AppendChild(*it);
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#else
#cgerror template type $(.NsName) is not supported
#ifeqend
\
#ifeqend
      }
#ifeqend // template
#ifeqend // struct, typedef, enum
#ifeqend // generic, string
#ifeqend // abstract
\
#ifeq($(Member.Return.Name),Optional||Nillable)
#indent -
      }
#ifeqend
#ifeq($(Member.Return.Name),Nillable)
      else
      {
        rOperation.Result().SetNil();
      }
#ifeqend
\
#contextend
    } else
#end
    {
      RISE_THROWS(staff::RemoteException, "Unknown Operation: " + rOperation.GetName());
    }
  }
}

const std::string& $(Class.Name)Wrapper::GetName() const
{
  return m_sName;
}

const std::string& $(Class.Name)Wrapper::GetDescr() const
{
  return m_sDescr;
}

const staff::Component* $(Class.Name)Wrapper::GetComponent() const
{
  return m_pComponent;
}

staff::Component* $(Class.Name)Wrapper::GetComponent()
{
  return m_pComponent;
}

staff::PIService& $(Class.Name)Wrapper::GetImpl(const std::string& sSessionId, const std::string& sInstanceId)
{
  return staff::ServiceInstanceManager::Inst().GetServiceInstance(sSessionId, m_sName, sInstanceId);
}

staff::PIService $(Class.Name)Wrapper::NewImpl()
{
  return new $(Class.Name)Impl;
}

bool $(Class.Name)Wrapper::IsLoadAtStartup() const
{
  return \
#ifeq($(Class.Options.*loadAtStartup),true)
true;
#else
false;
#ifeqend
}

std::string $(Class.Name)Wrapper::GetDependencies() const
{
  return "$(Class.Options.*dependencies)";
}

staff::DataObject $(Class.Name)Wrapper::GetOperations() const
{
  staff::DataObject tOperations("Operations");

#foreach $(Class.Members)
  {// Operation: $(Member.Return.NsName) $(Member.Name)($(Member.Params))$(Member.Const)
    staff::DataObject tOp$(Member.Name) = tOperations.CreateChild("Operation");
    tOp$(Member.Name).CreateChild("Name", "$(Member.Name)");
    tOp$(Member.Name).CreateChild("RestMethod", "$(Member.Options.*restMethod)");
    tOp$(Member.Name).CreateChild("RestLocation", "$(Member.Options.*restLocation)");
  }
#end

  return tOperations;
}

staff::DataObject $(Class.Name)Wrapper::GetServiceDescription() const
{
  staff::DataObject tServiceDescription;

  tServiceDescription.Create("ServiceDescription");
  tServiceDescription.DeclareDefaultNamespace("http://tempui.org/staff/service-description");

  tServiceDescription.CreateChild("Name", m_sName);
  tServiceDescription.CreateChild("Description", m_sDescr);

  tServiceDescription.AppendChild(GetOperations());

  return tServiceDescription;
}

const std::string $(Class.Name)Wrapper::m_sName = "$(Class.ServiceNsName)";
const std::string $(Class.Name)Wrapper::m_sDescr = \
#ifneq($(Class.Description),)
"$(Class.Description)";
#else
"Staff service $(Class.ServiceName)";
#ifeqend

$(Class.EndingNs)
#end
#ifeqend // #ifneq($(Interface.Classes.$Count),0)
