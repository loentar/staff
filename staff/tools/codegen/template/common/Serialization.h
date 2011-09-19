namespace staff
{
  class DataObject;
#foreach $(Interface.Enums)
  DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue);
  const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue);
  std::string& operator<<(std::string& sResult, const $(Enum.NsName) eEnumValue);
  const std::string& operator>>(const std::string& sParam, $(Enum.NsName)& reEnumValue);
#end
#foreach $(Interface.Structs)
#cginclude "StructSerialization.h"
#end
#foreach $(Interface.Typedefs)
  DataObject& SerializeTypedef_$(Typedef.NsName.!mangle)(DataObject& rdoParam, const $(Typedef.NsName)& rtType);
  const DataObject& DeserializeTypedef_$(Typedef.NsName.!mangle)(const DataObject& rdoParam, $(Typedef.NsName)& rtType);
#end // foreach $(Interface.Typedefs)
}
