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


#ifndef _STAFF_UTILS_STRINGUTILS_H_
#define _STAFF_UTILS_STRINGUTILS_H_

#include <string>

namespace staff
{

  //! perform replace in string
  /*! \param  sWhere - string where search and replace should be performed
      \param  sWhat - what data replace
      \param  sWith - replace with data
      \param  bAll - true - replace all, false - replace only first
      \return number of replaces made
      */
  inline unsigned StringReplace(std::string& sWhere, const std::string& sWhat,
                                const std::string& sWith, bool bAll = false)
  {
    unsigned nCount = 0;
    std::string::size_type nPos = 0;
    while ((nPos = sWhere.find(sWhat, nPos)) != std::string::npos)
    {
      sWhere.replace(nPos, sWhat.size(), sWith);
      ++nCount;
      if (!bAll)
      {
        break;
      }
      nPos += sWith.size();
    }

    return nCount;
  }

  //! remove leading whitespaces
  /*! \param  sString - string
      \param  szCharset - whitespace symbols
      */
  inline void StringTrimLeft(std::string& sString, const char* szCharset = " \n\r\t")
  {
    if (szCharset)
    {
      std::string::size_type stPos = sString.find_first_not_of(szCharset);
      if (stPos)
      {
        sString.erase(0, stPos);
      }
    }
  }

  //! remove trailing whitespaces
  /*! \param  sString - string
      \param  szCharset - space symbols
      */
  inline void StringTrimRight(std::string& sString, const char* szCharset = " \n\r\t")
  {
    if (szCharset)
    {
      std::string::size_type stPos = sString.find_last_not_of(szCharset);
      if (stPos != std::string::npos)
      {
        sString.erase(stPos + 1);
      }
    }
  }

  //! remove leading and trailing whitespaces
  /*! \param  sString - string
      \param  szCharset - space symbols
      */
  inline void StringTrim(std::string& sString, const char* szCharset = " \n\r\t")
  {
    StringTrimLeft(sString, szCharset);
    StringTrimRight(sString, szCharset);
  }

}

#endif // _STAFF_UTILS_STRINGUTILS_H_
