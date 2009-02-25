#ifndef _TYPES_H_
#define _TYPES_H_

#include <wchar.h>

namespace rise 
{
  //! однобайтный символ
  typedef char TCharA;    
  //! двубайтный символ
  typedef wchar_t TCharW; 

  //! символ в текущей кодировке
  typedef TCharA TChar;  

  //! константная однобайтная С-строка
  typedef const char* pcstring;
  //! однобайтная С-строка
  typedef char* pstring;
  //! константная двубайтная С-строка
  typedef const wchar_t* pcwstring;
  //! двубайтная С-строка
  typedef wchar_t* pwstring;
  //! С-строка в текущей кодировке
  typedef const TChar* pctstring;
  //! С-строка в текущей кодировке
  typedef TChar* ptstring;

  //! байт
  typedef unsigned char byte;
  //! слово
  typedef unsigned short word;
  //! двубайтовое беззнаковое целое
  typedef unsigned short ushort;
  //! двойное слово
  typedef unsigned long dword;
  //! беззнаковое длинное целое
  typedef unsigned long ulong;
  //! беззнаковае целое
  typedef unsigned int uint;

  //! размер
  typedef ulong TSize;
  //! тип буфера
  typedef byte TBuffer;
  //! указатель на буфер
  typedef TBuffer* PBuffer;
  //! константный указатель на буфер
  typedef const TBuffer* PCBuffer;
}

#endif
