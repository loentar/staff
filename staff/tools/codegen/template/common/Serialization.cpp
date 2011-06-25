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

#ifeq($(Typedef.DataType.IsTemplate),1)
DataObject& operator<<(DataObject& rdoParam, const $(Typedef.NsName)& rtType)
{
  for ($(Typedef.NsName)::const_iterator it = rtType.begin(); it != rtType.end(); ++it)
  {
#var ElementName $(Typedef.Options.*elementName||"Item")
#ifeq($(Typedef.DataType.NsName),std::map)
\
    DataObject tdoItem = rdoParam.CreateChild("$($ElementName)");

#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.CreateChild("Key", it->first);
#else
    DataObject tdoKey = tdoItem.CreateChild("Key");
    tdoKey << it->first;
#ifeqend

#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),generic||string)
    tdoItem.CreateChild("Value", it->second);
#else
    DataObject tdoValue = tdoItem.CreateChild("Value");
    tdoValue << it->second;
#ifeqend
\
#else
\
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    rdoParam.CreateChild("$($ElementName)", *it);
#else
    DataObject tdoItem = rdoParam.CreateChild("$($ElementName)");
    tdoItem << *it;
#ifeqend
\
#ifeqend
  }

  return rdoParam;
}
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)
DataObject& operator<<(DataObject& rdoParam, const $(Typedef.NsName)& rtType)
{
#ifeq($(Typedef.DataType.Type),generic||string)
  rdoParam.SetValue(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),dataobject)
  rdoParam.AppendChild(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),typedef||template)
  return rdoParam << rtType;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
}
#ifeqend

#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#ifeqend //ifeq($(Typedef.Extern),0) // do not serialize extern type
#end // foreach $(Interface.Structs)
#ifeqend

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef deserializators
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0)
#ifneq($(Typedef.DataType.Type),struct)
const DataObject& operator>>(const DataObject& rdoParam, $(Typedef.NsName)& rtType)
{
#ifeq($(Typedef.DataType.Type),generic||string)
  rdoParam.GetValue(rtType);
  return rdoParam;
#else
\
\
#ifeq($(Typedef.DataType.Type),dataobject)
  rtType = rdoParam.FirstChild();
  return rdoParam;
#else
\
\
#ifeq($(Typedef.DataType.Type),typedef)
  return rdoParam << rtType;
#else
\
\
#ifeq($(Typedef.DataType.Type),template)
#ifeq($(Typedef.DataType.NsName),std::map)
\
  $(Typedef.DataType.TemplateParams.TemplateParam1) tKey\
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
  $(Typedef.DataType.TemplateParams.TemplateParam2) tValue\
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),generic)
 = 0\
#ifeqend
;
\
#else
\
  $(Typedef.DataType.TemplateParams.TemplateParam1) tItem\
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
#ifeqend
  for (DataObject tdoItem = rdoParam.FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
#ifeq($(Typedef.DataType.NsName),std::map)
\
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),string||generic)
    tdoItem.GetChildByLocalName("Key").GetValue(tKey);
#else
    tdoItem.GetChildByLocalName("Key") >> tKey;
#ifeqend
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),string||generic)
    tdoItem.GetChildByLocalName("Value").GetValue(tValue);
#else
    tdoItem.GetChildByLocalName("Value") >> tValue;
#ifeqend
    rtType[tKey] = tValue;
#else
\
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.GetValue(tItem);
#else
    tdoItem >> tItem;
#ifeqend
    rtType.push_back(tItem);
\
#ifeqend
  }
  return rdoParam;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
}

#ifeqend // not struct
#ifeqend // ifeq($(Typedef.Extern),0) // do not serialize extern type
#end
}
