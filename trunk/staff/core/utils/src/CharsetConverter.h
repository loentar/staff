/*
 *  Copyright 2011 Utkin Dmitry
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


#ifndef _STAFF_UTILS_CHARSETCONVERTOR_H_
#define _STAFF_UTILS_CHARSETCONVERTOR_H_

#include <string>
#include "staffutilsexport.h"

#if !defined sun || !defined WITH_ICONV
extern "C"
{
  typedef void* iconv_t;
}
#else
#include <iconv.h>
#endif

namespace staff
{
  class STAFF_UTILS_EXPORT CharsetConverter
  {
  public:
    //! constructor
    /*! \param  szFrom - source encoding
        \param  szTo - destination encoding
      */
    CharsetConverter(const char* szFrom, const char* szTo);

    //! constructor
    /*! \param  sFrom - source encoding
        \param  sTo - destination encoding
      */
    CharsetConverter(const std::string& sFrom, const std::string& sTo);

    //! destructor
    ~CharsetConverter();

    //! convert a string from one encoding to another
    /*! \param  szSource - source buffer
        \param  nSourceSize - source string size
        \param  szResult - resulting buffer
        \param  nResultSize - resulting buffer size
        \param  pnConvertedSize - converted buffer size
        \return true, if conversion was succesful
      */
    bool Convert(const char* szSource, unsigned long nSourceSize,
                 char* szResult, unsigned long nResultSize,
                 unsigned long* pnConvertedSize = NULL);

    //!  convert from one charset encoding to another
    /*! if compiled without iconv returns source string
        \param sSource - source string
        \param sResult - resulting string
        \return true, if conversion was successful
      */
    bool Convert(const std::string& sSource, std::string& sResult);

  private:
    iconv_t m_pIconv;
  };
}

#endif // _STAFF_UTILS_CHARSETCONVERTOR_H_
