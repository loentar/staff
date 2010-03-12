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

#ifndef _TYPES_H_
#define _TYPES_H_

#include <wchar.h>

namespace rise 
{
  //! ansi char
  typedef char TCharA;    
  //! wide char
  typedef wchar_t TCharW; 

  //! char
  typedef TCharA TChar;  

  //! const ansi C-string
  typedef const char* pcstring;
  //! ansi C-string
  typedef char* pstring;
  //! const wide C-string
  typedef const wchar_t* pcwstring;
  //! wide C-string
  typedef wchar_t* pwstring;
  //! const С-string
  typedef const TChar* pctstring;
  //! С-string
  typedef TChar* ptstring;

  //! byte
  typedef unsigned char byte;
  //! word
  typedef unsigned short word;
  //! ushort
  typedef unsigned short ushort;
  //! dword
  typedef unsigned long dword;
  //! ulong
  typedef unsigned long ulong;
  //! uint
  typedef unsigned int uint;

  //! size type
  typedef ulong TSize;
  //! buffer type
  typedef byte TBuffer;
  //! pointer to buffer
  typedef TBuffer* PBuffer;
  //! const pointer to buffer
  typedef const TBuffer* PCBuffer;
}

#endif
