#ifndef _OSERROR_H_
#define _OSERROR_H_

#include "oscommon.h"
#include "osdllexport.h"
#include <rise/string/String.h>

namespace rise
{
  //!        получение строкового описания ошибки
  /*
     \param  nError - код ошибки
     \return строковое описание ошибки
  */
  RISE_EXPORT CString osGetErrorStr( int nError );
}

#endif // _OSERROR_H_
