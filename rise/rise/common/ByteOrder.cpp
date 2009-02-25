#ifndef _BYTEORDER_HPP_
#define _BYTEORDER_HPP_

#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include "ByteOrder.h"

namespace rise
{
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       SwapBytes
  //    DESCRIPTION:    обратить порядок байт
  //    PARAMETRS:      tData - ссылка на данные
  //    RETURN:         перевернутый порядок байт
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  bool CByteOrder::SwapBytes(bool tData)
  {
    return tData;
  }

  char CByteOrder::SwapBytes(char tData)
  {
    return tData;
  }

  byte CByteOrder::SwapBytes(unsigned char tData)
  {
    return tData;
  }

  short CByteOrder::SwapBytes(short tData)
  {
    return SWAPBYTES16(tData);
  }

  ushort CByteOrder::SwapBytes(ushort tData)
  {
    return SWAPBYTES16(tData);
  }

  int CByteOrder::SwapBytes(int tData)
  {
    return SWAPBYTES32(tData);
  }

  uint CByteOrder::SwapBytes(uint tData)
  {
    return SWAPBYTES32(tData);
  }

  long CByteOrder::SwapBytes(long tData)
  {
    return SWAPBYTES32(tData);
  }

  ulong CByteOrder::SwapBytes(ulong tData)
  {
    return SWAPBYTES32(tData);
  }

  float CByteOrder::SwapBytes(float tData)
  {
    return SwapBytesN(tData);
  }

  double CByteOrder::SwapBytes(double tData)
  {
    return SwapBytesN(tData);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetDefaultByteOrder
  //    DESCRIPTION:    получение текущего порядка байтов 
  //    RETURN:         текущий порядок байтов 
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CByteOrder::EByteOrder CByteOrder::GetDefaultByteOrder()
  {
    return static_cast<EByteOrder>(__BYTE_ORDER);
  }

} // namespace rise

#endif // _BYTEORDER_HPP_
