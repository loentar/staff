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

#ifndef _FILEFIND_H_
#define _FILEFIND_H_

#include <rise/os/osdllexport.h>
#include <list>
#include <rise/string/String.h>

namespace rise
{
  //! File finder
  class RISE_EXPORT CFileFind
  {
  public:
    enum EFindAttrs //!  find attributes
    {
      EFA_FILE = 1,  //!<  file
      EFA_DIR = 2    //!<  directory
    };

  public:
    //!         find file
    /*! \param  sInDir - source dir
        \param  rList - list of found files
        \param  sMask - find mask
        \param  nAttrs - file attributes
        */
    static void Find(const CString& sInDir, std::list<CString>& rList, const CString& sMask = "*", int nAttrs = EFA_FILE | EFA_DIR);
    
    //!         is file name = "." or ".."
    /*! \param  szName - filename
        \return true if file name = "." or ".."
        */
    static bool IsDots(const char* szName);
  };
}

#endif // _FILEFIND_H_

