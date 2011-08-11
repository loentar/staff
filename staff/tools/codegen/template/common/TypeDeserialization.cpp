#ifneq(,)
// in var: sParam: rstStruct.param1||(empty to use lvalue serialization)
// in var: sParamName: param1
// in var: sdoParam: rdoParam
#ifeqend
\
#var bLValue
#ifeq($($sOptMod)$(.Type),string||dataobject) // test it is lvalue and string or dataobject and not optional
#ifeq($($sParam),)
#var bLValue 1
#else
#ifeq($($sParamName),)
#var bLValue 1
#ifeqend
#ifeqend
#ifeqend
\
#var bUseParentElem
#ifeq($(.Options.*useParentElement),true||1)
#var bUseParentElem 1
#else
#ifeq($($sParamName),)
#var bUseParentElem 1
#ifeqend
#ifeqend
\
#ifeq($(.Name),Optional||Nillable)           // ######### Optional or Nillable ############
#ifeq($(.Name),Optional)
#ifeq($(.Options.*isAttribute),true||1)
  const staff::Attribute& rAttr$($sParamName) = $($sdoParam).GetAttributeByLocalNameOpt("$($sParamName)");
  if (!rAttr$($sParamName).IsNull()) // optional attribute
#else // not attribute
#ifneq($($bUseParentElem),)
#var doName $($sdoParam)
#else
#var doName $($sdoParam)$($sParamName)
  const staff::DataObject& $($sdoParam)$($sParamName) = $($sdoParam).GetChildByLocalNameOpt("$($sParamName)");
#ifeqend
  if (!$($doName).IsNull()) // optional
#ifeqend // is attr
#else                               // nillable
#ifneq($($bUseParentElem),)
#var doName $($sdoParam)
#else
#var doName $($sdoParam)$($sParamName)
#ifeq($($sOptMod),)
  const staff::DataObject& $($sdoParam)$($sParamName) = $($sdoParam).GetChildByLocalName("$($sParamName)");
#ifeqend
#ifeqend
  if (!$($doName).IsNil()) // $(.Name)
#ifeqend // optional || nillable
  {
#cgpushvars
#var sOptMod $($sOptMod)*
#indent +
#context $(.TemplateParams.TemplateParam1)
#cginclude "TypeDeserialization.cpp"
#contextend
#indent -
#cgpopvars
  }
#else // not optional or nillable */
\
#ifneq($($bUseParentElem),)
#var doName $($sdoParam)
#else
#var doName $($sdoParam).GetChildByLocalName("$($sParamName)")
#ifeqend
\
#ifeq($(.Name),Abstract)                     // ########## abstract #################

  $($doName) >> $($sOptMod)$($sParam);
#else                                        // ######### not abstract #####################
\
#ifeq($(.Options.*isAttribute),true||1)      // deserialize to attribute
#ifeq($(.Type),generic||string||typedef)
#ifneq($($sOptMod),) // is optional
  rAttr$($sParamName).GetValue($($sOptMod)$($sParam));
#else
  $($sdoParam).GetAttributeValueByName("$($sParamName)", $($sOptMod)$($sParam));
#ifeqend
#else
#ifeq($(.Type),enum)
#ifneq($($sOptMod),) // is optional
  rAttr$($sParamName).GetText() >> $($sOptMod)$($sParam);
#else
  $($sdoParam).GetAttributeTextByName("$($sParamName)") >> $($sOptMod)$($sParam);
#ifeqend
#else
#cgerror Cannot deserialize type $(.Type) to attribute value. $($sParamName)
#ifeqend
#ifeqend
#else                                        // ############ common deserialization ##########
\
\
#ifeq($(.Type),generic||string)                                // ==== generic, anyAttribute ====
#ifneq($(.Name),Operation||MessageContext||COperation||CMessageContext)
#ifeq($(.Name),anyAttribute)
  for (staff::DataObject::ConstAttributeIterator itAttr = ($($sOptMod)$($sdoParam)).AttributeBegin(),
       itAttrEnd = ($($sOptMod)$($sdoParam)).AttributeEnd(); itAttr != itAttrEnd; ++itAttr)
  {
    ($($sOptMod)$($sParamName)).push_back(*itAttr);
  }
#else
\
#ifneq($($bLValue),) // test it is lvalue and string
#ifneq($($bUseParentElem),)
$($sdoParam).GetText()\
#else
$($sdoParam).GetChildTextByLocalName("$($sParamName)")\
#ifeqend  // param optimization
#else
#ifneq($($bUseParentElem),)
  $($sdoParam).GetValue($($sOptMod)$($sParam));
#else
  $($sdoParam).GetChildValueByLocalName("$($sParamName)", $($sOptMod)$($sParam));
#ifeqend  // param optimization
#ifeqend // lvalue
#ifeqend // anyAttribute
#ifeqend // not operation or message context
#else
\
\
#ifeq($(.Type),dataobject)                                     // ==== dataobject ====
#ifeq($($bLValue),)
  $($sOptMod)$($sParam) = $($doName).FirstChild();
#else
$($doName).FirstChild()\
#ifeqend
#else
\
\
#ifeq($(.Type),typedef)
  DeserializeTypedef_$(.NsName.!mangle)($($doName), $($sOptMod)$($sParam));
#else
\
\
#ifeq($(.Type),struct||enum)                                  // ==== enum, struct ====
  $($doName) >> $($sOptMod)$($sParam);
#else
\
\
#ifeq($(.Type),template)                                       // ==== template ====
  for (staff::DataObject tdoItem = $($sdoParam).FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
#var sElementName $(.Options.*elementName||"Item")
    if (tdoItem.GetLocalName() == "$($sElementName)")
    {
#ifeq($(.Name),map||multimap)                                  // ==== map ====
    $(.TemplateParams.TemplateParam1) tKey\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
\
#indent +
#context $(.TemplateParams.TemplateParam1)
#cgpushvars
#var sOptMod
#var sParam tKey
#var sParamName Key
#var sdoParam tdoItem
#cginclude "TypeDeserialization.cpp"
#cgpopvars
#contextend
#indent -
\
    $(.TemplateParams.TemplateParam2)& rValue = ($($sOptMod)$($sParam))[tKey];
#indent +
#context $(.TemplateParams.TemplateParam2)
#cgpushvars
#var sOptMod
#var sParam rValue
#var sParamName Value
#var sdoParam tdoItem
#cginclude "TypeDeserialization.cpp"
#cgpopvars
#contextend
#indent -
#else
\
#ifeq($(.Name),list||vector)                                 // ==== list ====
#var sParamTmp
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||template)
      $(.TemplateParams.TemplateParam1.NsName)& rItem = *($($sOptMod)$($sParam)).\
insert(($($sOptMod)$($sParam)).end(), $(.TemplateParams.TemplateParam1.NsName)());
#var sParamTmp rItem
#else
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||enum)
      $(.TemplateParams.TemplateParam1.NsName) tItem\
#ifeq($(.TemplateParams.TemplateParam1.Type),generic)
 = 0\
#ifeqend
;
#var sParamTmp tItem
#else
      ($($sOptMod)$($sParam)).push_back(\
#ifeqend
#ifeqend
#indent +2
#context $(.TemplateParams.TemplateParam1)
#cgpushvars
#var sOptMod
#var sParam $($sParamTmp)
#var sParamName
#var sdoParam tdoItem
#cginclude "TypeDeserialization.cpp"
#cgpopvars
#contextend
#indent -2
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||enum)
      ($($sOptMod)$($sParam)).push_back(tItem);
#else
#ifneq($(.TemplateParams.TemplateParam1.Type),struct||typedef||template)
);
#ifeqend
#ifeqend
#else
#cgerror template type $(.NsName) is not supported
#ifeqend
#ifeqend
    }
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
