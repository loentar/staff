#ifeq($(.Type),generic||string)
  $($Element).setText($($Type));
#else
#ifeq($(.Type),dataobject)
  $($Element).appendChild($($Type));
#else
#ifeq($(.Type),typedef||struct||enum)
  $(.NsName.!dot).serialize($($Element), $($Type));
#else
#ifeq($(.Type),template)
\
  for (var sKey in $($Type))
  {
    var oElem = $($Element);
    var oValue = $($Type)[sKey];
    if (oValue != null && (!$($Type).hasOwnProperty || $($Type).hasOwnProperty(sKey)) && typeof oValue != 'function')
    {
      var oItem = oElem.createChild("$(.Options.*elementName||"Item")");
#ifeq($(.Name),map)
      var oItemKey = oItem.createChild("Key");
#indent +2
#context $(.TemplateParams.TemplateParam1)
#var Type sKey
#var Element oItemKey
#cginclude "TypeSerialize.js"
#contextend
#indent -2
      var oItemValue = oItem.createChild("Value");
#indent +2
#context $(.TemplateParams.TemplateParam2)
#var Type oValue
#var Element oItemValue
#cginclude "TypeSerialize.js"
#contextend
#indent -2
\
#else // list, vector, etc..
#indent +2
#context $(.TemplateParams.TemplateParam1)
#var Type oValue
#var Element oItem
#cginclude "TypeSerialize.js"
#contextend
#indent -2
#ifeqend
    }
  }

\
#else
#cgerror ".Type = $(.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
