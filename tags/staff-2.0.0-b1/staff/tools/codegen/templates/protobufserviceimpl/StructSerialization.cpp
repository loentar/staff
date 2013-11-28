#ifeq($(Struct.Extern),false) // do not serialize/deserialize extern type
#foreach $(Struct.Structs)
#cginclude "StructSerialization.cpp"
#end
#foreach $(Struct.Enums)
#cginclude "EnumSerialization.cpp"
#end

$(Struct.NsName.!deprefix/$($rootns)/)& operator<<($(Struct.NsName.!deprefix/$($rootns)/)& rLeft, const $(Struct.NsName)& rRight)
{
#ifneq($(Struct.ParentName),)
  // serialize parent struct
  rLeft << static_cast< const $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
#ifneq($($protobufEncoding),)
  // convert string encoding
  staff::CharsetConverter("$($protobufEncoding)", "UTF-8") tCharsetConverter;
#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Type),generic)
  rLeft.set_$(Param.Name.!tolower)(rRight.$(Param.Name));
#else
#ifeq($(Param.DataType.Type),string)
\
#ifneq($($protobufEncoding),)
  // convert string encoding
  tCharsetConverter.Convert((rRight.$(Param.Name), *rLeft.mutable_$(Param.Name.!tolower)());
#else
  rLeft.set_$(Param.Name.!tolower)(rRight.$(Param.Name));
#ifeqend
\
#else
#ifeq($(Param.DataType.Type),struct)
  $(Param.DataType.NsName.!deprefix/$($rootns)/)* pTmp$(Param.Name) = rLeft.mutable_$(Param.Name.!tolower)();
  STAFF_ASSERT_PARAM(pTmp$(Param.Name));
  *pTmp$(Param.Name) << rRight.$(Param.Name);
#else
#ifeq($(Param.DataType.Type),enum)
  $(Param.DataType.NsName.!deprefix/$($rootns)/) tpbTmp$(Param.Name);
  tpbTmp$(Param.Name) << rRight.$(Param.Name);
  rLeft.set_$(Param.Name.!tolower)(tpbTmp$(Param.Name));
#else
#ifeq($(Param.DataType.Type),template)
#ifneq($(Param.DataType.Name),list||vector||Array)
#cgerror $(Param.DataType.Name) is not supported
#ifeqend

  for ($(Param.DataType)::const_iterator itItem = rRight.$(Param.Name).begin();
      itItem != rRight.$(Param.Name).end(); ++itItem)
  {
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    rLeft.add_$(Param.Name.!tolower)(*itItem);
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
\
#ifneq($($protobufEncoding),)
    // convert string encoding
    std::string sItem$(Param.Name);
    tCharsetConverter.Convert(*itItem, sItem$(Param.Name));
    rLeft.add_$(Param.Name.!tolower)(sItem$(Param.Name));
#else
    rLeft.add_$(Param.Name.!tolower)(*itItem);
#ifeqend
\
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||enum)
    $(Param.DataType.TemplateParams.TemplateParam1.NsName.!deprefix/$($rootns)/)* pItem = rLeft.add_$(Param.Name.!tolower)();
    STAFF_ASSERT_PARAM(pItem);
    *pItem << *itItem;
#else
#cgerror list element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
  }

#else
#cgerror element type $(Param.DataType) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end
  return rLeft;
}

$(Struct.NsName)& operator<<($(Struct.NsName)& rLeft, const $(Struct.NsName.!deprefix/$($rootns)/)& rRight)
{
#ifneq($(Struct.ParentName),)
  // deserialize parent struct
  rLeft >> static_cast< $(Struct.ParentNsName)& >(rstStruct);

#ifeqend
#ifneq($($protobufEncoding),)
  // convert string encoding
  staff::CharsetConverter("$($protobufEncoding)", "UTF-8") tCharsetConverter;
#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Type),generic)
  rLeft.$(Param.Name) = rRight.$(Param.Name.!tolower)();
#else
#ifeq($(Param.DataType.Type),string)
\
#ifneq($($protobufEncoding),)
  // convert string encoding
  tCharsetConverter.Convert(rRight.$(Param.Name.!tolower)(), rLeft.$(Param.Name));
#else
  rLeft.$(Param.Name) = rRight.$(Param.Name.!tolower)();
#ifeqend
\
#else
#ifeq($(Param.DataType.Type),struct||enum)
  rLeft.$(Param.Name) << rRight.$(Param.Name.!tolower)();
#else
#ifeq($(Param.DataType.Type),template)
#ifneq($(Param.DataType.Name),list||vector||Array)
#cgerror $(Param.DataType.Name) is not supported
#ifeqend

  int n$(Param.Name)Size = rRight.$(Param.Name.!tolower)_size();
  for (int nIndex = 0; nIndex < n$(Param.Name)Size; ++nIndex)
  {
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),generic)
    rLeft.$(Param.Name).push_back(rRight.$(Param.Name.!tolower)(nIndex));
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),string)
\
#ifneq($($protobufEncoding),)
    // convert string encoding
    std::string sItem$(Param.Name);
    tCharsetConverter.Convert(rRight.$(Param.Name.!tolower)(nIndex), sItem$(Param.Name));
    rLeft.$(Param.Name).push_back(sItem$(Param.Name));
#else
    rLeft.$(Param.Name).push_back(rRight.$(Param.Name.!tolower)(nIndex));
#ifeqend
\
#else
#ifeq($(Param.DataType.TemplateParams.TemplateParam1.Type),struct||enum)
    $(Param.DataType.TemplateParams.TemplateParam1.NsName) t$(Param.Name);
    t$(Param.Name) << rRight.$(Param.Name.!tolower)(nIndex);
    rLeft.$(Param.Name).push_back(t$(Param.Name));
#else
#cgerror list element type $(Param.DataType.TemplateParams.TemplateParam1.Type) is not supported
#ifeqend
#ifeqend
#ifeqend
  }

#else
#cgerror element type $(Param.DataType) is not supported
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end
  return rLeft;
}
#ifeqend
