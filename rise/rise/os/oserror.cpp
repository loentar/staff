#include <string.h>
#include "oserror.h"

namespace rise
{
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osGetErrorStr
  //    DESCRIPTION:    получение строкового описания ошибки
  //    PARAMETRS:      nError - код ошибки
  //    RETURN:         строковое описание ошибки
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  RISE_EXPORT CString osGetErrorStr( int nError )
  {
#ifdef WIN32
    CString sMsg;

    sMsg.resize(1024);

    DWORD dwChars = ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
              0, nError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
              const_cast<TChar*>(sMsg.c_str()), static_cast<DWORD>(sMsg.size()), NULL );

    if (dwChars == 0)
      return "<Неизвестная ошибка>";

    sMsg.erase(dwChars);

    return sMsg;
#else
//    return CStringA(strerror(nError)).ToStr();
    return strerror(nError);
#endif
  }
}
