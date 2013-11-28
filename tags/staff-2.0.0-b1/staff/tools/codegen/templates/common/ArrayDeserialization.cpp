  /* soap array deserialization: $(.NsName) ($($sOptMod)$($sParam)) $($doName) */
#ifeq($($nArrayRecursionLevel),)
#var nArrayRecursionLevel 0
#ifeqend
\
#var sdoArray rdoArray_$($sParam.!mangledot)
#var sArrayTypeIn sArrayType_$($sParam.!mangledot)
#var nPos nPos_$($sParam.!mangledot)
  const staff::DataObject& $($sdoArray) = $($doName);
#ifeq($($nArrayRecursionLevel),0)
#var sSoapEncPrefix sSoapEncPrefix_$($sParam.!mangledot)
  const std::string& $($sSoapEncPrefix) = $($sdoArray).GetNamespacePrefixByUri("http://schemas.xmlsoap.org/soap/encoding/");
#ifeqend
  const std::string& $($sArrayTypeIn) = $($sdoArray).GetAttributeTextByName($($sSoapEncPrefix) + ":arrayType");
  std::string::size_type $($nPos) = $($sArrayTypeIn).find_last_of('[');
  STAFF_ASSERT($($nPos) != std::string::npos, "Invalid arrayType: \"" + $($sArrayTypeIn) + "\"");
  ($($sOptMod)$($sParam)).Resize($($sArrayTypeIn).substr($($nPos)));
#ifneq($($array_validate_type),false||no||0)
  { // validate type
    $($nPos) = $($sArrayTypeIn).find(':');
#var nPosEnd nPosEnd_$($sParam.!mangledot)
#var bSerialize 0
#var sArrayType
#cginclude "ArrayType.cpp"
    STAFF_ASSERT($($nPos) != std::string::npos, "Invalid arrayType: \"" + $($sArrayTypeIn) + "\"");
    std::string::size_type $($nPosEnd) = $($sArrayTypeIn).find('[');
    STAFF_ASSERT($($nPosEnd) != std::string::npos, "Invalid arrayType: \"" + $($sArrayTypeIn) + "\"");
    const std::string& sArrayItemType_$($sArrayTypeIn) =
        $($sArrayTypeIn).substr($($nPos) + 1, $($nPosEnd) - $($nPos) - 1);
    if (sArrayItemType_$($sArrayTypeIn) != "$($sArrayType)")
    {
      staff::LogError() << "Unexpected array item type: \"" << sArrayItemType_$($sArrayTypeIn)
                        << "\" expected: \"$($sArrayType)\"";
    }
  }
#ifeqend
  unsigned nIndex_$($sParam.!mangledot) = 0;
  staff::DataObject::AttributeIterator itAttr_$($sParam.!mangledot);
  for (staff::DataObject tdoItem = $($sdoArray).FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
    itAttr_$($sParam.!mangledot) = tdoItem.FindAttributeByLocalName("position");

    $(.TemplateParams.TemplateParam1.NsName)& rArrayItem$($nArrayRecursionLevel.!trunc) = \
(itAttr_$($sParam.!mangledot) != tdoItem.AttributeEnd()) ?
      ($($sOptMod)$($sParam)).At(itAttr_$($sParam.!mangledot)->GetText()) : \
($($sOptMod)$($sParam))[nIndex_$($sParam.!mangledot)++];

#cgpushvars
\
#var sParam rArrayItem$($nArrayRecursionLevel.!trunc)
#var sdoParam tdoItem
#var doName tdoItem
#var nArrayRecursionLevel $($nArrayRecursionLevel.!inc)
#indent +
#context $(.TemplateParams.TemplateParam1)
#ifeq($(.Namespace)$(.Name),staff::Array)
// array deserialization
#cginclude "ArrayDeserialization.cpp"
#else
#ifeq($(.Type),string)
    $($sdoParam).GetText($($sParam));
#else
// type deserialization
#var sParamName
#var sElementName
#cginclude "TypeDeserialization.cpp"
#ifeqend
#ifeqend
#contextend
#indent -
\
#cgpopvars

  }
