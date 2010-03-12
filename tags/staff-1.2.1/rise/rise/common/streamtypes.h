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

#ifndef _STREAMTYPES_H_
#define _STREAMTYPES_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include "types.h"

namespace rise 
{
#ifndef WIN32
  //! input string stream
  typedef std::istringstream CIStringStream;

  //! output string stream
  typedef std::ostringstream COStringStream;
#else
  //! input string stream
  typedef std::basic_ostringstream<TChar, std::char_traits<TChar>, std::allocator<TChar> > COStringStream;

  //! output string stream
  typedef std::basic_istringstream<TChar, std::char_traits<TChar>, std::allocator<TChar> > CIStringStream;
#endif

#ifndef WIN32
  //! output stream
  typedef std::ostream COStream;

  //! input stream
  typedef std::istream CIStream;
#else
  //! output stream
  typedef std::basic_ostream<TChar, std::char_traits<TChar> > COStream;

  //! input stream
  typedef std::basic_istream<TChar, std::char_traits<TChar> > CIStream;
#endif

#ifndef WIN32
  //! output file stream
  typedef std::ofstream COFStream;

  //! input file stream
  typedef std::ifstream CIFStream;
#else
  //! input file stream
  typedef std::basic_ifstream<TChar, std::char_traits<TChar> > CIFStream;

  //! output file stream
  typedef std::basic_ofstream<TChar, std::char_traits<TChar> > COFStream;
#endif

// #ifdef UNICODE
// #define tStdOut  std::wcout
// #else
#define tStdOut  std::cout
// #endif
#define tStdOutA std::cout
#define tStdOutW std::wcout
}

#endif // _STREAMTYPES_H_
