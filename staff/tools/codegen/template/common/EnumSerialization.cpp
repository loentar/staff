#cginclude "StringTypes"

std::string& operator<<(std::string& sResult, const $(Enum.NsName) eEnumValue)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
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
#else
  sResult = staff::ToString(static_cast<int>(eEnumValue));
#ifeqend
  return sResult;
}

DataObject& operator<<(DataObject& rdoParam, const $(Enum.NsName) eEnumValue)
{
  std::string sParam;
  rdoParam.SetText(sParam << eEnumValue);
  return rdoParam;
}

const std::string& operator>>(const std::string& sParam, $(Enum.NsName)& reEnumValue)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
#foreach $(Enum.Members)
  if (sParam == "$(Member.Value||Member.Name)")
  {
    reEnumValue = $(Enum.Namespace)$(Member.Name);
  }
  else
#end
  {
    RISE_THROWS(rise::CLogicNoItemException,
       "Value out of range while deserializing enum [$(Enum.NsName)]: [" + sParam + "]");
  }
#else
  int nValue = 0;
  rdoParam.GetValue(nValue);
  reEnumValue = static_cast< $(Enum.NsName) >(nValue);
#ifeqend
}

const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue)
{
  rdoParam.GetText() >> reEnumValue;
  return rdoParam;
}
