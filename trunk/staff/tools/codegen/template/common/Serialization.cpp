namespace staff
{
///////////////////////////////////////////////////////////////////////////////////////////////////////
// forward declarations
#foreach $(Interface.Structs)
#ifeq($(Struct.Extern),0) // do not serialize extern type
CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.NsName)& rstStruct);
const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.NsName)& rstStruct);
#ifeqend
#end

#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0) // do not serialize extern type
#ifeq($(Typedef.DataType.IsTemplate),1)
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.NsName)& rtType);
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.NsName)& rtType);
#ifeqend // #ifneq($(Typedef.DataType.Type),struct)
#ifeqend // #ifeq($(Typedef.DataType.IsTemplate),1)
#ifeq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have deserializator // !!list<struct>!!
#ifeq($(Typedef.DataType.IsTemplate),1)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.NsName)& rtType);
#ifeqend
#else                 // !!not_a_struct!!
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.NsName)& rtType);
#ifeqend
#ifeqend
#end // foreach $(Interface.Typedefs)

///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct serializators
#foreach $(Interface.Structs)
#ifeq($(Struct.Extern),0) // do not serialize extern type
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
#ifeq($(Param.DataType.Type),struct||typedef)
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
  for ($(Param.DataType.UsedTypedef)::const_iterator it = rstStruct.$(Param.Name).begin();
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
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef)
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
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef)
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
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef)
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
#cgerror unknown type of Param.Name: $(Struct.NsName)::$(Param.DataType.Name)
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
#end

///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct deserializators
#foreach $(Interface.Structs)
#ifeq($(Struct.Extern),0) // do not serialize extern type
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
#ifeq($(Param.DataType.Type),struct||typedef)
  rdoParam$($DeserializeNodeInstruct) >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),template)

  for (CDataObject::ConstIterator it = rdoParam$($DeserializeNodeInstruct).Begin(); it != rdoParam$($DeserializeNodeInstruct).End(); ++it)
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
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef)
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
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef)
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
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||typedef)
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
  rstStruct.$(Param.Name) = *rdoParam("$(Param.Name)").Begin();
#else
#ifeq($(Param.DataType.Type),generic)
  rstStruct.$(Param.Name) = rdoParam$($DeserializeNodeInstruct).GetValue();
#else
#ifeq($(Param.DataType.Type),string)
  rstStruct.$(Param.Name) = rdoParam$($DeserializeNodeInstruct).GetText();
#else
#cgerror unknown type of Param.Name: $(Param.Name)::$(Param.DataType.Name)
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
#end

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef serializators
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0) // do not serialize extern type

// $(Typedef.NsName)  Typedef.DataType.Type $(Typedef.DataType.Type) $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.IsTemplate),1) // there must be an serializer for each container
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.NsName)& rtType)
{
  for ($(Typedef.NsName)::const_iterator it = rtType.begin(); it != rtType.end(); ++it)
  {
    CDataObject tdoItem = rdoParam.CreateChild("Item");
#ifeq($(Typedef.DataType.NsName),std::map)
    CDataObject tdoKey = tdoItem.CreateChild("Key");
    CDataObject tdoValue = tdoItem.CreateChild("Value");
    tdoKey << it->first;
    tdoValue << it->second;
#else
    tdoItem << *it;
#ifeqend
  }

  return rdoParam;
}
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have serializator
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.NsName)& rtType)
{
#ifeq($(Typedef.DataType.Type),generic)    // !!generic!!
  rdoParam.SetValue(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),string)    // !!string!!
  rdoParam.SetText(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),dataobject) // !!dataobject!! 
  rdoParam.AppendChild(rtType);
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),typedef||template)    // !!typedef||template!!
  return rdoParam << rtType;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
}
#ifeqend

#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#ifeqend //ifeq($(Typedef.Extern),0) // do not serialize extern type
#end // foreach $(Interface.Typedefs)

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef deserializators
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0) // do not serialize extern type
#ifeq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have deserializator // !!list<struct>!!
#ifeq($(Typedef.DataType.IsTemplate),1)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.NsName)& rtType)
{
  for (CDataObject::ConstIterator it = rdoParam.Begin(); it != rdoParam.End(); ++it)
  {
    $(Typedef.DataType.NsName) tItem;
    *it >> tItem;
    rtType.push_back(tItem);
  }
  return rdoParam;
}
#ifeqend
#else                 // !!not_a_struct!!
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.NsName)& rtType)
{
#ifeq($(Typedef.DataType.IsTemplate),1)
// container :: $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.Type),typedef)
  $(Typedef.DataType.NsName) tItem;
#ifeqend
  for (CDataObject::ConstIterator it = rdoParam.Begin(); it != rdoParam.End(); ++it)
  {
#ifneq($(Typedef.DataType.Type),typedef)
#ifeq($(Typedef.DataType.Type),template)
#ifeq($(Typedef.DataType.NsName),std::map)
    $(Typedef.DataType.TemplateParams.TemplateParam1) tKey;
    $(Typedef.DataType.TemplateParams.TemplateParam2) tValue;
#else
    $(Typedef.DataType.TemplateParams.TemplateParam1) tItem;
#ifeqend
#ifeqend
#ifeqend

#ifeq($(Typedef.DataType.Type),generic)
    rtType.push_back(it->GetValue());
#else
#ifeq($(Typedef.DataType.Type),string)
    rtType.push_back(it->GetText());
#else
#ifeq($(Typedef.DataType.Type),dataobject)
    rtType.push_back(*(it->Begin()));
#else
#ifeq($(Typedef.DataType.Type),typedef)
    *it >> tItem;
    rtType.push_back(tItem);
#else
#ifeq($(Typedef.DataType.Type),template)
#ifeq($(Typedef.DataType.NsName),std::map)
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),string)
    tKey = (*it)["Key"].AsString();
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)    // !!generic!!
    tKey = (*it)["Key"];
#else
    (*it)("Key") >> tKey;
#ifeqend
#ifeqend
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),string)
    tValue = (*it)["Value"].AsString();
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam2.Type),generic)    // !!generic!!
    tValue = (*it)["Value"];
#else
    (*it)("Value") >> tValue;
#ifeqend
#ifeqend
    rtType[ tKey ] = tValue;
#else // ----------------------- list, vector, etc.
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),string)    // !!string!!
    tItem = it->GetText();
#else
#ifeq($(Typedef.DataType.TemplateParams.TemplateParam1.Type),generic)    // !!generic!!
    tItem = it->GetValue();
#else
    *it >> tItem;
#ifeqend
#ifeqend
    rtType.push_back(tItem);
#ifeqend
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
  }
  return rdoParam;
#else // !!DataType.IsTemplate!!
// not container :: $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.Type),generic)    // !!generic!!
  rtType = rdoParam.GetValue();
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),string)    // !!string!!
  rtType = rdoParam.GetText();
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),dataobject) // !!dataobject!! 
  rtType = *rdoParam.Begin();
  return rdoParam;
#else
#ifeq($(Typedef.DataType.Type),typedef||template)    // !!typedef||template!!
  return rdoParam << rtType;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
}

#ifeqend
#ifeqend // ifeq($(Typedef.Extern),0) // do not serialize extern type
#end
}
