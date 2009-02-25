#ifndef _STREAMTYPES_H_
#define _STREAMTYPES_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include "types.h"

namespace rise 
{
#ifndef WIN32
  //! входной строковый поток
  typedef std::istringstream CIStringStream;

  //! выходной строковый поток
  typedef std::ostringstream COStringStream;
#else
  //! входной строковый поток
  typedef std::basic_ostringstream<TChar, std::char_traits<TChar>, std::allocator<TChar> > COStringStream;

  //! выходной строковый поток
  typedef std::basic_istringstream<TChar, std::char_traits<TChar>, std::allocator<TChar> > CIStringStream;
#endif

#ifndef WIN32
  //! выходной поток
  typedef std::ostream COStream;

  //! входной поток
  typedef std::istream CIStream;
#else
  //! выходной поток
  typedef std::basic_ostream<TChar, std::char_traits<TChar> > COStream;

  //! входной поток
  typedef std::basic_istream<TChar, std::char_traits<TChar> > CIStream;
#endif

#ifndef WIN32
  //! выходной файловый поток
  typedef std::ofstream COFStream;

  //! входной файловый поток
  typedef std::ifstream CIFStream;
#else
  //! входной файловый поток
  typedef std::basic_ifstream<TChar, std::char_traits<TChar> > CIFStream;

  //! выходной файловый поток
  typedef std::basic_ofstream<TChar, std::char_traits<TChar> > COFStream;
#endif

// #ifdef UNICODE
// #define tStdOut  std::wcout
// #define tStdOutW std::wcout
// #define tStdOutA std::cout
// #else
#define tStdOut  std::cout
#define tStdOutA std::cout
#define tStdOutW std::wcout
// #endif
}

#endif // _STREAMTYPES_H_
