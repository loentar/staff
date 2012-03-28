/*
 *  Copyright 2012 Utkin Dmitry
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

#ifndef _STAFF_UTILS_ERROR_H_
#define _STAFF_UTILS_ERROR_H_

#include <string>
#include "staffutilsexport.h"

namespace staff
{

  //! get os error
  class STAFF_UTILS_EXPORT Error
  {
  public:
    //! get error string by error no
    /*! \param  lErrorNo - error no
        \return error string
      */
    static std::string GetErrorStr(long lErrorNo);

    //! get last error string
    /*! \return last error string
      */
    static std::string GetLastErrorStr();

    //! get last library error string
    /*! \return last library error string
      */
    static std::string GetLastLibraryErrorStr();
  };

}

#endif // _STAFF_UTILS_ERROR_H_
