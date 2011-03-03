#ifeq($(Struct.Extern),0)
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
  // *$($ThisNodeName): $($ThisNodeValue)
#end
  struct $(Struct.Name)\
#ifneq($(Struct.ParentName),)
: public $(Struct.ParentUsedName)
#else

#ifeqend
  {
#foreach $(Struct.Enums)
#indent +
#cginclude <common/InterfaceEnum.h>
#indent -
#end
#foreach $(Struct.Structs)
#indent +
#cginclude <common/InterfaceStruct.h>
#indent -
#end
#var bHaveDefaults 0
#foreach $(Struct.Members)
#foreach $(Param.Options)
    // *$($ThisNodeName): $($ThisNodeValue)
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
#ifeq($(Param.DataType.Type),string)
$(Param.Name)("$(Param.Options.*defaultValue)")\
#else
#ifeq($(Param.DataType.Name),char)
$(Param.Name)('$(Param.Options.*defaultValue)')\
#else
$(Param.Name)($(Param.Options.*defaultValue))\
#ifeqend
#ifeqend
#ifeqend
#end

    {
    }
#ifeqend
  };

#ifneq($($sNewOpeningNs),$($sOpeningNs))
#var sOpeningNs $($sNewOpeningNs)
#var sEndingNs $($sNewEndningNs)
#ifeqend   // namespace changed
#ifeqend   // extern
