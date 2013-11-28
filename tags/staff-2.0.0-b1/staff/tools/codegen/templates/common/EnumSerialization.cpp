#cginclude "StringTypes"

std::string SerializeEnum_$(Enum.NsName.!mangle)_ToString(const $(Enum.NsName) eEnumValue)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
  switch (eEnumValue)
  {
#foreach $(Enum.Members)
  case $(Enum.Namespace)$(Member.Name):
    return "$(Member.Value||Member.Name)";
#end
  default:
    STAFF_THROW_ASSERT("Value out of range while serializing enum [$(Enum.NsName)]: " +
                       staff::ToString(eEnumValue));
  };
#else
  return ToString(static_cast<int>(eEnumValue));
#ifeqend
}

DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue)
{
  rdoParam.SetText(SerializeEnum_$(Enum.NsName.!mangle)_ToString(eEnumValue));
  return rdoParam;
}

$(Enum.NsName) DeserializeEnum_$(Enum.NsName.!mangle)_FromString(const std::string& sParam)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
#foreach $(Enum.Members)
  if (sParam == "$(Member.Value||Member.Name)")
  {
    return $(Enum.Namespace)$(Member.Name);
  }
  else
#end
  {
    STAFF_THROW_ASSERT("Value out of range while deserializing enum [$(Enum.NsName)]: [" + sParam + "]");
  }
#else
  int nValue = 0;
  FromString(sParam, nValue);
  return static_cast< $(Enum.NsName) >(nValue);
#ifeqend
}

const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue)
{
  reEnumValue = DeserializeEnum_$(Enum.NsName.!mangle)_FromString(rdoParam.GetText());
  return rdoParam;
}
