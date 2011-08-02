#ifeq($(Struct.Extern),0) // do not serialize/deserialize extern type
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
    RISE_THROWS(rise::CLogicNoItemException, "Can't serialize dynamic type [" + sInstanceType + "]");
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
    RISE_THROWS(rise::CLogicNoItemException, "Can't deserialize dynamic type [" + sInstanceType + "]");
  }
  return rdoParam;
}
#ifeqend
\

DataObject& operator<<(DataObject& rdoParam, const $(Struct.NsName)& \
#ifeq($(Struct.Members.$Count),0)
/*\
#ifeqend
rstStruct\
#ifeq($(Struct.Members.$Count),0)
*/\
#ifeqend
)
{
#ifneq($(Struct.ParentName),)
  // serialize parent struct
  rdoParam << static_cast< const $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
\
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Name),Optional||Nillable)
  if (!rstStruct.$(Param.Name).IsNull())
  {
#var sContext Param.DataType.TemplateParams.TemplateParam1
#var sOptMod *
#indent +
#else
#var sContext Param.DataType
#var sOptMod
#ifeqend
#context $($sContext)
\
#ifeq($(.Name),Abstract) // abstract type

#ifeq($(Param.Options.*isAttribute),true||1) // attribute
#cgerror Can't serialize abstract member into attribute. In struct $(Struct.NsName), member $(Param.Name)
#ifeqend
#ifneq($(.TemplateParams.TemplateParam1.Type),struct)
#cgerror Abstract template type is not struct. In struct $(Struct.NsName), member $(Param.Name)
#ifeqend
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var doName rdoParam
#else
#var doName tdoParam$(Param.Name)
  DataObject $($doName) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend // serialize to parent element?
  $($doName) << $($sOptMod)rstStruct.$(Param.Name);
#else // not abstract
\
#ifeq($(Param.Options.*isAttribute),true||1) // serialize to attribute
#ifeq($(.Type),generic||string||typedef)
  rdoParam.CreateAttribute("$(Param.Name)", $($sOptMod)rstStruct.$(Param.Name));
#else
#ifeq($(.Type),enum)
  std::string sParam$(Param.Name);
  rdoParam.CreateAttribute("$(Param.Name)", sParam$(Param.Name) << $($sOptMod)rstStruct.$(Param.Name));
#else
#cgerror Cannot serialize type $(.Type) to attribute value. Struct: $(Struct.Name)::$(Param.Name)
#ifeqend
#ifeqend
#else // serialize to subnode
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var SerializeNodeInstruct
#else
#var SerializeNodeInstruct .CreateChild("$(Param.Name)")
#ifeqend
\
\
#ifeq($(.Type),struct||typedef||enum)
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var doName rdoParam
#else
#var doName tdoParam$(Param.Name)
  DataObject $($doName) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend // serialize to parent element?
#ifneq($(.Type),typedef)
  $($doName) << $($sOptMod)rstStruct.$(Param.Name);
#else
  SerializeTypedef_$(.NsName.!mangle)($($doName), $($sOptMod)rstStruct.$(Param.Name));
#ifeqend
#else
\
\
#ifeq($(.Type),template)
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var doName rdoParam
#else
#var doName tdoParam$(Param.Name)
  DataObject tdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend
  for ($(.NsName)::const_iterator it = ($($sOptMod)rstStruct.$(Param.Name)).begin();
      it != ($($sOptMod)rstStruct.$(Param.Name)).end(); ++it)
  {
#ifneq($(Param.Options.*elementName),)
#var ElementName $(Param.Options.*elementName)
#else
#var ElementName Item
#ifeqend
#ifeq($(.Name),map||multimap)
    DataObject tdoItem = $($doName).CreateChild("$($ElementName)");
\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.CreateChild("Key").SetValue(it->first);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    DataObject tdoKey = tdoItem.CreateChild("Key");
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
    tdoKey << it->first;
#else
    SerializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoKey, it->first);
#ifeqend
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.CreateChild("Value").tdoValue.SetValue(it->second);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    DataObject tdoValue = tdoItem.CreateChild("Value");
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
    tdoValue << it->second;
#else
    SerializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoValue, it->second);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),dataobject)
    tdoItem.CreateChild("Value").AppendChild(it->second);
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend

#else
\
#ifeq($(.Name),list||vector)
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
    $($doName).CreateChild("$($ElementName)").SetValue(*it);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    DataObject tdoItem = $($doName).CreateChild("$($ElementName)");
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
    tdoItem << *it;
#else
    SerializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoItem, *it);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),dataobject)
    $($doName).CreateChild("$($ElementName)").AppendChild(*it);
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#else
#cgerror template type $(.NsName) is not supported
#ifeqend
\
#ifeqend
  }

#else
\
\
#ifeq($(.Type),dataobject)
  rdoParam$($SerializeNodeInstruct).AppendChild($($sOptMod)rstStruct.$(Param.Name));
#else
\
\
#ifeq($(.Type),generic||string)
#ifeq($(.Name),anyAttribute)
  for (anyAttribute::const_iterator itAttr = ($($sOptMod)rstStruct.$(Param.Name)).begin(),
       itAttrEnd = ($($sOptMod)rstStruct.$(Param.Name)).end(); itAttr != itAttrEnd; ++itAttr)
  {
    rdoParam.AppendAttribute(const_cast<Attribute&>(*itAttr));
  }
#else
  rdoParam$($SerializeNodeInstruct).SetValue($($sOptMod)rstStruct.$(Param.Name));
#ifeqend
#else
#cgerror unknown type($(.Type)) of Param.Name: $(Struct.NsName)::$(.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // abstract
#ifeqend // if attribute
\
#ifeq($(Param.DataType.Name),Optional||Nillable)
#indent -
  }
#ifeqend
#ifeq($(Param.DataType.Name),Nillable)
  else
  {
    rdoParam.CreateChild("$(Param.Name)").SetNil();
  }
#ifeqend
#contextend
\
#end
  return rdoParam;
}

const DataObject& operator>>(const DataObject& rdoParam, $(Struct.NsName)& \
#ifeq($(Struct.Members.$Count),0)
/*\
#ifeqend
rstStruct\
#ifeq($(Struct.Members.$Count),0)
*/\
#ifeqend
)
{
#ifneq($(Struct.ParentName),)
  // deserialize parent struct
  rdoParam >> static_cast< $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Name),Optional||Nillable)
#ifeq($(Param.Options.*isAttribute),true||1)
#ifeq($(Param.DataType.Name),Nillable)
#cgerror nillable attributes is not supported: in $(Struct.NsName)::$(Param.Name)
#ifeqend
  const staff::Attribute& rAttr$(Param.Name) = rdoParam.GetAttributeByLocalNameOpt("$(Param.Name)");
  if (!rAttr$(Param.Name).IsNull())
#else // not attribute
#ifeq($(Param.DataType.Name),Nillable)
  const staff::DataObject& rdoParam$(Param.Name) = rdoParam.GetChildByLocalName("$(Param.Name)");
  if (!rdoParam$(Param.Name).IsNil())
#else
  const staff::DataObject& rdoParam$(Param.Name) = rdoParam.GetChildByLocalNameOpt("$(Param.Name)");
  if (!rdoParam$(Param.Name).IsNull())
#ifeqend // nillable||optional
#ifeqend // is attr
  {
#ifeq($(Param.Options.*useParentElement),true||1) // deserialize from parent element?
#var sParamNode rdoParam
#else
#var sParamNode rdoParam$(Param.Name)
#ifeqend
#var sContext Param.DataType.TemplateParams.TemplateParam1
#var sOptMod *
#indent +
#else
#ifeq($(Param.Options.*useParentElement),true||1) // deserialize from parent element?
#var sParamNode rdoParam
#else
#var sParamNode rdoParam.GetChildByLocalName("$(Param.Name)")
#ifeqend
#var sContext Param.DataType
#var sOptMod
#ifeqend
#context $($sContext)
\
#ifeq($(.Name),Abstract) // abstract type
  $($sParamNode) >> $($sOptMod)rstStruct.$(Param.Name);
#else // not abstract
\
#ifeq($(Param.Options.*isAttribute),true||1) // deserialize from attribute
#ifeq($(.Type),generic||string||typedef)
#ifeq($(Param.DataType.Name),Optional)
  rAttr$(Param.Name).GetValue($($sOptMod)rstStruct.$(Param.Name));
#else
  rdoParam.GetAttributeValueByName("$(Param.Name)", $($sOptMod)rstStruct.$(Param.Name));
#ifeqend
#else
#ifeq($(.Type),enum)
#ifeq($(Param.DataType.Name),Optional)
  rAttr$(Param.Name).GetText() >> $($sOptMod)rstStruct.$(Param.Name);
#else
  rdoParam.GetAttributeTextByName("$(Param.Name)") >> $($sOptMod)rstStruct.$(Param.Name);
#ifeqend
#else
#cgerror Cannot deserialize type $(.Type) to attribute value. Struct: $(Struct.Name)::$(Param.Name)
#ifeqend
#ifeqend
#else // deserialize from subnode node
#ifeq($(.Type),struct||enum)
  $($sParamNode) >> $($sOptMod)rstStruct.$(Param.Name);
#else
#ifeq($(.Type),typedef)
  DeserializeTypedef_$(.NsName.!mangle)($($sParamNode), $($sOptMod)rstStruct.$(Param.Name));
#else
#ifeq($(.Type),template)

  for (DataObject tdoItem = $($sParamNode).FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
#ifeq($(.Name),map||multimap)  // -------- map ----------
    $(.TemplateParams.TemplateParam1) tKey\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.GetChildByLocalName("Key").GetValue(tKey);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||enum)
    DataObject tdoKey = tdoItem.GetChildByLocalName("Key");
#ifneq($(.TemplateParams.TemplateParam1.Type),typedef)
    tdoKey >> tKey;
#else
    DeserializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoKey, tKey);
#ifeqend
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend

    $(.TemplateParams.TemplateParam2)& rValue = ($($sOptMod)rstStruct.$(Param.Name))[tKey];
#ifeq($(.TemplateParams.TemplateParam2.Type),generic||string)
    tdoItem.GetChildByLocalName("Value").GetValue(rValue);
#else
#ifeq($(.TemplateParams.TemplateParam2.Type),struct||typedef||enum)
    DataObject tdoValue = tdoItem.GetChildByLocalName("Value");
#ifneq($(.TemplateParams.TemplateParam2.Type),typedef)
    tdoValue >> rValue;
#else
    DeserializeTypedef_$(.TemplateParams.TemplateParam2.NsName.!mangle)(tdoValue, rValue);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam2.Type),dataobject)
    rValue = tdoItem.GetChildByLocalName("Value").FirstChild();
#else
#cgerror key element type $(.TemplateParams.TemplateParam2.Type) is not supported
#ifeqend
#ifeqend
#ifeqend

#else // -------- list ----------
\
#ifeq($(.Name),list||vector)
#ifeq($(.TemplateParams.TemplateParam1.Type),generic)
    $(.TemplateParams.TemplateParam1) tItem = 0;
    tdoItem.GetValue(tItem);
    ($($sOptMod)rstStruct.$(Param.Name)).push_back(tItem);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),string)
    ($($sOptMod)rstStruct.$(Param.Name)).push_back(tdoItem.GetText());
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    $(.TemplateParams.TemplateParam1)& rItem = *($($sOptMod)rstStruct.$(Param.Name))\
.insert(($($sOptMod)rstStruct.$(Param.Name)).end(), $(.TemplateParams.TemplateParam1)());
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    tdoItem >> rItem;
#else
    DeserializeTypedef_$(.TemplateParams.TemplateParam1.NsName.!mangle)(tdoItem, rItem);
#ifeqend
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),dataobject)
    ($($sOptMod)rstStruct.$(Param.Name)).push_back(tdoItem);
#else
#cgerror item element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#else
#cgerror template type $(.NsName) is not supported
#ifeqend
\
#ifeqend
  }

#else
#ifeq($(.Type),dataobject)
  $($sOptMod)rstStruct.$(Param.Name) = rdoParam.GetChildByLocalName("$(Param.Name)").FirstChild();
#else
#ifeq($(.Type),generic||string)
#ifeq($(.Name),anyAttribute)
  for (DataObject::ConstAttributeIterator itAttr = rdoParam.AttributeBegin(),
       itAttrEnd = rdoParam.AttributeEnd(); itAttr != itAttrEnd; ++itAttr)
  {
    ($($sOptMod)rstStruct.$(Param.Name)).push_back(*itAttr);
  }
#else
  $($sParamNode).GetValue($($sOptMod)rstStruct.$(Param.Name));
#ifeqend
#else
#cgerror unknown type($(.Type)) of Param.Name: $(Param.Name)::$(.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // if attribute
#ifeqend // abstract type
\
#ifeq($(Param.DataType.Name),Optional||Nillable)
#indent -
  }
#ifeqend
#contextend
\
#end
  return rdoParam;
}
#ifeqend
