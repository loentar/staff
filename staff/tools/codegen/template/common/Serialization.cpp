namespace staff
{
#ifneq($(Interface.Enums.$Count),0)
///////////////////////////////////////////////////////////////////////////////////////////////////////
// enums
#foreach $(Interface.Enums)
#ifeq($(Enum.Extern),0) // do not serialize extern type
#cginclude "EnumSerialization.cpp"
#ifeqend
#end

#ifeqend // enums
#ifneq($(Interface.Structs.$Count),0)
///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct serializators/deserializators
#foreach $(Interface.Structs)
#cginclude "StructSerialization.cpp"
#end

#ifeqend
#ifneq($(Interface.Typedefs.$Count),0)
///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef serializators
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0) // do not serialize extern type

DataObject& SerializeTypedef_$(Typedef.NsName.!mangle)(DataObject& rdoParam, const $(Typedef.NsName)& rtType)
{
#ifeq($(Typedef.DataType.Type),generic||string)
  rdoParam.SetValue(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),dataobject)
  rdoParam.AppendChild(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),typedef)
  return SerializeTypedef_$(Typedef.DataType.NsName.!mangle)(rdoParam, rtType);
#else
#ifeq($(Typedef.DataType.Type),struct)
  return rdoParam << rtType;
#else
#ifeq($(Typedef.DataType.Type),template)
  for ($(Typedef.NsName)::const_iterator it = rtType.begin(); it != rtType.end(); ++it)
  {
#var ElementName $(Typedef.Options.*elementName||"Item")
#ifeq($(Typedef.DataType.Name),map||multimap)
\
    DataObject tdoItem = rdoParam.CreateChild("$($ElementName)");

#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.CreateChild("Key", it->first);
#else
    DataObject tdoKey = tdoItem.CreateChild("Key");
#ifneq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),typedef)
    tdoKey << it->first;
#else
    SerializeTypedef_$(Typedef.DataType.TemplateParams.TemplateParam1.NsName.!mangle)(tdoKey, it->first);
#ifeqend
#ifeqend

#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),generic||string)
    tdoItem.CreateChild("Value", it->second);
#else
    DataObject tdoValue = tdoItem.CreateChild("Value");
#ifneq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),typedef)
    tdoValue << it->second;
#else
    SerializeTypedef_$(Typedef.DataType.TemplateParams.TemplateParam2.NsName.!mangle)(tdoValue, it->second);
#ifeqend
#ifeqend
\
#else
\
#ifeq($(Typedef.DataType.Name),list||vector)
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    rdoParam.CreateChild("$($ElementName)", *it);
#else
    DataObject tdoItem = rdoParam.CreateChild("$($ElementName)");
#ifneq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),typedef)
    tdoItem << *it;
#else
    SerializeTypedef_$(Typedef.DataType.TemplateParams.TemplateParam1.NsName.!mangle)(tdoItem, *it);
#ifeqend
#ifeqend
#else
#cgerror template type $(.NsName) is not supported
#ifeqend
\
#ifeqend
  }

  return rdoParam;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type) while serializing typedef"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // ifeq($(Typedef.DataType.Type),template)
}

#ifeqend //ifeq($(Typedef.Extern),0) // do not serialize extern type
#end // foreach $(Interface.Structs)
#ifeqend

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef deserializators
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0)
const DataObject& DeserializeTypedef_$(Typedef.NsName.!mangle)\
(const DataObject& rdoParam, $(Typedef.NsName)& rtType)
{
#ifeq($(Typedef.DataType.Type),generic||string)
  rdoParam.GetValue(rtType);
  return rdoParam;
#else
\
#ifeq($(Typedef.DataType.Type),dataobject)
  rtType = rdoParam.FirstChild();
  return rdoParam;
#else
\
#ifeq($(Typedef.DataType.Type),typedef)
  return DeserializeTypedef_$(Typedef.DataType.NsName.!mangle)(rdoParam, rtType);
#else
\
#ifeq($(Typedef.DataType.Type),struct)
  return rdoParam >> rtType;
#else
\
#ifeq($(Typedef.DataType.Type),template)
  for (DataObject tdoItem = rdoParam.FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
#ifeq($(Typedef.DataType.Name),map||multimap)
\
    $(Typedef.DataType.TemplateParams.TemplateParam1) tKey\
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),string||generic)
    tdoItem.GetChildByLocalName("Key").GetValue(tKey);
#else
#ifneq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),typedef)
    tdoItem.GetChildByLocalName("Key") >> tKey;
#else
    DeserializeTypedef_$(Typedef.DataType.TemplateParams.TemplateParam1)\
(tdoItem.GetChildByLocalName("Key"), tKey);
#ifeqend
#ifeqend

    $(Typedef.DataType.TemplateParams.TemplateParam2)& rValue = rtType[tKey];
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),string||generic)
    tdoItem.GetChildByLocalName("Value").GetValue(rValue);
#else
#ifneq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),typedef)
    tdoItem.GetChildByLocalName("Value") >> rValue;
#else
    DeserializeTypedef_$(Typedef.DataType.TemplateParams.TemplateParam1)\
(tdoItem.GetChildByLocalName("Value"), rValue);
#ifeqend
#ifeqend
#else  //  ----- list -----
\
#ifeq($(Typedef.DataType.Name),list||vector)
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)
    $(Typedef.DataType.TemplateParams.TemplateParam1) tItem = 0;
    tdoItem.GetValue(tItem);
    rtType.push_back(tItem);
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),string)
    rtType.push_back(tdoItem.GetText());
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    $(Typedef.DataType.TemplateParams.TemplateParam1)& rItem = *rtType\
.insert(rtType.end(), $(Typedef.DataType.TemplateParams.TemplateParam1)());
#ifneq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),typedef)
    tdoItem >> rItem;
#else
    DeserializeTypedef_$(Typedef.DataType.TemplateParams.TemplateParam1.NsName.!mangle)(tdoItem, rItem);
#ifeqend
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    rtType.push_back(tdoItem);
#else
#cgerror item element type $(Typedef.DataType.TemplateParams.TemplateParam1.Type) is not supported
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
  return rdoParam;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type) while deserializing typedef"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
}

#ifeqend // ifeq($(Typedef.Extern),0) // do not serialize extern type
#end
}
