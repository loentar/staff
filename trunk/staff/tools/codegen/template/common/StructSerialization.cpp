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
#foreach $(Struct.Members)
#ifeq($(Param.Options.*isAttribute),true||1) // serialize to attribute
#ifeq($(Param.DataType.Type),generic||string)
  CAttribute tAttr$(Param.Name)("$(Param.Name)", rstStruct.$(Param.Name));
  rdoParam.AppendAttribute(tAttr$(Param.Name));
#else
#cgerror Cannot serialize type $(Param.DataType.Type) to attribute value. Struct: $(Struct.Name)::$(Param.Name)
#ifeqend
#else // serialize to subnode
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var SerializeNodeInstruct
#else
#var SerializeNodeInstruct .CreateChild("$(Param.Name)")
#ifeqend
#ifeq($(Param.DataType.Type),struct||typedef||enum)
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var doName rdoParam
#else
#var doName tdoParam$(Param.Name)
  CDataObject tdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend // serialize to parent element?
  $($doName) << rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),template)
#ifeq($(Param.Options.*useParentElement),true||1) // serialize to parent element?
#var doName rdoParam
#else
#var doName tdoParam$(Param.Name)
  CDataObject tdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
#ifeqend // serialize to parent element?
  for ($(Param.DataType)::const_iterator it = rstStruct.$(Param.Name).begin();
      it != rstStruct.$(Param.Name).end(); ++it)
  {
    CDataObject tdoItem = $($doName).CreateChild("Item");
#ifeq($(Param.DataType.NsName),std::map)
    CDataObject tdoKey = tdoItem.CreateChild("Key");
    CDataObject tdoValue = tdoItem.CreateChild("Value");


#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    tdoKey.SetValue(it->first);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
    tdoKey.SetText(it->first);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    tdoKey << tKey;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend


#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    tdoValue.SetValue(it->second);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
    tdoValue.SetText(it->second);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    tdoValue << it->second;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    tdoValue = it->second;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend

#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    tdoItem.SetValue(*it);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
    tdoItem.SetText(*it);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    tdoItem << *it;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    tdoItem = *it;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend
\
#ifeqend
  }

#else
#ifeq($(Param.DataType.Type),dataobject)
  rdoParam$($SerializeNodeInstruct).AppendChild(const_cast<CDataObject&>(rstStruct.$(Param.Name)));
#else
#ifeq($(Param.DataType.Type),generic)
  rdoParam$($SerializeNodeInstruct).SetValue(rstStruct.$(Param.Name));
#else
#ifeq($(Param.DataType.Type),string)
  rdoParam$($SerializeNodeInstruct).SetText(rstStruct.$(Param.Name));
#else
#cgerror unknown type($(Param.DataType.Type)) of Param.Name: $(Struct.NsName)::$(Param.DataType.Name)
#ifeqend
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
  rstStruct.$(Param.Name) = rdoParam.GetAttributeTextByName("$(Param.Name)");
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
  const staff::CDataObject& rdoParamDeserialize$(Param.Name) = rdoParam$($DeserializeNodeInstruct);

  for (CDataObject::ConstIterator it = rdoParamDeserialize$(Param.Name).Begin(); it != rdoParamDeserialize$(Param.Name).End(); ++it)
  {
#ifeq($(Param.DataType.NsName),std::map)
    $(Param.DataType.TemplateParams.TemplateParam1.NsName) tKey;
    $(Param.DataType.TemplateParams.TemplateParam2.NsName) tValue;
    CDataObject tdoKey = it->GetChildByLocalName("Key");
    CDataObject tdoValue = it->GetChildByLocalName("Value");

#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    tKey = tdoKey.GetValue();
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
    tKey = tdoKey.GetText();
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    tdoKey >> tKey;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend

#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    tValue = tdoValue.GetValue();
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
    tValue = tdoValue.GetText();
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    tdoValue >> tValue;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    tValue = tdoValue;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend
    rstStruct.$(Param.Name)[tKey] = tValue;

#else // list etc..
    $(Param.DataType.TemplateParams.TemplateParam1.NsName) tItem;
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    tItem = it->GetValue();
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
    tItem = it->GetText();
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef||enum)
    (*it) >> tItem;
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),dataobject)
    tItem = *it;
#else
#cgerror key element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend
    rstStruct.$(Param.Name).push_back(tItem);
#ifeqend
  }

#else
#ifeq($(Param.DataType.Type),dataobject)
  rstStruct.$(Param.Name) = rdoParam.GetChildByLocalName("$(Param.Name)").FirstChild();
#else
#ifeq($(Param.DataType.Type),generic)
  rstStruct.$(Param.Name) = rdoParam$($DeserializeNodeInstruct).GetValue();
#else
#ifeq($(Param.DataType.Type),string)
  rstStruct.$(Param.Name) = rdoParam$($DeserializeNodeInstruct).GetText();
#else
#cgerror unknown type($(Param.DataType.Type)) of Param.Name: $(Param.Name)::$(Param.DataType.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // if attribute
#end
  return rdoParam;
}
#ifeqend
