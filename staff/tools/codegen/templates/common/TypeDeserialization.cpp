#ifneq(,)
// in var: sParam: rstStruct.param1||(empty to use lvalue serialization)
// in var: sParamName: param1
// in var: sdoParam: rdoParam
#ifeqend
\
#var bLValue
#ifeq($($bRValue),)
#ifeq($($sOptMod)$(.Type),string||dataobject) // test it is lvalue and string or dataobject and not optional
#ifeq($($sParam),)
#var bLValue 1
#else
#ifeq($($sParamName),)
#var bLValue 1
#ifeqend
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
  const staff::Attribute& rAttr$($sParamName) = \
$($sdoParam).GetAttributeByLocalNameOpt("$(.Options.*elementName||$sParamName)");
  if (!rAttr$($sParamName).IsNull()) // optional attribute
#else // not attribute
#ifneq($($bUseParentElem),)
#var doName $($sdoParam)
#else
#var doName $($sdoParam)$($sParamName)
  const staff::DataObject& $($doName) = $($sdoParam).GetChildByLocalNameOpt("$(.Options.*elementName||$sParamName)");
#ifeqend
  if (!$($doName).IsNull()) // optional
#ifeqend // is attr
#else                               // nillable
#ifneq($($bUseParentElem),)
#var doName $($sdoParam)
#else
#ifeq($($sOptMod),)
#var doName $($sdoParam)$($sParamName)
  const staff::DataObject& $($sdoParam)$($sParamName) = $($sdoParam).GetChildByLocalName("$(.Options.*elementName||$sParamName)");
#else
#var doName $($sdoParam)
#ifeqend
#ifeqend
  if (!$($doName).IsNil()) // $(.Name)
#ifeqend // optional || nillable
  {
#cgpushvars
#var sOptMod $($sOptMod)*
#var sdoParam $($doName)
#indent +
#context $(.TemplateParams.TemplateParam1)
#ifneq($(.Options.*isAttribute),true||1)
#var sParamName
#ifeqend
#cginclude "TypeDeserialization.cpp"
#contextend
#indent -
#cgpopvars
  }
#ifeq($(.Name),Nillable)
#ifneq($($sOptMod),)
  else
  {
    // touch the variable to make it nil instead of null: Optional< Nillable< Data > >
    $($sOptMod)$($sParam);
  }
#ifeqend
#ifeqend
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
  STAFF_ASSERT(rAttr$($sParamName).GetValue($($sOptMod)$($sParam)), "Invalid value for attribute $($sParamName)");
#else
  $($sdoParam).GetAttributeValueByName("$(.Options.*elementName||$sParamName)", $($sOptMod)$($sParam));
#ifeqend
#else
#ifeq($(.Type),enum)
  $($sOptMod)$($sParam) = ::staff::DeserializeEnum_$(.NsName.!mangle)_FromString(\
#ifneq($($sOptMod),) // is optional
rAttr$($sParamName).GetText());
#else
$($sdoParam).GetAttributeTextByName("$(.Options.*elementName||$sParamName)"));
#ifeqend
#else
#cgerror Cannot deserialize type $(.Type) to attribute value. $($sParamName)
#ifeqend
#ifeqend
#else                                        // ############ common deserialization ##########
\
\
#ifeq($(.Type),generic||string)                                // ==== generic, anyAttribute ====
#ifneq($(.Name),Operation||MessageContext)
#ifeq($(.Name),anyAttribute)
  for (staff::DataObject::ConstAttributeIterator itAttr = ($($sOptMod)$($sdoParam)).AttributeBegin(),
       itAttrEnd = ($($sOptMod)$($sdoParam)).AttributeEnd(); itAttr != itAttrEnd; ++itAttr)
  {
    ($($sOptMod)$($sParam)).push_back(*itAttr);
  }
#else
\
#ifneq($($bLValue),) // test it is lvalue and string
#ifneq($($bUseParentElem),)
$($sdoParam).GetText()\
#else
$($sdoParam).GetChildTextByLocalName("$($sElementName||.Options.*elementName||$sParamName)")\
#ifeqend  // param optimization
#else
#ifneq($($bUseParentElem),)
  STAFF_ASSERT($($sdoParam).GetValue($($sOptMod)$($sParam)), "Invalid value for element $($sParam)");
#else
  STAFF_ASSERT($($sdoParam).GetChildValueByLocalName("$($sElementName||.Options.*elementName||$sParamName)", $($sOptMod)$($sParam)), "Invalid value for element $($sParam)");
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
#ifeq($(.Namespace)$(.Name),staff::Array)    // ### soapArray ###
#var bSoapEncPrefix
#cginclude "ArrayDeserialization.cpp"
#else
\
#ifeq($(.Type),template)                                       // ==== template ====
  for (staff::DataObject tdoItem = $($sdoParam).FirstChild(); !tdoItem.IsNull(); tdoItem.SetNextSibling())
  {
#var sItemName $(.Options.*itemName||"Item")
#ifneq($($bUseParentElement),true||1)
    if (tdoItem.GetLocalName() == "$($sItemName)")
    {
#indent +
#else
#ifeqend
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
#var bUseParentElement
#var sOptMod
#var sParam tKey
#var sParamName Key
#var sdoParam tdoItem
#var bRValue
#cginclude "TypeDeserialization.cpp"
#cgpopvars
#contextend
#indent -
\
    $(.TemplateParams.TemplateParam2)& rValue = ($($sOptMod)$($sParam))[tKey];
#indent +
#context $(.TemplateParams.TemplateParam2)
#cgpushvars
#var bUseParentElement
#var sOptMod
#var sParam rValue
#var sParamName Value
#var sdoParam tdoItem
#var bRValue
#cginclude "TypeDeserialization.cpp"
#cgpopvars
#contextend
#indent -
#else
\
#ifeq($(.Name),list||vector)                                 // ==== list ====
#var sParamTmp
#ifeq($(.TemplateParams.TemplateParam1.Type),struct||typedef||template)
    ($($sOptMod)$($sParam)).resize(($($sOptMod)$($sParam)).size() + 1);
#var sParamTmp ($($sOptMod)$($sParam)).back()
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
#indent +
#context $(.TemplateParams.TemplateParam1)
#cgpushvars
#var bUseParentElement
#var sOptMod
#var sParam $($sParamTmp)
#var sParamName
#var sdoParam tdoItem
#var bRValue
#cginclude "TypeDeserialization.cpp"
#cgpopvars
#contextend
#indent -
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
#ifneq($($bUseParentElement),true||1)
#indent -
    }
#else
#ifeqend
  }

#else
#cgerror unknown type($(.Type)) of sParamName: $(.Namespace)::$(.Name)
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend // abstract
#ifeqend // attribute
#ifeqend // optional or nillable
