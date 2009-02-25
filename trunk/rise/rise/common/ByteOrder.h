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
#include <endian.h>
#include <byteswap.h>

#define SWAPBYTES16(RISE_LOCAL_ARG) \
  bswap_16(RISE_LOCAL_ARG)
#define SWAPBYTES32(RISE_LOCAL_ARG) \
  bswap_32(RISE_LOCAL_ARG)
#endif

#ifndef __BYTE_ORDER
#error "__BYTE_ORDER IS NOT DEFINED" __LINE__ " " __FILE__
#endif

namespace rise
{
  //!        класс поддержки конвертации порядка байт
  class RISE_EXPORT CByteOrder
  {
  public:
    enum EByteOrder //! порядок байт
    {
      EBO_LITTLE_ENDIAN = __LITTLE_ENDIAN, //!< Little Endian (intel)
      EBO_BIG_ENDIAN = __BIG_ENDIAN        //!< Big Endian (MIPS)
    };

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static bool   SwapBytes(bool tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static char   SwapBytes(char tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static byte   SwapBytes(unsigned char tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static short  SwapBytes(short tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static ushort SwapBytes(ushort tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static int    SwapBytes(int tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static uint   SwapBytes(uint tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static long   SwapBytes(long tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static ulong  SwapBytes(ulong tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static float  SwapBytes(float tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    static double SwapBytes(double tData);

    //!        обратить порядок байт
    /*
       \param  tData - данные
       \return перевернутый порядок байт
    */
    template<typename TDATA>
    static TDATA SwapBytesN(TDATA tData)  // VC++ 2005 не находит символы при линковке
    {                                     // если поместить реализацию в hpp файл
      TDATA tDataRet; 
      byte uchSize = sizeof(tData);
      const byte* pSrc = reinterpret_cast<const byte*>(&tData);
      byte* pDst = reinterpret_cast<byte*>(&tDataRet) + uchSize - 1;
      while(uchSize-- != 0)
        *pDst-- = *pSrc++;
      return tDataRet;
    }

    //!        преобразование кодировки в BigEndian
    /*
       \param  tData - данные для преобразования
       \return преобразованные данные
    */
    template<typename TDATA>
    static TDATA ToBigEndian(const TDATA& tData);

    //!        преобразование кодировки в LittleEndian
    /*
       \param  tData - данные для преобразования
       \return преобразованные данные
    */
    template<typename TDATA>
    static TDATA ToLittleEndian(const TDATA& tData);

    //!        преобразование кодировки в указанную
    /*
       \param  tData - данные для преобразования
       \param  eByteOrder - кодировка
       \return преобразованные данные
    */
    template<typename TDATA>
    static TDATA ToByteOrder(const TDATA& tData, EByteOrder eByteOrder);

    //!        получение текущего порядка байтов
    /*
       \return текущий порядок байтов 
    */
    static EByteOrder GetDefaultByteOrder();
  };

} // namespace rise

#include "ByteOrder.hpp"

#endif
