#ifneq(,)
// in var: sParam: rstStruct.param1
// in var: sParamName: param1
// in var: sdoParam: rdoParam || rdoParam.CreateChild("$($sParam)")
#ifeqend
\
#ifeq($(.Name),Optional||Nillable)           // ######### Optional or Nillable ############
  if (!$($sParam).IsNull()) // $(.Name)
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
#ifeq($(.Name),Nillable) // */
  else
  {
    $($sdoParam).SetNil();
  }
#ifeqend
#else // not optional or nillable
\
#ifeq($(.Name),Abstract)                     // ########## abstract #################

#ifeq($(.Options.*isAttribute),true||1) // attribute
#cgerror Can't serialize abstract member $($sParam) into attribute.
#ifeqend
#ifneq($(.TemplateParams.TemplateParam1.Type),struct)
#cgerror Abstract template type is not struct. Member $($sParam)
#ifeqend
#ifneq($($sdoParam.!match/.CreateChild/),true)
#var doName $($sdoParam)
#else
#var doName tdoParam$($sParamName)
  staff::DataObject $($doName) = $($sdoParam);
#ifeqend
  $($doName) << $($sOptMod)$($sParam);
#else                                        // ######### not abstract #####################
\
#ifeq($(.Options.*isAttribute),true||1)      // serialize to attribute
#ifeq($(.Type),generic||string||typedef)
  $($sdoParam).CreateAttribute("$($sParamName)", $($sOptMod)$($sParam));
#else
#ifeq($(.Type),enum)
  std::string sParam$($sParamName);
  $($sdoParam).CreateAttribute("$($sParamName)", sParam$($sParamName) << $($sOptMod)$($sParam));
#else
#cgerror Cannot serialize type $(.Type) to attribute value. $($sParamName)
#ifeqend
#ifeqend
#else                                        // ############ common serialization ##########
\
\
#ifeq($(.Type),generic||string)                                // ==== generic, anyAttribute ====
#ifeq($(.Name),anyAttribute)
  for (anyAttribute::const_iterator itAttr = ($($sOptMod)$($sParam)).begin(),
       itAttrEnd = ($($sOptMod)$($sParam)).end(); itAttr != itAttrEnd; ++itAttr)
  {
    $($sdoParam).AppendAttribute(const_cast<Attribute&>(*itAttr));
  }
#else
#ifeq($($sdoParam.!match/.CreateChild/),true)
  $($sdoParam.!depostfix/\)/), $($sOptMod)$($sParam));
#else
  $($sdoParam).SetValue($($sOptMod)$($sParam));
#ifeqend  // param optimization
#ifeqend // anyAttribute
#else
\
\
#ifeq($(.Type),dataobject)                                     // ==== dataobject ====
  $($sdoParam).AppendChild($($sOptMod)$($sParam));
#else
\
\
#ifneq($($sdoParam.!match/.CreateChild/),true)
#var doName $($sdoParam)
#else
#var doName tdoParam$($sParamName)
  staff::DataObject $($doName) = $($sdoParam);
#ifeqend
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
#ifeq($(.Type),template)                                       // ==== template ====
  for ($(.NsName)::const_iterator itItem = ($($sOptMod)$($sParam)).begin(), itItemEnd = ($($sOptMod)$($sParam)).end();
       itItem != itItemEnd; ++itItem)
  {
#var sElementName $(.Options.*elementName||"Item")
#ifeq($(.Name),map||multimap)                                  // ==== map ====
    staff::DataObject tdoItem = $($doName).CreateChild("$($sElementName)");
\
#indent +
#context $(.TemplateParams.TemplateParam1)
#cgpushvars
#var sOptMod
#var sParam itItem->first
#var sParamName Item
#var sdoParam tdoItem.CreateChild("Key")
#cginclude "TypeSerialization.cpp"
#cgpopvars
#contextend
#indent -
\
#indent +
#context $(.TemplateParams.TemplateParam2)
#cgpushvars
#var sOptMod
#var sParam itItem->second
#var sParamName Item
#var sdoParam tdoItem.CreateChild("Value")
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
#var sOptMod
#var sParam (*itItem)
#var sParamName Item
#var sdoParam $($doName).CreateChild("$($sElementName)")
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
#ifeqend // abstract
#ifeqend // attribute
#ifeqend // optional or nillable
