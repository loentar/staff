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

#ifndef _STAFF_COMMON_EXCEPTION_H_
#define _STAFF_COMMON_EXCEPTION_H_

#include <staff/utils/Exception.h>
#include "staffcommonexport.h"

//! assert expression and generate DOM exception
#define STAFF_ASSERT_DOM(EXPRESSION, DESCRIPTION) \
  if (!(EXPRESSION)) STAFF_THROW(::staff::DomException, DESCRIPTION)

//! assert expression and generate remote exception
#define STAFF_ASSERT_REMOTE(EXPRESSION, DESCRIPTION) \
  if (!(EXPRESSION)) STAFF_THROW(::staff::RemoteException, DESCRIPTION)


namespace staff
{

  //! remote exception class
  class RemoteException: public Exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
      */
    inline RemoteException(const char* szFileLine, const char* szFunction, const std::string& sDescr):
      Exception(szFileLine, szFunction, sDescr)
    {
    }
  };

  class DomException: public Exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
      */
    inline DomException(const char* szFileLine, const char* szFunction, const std::string& sDescr):
      Exception(szFileLine, szFunction, sDescr)
    {
    }
  };

}

#endif // _STAFF_COMMON_EXCEPTION_H_
