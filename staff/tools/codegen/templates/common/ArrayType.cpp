#ifeq($($bSerialize),1)
#ifeq($($sArrayType),)
#var sArrayType " + $($sParam).GetDimensionsStr() + "
#else
#var sArrayType []$($sArrayType)
#ifeqend
#ifeqend
\
#ifeq($(.TemplateParams.TemplateParam1.Namespace)$(.TemplateParams.TemplateParam1.Name),staff::Array)
\
#context $(.TemplateParams.TemplateParam1)
#cginclude "ArrayType.cpp"
#contextend
\
#else // --------------- leaf or 1D array: write type -------------------
\
#var sArrayType $(.TemplateParams.TemplateParam1.Name)$($sArrayType)
#ifeq($(.TemplateParams.TemplateParam1.Type),generic||string)
#ifeq($($bSerialize),1)
#var sArrayType xsd:$($sArrayType)
#else
#var sArrayType $($sArrayType)
#ifeqend
#else
#context $(.TemplateParams.TemplateParam1)
#contextend
#ifeq($($bSerialize),1)
#var sArrayType ns0:$($sArrayType)
#else
#var sArrayType $($sArrayType)
#ifeqend
#ifeqend
\
#ifeqend
