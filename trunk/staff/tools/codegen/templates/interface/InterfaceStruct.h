#ifeq($(Struct.Extern),false)
\
#var sNewOpeningNs $(Struct.OpeningNs)
#var sNewEndningNs $(Struct.EndingNs)
#ifneq($($sNewOpeningNs),$($sOpeningNs))
$($sEndingNs)
$($sNewOpeningNs)
#ifeqend   // namespace changed
#ifneq($(Struct.Description),)
  //! $(Struct.Description)
#ifeqend
#ifneq($(Struct.Detail),)
  /*! $(Struct.Detail) */
#ifeqend
#foreach $(Struct.Options)
  // *$($ThisElementName): $($ThisElementValue)
#end
  struct $(Struct.Name)\
#ifneq($(Struct.ParentName),)
: public $(Struct.ParentNsName)
#else

#ifeqend
  {
#foreach $(Struct.Enums)
#indent +
#cginclude "InterfaceEnum.h"
#indent -
#end
#foreach $(Struct.Structs)
#indent +
#cginclude "InterfaceStruct.h"
#indent -
#end
#var bHaveDefaults 0
#foreach $(Struct.Members)
#foreach $(Param.Options)
    // *$($ThisElementName): $($ThisElementValue)
#end
#ifneq($(Param.Options.*defaultValue),)
#var bHaveDefaults 1
#ifeqend
    $(Param.DataType.UsedName) $(Param.Name);\
#ifneq($(Param.Description),)
  //!< $(Param.Description)
#else

#ifeqend
#end
#ifeq($($bHaveDefaults),1) // default constructor
#var nDefFirst 0

    $(Struct.Name)():
      \
#foreach $(Struct.Members)
#ifneq($(Param.Options.*defaultValue),)
#ifeq($($nDefFirst),1)
, \
#ifeqend
#var nDefFirst 1
#ifeq($(Param.DataType.Type),template)
#var sDefValueTypeName $(Param.DataType.TemplateParams.TemplateParam1.Name)
#var sDefValueTypeNsName $(Param.DataType.TemplateParams.TemplateParam1.NsName)
#var sDefValueDataType $(Param.DataType.TemplateParams.TemplateParam1.Type)
#else
#var sDefValueTypeName $(Param.DataType.Name)
#var sDefValueTypeNsName $(Param.DataType.NsName)
#var sDefValueDataType $(Param.DataType.Type)
#ifeqend
\
#ifeq($($sDefValueDataType),typedef)
#foreach $(Interface.Typedefs)
#ifeq($(Typedef.NsName),$($sDefValueTypeNsName))
#var sDefValueTypeName $(Typedef.DataType.Name)
#ifeqend
#end
#ifeqend
\
#ifeq($($sDefValueTypeName),$($sStringTypes))
$(Param.Name)("$(Param.Options.*defaultValue)")\
#else
#ifeq($($sDefValueTypeName),char)
$(Param.Name)('$(Param.Options.*defaultValue)')\
#else
$(Param.Name)($(Param.Options.defaultValue.!fixid))\
#ifeqend
#ifeqend
#ifeqend
#end

    {
    }
#ifeqend
#ifeq($(Struct.Options.*abstract),true||1)
    virtual ~$(Struct.Name)() {}
#ifeqend
  };

#ifneq($($sNewOpeningNs),$($sOpeningNs))
#var sOpeningNs $($sNewOpeningNs)
#var sEndingNs $($sNewEndningNs)
#ifeqend   // namespace changed
#ifeqend   // extern
