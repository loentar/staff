namespace staff
{
  class CDataObject;
#foreach $(Interface.Enums)
  CDataObject& operator<<(CDataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const CDataObject& operator>>(const CDataObject& rdoParam, $(Enum.NsName)& reEnumValue);
#end
#foreach $(Interface.Structs)
#cginclude "StructSerialization.h"
#end
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.DataType.IsTemplate),1) // there must be an serializer for each container
  CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.NsName)& rtType);
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have serializator
  CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.NsName)& rtType);
#ifeqend // ifneq($(Typedef.DataType.Type),struct
#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#ifeq($(Typedef.DataType.IsTemplate),1) // there must be an serializer for each container
  const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.NsName)& rtType);
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have serializator
  const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.NsName)& rtType);
#ifeqend // ifneq($(Typedef.DataType.Type),struct
#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#end // foreach $(Interface.Typedefs)
}
