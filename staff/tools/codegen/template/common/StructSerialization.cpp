#ifeq($(Struct.Extern),0) // do not serialize/deserialize extern type
#foreach $(Struct.Structs)
#cginclude "StructSerialization.cpp"
#end
#foreach $(Struct.Enums)
#cginclude "EnumSerialization.cpp"
#end

DataObject& operator<<(DataObject& rdoParam, const $(Struct.NsName)& rstStruct)
{
#ifneq($(Struct.ParentName),)
  // serialize parent struct
  rdoParam << static_cast< const $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
\
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Name),Optional)
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
  DataObject tdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend // serialize to parent element?
  $($doName) << $($sOptMod)rstStruct.$(Param.Name);
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
#ifeq($(.NsName),std::map)
    DataObject tdoItem = $($doName).CreateChild("$($ElementName)");
\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.CreateChild("Key").SetValue(it->first);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    DataObject tdoKey = tdoItem.CreateChild("Key");
    tdoKey << tKey;
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
    tdoValue << it->second;
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
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
    $($doName).CreateChild("$($ElementName)").SetValue(*it);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    DataObject tdoItem = $($doName).CreateChild("$($ElementName)");
    tdoItem << *it;
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),dataobject)
    $($doName).CreateChild("$($ElementName)").AppendChild(*it);
#else
#cgerror key element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
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
#ifeqend // if attribute
\
#ifeq($(Param.DataType.Name),Optional)
#indent -
  }
#ifeqend
#contextend
\
#end
  return rdoParam;
}

const DataObject& operator>>(const DataObject& rdoParam, $(Struct.NsName)& rstStruct)
{
#ifneq($(Struct.ParentName),)
  // deserialize parent struct
  rdoParam >> static_cast< $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Name),Optional)
#ifeq($(Param.Options.*isAttribute),true||1)
  const staff::Attribute& rAttr$(Param.Name) = rdoParam.GetAttributeByLocalNameOpt("$(Param.Name)");
  if (!rAttr$(Param.Name).IsNull())
#else
  const staff::DataObject& rdoParam$(Param.Name) = rdoParam.GetChildByLocalNameOpt("$(Param.Name)");
  if (!rdoParam$(Param.Name).IsNull())
#ifeqend
  {
#var sParamNode rdoParam$(Param.Name)
#var sContext Param.DataType.TemplateParams.TemplateParam1
#var sOptMod *
#indent +
#else
#var sParamNode rRequest.GetChildByLocalName("$(Param.Name)")
#var sContext Param.DataType
#var sOptMod
#ifeqend
#context $($sContext)
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
#ifeq($(Param.Options.*useParentElement),true||1) // deserialize from parent element?
#var DeserializeNodeInstruct
#else
#var DeserializeNodeInstruct .GetChildByLocalName("$(Param.Name)")
#ifeqend
#ifeq($(.Type),struct||typedef||enum)
  rdoParam$($DeserializeNodeInstruct) >> $($sOptMod)rstStruct.$(Param.Name);
#else
#ifeq($(.Type),template)

  for (DataObject tdoItem = rdoParam$($DeserializeNodeInstruct).FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
#ifeq($(.NsName),std::map)  // -------- map ----------
    $(.TemplateParams.TemplateParam1) tKey\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.GetChildByLocalName("Key").GetValue(tKey);
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    DataObject tdoKey = tdoItem.GetChildByLocalName("Key");
    tdoKey >> tKey;
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
    tdoValue >> rValue;
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
    tdoItem >> rItem;
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),dataobject)
    ($($sOptMod)rstStruct.$(Param.Name)).push_back(tdoItem);
#else
#cgerror item element type $(.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
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
  rdoParam$($DeserializeNodeInstruct).GetValue($($sOptMod)rstStruct.$(Param.Name));
#ifeqend
#else
#cgerror unknown type($(.Type)) of Param.Name: $(Param.Name)::$(.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // if attribute
\
#ifeq($(Param.DataType.Name),Optional)
#indent -
  }
#ifeqend
#contextend
\
#end
  return rdoParam;
}
#ifeqend
