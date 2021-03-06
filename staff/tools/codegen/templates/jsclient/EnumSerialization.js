#ifeq($(Enum.Extern),false) // do not serialize extern type
#cginclude <common/StringTypes>

// enum $(Enum.NsName.!mangle)
var o$(Enum.NsName.!mangle) = staff.object("$(Enum.NsName.!dot)");
#var EnumValue 0
#foreach $(Enum.Members)
o$(Enum.NsName.!mangle).$(Member.Name) =\
#ifneq($(Member.Value),)
 $(Member.Value);
#var EnumValue $(Member.Value.!inc)
#else
 $($EnumValue);
#var EnumValue $($EnumValue.!inc)
#ifeqend
#end

o$(Enum.NsName.!mangle).serialize = function(oElement, oType)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
  var sResult = "<INVALID>";
  switch (oType)
  {
#foreach $(Enum.Members)
  case $(Enum.NsName.!mangle).$(Member.Name):
    sResult = "$(Member.Value||Member.Name)";
    break;
#end
  };
  oElement.setText(sResult)
#else
  oElement.setText(oType)
#ifeqend
  return oElement;
}

o$(Enum.NsName.!mangle).deserialize = function(oElement)
{
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
  var oResult = -1;
  switch (staff.getElementValue(oElement))
  {
#foreach $(Enum.Members)
  case "$(Member.Value||Member.Name)":
    oResult = $(Enum.NsName.!mangle).$(Member.Name);
    break;
#end
  };
  return oResult;
#else
  return parseInt(staff.getElementValue(oElement));
#ifeqend
}

#ifeqend //ifeq((Enum.Extern),false) // do not serialize extern type
