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
