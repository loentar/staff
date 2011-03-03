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
    $(Member.Name)\
#ifneq($(Member.Value),)
 = $(Member.Value)\
#ifeqend
#end

  };

#ifneq($($sNewOpeningNs),$($sOpeningNs))
#var sOpeningNs $($sNewOpeningNs)
#var sEndingNs $($sNewEndningNs)
#ifeqend   // namespace changed
#ifeqend   // extern
