#include <iostream>
#include "String.h"

namespace rise
{

  RISE_EXPORT int StrReplace(CString& sWhere, const CString& sWhat, const CString& sWith, bool bAll/* = false*/)
  {
    int nCount = 0;
    CString::size_type stPos = 0;
    while ((stPos = sWhere.find(sWhat, stPos)) != CString::npos)
    {
      sWhere.replace(stPos, sWhat.size(), sWith);
      ++nCount;
      stPos += sWith.size();
      if (!bAll)
        break;
    } 

    return nCount;
  }

  RISE_EXPORT void StrTrim(CString& rStrIn, const TChar* tszCharset /*= " \n\r\t"*/)
  {
    StrTrimLeft(rStrIn, tszCharset);
    StrTrimRight(rStrIn, tszCharset);
  }

  RISE_EXPORT void StrTrimLeft(CString& rStrIn, const TChar* tszCharset /*= " \n\r\t"*/)
  {
    if (tszCharset == NULL)
      return;

    CString::size_type stPos = rStrIn.find_first_not_of(tszCharset);
    if ( stPos != 0 )
      rStrIn.erase(0, stPos);
  }

  RISE_EXPORT void StrTrimRight(CString& rStrIn, const TChar* tszCharset /*= " \n\r\t"*/)
  {
    if (tszCharset == NULL)
      return;

    CString::size_type stPos = rStrIn.find_last_not_of(tszCharset);
    if ( stPos != CString::npos )
      rStrIn.erase(stPos + 1);
  }

  RISE_EXPORT CString StrLeft(const CString& rStrIn, CString::size_type nCount)
  {
    return CString(rStrIn, 0, nCount);
  }

  RISE_EXPORT void StrLeft(const CString& rStrIn, CString& rtStrOut, CString::size_type nCount)
  {
    rtStrOut.assign(rStrIn, 0, nCount);
  }

  RISE_EXPORT CString StrRight(const CString& rStrIn, CString::size_type nCount)
  {
    return CString(rStrIn, rStrIn.size() - nCount, nCount);
  }

  RISE_EXPORT void StrRight(const CString& rStrIn, CString& rtStrOut, CString::size_type nCount)
  {
    rtStrOut.assign(rStrIn, rStrIn.size() - nCount, nCount);
  }

  RISE_EXPORT CString StrMid(const CString& rStrIn, CString::size_type nOffset, CString::size_type nCount/* = CString::npos*/)
  {
    return CString(rStrIn, nOffset, nCount);
  }

  RISE_EXPORT void StrMid(const CString& rStrIn, CString& rtStrOut, CString::size_type nOffset, CString::size_type nCount/* = CString::npos*/)
  {
    rtStrOut.assign(rStrIn, nOffset, nCount);
  }

} // namespace rise
