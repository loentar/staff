#foreach $(Interface.Enums)
#ifeq($(Enum.Extern),0) // do not serialize extern type
#cginclude "EnumSerialization.js"
#ifeqend
#end
#foreach $(Interface.Structs)
#cginclude "StructSerialization.js"
#end
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0) // do not serialize extern type

// typedef $(Typedef.NsName.!dot)
var o$(Typedef.NsName.!mangle) = staff.object("$(Typedef.NsName.!dot)");
o$(Typedef.NsName.!mangle).serialize = function(oElement, oType)
{
#var Type oType
#var Element oElement
#context $(Typedef.DataType)
#cginclude "TypeSerialize.js"
#contextend
  return oElement;
}

o$(Typedef.NsName.!mangle).deserialize = function(oElement)
{
  var oType = \
#var Type oType
#var Element oElement
#context $(Typedef.DataType)
#cginclude "TypeDeserialize.js"
#contextend
  return oType;
}

#ifeqend //ifeq($(Typedef.Extern),0) // do not serialize extern type
#end // foreach $(Interface.Structs)
