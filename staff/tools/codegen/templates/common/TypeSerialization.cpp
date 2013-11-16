#ifneq(,)
// in var: sParam: rstStruct.param1
// in var: sParamName: param1
// in var: sdoParam: rdoParam || rdoParam.CreateChild("$($sParam)")
#ifeqend
\
\
\
#var elementForm $(Interface.Options.*elementFormDefault)
#var attributeForm $(Interface.Options.*attributeFormDefault)
\
#ifeq($(.Name),Nillable||Optional)
#var recreateChild false
#else
#ifeq($(.Type),generic||string)
#var recreateChild false
#else
#var recreateChild $($sdoParam.!match/.CreateChild/)
#ifeqend
\
\
#ifneq($(.Options.*form),)
#ifeq($(.Options.*isAttribute),true||1)
#var attributeForm $(.Options.form)
#else
#var elementForm $(.Options.form)
#ifeqend
#ifeqend
#ifeq($(.Options.*isRef),true||1)
#var elementForm qualified
#ifeqend
\
#ifeqend // Nillable||Optional
\
#ifeq($($recreateChild),true)
#var doName tdoParam$($sParamName)$($nItemLevel)
  staff::DataObject $($doName) = $($sdoParam);
#else
#var doName $($sdoParam)
#ifeqend
\
\
#ifeq($(.Name),Optional||Nillable)           // ######### Optional or Nillable ############
  if (!($($sOptMod)$($sParam)).IsNull()) // $(.Name)
  {
#cgpushvars
#var sOptMod $($sOptMod)*
#indent +
#context $(.TemplateParams.TemplateParam1)
#cginclude "TypeSerialization.cpp"
#contextend
#indent -
#cgpopvars
  }
#ifeq($(.Name),Nillable)
  else
  {
#ifeq($($elementForm),qualified)
#ifeq($($bDontWriteNs),)
#ifeq($($doName.!match/.CreateChild/),true)
#var doName1 $($doName)
#var doName tdoParam$($sParamName)$($nItemLevel)
    staff::DataObject $($doName) = $($doName1);
#ifeqend
#ifneq($(.Options.*targetNamespace),)
    $($doName).SetNamespaceUriGenPrefix("$(.Options.*targetNamespace)");
#else
    $($doName).RedeclareParentNamespace();
#ifeqend
#ifeqend
#ifeqend
    $($doName).SetNil();
  }
#ifeqend
#else // not optional or nillable
\
#ifeq($($elementForm),qualified)
#ifneq($(.Type),generic||string)
#ifeq($($bDontWriteNs),)
#var elPath $($ThisElementPath)
#ifeq($($elPath),Project.Interfaces.Interface.Classes.Class.Members.Member.Params.Param.DataType)
#ifneq($(Member.Options.*requestElement),) // don't skip namespace setting, if this is not a root request element
#var elPath
#ifeqend
#ifeqend
#ifneq($($elPath),Project.Interfaces.Interface.Classes.Class.Members.Member.Params.Param.DataType) // don't set namespace for request element
#ifneq($(.Options.*targetNamespace),)
  $($doName).SetNamespaceUriGenPrefix("$(.Options.*targetNamespace)");
#else
  $($doName).RedeclareParentNamespace();
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
\
\
#ifeq($(.Name),Abstract)                     // ########## abstract #################

#ifeq($(.Options.*isAttribute),true||1) // attribute
#cgerror Can't serialize abstract member $($sParam) into attribute.
#ifeqend
#ifneq($(.TemplateParams.TemplateParam1.Type),struct)
#cgerror Abstract template type is not struct. Member $($sParam)
#ifeqend
  $($doName) << $($sOptMod)$($sParam);
#else                                        // ######### not abstract #####################
\
#ifeq($(.Options.*isAttribute),true||1)      // serialize to attribute
\
#ifeq($($attributeForm),qualified)
  std::string sPrefix$($sParamName);
  $($doName).SetNamespaceUriGenPrefix("$(.Options.*targetNamespace||Interface.Options.*targetNamespace)", &sPrefix$($sParamName), false);
#var sAttrPrefix sPrefix$($sParamName) + ": \
#else
#var sAttrPrefix "
#ifeqend
\
#ifeq($(.Type),generic||string||typedef)
  $($doName).CreateAttribute($($sAttrPrefix)$(.Options.*elementName||$sParamName)", $($sOptMod)$($sParam));
#else
#ifeq($(.Type),enum)
  $($doName).CreateAttribute($($sAttrPrefix)$(.Options.*elementName||$sParamName)", \
::staff::SerializeEnum_$(.NsName.!mangle)_ToString($($sOptMod)$($sParam))$($attrFormDecl));
#else
#cgerror Cannot serialize type $(.Type) to attribute value. $($sParamName)
#ifeqend
#ifeqend
\
#else                                        // ############ common serialization ##########
\
\
#ifeq($(.Type),generic||string)                                // ==== generic, anyAttribute ====
#ifeq($(.Name),anyAttribute)
  for (anyAttribute::const_iterator itAttr = ($($sOptMod)$($sParam)).begin(),
       itAttrEnd = ($($sOptMod)$($sParam)).end(); itAttr != itAttrEnd; ++itAttr)
  {
    $($doName).AppendAttribute(const_cast<Attribute&>(*itAttr));
  }
#else
#ifeq($($sdoParam.!match/.CreateChild/),true)
#ifeq($($elementForm),qualified)
#var doName tdoParam$($sParamName)$($nItemLevel)
  $($sdoParam.!depostfix/\)/), $($sOptMod)$($sParam))\
#ifeq($($bDontWriteNs),)
#ifneq($(.Options.*targetNamespace),)
.SetNamespaceUriGenPrefix("$(.Options.*targetNamespace)")\
#else
.RedeclareParentNamespace()\
#ifeqend
#ifeqend
;
#else  // !qualified
  $($sdoParam.!depostfix/\)/), $($sOptMod)$($sParam));
#ifeqend
#else
#ifeq($($elementForm),qualified)
#ifeq($($bDontWriteNs),)
#ifneq($(.Options.*targetNamespace),)
  $($doName).SetNamespaceUriGenPrefix("$(.Options.*targetNamespace)");
#else
  $($doName).RedeclareParentNamespace();
#ifeqend
#ifeqend // bDontWriteNs
#ifeqend // form
  $($doName).SetValue($($sOptMod)$($sParam));
#ifeqend  // param optimization
#ifeqend // anyAttribute
#else
\
\
#ifeq($(.Type),dataobject)                                     // ==== dataobject ====
  $($doName).AppendChild($($sOptMod)$($sParam));
#else
\
\
#ifeq($(.Type),typedef)
  SerializeTypedef_$(.NsName.!mangle)($($doName), $($sOptMod)$($sParam));
#else
\
\
#ifeq($(.Type),struct||enum)                                   // ==== enum, struct ====
  $($doName) << $($sOptMod)$($sParam);
#else
\
\
#ifeq($(.Namespace)$(.Name),staff::Array)                      // ### soapArray ###
#cginclude "ArraySerialization.cpp"
#else
\
#ifeq($(.Type),template)                                       // ==== template ====
  for ($(.NsName)::const_iterator itItem$($nItemLevel) = ($($sOptMod)$($sParam)).begin(),\
 itItem$($nItemLevel)End = ($($sOptMod)$($sParam)).end();
       itItem$($nItemLevel) != itItem$($nItemLevel)End; ++itItem$($nItemLevel))
  {
#var sItemName $(.Options.*itemName||"Item")
#ifeq($(.Name),map||multimap)                                  // ==== map ====
#ifeq($($bUseParentElement),true||1)
#var sdoItem $($doName)
#else
#var sdoItem tdoItem
    staff::DataObject $($sdoItem) = $($doName).CreateChild("$($sItemName)");
#ifeqend
\
#indent +
#context $(.TemplateParams.TemplateParam1)
#cgpushvars
#var nItemLevelPrev $($nItemLevel)
#var nItemLevel $($nItemLevel.!inc.!trunc)
#var bUseParentElement
#var sOptMod
#var sParam itItem$($nItemLevelPrev)->first
#var sParamName Item
#var sdoParam $($sdoItem).CreateChild("Key")
#cginclude "TypeSerialization.cpp"
#cgpopvars
#contextend
#indent -
\
#indent +
#context $(.TemplateParams.TemplateParam2)
#cgpushvars
#var nItemLevelPrev $($nItemLevel)
#var nItemLevel $($nItemLevel.!inc.!trunc)
#var bUseParentElement
#var sOptMod
#var sParam itItem$($nItemLevelPrev)->second
#var sParamName Item
#var sdoParam $($sdoItem).CreateChild("Value")
#cginclude "TypeSerialization.cpp"
#cgpopvars
#contextend
#indent -
#else
\
#ifeq($(.Name),list||vector)                                 // ==== list ====
#indent +
#context $(.TemplateParams.TemplateParam1)
#cgpushvars
#var nItemLevelPrev $($nItemLevel)
#var nItemLevel $($nItemLevel.!inc.!trunc)
#var sOptMod
#var sParam (*itItem$($nItemLevelPrev))
#var sParamName Item
#ifeq($($bUseParentElement),true||1)
#var sdoItem $($doName)
#else
#var sdoItem tdoItem
#var sdoParam tdoItem
  staff::DataObject $($sdoItem) = $($doName).CreateChild("$($sItemName)");
#ifeqend
#var bUseParentElement
#cginclude "TypeSerialization.cpp"
#cgpopvars
#contextend
#indent -
#else
#cgerror template type $(.NsName) is not supported
#ifeqend
#ifeqend
  }

#else
#cgerror unknown type($(.Type)) of sParamName: $(Struct.NsName)::$(.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // abstract
#ifeqend // attribute
#ifeqend // optional or nillable
