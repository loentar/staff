#ifndef _ADDRINFO_H_
#define _ADDRINFO_H_

#include <rise/os/osdllexport.h>
#include <rise/string/String.h>

namespace rise
{
  namespace tools
  {
    //!  Информация об адресе
    struct RISE_EXPORT SAddrInfo
    {
      CString sContext;       //!<  контекст (имя исполняемого файла или библиотеки)
      CString sAddrHex;       //!<  адрес в шестнадцатеричной форме
      CString sFileName;      //!<  имя исходного файла
      CString sFunctionName;  //!<  имя функции(завернутое по правилам С)
      unsigned int unLine;    //!<  строка в исходном файле

      //!         конструктор
      SAddrInfo();

      //!         сброс информации
      void Reset();
    };
  }
}

#endif // _ADDRINFO_H_
