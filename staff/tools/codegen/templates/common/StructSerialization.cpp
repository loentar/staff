#ifeq($(Struct.Extern),false) // do not serialize/deserialize extern type
#foreach $(Struct.Structs)
#cginclude "StructSerialization.cpp"
#end
#foreach $(Struct.Enums)
#cginclude "EnumSerialization.cpp"
#end
\
#ifeq($(Struct.Options.*abstract),true||1)

DataObject& operator<<(DataObject& rdoParam, const Abstract< $(Struct.NsName) >& tpAbstractStruct)
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
  const std::string& sInstanceType = tpAbstractStruct.GetType();
#var sThisStructNsName $(.NsName)
#foreach $(Interface.Structs) // check all top-level structs including external
#ifeq($(Struct.ParentNsName),$($sThisStructNsName))
  if (sInstanceType == "$(Struct.NsName.!dot)")
  {
    rdoParam << static_cast< const $(Struct.NsName)& >(*tpAbstractStruct);
  }
  else
#ifeqend
#end
  {
    STAFF_THROW_ASSERT("Can't serialize dynamic type [" + sInstanceType + "]");
  }
  rdoParam.SetInstanceType(sInstanceType);
  return rdoParam;
}

const DataObject& operator>>(const DataObject& rdoParam, Abstract< $(Struct.NsName) >& tpAbstractStruct)
{
  const std::string& sInstanceType = rdoParam.GetInstanceType();
#var sThisStructNsName $(.NsName)
#foreach $(Interface.Structs) // check all top-level structs including external
#ifeq($(Struct.ParentNsName),$($sThisStructNsName))
  if (sInstanceType == "$(Struct.NsName.!dot)")
  {
    tpAbstractStruct = new $(Struct.NsName);
    rdoParam >> static_cast< $(Struct.NsName)& >(*tpAbstractStruct);
  }
  else
#ifeqend
#end
  {
    STAFF_THROW_ASSERT("Can't deserialize dynamic type [" + sInstanceType + "]");
  }
  return rdoParam;
}
#ifeqend
\

DataObject& operator<<(DataObject& rdoParam, const struct $(Struct.NsName)& \
#ifeq($(Struct.Members.$Count)$(Struct.ParentNsName),0)
/*\
#ifeqend
rstStruct\
#ifeq($(Struct.Members.$Count)$(Struct.ParentNsName),0)
*/\
#ifeqend
)
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
#ifneq($(Struct.ParentName),)
  // serialize parent struct
  rdoParam << static_cast< const struct $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
\
#var bFirstChoiceArray 1
#foreach $(Struct.Members)
#context $(Param.DataType)
#var sParam rstStruct.$(Param.Name)
#var sParamName $(Param.Name)
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var sdoParam rdoParam
#else
#ifeq($(Param.Options.*isAttribute),true||1)      // serialize to attribute
#var sdoParam rdoParam
#else
#var sdoParam rdoParam.CreateChild("$(Param.Name)")
#ifeqend
#ifeqend
#ifeq($(Struct.Options.*choiceArrayItem),true||1)
#ifneq($(Param.Options.*isAttribute),true||1)
#ifneq($(Param.Options.*choiceId),)
#ifeq($($bFirstChoiceArray),1)
#var bFirstChoiceArray 0
#else
  else
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeq($(Param.Options.*useParentElement),)
#var sElementName
#else
#var sElementName $(Param.Name)
#ifeqend
#cginclude "TypeSerialization.cpp"
#contextend
#end
  return rdoParam;
}

const DataObject& operator>>(const DataObject& rdoParam, struct $(Struct.NsName)& \
#ifeq($(Struct.Members.$Count)$(Struct.ParentNsName),0)
/*\
#ifeqend
rstStruct\
#ifeq($(Struct.Members.$Count)$(Struct.ParentNsName),0)
*/\
#ifeqend
)
{
#ifneq($(Struct.ParentName),)
  // deserialize parent struct
  rdoParam >> static_cast< struct $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
#foreach $(Struct.Members)
#context $(Param.DataType)
#ifneq($(Struct.Options.*choiceArrayItem),true||1)
#var sParamName $(Param.Name)
#else
#var sParamName
#ifneq($(Param.$Num),0)
  else
#ifeqend
  if (rdoParam.GetLocalName() == "$(Param.Name)")
  {
#indent +
#ifeqend
#ifeq($(Param.Options.*useParentElement),)
#var sElementName
#else
#var sElementName $(Param.Name)
#ifeqend
#var sParam rstStruct.$(Param.Name)
#var sdoParam rdoParam
#cginclude "TypeDeserialization.cpp"
#ifeq($(Struct.Options.*choiceArrayItem),true||1)
#indent -
  }
#ifeqend
#contextend
#end
  return rdoParam;
}
#ifeqend
