#ifeq($(Struct.Extern),0) // do not serialize/deserialize extern type
  CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.NsName)& rstStruct);
  const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.NsName)& rstStruct);
#foreach $(Struct.Structs)
#cginclude <common/StructSerialization.h>
#end
#ifeqend
