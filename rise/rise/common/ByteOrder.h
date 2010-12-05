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

#ifndef _BYTEORDER_H_
#define _BYTEORDER_H_

#include <rise/common/types.h>
#include <rise/os/osdllexport.h>

#ifdef WIN32
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN  4321
#define __BYTE_ORDER __LITTLE_ENDIAN

#define SWAPBYTES16(RISE_LOCAL_ARG) \
  (((0xff00 & (RISE_LOCAL_ARG)) >> 8) | ((0x00ff & (RISE_LOCAL_ARG)) << 8))

#define SWAPBYTES32(RISE_LOCAL_ARG) \
  ((((RISE_LOCAL_ARG) & 0xff000000) >> 24) | (((RISE_LOCAL_ARG) & 0x00ff0000) >>  8) | \
  (((RISE_LOCAL_ARG) & 0x0000ff00) <<  8) | (((RISE_LOCAL_ARG) & 0x000000ff) << 24))

#else
#if defined OS_FreeBSD
#include <sys/endian.h>
#endif
#if defined OS_Linux
#include <endian.h>
#include <byteswap.h>
#endif

#if defined OS_FreeBSD
#define __LITTLE_ENDIAN _LITTLE_ENDIAN
#define __BIG_ENDIAN  _BIG_ENDIAN
#define __BYTE_ORDER _BYTE_ORDER
#define SWAPBYTES16(RISE_LOCAL_ARG) \
  __bswap16(RISE_LOCAL_ARG)
#define SWAPBYTES32(RISE_LOCAL_ARG) \
  __bswap32(RISE_LOCAL_ARG)
#else
#define SWAPBYTES16(RISE_LOCAL_ARG) \
  bswap_16(RISE_LOCAL_ARG)
#define SWAPBYTES32(RISE_LOCAL_ARG) \
  bswap_32(RISE_LOCAL_ARG)
#endif

#endif

#ifndef __BYTE_ORDER
#error "__BYTE_ORDER IS NOT DEFINED" __LINE__ " " __FILE__
#endif

namespace rise
{
  //!        byte order conversion class
  class RISE_EXPORT CByteOrder
  {
  public:
    enum EByteOrder //! byte order
    {
      EBO_LITTLE_ENDIAN = __LITTLE_ENDIAN, //!< Little Endian (intel)
      EBO_BIG_ENDIAN = __BIG_ENDIAN        //!< Big Endian (MIPS)
    };

    //!        reverse byte order
    /*!
       \param  tData - bytes
       \return reversed byte order
    */
    static bool   SwapBytes(bool tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static char   SwapBytes(char tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static byte   SwapBytes(unsigned char tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static short  SwapBytes(short tData);

    //!        reverse byte order
    /* \param  tData - bytes
       \return reversed byte order
    */
    static ushort SwapBytes(ushort tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static int    SwapBytes(int tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static uint   SwapBytes(uint tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static long   SwapBytes(long tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static ulong  SwapBytes(ulong tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static float  SwapBytes(float tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    static double SwapBytes(double tData);

    //!        reverse byte order
    /*! \param  tData - bytes
        \return reversed byte order
    */
    template<typename TDATA>
    static TDATA SwapBytesN(TDATA tData)  // VC++ 2005 can't find this symbol,
    {                                     // if move implementation into hpp file
      TDATA tDataRet; 
      byte uchSize = sizeof(tData);
      const byte* pSrc = reinterpret_cast<const byte*>(&tData);
      byte* pDst = reinterpret_cast<byte*>(&tDataRet) + uchSize - 1;
      while(uchSize-- != 0)
        *pDst-- = *pSrc++;
      return tDataRet;
    }

    //!        convert data to BigEndian
    /*! \param  tData - source bytes
        \return converted bytes
    */
    template<typename TDATA>
    static TDATA ToBigEndian(const TDATA& tData);

    //!        convert data to LittleEndian
    /*! \param  tData - source bytes
        \return converted bytes
    */
    template<typename TDATA>
    static TDATA ToLittleEndian(const TDATA& tData);

    //!        convert data to LittleEndian
    /*! \param  tData - source bytes
        \param  eByteOrder - bytes order
	      \return converted bytes
    */
    template<typename TDATA>
    static TDATA ToByteOrder(const TDATA& tData, EByteOrder eByteOrder);

    //!        get current byte order
    /*! \return current byte order
    */
    static EByteOrder GetDefaultByteOrder();
  };

} // namespace rise

#include "ByteOrder.hpp"

#endif
