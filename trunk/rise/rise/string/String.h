#ifndef _STRING_H_
#define _STRING_H_

#include <rise/os/osdllexport.h>
#include <rise/os/oscommon.h>
#include <string>
#include <rise/common/types.h>

namespace rise
{
  typedef std::string CString; //!< Класс "строка а текущей кодировке"

  typedef std::string CStringA; //!< Класс "строка в однобайтной кодировке"
#ifndef WIN32
  typedef std::basic_string<wchar_t> CStringW; //!< Класс "строка в двубайтной кодировке"
#else
  typedef std::wstring CStringW; //!< Класс "строка в двубайтной кодировке"
#endif

  //!         произвести замену в строке
  /*! \param  sWhere - строка где происходит замена
      \param  sWhat - что заменить 
      \param  sWith - чем заменить
      \param  bAll - true - заменить все вхождения, false - только первое найденное
      \return количество произведеных замен
      */
  RISE_EXPORT int StrReplace(CString& sWhere, const CString& sWhat, const CString& sWith, bool bAll = false);

  //!         удалить ведущие и ведомые пробельные символы в строке
  /*! \param  rStrIn - строка
      \param  tszCharset - пробельные символы
      */
  RISE_EXPORT void StrTrim(CString& rStrIn, const TChar* tszCharset = " \n\r\t");

  //!         удалить ведущие пробельные символы в строке
  /*! \param  rStrIn - строка
      \param  tszCharset - пробельные символы
      */
  RISE_EXPORT void StrTrimLeft(CString& rStrIn, const TChar* tszCharset = " \n\r\t");

  //!         удалить ведомые пробельные символы в строке
  /*! \param  rStrIn - строка
      \param  tszCharset - пробельные символы
      */
  RISE_EXPORT void StrTrimRight(CString& rStrIn, const TChar* tszCharset = " \n\r\t");

  //!         выделить первых nCount символов из строки
  /*! \param  nCount - количество выделяемых символов
      \return результирующая подстрока
      */
  RISE_EXPORT CString StrLeft(const CString& rStrIn, CString::size_type nCount);

  //!         выделить первых nCount символов из строки
  /*! \param  rtStrOut результирующая подстрока
      \param  nCount - количество выделяемых символов
      */
  RISE_EXPORT void StrLeft(const CString& rStrIn, CString& rtStrOut, CString::size_type nCount);

  //!         выделить последних nCount символов из строки
  /*! \param  nCount - количество выделяемых символов
      \return результирующая подстрока
      */
  RISE_EXPORT CString StrRight(const CString& rStrIn, CString::size_type nCount);

  //!         выделить последних nCount символов из строки
  /*! \param  rtStrOut результирующая подстрока
      \param  nCount - количество выделяемых символов
      */
  RISE_EXPORT void StrRight(const CString& rStrIn, CString& rtStrOut, CString::size_type nCount);

  //!         выделить подстроку
  /*! \param  rStrIn - входная строка
      \param  nOffset - смещение относительно начала строки
      \param  nCount - количество символов
      \return результирующая подстрока
      */
  RISE_EXPORT CString StrMid(const CString& rStrIn, CString::size_type nOffset, CString::size_type nCount = CString::npos);

  //!         выделить подстроку
  /*! \param  rStrIn - входная строка
      \param  nOffset - смещение относительно начала строки
      \param  nCount - количество символов
      \param  rtStrOut - результирующая подстрока
      */
  RISE_EXPORT void StrMid(const CString& rStrIn, CString& rtStrOut, CString::size_type nOffset, CString::size_type nCount = CString::npos);
  
  //!         преобразовать строку в перечислимый тип
  /*! \param  sData - строка
      \param  tResult - результат
      \return результат
      */
  template<class TData>
  TData& FromStr(const CString& sData, TData& tResult);

  //!         преобразовать перечислимый тип в строку
  /*! \param  sData - значение
      \return результирующая строка
      */
  template<class TData>
  CString ToStr(const TData& rtData);

  //!         преобразовать перечислимый тип в строку
  /*! \param  sData - значение
      \param  tResult - результирующая строка
      */
  template<class TData>
  void ToStr(const TData& rtData, CString& sResult);

} // namespace rise

#include "String.hpp"

#endif // _STRING_H_
