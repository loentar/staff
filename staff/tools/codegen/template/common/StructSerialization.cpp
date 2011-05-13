#ifeq($(Struct.Extern),0) // do not serialize/deserialize extern type
#foreach $(Struct.Structs)
#cginclude "StructSerialization.cpp"
#end
#foreach $(Struct.Enums)
#cginclude "EnumSerialization.cpp"
#end

CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.NsName)& rstStruct)
{
#ifneq($(Struct.ParentName),)
  // serialize parent struct
  rdoParam << static_cast< const $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
\
#foreach $(Struct.Members)
#ifeq($(Param.Options.*isAttribute),true||1) // serialize to attribute
#ifeq($(Param.DataType.Type),generic||string)
  rdoParam.CreateAttribute("$(Param.Name)", rstStruct.$(Param.Name));
#else
#cgerror Cannot serialize type $(Param.DataType.Type) to attribute value. Struct: $(Struct.Name)::$(Param.Name)
#ifeqend
#else // serialize to subnode
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var SerializeNodeInstruct
#else
#var SerializeNodeInstruct .CreateChild("$(Param.Name)")
#ifeqend
\
\
#ifeq($(Param.DataType.Type),struct||typedef||enum)
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var doName rdoParam
#else
#var doName tdoParam$(Param.Name)
  CDataObject tdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend // serialize to parent element?
  $($doName) << rstStruct.$(Param.Name);
#else
\
\
#ifeq($(Param.DataType.Type),template)
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var doName rdoParam
#else
#var doName tdoParam$(Param.Name)
  CDataObject tdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend
  for ($(Param.DataType)::const_iterator it = rstStruct.$(Param.Name).begin();
      it != rstStruct.$(Param.Name).end(); ++it)
  {
#ifneq($(Param.Options.*elementName),)
#var ElementName $(Param.Options.*elementName)
#else
#var ElementName Item
#ifeqend
#ifeq($(Param.DataType.NsName),std::map)
    CDataObject tdoItem = $($doName).CreateChild("$($ElementName)");
\
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.CreateChild("Key").SetValue(it->first);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    CDataObject tdoKey = tdoItem.CreateChild("Key");
    tdoKey << tKey;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
\
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.CreateChild("Value").tdoValue.SetValue(it->second);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    CDataObject tdoValue = tdoItem.CreateChild("Value");
    tdoValue << it->second;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    tdoItem.CreateChild("Value").AppendChild(it->second);
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend

#else
\
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    $($doName).CreateChild("$($ElementName)").SetValue(*it);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    CDataObject tdoItem = $($doName).CreateChild("$($ElementName)");
    tdoItem << *it;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    $($doName).CreateChild("$($ElementName)").AppendChild(*it);
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
\
#ifeqend
  }

#else
\
\
#ifeq($(Param.DataType.Type),dataobject)
  rdoParam$($SerializeNodeInstruct).AppendChild(const_cast<CDataObject&>(rstStruct.$(Param.Name)));
#else
\
\
#ifeq($(Param.DataType.Type),generic||string)
  rdoParam$($SerializeNodeInstruct).SetValue(rstStruct.$(Param.Name));
#else
#cgerror unknown type($(Param.DataType.Type)) of Param.Name: $(Struct.NsName)::$(Param.DataType.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // if attribute
#end
  return rdoParam;
}

const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.NsName)& rstStruct)
{
#ifneq($(Struct.ParentName),)
  // deserialize parent struct
  rdoParam >> static_cast< $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.Options.*isAttribute),true||1) // deserialize from attribute
#ifeq($(Param.DataType.Type),string)
  rdoParam.GetAttributeTextByName("$(Param.Name)", rstStruct.$(Param.Name));
#else
#ifeq($(Param.DataType.Type),generic)
  rstStruct.$(Param.Name) = rdoParam.GetAttributeValueByName("$(Param.Name)");
#else
#cgerror Cannot deserialize type $(Param.DataType.Type) to attribute value. Struct: $(Struct.Name)::$(Param.Name)
#ifeqend
#ifeqend
#else // deserialize from subnode node
#ifeq($(Param.Options.*useParentElement),true||1) // deserialize from parent element?
#var DeserializeNodeInstruct
#else
#var DeserializeNodeInstruct .GetChildByLocalName("$(Param.Name)")
#ifeqend
#ifeq($(Param.DataType.Type),struct||typedef||enum)
  rdoParam$($DeserializeNodeInstruct) >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),template)

#ifeq($(Param.DataType.NsName),std::map)
\
  $(Param.DataType.TemplateParams.TemplateParam1) tKey\
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
  $(Param.DataType.TemplateParams.TemplateParam2) tValue\
#ifeq($(Param.DataType.TemplateParams.TemplateParam2.Type),generic)
 = 0\
#ifeqend
;
\
#else
\
  $(Param.DataType.TemplateParams.TemplateParam1) tItem\
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
\
#ifeqend
  for (CDataObject tdoItem = rdoParam$($DeserializeNodeInstruct).FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
#ifeq($(Param.DataType.NsName),std::map)
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    tdoItem.GetChildByLocalName("Key").GetValue(tKey);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    CDataObject tdoKey = tdoItem.GetChildByLocalName("Key");
    tdoKey >> tKey;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend

#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.GetChildByLocalName("Value").GetValue(tValue);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    CDataObject tdoValue = tdoItem.GetChildByLocalName("Value");
    tdoValue >> tValue;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    tValue = tdoItem.GetChildByLocalName("Value").FirstChild();
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
    rstStruct.$(Param.Name)[tKey] = tValue;

#else
\
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic||string)
    tdoItem.GetValue(tItem);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    tdoItem >> tItem;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    tItem = tdoItem;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
\
    rstStruct.$(Param.Name).push_back(tItem);
#ifeqend
  }

#else
#ifeq($(Param.DataType.Type),dataobject)
  rstStruct.$(Param.Name) = rdoParam.GetChildByLocalName("$(Param.Name)").FirstChild();
#else
#ifeq($(Param.DataType.Type),generic||string)
  rdoParam$($DeserializeNodeInstruct).GetValue(rstStruct.$(Param.Name));
#else
#cgerror unknown type($(Param.DataType.Type)) of Param.Name: $(Param.Name)::$(Param.DataType.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // if attribute
#end
  return rdoParam;
}
#ifeqend
