#ifndef _DATAENCODER_H_
#define _DATAENCODER_H_

#include <rise/common/containertypes.h>
#include <rise/os/osdllexport.h>
#include <rise/string/String.h>

namespace rise
{
  //! кодировщик/декодировщик base64
  class RISE_EXPORT CBase64Encoder
  {
  public:
    //!         кодировать массив байт в строку base64
    /*
        \param  baIn - массив байт для кодирования
        \param  asOut - результирующая строка
        \param  nRowWidth - количество символов в результирующей строке
      */
    static void Encode(const CByteArray& baIn, CStringA& asOut, int nRowWidth = 19);

    //!         декодировать строку base64 в массив байт
    /*
        \param  asIn - строка в base64
        \param  asOut - результирующий массив байт
      */
    static void Decode(const CStringA& asIn, CByteArray& baOut);


    //!         кодировать буфер в строку base64
    /*
        \param  baIn - буфер для кодирования
        \param  asOut - результирующая строка
        \param  nRowWidth - количество символов в результирующей строке
      */
    static void Encode(PCBuffer pBuffIn, TSize tBufferSize, CStringA& asOut, int nRowWidth = 19); // throw

    //!         декодировать строку base64 в массив байт
    /*
        \param  asIn - строка в base64
        \param  asOut - результирующий буфер
      */
    static void Decode(const CStringA& asIn, PBuffer pBuffOut, TSize& tBufferSize); // throw
  
  private:
    static const CStringA m_sEncodeTable;             //! таблица кодировки
    static const byte m_baDecodeTable['z' - '+' + 1]; //! таблица декодировки
  };
}

#endif // _DATAENCODER_H_
