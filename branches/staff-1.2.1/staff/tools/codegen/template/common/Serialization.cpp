namespace staff
{
///////////////////////////////////////////////////////////////////////////////////////////////////////
// forward declarations
#foreach $(Interface.Structs)
CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.Name)& rstStruct);
const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.Name)& rstStruct);
#end

#foreach $(Interface.Typedefs)
#ifeq($(Typedef.DataType.IsTemplate),1)
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType);
#else // DataType.IsTemplate
#ifneq($(Typedef.DataType.Type),struct)
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType);
#else
\
#ifeqend // #ifneq($(Typedef.DataType.Type),struct)
#ifeqend // #ifeq($(Typedef.DataType.IsTemplate),1)
#ifeq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have deserializator // !!list<struct>!!
#ifeq($(Typedef.DataType.IsTemplate),1)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType);
#else
\
#ifeqend
#else                 // !!not_a_struct!!
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType);
#ifeqend
#end // foreach $(Interface.Typedefs)

///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct serializators
#foreach $(Interface.Structs)
CDataObject& operator<<(CDataObject& rdoParam, const $(Struct.Name)& rstStruct)
{
#ifneq($(Struct.Parent),)
  // serialize parent struct
  rdoParam << static_cast<const $(Struct.Parent)&>(rstStruct);

#else
\
#ifeqend
#foreach $(Struct.Members)
  CDataObject tdoParam$(Param.Name) = rdoParam.CreateChild("$(Param.Name)");
#ifeq($(Param.DataType.Type),struct)
  tdoParam$(Param.Name) << rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),typedef)
  tdoParam$(Param.Name) << rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),dataobject)
  tdoParam$(Param.Name).AppendChild(const_cast<CDataObject&>(rstStruct.$(Param.Name)));
#else
#ifeq($(Param.DataType.Type),template)
  tdoParam$(Param.Name) << rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),generic)
  tdoParam$(Param.Name).SetValue(rstStruct.$(Param.Name));
#else
#ifeq($(Param.DataType.Type),string)
  tdoParam$(Param.Name).SetText(rstStruct.$(Param.Name));
#else
#cgerror unknown type of Param.Name: $(Struct.Name)::$(Param.DataType.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end
  return rdoParam;
}

#end

///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct deserializators
#foreach $(Interface.Structs)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Struct.Name)& rstStruct)
{
#ifneq($(Struct.Parent),)
  // deserialize parent struct
  rdoParam >> static_cast<$(Struct.Parent)&>(rstStruct);

#else
\
#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Type),struct)
  rdoParam("$(Param.Name)") >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),typedef)
  rdoParam("$(Param.Name)") >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),template)
  rdoParam("$(Param.Name)") >> rstStruct.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),dataobject)
  rstStruct.$(Param.Name) = *rdoParam("$(Param.Name)").Begin();
#else
#ifeq($(Param.DataType.Type),generic)
  rstStruct.$(Param.Name) = const_cast<const CDataObject&>(rdoParam)["$(Param.Name)"];
#else
#ifeq($(Param.DataType.Type),string)
  rstStruct.$(Param.Name) = const_cast<const CDataObject&>(rdoParam)["$(Param.Name)"].AsString();
#else
#cgerror unknown type of Param.Name: $(Param.Name)::$(Param.DataType.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end
  return rdoParam;
}

#end

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef serializators
#foreach $(Interface.Typedefs)

// $(Typedef.Name)  Typedef.DataType.Type $(Typedef.DataType.Type) $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.IsTemplate),1) // there must be an serializer for each container
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType)
{
  for($(Typedef.Name)::const_iterator it = rtType.begin(); it != rtType.end(); ++it)
  {
    CDataObject tdoItem = rdoParam.CreateChild("Item");
#ifeq($(Typedef.DataType.Name),std::map)
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
CDataObject& operator<<(CDataObject& rdoParam, const $(Typedef.Name)& rtType)
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
#ifeq($(Typedef.DataType.Type),typedef)    // !!typedef!!
  return rdoParam << rtType;
#else
#ifeq($(Typedef.DataType.Type),template)    // !!template!!
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

#ifeqend // ifeq($(Typedef.DataType.IsTemplate),1)
#end // foreach $(Interface.Typedefs)

///////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef deserializators
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.DataType.Type),struct)     // !!struct!! structs already have deserializator // !!list<struct>!!
#ifeq($(Typedef.DataType.IsTemplate),1)
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType)
{
  for(CDataObject::ConstIterator it = rdoParam.Begin(); it != rdoParam.End(); ++it)
  {
    $(Typedef.DataType.Name) tItem;
    *it >> tItem;
    rtType.push_back(tItem);
  }
  return rdoParam;
}
#ifeqend
#else                 // !!not_a_struct!!
const CDataObject& operator>>(const CDataObject& rdoParam, $(Typedef.Name)& rtType)
{
#ifeq($(Typedef.DataType.IsTemplate),1)

// container :: $(Typedef.DataType.Name)
#ifeq($(Typedef.DataType.Type),typedef)
  $(Typedef.DataType.Name) tItem;
#ifeqend
  for(CDataObject::ConstIterator it = rdoParam.Begin(); it != rdoParam.End(); ++it)
  {
#ifneq($(Typedef.DataType.Type),typedef)
#ifeq($(Typedef.DataType.Type),template)
#ifeq($(Typedef.DataType.Name),std::map)
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
#ifeq($(Typedef.DataType.Name),std::map)
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
#ifeq($(Typedef.DataType.Type),typedef)    // !!typedef!!
  return rdoParam << rtType;
#else
#ifeq($(Typedef.DataType.Type),template)    // !!template!!
  return rdoParam << rtType;
#else
#cgerror "Typedef.DataType.Type = $(Typedef.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
}

#ifeqend
#end
}
