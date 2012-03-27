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
#include "staffcommonexport.h"

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
  class STAFF_COMMON_EXPORT DomException: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCDOM))>
  {
  public:
    virtual ~DomException() throw();
  };


  //! template DOM exception 
  template<EXCODE CODE>
  class DomExceptionTemplate: public DomException
  {
  public:
    //!         get exception code
    /*! \return exception code
    */
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };


  //! basic exception for remote exceptions
  class STAFF_COMMON_EXPORT RemoteExceptionBase: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCREMOTE))>
  {
  public:
    virtual ~RemoteExceptionBase() throw();
  };


  //! template remote exception
  template<EXCODE CODE>
  class RemoteExceptionTemplate: public RemoteExceptionBase
  {
  public:
    //!         get exception code
    /*! \return exception code
    */
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };

  //! invalid param
  typedef DomExceptionTemplate<EXPARAM> DomParamException;
  //! invalid type
  typedef DomExceptionTemplate<EXTYPE> DomTypeException;
  //! item is not found
  typedef DomExceptionTemplate<EXNOITEM> DomNoItemException;
  //! invalid format
  typedef DomExceptionTemplate<EXFORMAT> DomFormatException;
  //! internal
  typedef DomExceptionTemplate<EXINTERNAL> DomInternalException;

  //! DOM exception
  typedef RemoteExceptionTemplate<EXINTERNAL> RemoteException;
  //! remote exception
  typedef RemoteExceptionTemplate<EXINTERNAL> RemoteInternalException;

#ifndef STAFF_NO_DEPRECATED
  STAFF_DEPRECATED(DomParamException) typedef DomParamException CDomParamException;
  STAFF_DEPRECATED(DomTypeException) typedef DomTypeException CDomTypeException;
  STAFF_DEPRECATED(DomNoItemException) typedef DomNoItemException CDomNoItemException;
  STAFF_DEPRECATED(DomFormatException) typedef DomFormatException CDomFormatException;
  STAFF_DEPRECATED(DomInternalException) typedef DomInternalException CDomInternalException;
  STAFF_DEPRECATED(RemoteException) typedef RemoteException CRemoteException;
  STAFF_DEPRECATED(RemoteInternalException) typedef RemoteInternalException CRemoteInternalException;
#endif

}

#endif // _STAFF_EXCEPTION_H_
