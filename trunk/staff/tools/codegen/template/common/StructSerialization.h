#ifeq($(Struct.Extern),0) // do not serialize/deserialize extern type
  DataObject& operator<<(DataObject& rdoParam, const $(Struct.NsName)& rstStruct);
  const DataObject& operator>>(const DataObject& rdoParam, $(Struct.NsName)& rstStruct);
#foreach $(Struct.Enums)
  DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue);
  std::string& operator<<(std::string& sResult, const $(Enum.NsName) eEnumValue);
  const std::string& operator>>(const std::string& sParam, $(Enum.NsName)& reEnumValue);
#end
#foreach $(Struct.Structs)
#cginclude "StructSerialization.h"
#end
#ifeqend
#ifeq($(Struct.Options.*abstract),true||1)
  DataObject& operator<<(DataObject& rdoParam, const Abstract< $(Struct.NsName) >& tpAbstractStruct);
  const DataObject& operator>>(const DataObject& rdoParam, Abstract< $(Struct.NsName) >& tpAbstractStruct);
#ifeqend
