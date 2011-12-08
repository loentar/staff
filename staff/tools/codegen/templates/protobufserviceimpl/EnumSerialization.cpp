
$(Enum.NsName.!deprefix/$($rootns)/)& operator<<($(Enum.NsName.!deprefix/$($rootns)/)& rLeft, const $(Enum.NsName) eRight)
{
  rLeft = static_cast< $(Enum.NsName.!deprefix/$($rootns)/) >(static_cast<int>(eRight));
  return rLeft;
}

$(Enum.NsName)& operator<<($(Enum.NsName)& rLeft, const $(Enum.NsName.!deprefix/$($rootns)/) eRight)
{
  rLeft = static_cast< $(Enum.NsName) >(static_cast<int>(eRight));
  return rLeft;
}
