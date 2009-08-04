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

#ifndef _EXCODES_H_
#define _EXCODES_H_

namespace rise
{
  enum EXCLASS  //! exception classes
  {
    EXCFILE,     //!< file exceptions
    EXCLOGIC,    //!< logic exceptions
    EXCINTERNAL, //!< internal exceptions
    EXCXML,      //!< XML exceptions
    EXCLAST      //!< last class
  };

  enum EXCODE  //! exception codes
  {
    EXCGENERIC,        //!< generic

    // file exceptions
    EXCREATE,          //!< creating
    EXDELETE,          //!< deleting
    EXOPEN,            //!< opening
    EXCLOSE,           //!< closing
    EXREAD,            //!< reading
    EXWRITE,           //!< writing
    EXIO,              //!< io mode setting

    // logic exceptions
    EXNOINIT = 32,     //!< not initialized
    EXALREADYEXISTS,   //!< already initialized
    EXCONTINUE,        //!< it's need to continue
    EXSIZE,            //!< invalid size
    EXNOITEM,          //!< item not found
    EXTIMEOUT,         //!< timeout
    EXNOPERMISSION,    //!< permission
    EXFORMAT,          //!< invalid format
    EXPARAM,           //!< invalid parameter

    // internal exceptions
    EXINTERNAL = 64,   //!< internal error
    EXNOMEM,           //!< memory allocation failed
    EXASSERT,          //!< assert failed
    EXNOTIMPLEMENTED,  //!< not implemented
    EXTEMPDECISION     //!< temporary decision was reached
  };

} // namespace rise

#endif // #ifndef _EXCODES_H_
