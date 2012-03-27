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
    //!  Physical address info
    struct RISE_EXPORT SAddrInfo
    {
      CString sContext;       //!<  context (library or executable file name)
      CString sAddrHex;       //!<  hex decimal address
      CString sFileName;      //!<  source file name
      CString sFunctionName;  //!<  mangled function name
      unsigned int unLine;    //!<  source code line number

      //!         constructor
      SAddrInfo();

      //!         reset info
      void Reset();
    };
  }
}

#endif // _ADDRINFO_H_
