namespace staff
{
  class DataObject;
#foreach $(Interface.Enums)
  DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue);
#end
#foreach $(Interface.Structs)
#cginclude "StructSerialization.h"
#end
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.DataType.IsTemplate),1) // there must be an serializer for each container
  DataObject& operator<<(DataObject& rdoParam, const $(Typedef.NsName)& rtType);
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have serializator
  DataObject& operator<<(DataObject& rdoParam, const $(Typedef.NsName)& rtType);
#ifeqend // ifneq($(Typedef.DataType.Type),struct
#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#ifeq($(Typedef.DataType.IsTemplate),1) // there must be an serializer for each container
  const DataObject& operator>>(const DataObject& rdoParam, $(Typedef.NsName)& rtType);
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have serializator
  const DataObject& operator>>(const DataObject& rdoParam, $(Typedef.NsName)& rtType);
#ifeqend // ifneq($(Typedef.DataType.Type),struct
#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#end // foreach $(Interface.Typedefs)
}
