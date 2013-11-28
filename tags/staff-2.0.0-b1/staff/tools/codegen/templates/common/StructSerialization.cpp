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
#var sStructNsName $(Struct.Namespace)
\
  const std::string& sInstanceType = tpAbstractStruct.GetType();
#var sThisStructNsName $(.NsName)
#foreach $(Project.Interfaces)
#foreach $(Interface.Structs)
#ifeq($(Struct.ParentNsName)-$(Struct.Extern),$($sThisStructNsName)-false)
  if (sInstanceType == "$(Struct.NsName.!dot)")
  {
#var sStructTns $(Interface.Options.*targetNamespace)
#ifeq($($sStructTns),)
#var sStructTns http://tempui.org/$(Interface.NsName.!dot)
#ifeqend
    rdoParam.SetCppInstanceType("$(Struct.Name)", "$($sStructTns)");
    rdoParam << static_cast< const $(Struct.NsName)& >(*tpAbstractStruct);
  }
  else
#ifeqend
#end
#end
  {
    STAFF_THROW_ASSERT("Can't serialize dynamic type [" + sInstanceType + "]");
  }

  return rdoParam;
}

const DataObject& operator>>(const DataObject& rdoParam, Abstract< $(Struct.NsName) >& tpAbstractStruct)
{
  std::string sCppInstanceType;
  std::string sInstanceUri;
  rdoParam.GetCppInstanceType(sCppInstanceType, sInstanceUri);

#var bFirstInheritedType 1
#var sThisStructNsName $(.NsName)
#foreach $(Project.Interfaces)
#var bHasInheritedStruct 0
#foreach $(Interface.Structs)
#ifeq($(Struct.ParentNsName)-$(Struct.Extern),$($sThisStructNsName)-false)
#var bHasInheritedStruct 1
#ifeqend
#end
\
#ifeq($($bHasInheritedStruct),1)
#var sStructTns $(Interface.Options.*targetNamespace)
#ifeq($($sStructTns),)
#var sStructTns http://tempui.org/$(Interface.NsName.!dot)
#ifeqend
#ifeq($($bFirstInheritedType),1)
#var bFirstInheritedType 0
#else
  else
#ifeqend
  if (sInstanceUri == "$($sStructTns)")
  {
#var bFirstInheritedTypeStruct 1
#foreach $(Interface.Structs)
#ifeq($(Struct.ParentNsName)-$(Struct.Extern),$($sThisStructNsName)-false)
\
#ifeq($($bFirstInheritedTypeStruct),1)
#var bFirstInheritedTypeStruct 0
#else
    else
#ifeqend
    if (sCppInstanceType == "$(Struct.Name)")
    {
      tpAbstractStruct = new $(Struct.NsName);
      rdoParam >> static_cast< $(Struct.NsName)& >(*tpAbstractStruct);
    }
#ifeqend
#end
  }
#ifeqend
#end

  STAFF_ASSERT(tpAbstractStruct, "Can't deserialize dynamic type [" + sCppInstanceType
               + "] with uri [" + sInstanceUri + "]");

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
#var sdoParam rdoParam.CreateChild("$(Param.Options.*elementName||Param.Name)")
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
#var sElementName $(Param.Options.*elementName||Param.Name)
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
  if (rdoParam.GetLocalName() == "$(Param.Options.*elementName||Param.Name)")
  {
#indent +
#ifeqend
#ifeq($(Param.Options.*useParentElement),)
#var sElementName
#else
#var sElementName $(Param.Options.*elementName||Param.Name)
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
