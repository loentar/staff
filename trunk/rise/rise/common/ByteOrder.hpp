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

namespace rise
{
  template<typename TDATA>
  TDATA CByteOrder::ToBigEndian( const TDATA& tData )
  {
    return 
#if (__BYTE_ORDER == __BIG_ENDIAN) // MIPS
      tData;
#elif (__BYTE_ORDER == __LITTLE_ENDIAN) // Intel
      SwapBytes(tData);
#else
#error "UNKNOWN __BYTE_ORDER" __LINE__ " " __FILE__
#endif
  }

  template<typename TDATA>
  TDATA CByteOrder::ToLittleEndian( const TDATA& tData )
  {
    return 
#if (__BYTE_ORDER == __BIG_ENDIAN) // MIPS
      SwapBytes(tData);
#elif (__BYTE_ORDER == __LITTLE_ENDIAN) // Intel
      tData;
#else
#error "UNKNOWN __BYTE_ORDER" __LINE__ " " __FILE__
#endif
  }

  template<typename TDATA>
  TDATA CByteOrder::ToByteOrder( const TDATA& tData, CByteOrder::EByteOrder eByteOrder )
  {
    switch(eByteOrder)
    {
    case EBO_BIG_ENDIAN:
      return ToBigEndian(tData);
    case EBO_LITTLE_ENDIAN:
      return ToLittleEndian(tData);
    default:
      RISE_THROWS(CLogicFormatException, "Unknown byte order: " + rise::ToStr(eByteOrder));
    }
  }

} // namespace rise

#endif // _BYTEORDER_HPP_
