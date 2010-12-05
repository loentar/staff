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

#ifndef _RISE_STRING_H_
#define _RISE_STRING_H_

#include <rise/os/osdllexport.h>
#include <rise/os/oscommon.h>
#include <string>
#include <rise/common/types.h>

namespace rise
{
  typedef std::string CString; //!< string

  typedef std::string CStringA; //!< ansi string
#ifndef WIN32
  typedef std::basic_string<wchar_t> CStringW; //!< wide string
#else
  typedef std::wstring CStringW; //!< wide string
#endif

  //!         replace
  /*! \param  sWhere - string
      \param  sWhat - what data replace
      \param  sWith - replace with data
      \param  bAll - true - replace all, false - replace only first
      \return number of replaces made
      */
  RISE_EXPORT int StrReplace(CString& sWhere, const CString& sWhat, const CString& sWith, bool bAll = false);

  //!         trim string
  /*! \param  rStrIn - string
      \param  tszCharset - space symbols
      */
  RISE_EXPORT void StrTrim(CString& rStrIn, const TChar* tszCharset = " \n\r\t");

  //!         trim string left
  /*! \param  rStrIn - string
      \param  tszCharset - space symbols
      */
  RISE_EXPORT void StrTrimLeft(CString& rStrIn, const TChar* tszCharset = " \n\r\t");

  //!         trim string right
  /*! \param  rStrIn - string
      \param  tszCharset - space symbols
      */
  RISE_EXPORT void StrTrimRight(CString& rStrIn, const TChar* tszCharset = " \n\r\t");

  //!         get n first symbols
  /*! \param  rStrIn - source string
      \param  nCount - count of first symbols
      \return resulting string
      */
  RISE_EXPORT CString StrLeft(const CString& rStrIn, CString::size_type nCount);

  //!         get n first symbols
  /*! \param  rStrIn - source string
      \param  nCount - count of first symbols
      \param  rtStrOut - resulting string
      */
  RISE_EXPORT void StrLeft(const CString& rStrIn, CString& rtStrOut, CString::size_type nCount);

  //!         get last n symbols
  /*! \param  rStrIn - source string
      \param  nCount - count of symbols
      \return resulting string
      */
  RISE_EXPORT CString StrRight(const CString& rStrIn, CString::size_type nCount);

  //!         get last n symbols
  /*! \param  rStrIn - source string
      \param  nCount - count of symbols
      \param  rtStrOut - resulting string
      */
  RISE_EXPORT void StrRight(const CString& rStrIn, CString& rtStrOut, CString::size_type nCount);

  //!         get substring
  /*! \param  rStrIn - source string
      \param  nOffset - offset
      \param  nCount - count
      \return resulting string
      */
  RISE_EXPORT CString StrMid(const CString& rStrIn, CString::size_type nOffset, CString::size_type nCount = CString::npos);

  //!         get substring
  /*! \param  rStrIn - source string
      \param  rStrOut - resulting string
      \param  nOffset - offset
      \param  nCount - count
      */
  RISE_EXPORT void StrMid(const CString& rStrIn, CString& rStrOut, CString::size_type nOffset, CString::size_type nCount = CString::npos);
  
  //!         convert string to object
  /*! \param  sData - source string
      \param  tResult - resulting object
      \return resulting object
      */
  template<class TData>
  TData& FromStr(const CString& sData, TData& tResult);

  //!         convert object to string
  /*! \param  rtData - source data
      \return resulting string
      */
  template<class TData>
  CString ToStr(const TData& rtData);

  //!         convert object to string
  /*! \param  rtData - object
      \param  sResult - resulting string
      */
  template<class TData>
  void ToStr(const TData& rtData, CString& sResult);

} // namespace rise

#include "String.hpp"

#endif // _RISE_STRING_H_
