#ifeq($(Enum.Extern),false)
\
#var sNewOpeningNs $(Enum.OpeningNs)
#var sNewEndningNs $(Enum.EndingNs)
#ifneq($($sNewOpeningNs),$($sOpeningNs))
$($sEndingNs)
$($sNewOpeningNs)
#ifeqend   // namespace changed
#ifneq($(Enum.Description),)
  //! $(Enum.Description)
#ifeqend
#ifneq($(Enum.Detail),)
  /*! $(Enum.Detail) */
#ifeqend
#foreach $(Enum.Options)
  // *$($ThisElementName): $($ThisElementValue)
#end
  enum $(Enum.Name)
  {
#var nEnumMembers $(Enum.Members.$Count.!dec)
#foreach $(Enum.Members)
#ifneq($(Member.Value),)
#ifeq($(Enum.Options.*baseType),$($sStringTypes))
    // *value: $(Member.Value)
#ifeqend
#ifeqend
    $(Member.Name)\
#ifneq($(Member.Value),)
#ifneq($(Enum.Options.*baseType),$($sStringTypes))
 = $(Member.Value)\
#ifeqend
#ifeqend
#ifneq($($nEnumMembers),$(Member.$Num))
,\
#ifeqend
#ifneq($(Member.Description),)
 //!< $(Member.Description)\
#ifeqend

#end
  };

#ifneq($($sNewOpeningNs),$($sOpeningNs))
#var sOpeningNs $($sNewOpeningNs)
#var sEndingNs $($sNewEndningNs)
#ifeqend   // namespace changed
#ifeqend   // extern
