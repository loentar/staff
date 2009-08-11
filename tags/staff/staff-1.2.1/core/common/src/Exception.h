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
  //! exception class
  enum EXCLASS
  {
    EXCFIRST = rise::EXCLAST, //!<  first used
    EXCDOM,                   //!<  DOM exception
    EXCREMOTE                 //!<  remote exception
  };

  //! exception code
  enum EXCODE
  {
    EXFIRST = rise::EXTEMPDECISION,  //!<  first used
    EXPARAM,                         //!<  invalid param
    EXTYPE,                          //!<  invalid type
    EXNOITEM,                        //!<  item is not found
    EXFORMAT,                        //!<  invalid format
    EXINTERNAL                       //!<  internal
  };

  //! basic exception for DOM exceptions
  class CDomException: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCDOM))>
  {
  };


  //! template DOM exception 
  template<EXCODE CODE>
  class CDomExceptionTemplate: public CDomException
  {
  public:
    //!         get exception code
    /*! \return exception code
    */
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };


  //! basic exception for remote exceptions
  class CRemoteExceptionBase: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCREMOTE))>
  {
  };


  //! template remote exception
  template<EXCODE CODE>
  class CRemoteExceptionTemplate: public CRemoteExceptionBase
  {
  public:
    //!         get exception code
    /*! \return exception code
    */
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };

  //! invalid param
  typedef CDomExceptionTemplate<EXPARAM> CDomParamException;
  //! invalid type
  typedef CDomExceptionTemplate<EXTYPE> CDomTypeException;
  //! item is not found
  typedef CDomExceptionTemplate<EXNOITEM> CDomNoItemException;
  //! invalid format
  typedef CDomExceptionTemplate<EXFORMAT> CDomFormatException;
  //! internal
  typedef CDomExceptionTemplate<EXINTERNAL> CDomInternalException;

  //! DOM exception
  typedef CRemoteExceptionTemplate<EXINTERNAL> CRemoteException;
  //! remote exception
  typedef CRemoteExceptionTemplate<EXINTERNAL> CRemoteInternalException;
}

#endif // _STAFF_EXCEPTION_H_
