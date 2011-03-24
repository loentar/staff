#ifeq($(Struct.Extern),0) // do not serialize/deserialize extern type
  CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.NsName)& rstStruct);
  const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.NsName)& rstStruct);
#foreach $(Struct.Enums)
  CDataObject& operator<<(CDataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const CDataObject& operator>>(const CDataObject& rdoParam, $(Enum.NsName)& reEnumValue);
#end
#foreach $(Struct.Structs)
#cginclude "StructSerialization.h"
#end
#ifeqend
