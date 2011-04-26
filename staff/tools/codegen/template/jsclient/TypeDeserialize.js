#ifeq($(.Type),generic)
\
#ifeq($(.Name),byte||int||long||short||unsigned)
parseInt(staff.getElementValue($($Element)))\
#else
#ifeq($(.Name),float||double)
parseFloat(staff.getElementValue($($Element)))\
#else
#ifeq($(.Name),bool)
staff.getElementValue($($Element)).toLowerCase() == "true"\
#else
#cgwarning unsupported generic: $(Param.DataType.Name), parsing as string
staff.getElementValue($($Element))\
#ifeqend
#ifeqend
#ifeqend
\
#else
#ifeq($(.Type),string)
staff.getElementValue($($Element))\
#else
#ifeq($(.Type),dataobject)
$($Element)\
#else
#ifeq($(.Type),typedef||struct||enum)
$(.NsName.!dot).deserialize($($Element))\
#else
#ifeq($(.Type),template)
#ifeq($(.Name),map)
{};
#else
[];
#ifeqend
  staff.eachChildElementName($($Element), "$(.Options.*elementName||"Item")", function(oElem){
#ifeq($(.Name),map)
    var oItem = {};
    var oItemKey = \
#cgpushvars
#indent +2
#context $(.TemplateParams.TemplateParam1)
#var Type oItemKey
#var Element staff.getChildElement(oElem, "Key")
#cginclude "TypeDeserialize.js"
;
#contextend
#cgpushvars
#indent -2
#cgpopvars
    var oItemValue = \
#cgpushvars
#indent +2
#context $(.TemplateParams.TemplateParam2)
#var Type oValue
#var Element staff.getChildElement(oElem, "Value")
#cginclude "TypeDeserialize.js"
;
#contextend
#indent -2
#cgpopvars
      oItem[oItemKey] = oItemValue;
\
#else // list, vector, etc..
\
\
    var oItem = \
#cgpushvars
#indent +2
#context $(.TemplateParams.TemplateParam1)
#var Type oItem
#var Element oElem
#cginclude "TypeDeserialize.js"
;
#contextend
#indent -2
#cgpopvars
    $($Type).push(oItem);
#ifeqend
  });

\
#else
#cgerror ".Type = $(.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#ifeqend
\
