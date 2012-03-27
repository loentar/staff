#ifeq($(Struct.Extern),0) // do not serialize extern type

// struct $(Struct.NsName.!dot)
var o$(Struct.NsName.!mangle) = staff.object("$(Struct.NsName.!dot)");
o$(Struct.NsName.!mangle).serialize = function(oElement, oType)
{
#ifneq($(Struct.ParentName),)
  // serialize parent struct
  $(Struct.ParentNsName.!dot).serialize(oElement, oType);

#ifeqend
#foreach $(Struct.Members)
\
#var Type oType.$(Param.Name)
#var Element oElement.createChild("$(Param.Name)")
#context $(Param.DataType)
#cginclude "TypeSerialize.js"
#contextend
\
#end
  return oElement;
}

o$(Struct.NsName.!mangle).deserialize = function(oElement)
{
#ifneq($(Struct.ParentName),)
  // deserialize parent struct
  var tResult = $(Struct.ParentNsName.!dot).deserialize(oElement);

#else
  var tResult = {};
#ifeqend
#foreach $(Struct.Members)
\
  tResult.$(Param.Name) = \
#var Type tResult.$(Param.Name)
#var Element staff.getChildElement(oElement, '$(Param.Name)')
#context $(Param.DataType)
#cginclude "TypeDeserialize.js"
#contextend
;
\
#end
  return tResult;
}

#foreach $(Struct.Enums)
#cginclude "EnumSerialization.js"
#end
#foreach $(Struct.Structs)
#cginclude "StructSerialization.js"
#end

#ifeqend //ifeq((Struct.Extern),0) // do not serialize extern type
