  /* soap array serialization: $(.NsName) ($($sOptMod)$($sParam)) $($doName) */
#ifeq($($nArrayRecursionLevel),)
#var nArrayRecursionLevel 0
#ifeqend
\
#ifeq($($nArrayRecursionLevel),0) // declare array ns
  $($doName).SetNamespace("http://schemas.xmlsoap.org/soap/encoding/", "enc");
  $($doName).DeclareNamespace("http://www.w3.org/2001/XMLSchema", "xsd");
#var sArrayPos
#ifeqend
\
#var sSparseName bIsSparse_$($sParam.!mangledot)
  const unsigned nSize_$($sParam.!mangledot) = ($($sOptMod)$($sParam)).GetSize();
  const bool $($sSparseName) = ($($sOptMod)$($sParam)).IsSparse();
#var sArrayType
#var bSerialize 1
#cginclude "ArrayType.cpp"
\
#var sArrayType "$($sArrayType)"
#var sArrayType $($sArrayType.!replace/ + ""//)
  $($doName).CreateAttribute("enc:arrayType", $($sArrayType));
\
#var sPosParent nPos$($nArrayRecursionLevel.!dec.!trunc)
#var sPos nPos$($nArrayRecursionLevel.!trunc)
  for (unsigned $($sPos) = 0; $($sPos) < nSize_$($sParam.!mangledot); ++$($sPos))
  {
    if (!($($sOptMod)$($sParam)).IsEmpty($($sPos)))
    {
#cgpushvars
\
#var sArrayPos ($($sOptMod)$($sParam)).GetPositionStr($($sPos))
\
#var doPrevName $($doName)
#ifeq($(.TemplateParams.TemplateParam1.Namespace)$(.TemplateParams.TemplateParam1.Name),staff::Array)
#var sParamOld $($sParam)
#var sParam raParam$($nArrayRecursionLevel.!trunc)
      const $(.TemplateParams.TemplateParam1.NsName)& $($sParam) = ($($sOptMod)$($sParamOld))[$($sPos)];
#var sSparseParentName bIsSparse_$($sParamOld.!mangledot)
#var nArrayRecursionLevel $($nArrayRecursionLevel.!inc)
#context $(.TemplateParams.TemplateParam1)
#var doName tdoArray$($nArrayRecursionLevel.!trunc)
      staff::DataObject $($doName) = $($doPrevName).CreateChild("enc:Array");
#indent +2
#cginclude "ArraySerialization.cpp"
#indent -2
#contextend
#var nArrayRecursionLevel $($nArrayRecursionLevel.!dec)
#else  // --------------------------- not a sub-array ---------------------------------
\
#var sOptMod
#var sParam $($sOptMod)$($sParam)[$($sPos)]
#var sParamName Item
#var doName tdoParam$($sParamName)$($nItemLevel)
#context $(.TemplateParams.TemplateParam1)
#var sElementName $($sElementName||.Options.*elementName||"Item")
#var sdoParam $($doPrevName).CreateChild("$($sElementName)")
#ifeq($(.Type),generic||string)
      ::staff::DataObject $($doName) =\
#else
#indent +2
#ifeqend
#cginclude "TypeSerialization.cpp"
#ifneq($(.Type),generic||string)
#indent -2
#ifeqend
#contextend
#ifeqend

#ifeq($(.TemplateParams.TemplateParam1.Namespace)$(.TemplateParams.TemplateParam1.Name),staff::Array)
      if ($($sSparseParentName))
#else
      if ($($sSparseName))
#ifeqend
      {
        $($doName).CreateAttribute("enc:position", $($sArrayPos));
      }
\
#cgpopvars
    }
  }
\
#ifeq($($nArrayRecursionLevel),0)
#ifeqend
