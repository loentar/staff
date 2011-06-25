#ifeq($(Struct.Extern),0) // do not serialize/deserialize extern type
  DataObject& operator<<(DataObject& rdoParam, const $(Struct.NsName)& rstStruct);
  const DataObject& operator>>(const DataObject& rdoParam, $(Struct.NsName)& rstStruct);
#foreach $(Struct.Enums)
  DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue);
#end
#foreach $(Struct.Structs)
#cginclude "StructSerialization.h"
#end
#ifeqend
