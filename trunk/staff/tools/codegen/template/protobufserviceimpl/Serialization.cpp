#ifneq($(Interface.Enums.$Count),0)
///////////////////////////////////////////////////////////////////////////////////////////////////////
// enums
#foreach $(Interface.Enums)
#ifeq($(Enum.Extern),0) // do not serialize extern type
#cginclude <protobufserviceimpl/EnumSerialization.cpp>
#ifeqend
#end

#ifeqend
#ifneq($(Interface.Structs.$Count),0)
#ifneq($(Interface.Classes.$Count),0)
///////////////////////////////////////////////////////////////////////////////////////////////////////
// forward declarations
#foreach $(Interface.Structs)
#cginclude <protobufserviceimpl/StructSerialization.h>
#end
#ifeqend // forward

///////////////////////////////////////////////////////////////////////////////////////////////////////
// struct serializators/deserializators
#foreach $(Interface.Structs)
#cginclude <protobufserviceimpl/StructSerialization.cpp>
#end

#ifeqend // Interface.Structs.$Count != 0
