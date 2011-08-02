#ifeq($(.Name),Optional||Nillable)
\
#ifneq($($aStaffIncludes.!match/$(.Name).h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/$(.Name).h]
#ifeqend
\
#var sOptContext .TemplateParams.TemplateParam1
#else
#var sOptContext .
#ifeqend // optional
#context $($sOptContext)
\
#ifeq($(.Namespace),std::)
\
#ifneq($($aStdIncludes.!match/[$(.Name)]/),true)
#var sCurrCheckType $(.Name)
#ifeq($($sCurrCheckType),multimap)
#var sCurrCheckType map
#ifeqend
#var aStdIncludes $($aStdIncludes)[$($sCurrCheckType)]
#ifeqend
\
#else
#ifeq($(.Namespace),staff::)
\
#ifeq($(.Name),DataObject||Abstract||Attribute||Operation||ICallback)
#ifneq($($aStaffIncludes.!match/$(.Name).h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/$(.Name).h]
#ifeqend
#else
\
#ifneq($($aStaffIncludes.!match/WsdlTypes.h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/WsdlTypes.h]
#ifeqend // wsdl types
\
#ifeqend // staff types
#ifeqend // staff
#ifeqend // std
\
#ifeq($(.Type),template)
#ifneq($(.Name),Optional)
#foreach $(.TemplateParams)
#cginclude "DetectTypeInclude.h"
#end
#ifeqend
#ifeqend
\
#contextend
