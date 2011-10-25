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
#ifneq($(Interface.Options.*targetNamespace),)
  rdoParam.SetNamespaceUriGenPrefix("$(Interface.Options.*targetNamespace)", true);
#ifeqend
\
#ifneq($(Interface.Options.*elementFormDefault),)
#ifeq($(Interface.Options.*elementFormDefault),qualified)
  rdoParam.SetElementFormDefaultQualified(true);
#else
  rdoParam.SetElementFormDefaultQualified(false);
#ifeqend
#ifeqend
\
#ifneq($(Interface.Options.*attributeFormDefault),)
#ifeq($(Interface.Options.*attributeFormDefault),qualified)
  rdoParam.SetAttributeFormDefaultQualified(true);
#else
  rdoParam.SetAttributeFormDefaultQualified(false);
#ifeqend
#ifeqend
\
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
  staff::FromString(sParam, nValue);
  reEnumValue = static_cast< $(Enum.NsName) >(nValue);
#ifeqend
  return sParam;
}

const DataObject& operator>>(const DataObject& rdoParam, $(Enum.NsName)& reEnumValue)
{
  rdoParam.GetText() >> reEnumValue;
  return rdoParam;
}
