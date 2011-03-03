
CDataObject& operator<<(CDataObject& rdoParam, const $(Enum.NsName) eEnumValue)
{
#ifeq($(Enum.Options.*baseType),string)
  std::string sResult;
  switch (eEnumValue)
  {
#foreach $(Enum.Members)
  case $(Member.Name):
    sResult = "$(Member.Name)";
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

const CDataObject& operator>>(const CDataObject& rdoParam, $(Enum.NsName)& reEnumValue)
{
#ifeq($(Enum.Options.*baseType),string)
  const std::string& sValue = rdoParam.GetText();
#foreach $(Enum.Members)
  if (sValue == "$(Member.Name)")
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
