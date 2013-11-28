#ifeq($(Struct.Extern),false) // do not serialize/deserialize extern type
  DataObject& operator<<(DataObject& rdoParam, const struct $(Struct.NsName)& rstStruct);
  const DataObject& operator>>(const DataObject& rdoParam, struct $(Struct.NsName)& rstStruct);
#foreach $(Struct.Enums)
  DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue);
  std::string SerializeEnum_$(Enum.NsName.!mangle)_ToString(const $(Enum.NsName) eEnumValue);
  $(Enum.NsName) DeserializeEnum_$(Enum.NsName.!mangle)_FromString(const std::string& sParam);
#end
#foreach $(Struct.Structs)
#cginclude "StructSerialization.h"
#end
#ifeqend
#ifeq($(Struct.Options.*abstract),true||1)
  DataObject& operator<<(DataObject& rdoParam, const Abstract< $(Struct.NsName) >& tpAbstractStruct);
  const DataObject& operator>>(const DataObject& rdoParam, Abstract< $(Struct.NsName) >& tpAbstractStruct);
#ifeqend
