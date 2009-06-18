// This file generated by staff_codegen
// DO NOT EDIT

#include <map>
#include <rise/common/MutablePtr.h>
#include <staff/common/Exception.h>
#include <staff/common/Operation.h>
#include <staff/common/Value.h>
#include "$(Interface.Name)Wrapper.h"
#include "$(Interface.Name)Context.h"
#include "$(Interface.Name)Impl.h"

namespace staff
{
///////////////////////////////////////////////////////////////////////////////////////////////////////
// forward declarations
#foreach $(Interface.Structs)
CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.Name)& rstStruct);
const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.Name)& rstStruct);
#end

#foreach $(Interface.Typedefs)
#ifeq($(Typedef.DataType.IsTemplate),1)
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType);
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType);
#else
\
#ifeqend
#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#ifeq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have deserializator // !!list<struct>!!
#ifeq($(Typedef.DataType.IsTemplate),1)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType);
#else
\
#ifeqend
#else                 // !!not_a_struct!!
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType);
#ifeqend
#end // foreach $(Interface.Typedefs)

///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct serializators
#foreach $(Interface.Structs)
CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.Name)& rstStruct)
{
#ifneq($(Struct.Parent),)
  // serialize parent struct
  rdoParam << static_cast<const $(Struct.Parent)&>(rstStruct);

#else
\
#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Type),struct)
  CDataObject rdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
   rdoParam$(Param.Name) << rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),typedef)
  CDataObject rdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
  rdoParam$(Param.Name) << rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),dataobject)
  CDataObject rdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
  rdoParam$(Param.Name).AppendChild(const_cast<CDataObject&>(rstStruct.$(Param.Name)));
#else
#ifeq($(Param.DataType.Type),template)
  CDataObject rdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
  rdoParam$(Param.Name) << rstStruct.$(Param.Name);
#else
  rdoParam.CreateChild("$(Param.Name)", rstStruct.$(Param.Name)); // $(Param.DataType.Type)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end
  return rdoParam;
}

#end

///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct deserializators
#foreach $(Interface.Structs)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.Name)& rstStruct)
{
#ifneq($(Struct.Parent),)
  // deserialize parent struct
  rdoParam >> static_cast<$(Struct.Parent)&>(rstStruct);

#else
\
#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Type),struct)
  rdoParam("$(Param.Name)") >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),typedef)
  rdoParam("$(Param.Name)") >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),template)
  rdoParam("$(Param.Name)") >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),dataobject)
  rstStruct.$(Param.Name) = *rdoParam("$(Param.Name)").Begin();
#else
#ifeq($(Param.DataType.Name),staff::string)
  rstStruct.$(Param.Name) = rdoParam["$(Param.Name)"].AsString();
#else
#ifeq($(Param.DataType.Name),std::string)
  rstStruct.$(Param.Name) = rdoParam["$(Param.Name)"].AsString();
#else
#ifeq($(Param.DataType.Name),rise::CString)
  rstStruct.$(Param.Name) = rdoParam["$(Param.Name)"].AsString();
#else
  rstStruct.$(Param.Name) = rdoParam["$(Param.Name)"];
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end
  return rdoParam;
}

#end

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef serializators
#foreach $(Interface.Typedefs)

// $(Typedef.Name)  Typedef.DataType.Type $(Typedef.DataType.Type) $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.IsTemplate),1) // для всех контейнеров должен быть сериализатор
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType)
{
  for($(Typedef.Name)::const_iterator it = rtType.begin(); it != rtType.end(); ++it)
  {
    CDataObject tdoItem = rdoParam.CreateChild("Item");
#ifeq($(Typedef.DataType.Name),std::map)
    CDataObject tdoKey = tdoItem.CreateChild("Key");
    CDataObject tdoValue = tdoItem.CreateChild("Value");
    tdoKey << it->first;
    tdoValue << it->second;
#else
    tdoItem << *it;
#ifeqend
  }

  return rdoParam;
}
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have serializator
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType)
{
#ifeq($(Typedef.DataType.Type),generic)    // !!generic!!
  rdoParam.SetValue(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),dataobject) // !!dataobject!! 
  rdoParam.AppendChild(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),typedef)    // !!typedef!!
  return rdoParam << rtType;
#else
#ifeq($(Typedef.DataType.Type),template)    // !!template!!
  return rdoParam << rtType;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
}
#ifeqend

#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#end // foreach $(Interface.Typedefs)

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef deserializators
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have deserializator // !!list<struct>!!
#ifeq($(Typedef.DataType.IsTemplate),1)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType)
{
  for(CDataObject::ConstIterator it = rdoParam.Begin(); it != rdoParam.End(); ++it)
  {
    $(Typedef.DataType.Name) tItem;
    *it >> tItem;
    rtType.push_back(tItem);
  }
  return rdoParam;
}
#ifeqend
#else                 // !!not_a_struct!!
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType)
{
#ifeq($(Typedef.DataType.IsTemplate),1)
// container :: $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.Type),typedef)
  $(Typedef.DataType.Name) tItem;
#ifeqend
  for(CDataObject::ConstIterator it = rdoParam.Begin(); it != rdoParam.End(); ++it)
  {
#ifneq($(Typedef.DataType.Type),typedef)
#ifeq($(Typedef.DataType.Type),template)
#ifeq($(Typedef.DataType.Name),std::map)
    $(Typedef.DataType.TemplateParams.TemplateParam1) tKey;
    $(Typedef.DataType.TemplateParams.TemplateParam2) tValue;
#else
    $(Typedef.DataType.TemplateParams.TemplateParam1) tItem;
#ifeqend
#ifeqend
#ifeqend

#ifeq($(Typedef.DataType.Type),generic)
    rtType.push_back(it->GetValue());
#else
#ifeq($(Typedef.DataType.Type),dataobject)
    rtType.push_back(*(it->Begin()));
#else
#ifeq($(Typedef.DataType.Type),typedef)
    *it >> tItem;
    rtType.push_back(tItem);
#else
#ifeq($(Typedef.DataType.Type),template)
#ifeq($(Typedef.DataType.Name),std::map)
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)    // !!generic!!
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Name),std::string)
    tKey = (*it)["Key"].AsString();
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Name),staff::string)
    tKey = (*it)["Key"].AsString();
#else
    tKey = (*it)["Key"];
#ifeqend
#ifeqend
#else
    (*it)("Key") >> tKey;
#ifeqend
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),generic)    // !!generic!!
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Name),std::string)
    tValue = (*it)["Value"].AsString();
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Name),staff::string)
    tValue = (*it)["Value"].AsString();
#else
    tValue = (*it)["Value"];
#ifeqend
#ifeqend
#else
    (*it)("Value") >> tValue;
#ifeqend
    rtType[ tKey ] = tValue;
#else // ----------------------- list, vector, etc.
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)    // !!generic!!
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Name),std::string)    // !!string!!
    tItem = it->GetText();
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Name),staff::string)    // !!string!!
    tItem = it->GetText();
#else
    tItem = it->GetValue();
#ifeqend
#ifeqend
#else
    *it >> tItem;
#ifeqend
    rtType.push_back(tItem);
#ifeqend
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
  }
  return rdoParam;
#else // !!DataType.IsTemplate!!
// not container :: $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.Type),generic)    // !!generic!!
  rtType = rdoParam.GetValue();
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),dataobject) // !!dataobject!! 
  rtType = *rdoParam.Begin();
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),typedef)    // !!typedef!!
  return rdoParam << rtType;
#else
#ifeq($(Typedef.DataType.Type),template)    // !!template!!
  return rdoParam << rtType;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
}

#ifeqend
#end
}

#foreach $(Interface.Classes)

$(Class.OpeningNs)
class $(Class.Name)Wrapper::$(Class.Name)WrapperImpl
{
public:
  typedef rise::CMutablePtr< $(Class.Name)Impl > P$(Class.ServiceName)Impl;
  typedef std::map<rise::CString, P$(Class.ServiceName)Impl> TServiceMap;
  TServiceMap m_mServices;
  staff::CComponent* m_pComponent;
  static rise::CString m_sName;
  static rise::CString m_sDescr;
};

$(Class.Name)Wrapper::$(Class.Name)Wrapper(staff::CComponent* pComponent)
{
  m_pImpl = new $(Class.Name)WrapperImpl;
  m_pImpl->m_pComponent = pComponent;
}

$(Class.Name)Wrapper::$(Class.Name)Wrapper()
{
  m_pImpl = new $(Class.Name)WrapperImpl;
  m_pImpl->m_pComponent = NULL;
}

$(Class.Name)Wrapper::~$(Class.Name)Wrapper()
{
  if(m_pImpl != NULL)
  {
    delete m_pImpl;
    m_pImpl = NULL;
  }
}

$(Class.Name)Impl& $(Class.Name)Wrapper::ServiceImpl(const rise::CString& sID)
{
  return *$(Class.Name)Context::GetContext().GetServiceImpl(sID);
}

void $(Class.Name)Wrapper::Invoke( staff::COperation& rOperation, const rise::CString& sID )
{
  const staff::CDataObject& rRequest = rOperation.Request();
  staff::CDataObject& rResult = rOperation.Result();
  const rise::CString& sOperationName = rOperation.GetName();

  if (sOperationName == "GetServiceDescription")
  {
    rResult = GetServiceDescription();
  } else
#foreach $(Class.Members)
  if (sOperationName == "$(Member.Name)")
  {
    rOperation.SetResponseName("$(Member.Return.NodeName)");
#foreach $(Member.Params) // для структур и типов создаем локальные переменные 
#ifeq($(Param.DataType.Type),struct)     // !!struct!! 
    $(Param.DataType.Name) $(Param.Name);
#else
#ifeq($(Param.DataType.Type),typedef)    // !!typedef!!
    $(Param.DataType.Name) $(Param.Name);
#else
\
#ifeqend
#ifeqend
#end
\
#foreach $(Member.Params) // для структур и типов заполняем локальные переменные 
#ifeq($(Param.DataType.Type),struct)     // !!struct!! 
    rRequest("$(Param.Name)") >> $(Param.Name);
#else
#ifeq($(Param.DataType.Type),typedef)    // !!typedef!!
    rRequest("$(Param.Name)") >> $(Param.Name);
#else
\
#ifeqend
#ifeqend
#end
\
#ifeq($(Member.Return.Type),struct) // возвращаемое значение // !!struct!! 
    $(Member.Return.Name) tResult = \
#else
#ifeq($(Member.Return.Type),typedef)    // !!typedef!!
    $(Member.Return.Name) tResult = \
#else
#ifeq($(Member.Return.Type),generic)    // !!generic!!
#ifneq($(Member.Return.Name),void)      // !!not_void!!
    rOperation.ResultValue() = \
#else                                   // !!void!!
    \
#ifeqend
#else
#ifeq($(Member.Return.Type),dataobject) // !!dataobject!! 
    staff::CDataObject tResultDO = \
#ifeqend
#ifeqend
#ifeqend
#ifeqend // вызываем функцию
\
ServiceImpl(sID).$(Member.Name)(\
#foreach $(Member.Params)
#ifeq($(Param.$Num),0) // запятая между параметрами
\
#else
, \
#ifeqend // параметры
#ifeq($(Param.DataType.Type),generic)    // !!generic!!
rRequest["$(Param.Name)"]\
#else
#ifeq($(Param.DataType.Type),dataobject) // !!dataobject!! 
*rRequest.GetChildByLocalName("$(Param.Name)").Begin()\
#else
#ifeq($(Param.DataType.Type),struct)     // !!struct!! 
$(Param.Name)\
#else
#ifeq($(Param.DataType.Type),typedef)    // !!typedef!!
$(Param.Name)\
#else
#cgerror "Param.DataType.Type = $(Param.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end // конец параметров функции
);
#ifeq($(Member.Return.Type),dataobject) // !!dataobject!! 
    rOperation.Result().AppendChild(tResultDO);
#else
\
#ifeqend // конец вызова функции
#ifeq($(Member.Return.Type),struct) // результат для структур и типов // !!struct!! 
    rResult << tResult;
#else
#ifeq($(Member.Return.Type),typedef)    // !!typedef!!
    rResult << tResult;
#else
\
#ifeqend
#ifeqend
  } else
#end
  {
    RISE_THROWS(staff::CRemoteException, "Unknown Operation: " + rOperation.GetName());
  }
}

const rise::CString& $(Class.Name)Wrapper::GetName() const
{
  if($(Class.Name)WrapperImpl::m_sName.size() == 0)
  {
    $(Class.Name)WrapperImpl::m_sName = "$(Class.ServiceName)";
  }
  
  return $(Class.Name)WrapperImpl::m_sName;
}

const rise::CString& $(Class.Name)Wrapper::GetDescr() const
{
  if($(Class.Name)WrapperImpl::m_sDescr.size() == 0)
  {
    $(Class.Name)WrapperImpl::m_sDescr = "Staff service $(Class.Name)";
  }
  
  return $(Class.Name)WrapperImpl::m_sDescr;
}

const staff::CComponent* $(Class.Name)Wrapper::GetComponent() const
{
  return m_pImpl->m_pComponent;
}

staff::CComponent* $(Class.Name)Wrapper::GetComponent()
{
  return m_pImpl->m_pComponent;
}

void* $(Class.Name)Wrapper::GetImpl(const rise::CString& sID)
{
  return $(Class.Name)Context::GetContext().GetServiceImpl(sID);
}

staff::CDataObject $(Class.Name)Wrapper::GetOperations() const
{
  staff::CDataObject tOperations("Operations");

#foreach $(Class.Members)
  {// Operation: $(Member.Return.Name) $(Member.Name)($(Member.Params))$(Member.Const)
    staff::CDataObject tOp$(Member.Name) = tOperations.CreateChild("Operation");
    tOp$(Member.Name).CreateChild("Name", "$(Member.Name)");
    tOp$(Member.Name).CreateChild("IsConst", $(Member.IsConst));

    staff::CDataObject tOpReturn$(Member.Name) = tOp$(Member.Name).CreateChild("Return");
    tOpReturn$(Member.Name).CreateChild("Type", "$(Member.Return.Name)");
    tOpReturn$(Member.Name).CreateChild("IsConst", $(Member.Return.IsConst));

    staff::CDataObject tOp$(Member.Name)Params = tOp$(Member.Name).CreateChild("Parameters");
#foreach $(Member.Params)
    {
      staff::CDataObject tOpParam$(Param.Name) = tOp$(Member.Name)Params.CreateChild("Param");
      tOpParam$(Param.Name).CreateChild("Name", "$(Param.Name)");
      tOpParam$(Param.Name).CreateChild("Type", "$(Param.DataType.Name)");
      tOpParam$(Param.Name).CreateChild("IsConst", $(Param.DataType.IsConst));
      tOpParam$(Param.Name).CreateChild("IsRef", $(Param.DataType.IsRef));
    }
#end
  }
#end

  return tOperations;
}

staff::CDataObject $(Class.Name)Wrapper::GetServiceDescription() const
{
  staff::CDataObject tServiceDescription;
  staff::CDataObject tOperations = GetOperations();

  tServiceDescription.Create("ServiceDescription");
    
  tServiceDescription.CreateChild("Name", GetName());
  tServiceDescription.CreateChild("Description", GetDescr());
  
  tServiceDescription.AppendChild(tOperations);

  return tServiceDescription;
}

rise::CString $(Class.Name)Wrapper::$(Class.Name)WrapperImpl::m_sName;
rise::CString $(Class.Name)Wrapper::$(Class.Name)WrapperImpl::m_sDescr;

$(Class.EndingNs)
#end
