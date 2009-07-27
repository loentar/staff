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

#ifndef _STAFF_EXCEPTION_H_
#define _STAFF_EXCEPTION_H_

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/excodes.h>

namespace staff
{
  enum EXCLASS
  {
    EXCFIRST = rise::EXCLAST,
    EXCDOM,
    EXCREMOTE
  };

  enum EXCODE
  {
    EXFIRST = rise::EXTEMPDECISION,
    EXPARAM,
    EXTYPE,
    EXNOITEM,
    EXFORMAT,
    EXINTERNAL
  };

  class CDomException: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCDOM))>
  {
  };


  template<EXCODE CODE>
  class CDomExceptionTemplate: public CDomException
  {
  public:
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };



  class CRemoteExceptionBase: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCREMOTE))>
  {
  };


  template<EXCODE CODE>
  class CRemoteExceptionTemplate: public CRemoteExceptionBase
  {
  public:
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };

  typedef CDomExceptionTemplate<EXPARAM> CDomParamException;
  typedef CDomExceptionTemplate<EXTYPE> CDomTypeException;
  typedef CDomExceptionTemplate<EXNOITEM> CDomNoItemException;
  typedef CDomExceptionTemplate<EXFORMAT> CDomFormatException;
  typedef CDomExceptionTemplate<EXINTERNAL> CDomInternalException;

  typedef CRemoteExceptionTemplate<EXINTERNAL> CRemoteException;
  typedef CRemoteExceptionTemplate<EXINTERNAL> CRemoteInternalException;
}

#endif // _STAFF_EXCEPTION_H_
