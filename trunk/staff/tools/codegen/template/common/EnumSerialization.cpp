#cginclude "StringTypes"

DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
  std::string sResult;
  switch (eEnumValue)
  {
#foreach $(Enum.Members)
  case $(Enum.Namespace)$(Member.Name):
    sResult = "$(Member.Value||Member.Name)";
    break;
#end
  default:
    RISE_THROWS(rise::CLogicNoItemException,
       "Value out of range while serializing enum [$(Enum.NsName)]: " + rise::ToStr(eEnumValue));
  };
  rdoParam.SetText(sResult);
#else
  rdoParam.SetValue(eEnumValue);
#ifeqend
  return rdoParam;
}

const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
  const std::string& sValue = rdoParam.GetText();
#foreach $(Enum.Members)
  if (sValue == "$(Member.Value||Member.Name)")
  {
    reEnumValue = $(Enum.Namespace)$(Member.Name);
  }
  else
#end
  {
    RISE_THROWS(rise::CLogicNoItemException,
       "Value out of range while deserializing enum [$(Enum.NsName)]: [" + sValue + "]");
  }
#else
  reEnumValue = static_cast< $(Enum.NsName) >(static_cast<int>(rdoParam.GetValue()));
#ifeqend
  return rdoParam;
}
