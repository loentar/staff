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
