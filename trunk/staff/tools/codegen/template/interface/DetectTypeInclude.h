#ifeq($(.Name),Optional||Nillable)
#ifneq($($aStaffIncludes.!match/$(.Name).h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/$(.Name).h]
#ifeqend
#ifeqend // optional or nillable
\
#ifeq($(.Namespace),std::)
\
#var sCurrCheckType $(.Name)
#ifeq($($sCurrCheckType),multimap)
#var sCurrCheckType map
#ifeqend
#ifneq($($aStdIncludes.!match/[$($sCurrCheckType)]/),true)
#var aStdIncludes $($aStdIncludes)[$($sCurrCheckType)]
#ifeqend
\
#else
#ifeq($(.Namespace),staff::)
\
#ifeq($(.Name),DataObject||Abstract||Attribute||QName||Operation||ICallback)
#ifneq($($aStaffIncludes.!match/$(.Name).h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/$(.Name).h]
#ifeqend
#else
\
#ifneq($($aStaffIncludes.!match/WsdlTypes.h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/WsdlTypes.h]
#ifeqend // wsdl types
\
#ifeq($(.Name),anyAttribute)
#ifneq($($aStaffIncludes.!match/Attribute.h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/common/Attribute.h]
#ifeqend // any attribute
#ifeqend // any attribute
\
#ifeq($(.Name),hexBinary)
#ifneq($($aStaffIncludes.!match/HexBinary.h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/utils/HexBinary.h]
#ifeqend
#ifeqend
\
#ifeq($(.Name),base64Binary)
#ifneq($($aStaffIncludes.!match/Base64Binary.h/),true)
#var aStaffIncludes $($aStaffIncludes)[staff/utils/Base64Binary.h]
#ifeqend
#ifeqend
\
#ifeqend // staff types
#ifeqend // staff
#ifeqend // std
\
#ifeq($(.Type),template)
#foreach $(.TemplateParams)
#cginclude "DetectTypeInclude.h"
#end
#ifeqend
\
