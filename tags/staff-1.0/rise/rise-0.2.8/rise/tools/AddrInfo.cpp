#include "AddrInfo.h"

namespace rise
{
  namespace tools
  {
    SAddrInfo::SAddrInfo():
      sContext("??"), sAddrHex("0x00000000"), sFileName("??"), sFunctionName("??"), unLine(0)
    {
    }

    void SAddrInfo::Reset()
    {
      sContext = "??";
      sAddrHex = "0x00000000";
      sFileName = "??";
      sFunctionName = "??";
      unLine = 0;
    }

  }
}
