#ifeq($(Struct.Extern),0)
#foreach $(Struct.Members)
#context $(Param.DataType)
#cginclude "DetectTypeInclude.h"
#contextend
#end       // foreach Struct.Members
#foreach $(Struct.Structs)
#cginclude "DetectStructInclude.h"
#end
#ifeqend
#ifeq($(Struct.Options.*abstract),true||1)
#ifneq($($aStaffIncludes.!match/Abstract.h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/Abstract.h]
#ifeqend
#ifeqend
