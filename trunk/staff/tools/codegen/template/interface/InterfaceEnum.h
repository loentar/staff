#ifeq($(Enum.Extern),0)
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
  // *$($ThisNodeName): $($ThisNodeValue)
#end
  enum $(Enum.Name)
  {
#foreach $(Enum.Members)
#ifneq($(Member.$Num),0)
,
#ifeqend
#ifneq($(Member.Value),)
#ifeq($(Enum.Options.*baseType),string)
    // *value: $(Member.Value)
#ifeqend
#ifeqend
    $(Member.Name)\
#ifneq($(Member.Value),)
#ifneq($(Enum.Options.*baseType),string)
 = $(Member.Value)\
#ifeqend
#ifeqend
#end

  };

#ifneq($($sNewOpeningNs),$($sOpeningNs))
#var sOpeningNs $($sNewOpeningNs)
#var sEndingNs $($sNewEndningNs)
#ifeqend   // namespace changed
#ifeqend   // extern
