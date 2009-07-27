/*
 *  Copyright 2009 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

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
