#var aStdIncludes
#var aStaffIncludes
\
#foreach $(Interface.Structs)
#cginclude "DetectStructInclude.h"
#end       // foreach Interface.Structs
\
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.Extern),0)
#context $(Typedef.DataType)
#cginclude "DetectTypeInclude.h"
#contextend
#ifeqend
#end       // foreach Interface.Typedefs
\
#var bClasses 0
#foreach $(Interface.Classes)
#ifeq($(Class.Extern),0)
#var bClasses 1
#foreach $(Class.Members)
#foreach $(Member.Params)
#context $(Param.DataType)
#cginclude "DetectTypeInclude.h"
#contextend
#end // also check return type
#context $(Member.Return)
#cginclude "DetectTypeInclude.h"
#contextend
#end
#ifeqend
#end       // foreach Interface.Classes
\
\
\
$($aStdIncludes.!replace/[/#include </.!replace/]/>\n/)\
$($aStaffIncludes.!replace/[/#include </.!replace/]/>\n/)\
\
#ifneq($($bClasses),0)
#include <staff/common/IService.h>
#ifeqend
