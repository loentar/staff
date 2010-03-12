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

#ifdef RISE_TEST
//!        rise root namespace
namespace rise
{
  //!  test stl-library linking(win32)
  /*! \param  sResult - output string
  */
  void RISE_EXPORT RiseLinkRuntimeTest(std::string& sResult);
}
#endif

#endif // _osdllexport_h_
