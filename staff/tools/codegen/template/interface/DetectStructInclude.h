#foreach $(Struct.Members)
#context $(Param.DataType)
#cginclude "DetectTypeInclude.h"
#contextend
#end       // foreach Struct.Members
#foreach $(Struct.Structs)
#cginclude "DetectStructInclude.h"
#end
