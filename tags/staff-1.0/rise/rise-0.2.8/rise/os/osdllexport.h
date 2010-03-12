#ifndef _osdllexport_h_
#define _osdllexport_h_

#include "oscommon.h"

#ifdef WIN32

#pragma warning (disable : 4251) // needs to have dll-interface to be used by clients of class rise...

  #define RISE_DLL_EXPORT __declspec(dllexport)
  #define RISE_DLL_IMPORT __declspec(dllimport)

  #ifdef RISE_DLL_EXPORTS
    #define RISE_EXPORT RISE_DLL_EXPORT
  #else
    #define RISE_EXPORT RISE_DLL_IMPORT
  #endif

#else
  #define RISE_EXPORT 
  #define RISE_DLL_EXPORT
  #define RISE_DLL_IMPORT
#endif

#include <string>

namespace rise
{
  //!  тестирование правильности линковки символов библиотеки
  /*
    \param  sResult - строка
  */
  void RISE_EXPORT RiseLinkRuntimeTest(std::string& sResult);
}

#endif // _osdllexport_h_
