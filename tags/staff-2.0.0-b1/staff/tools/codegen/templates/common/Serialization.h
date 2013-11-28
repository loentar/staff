namespace staff
{
  class DataObject;
#foreach $(Interface.Enums)
#ifeq($(Enum.Extern),false) // do not serialize/deserialize extern type
  DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue);
  std::string SerializeEnum_$(Enum.NsName.!mangle)_ToString(const $(Enum.NsName) eEnumValue);
  $(Enum.NsName) DeserializeEnum_$(Enum.NsName.!mangle)_FromString(const std::string& sParam);
#ifeqend
#end
#foreach $(Interface.Structs)
#cginclude "StructSerialization.h"
#end
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),false) // do not serialize/deserialize extern type
  DataObject& SerializeTypedef_$(Typedef.NsName.!mangle)(DataObject& rdoParam, const $(Typedef.NsName)& rtType);
  const DataObject& DeserializeTypedef_$(Typedef.NsName.!mangle)(const DataObject& rdoParam, $(Typedef.NsName)& rtType);
#ifeqend
#end // foreach $(Interface.Typedefs)
}
