#ifeq($(Struct.Extern),0)
$(Struct.NsName.!deprefix/$($rootns)/)& operator<<($(Struct.NsName.!deprefix/$($rootns)/)& rLeft, const $(Struct.NsName)& rRight);
$(Struct.NsName)& operator<<($(Struct.NsName)& rLeft, const $(Struct.NsName.!deprefix/$($rootns)/)& rRight);
#foreach $(Struct.Enums)
#cginclude <protobufserviceimpl/EnumSerialization.h>
#end
#foreach $(Struct.Structs)
#cginclude <protobufserviceimpl/StructSerialization.h>
#end
#ifeqend
