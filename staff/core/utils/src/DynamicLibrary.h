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

#ifndef _STAFF_UTILS_DYNAMICLIBRARY_H_
#define _STAFF_UTILS_DYNAMICLIBRARY_H_

#include <string>
#include "staffutilsexport.h"

#ifdef WIN32
#define STAFF_LIBRARY_PREFIX ""
#define STAFF_LIBRARY_EXT ".dll"
#define STAFF_LIBRARY_VEREXT STAFF_LIBRARY_EXT
#else
#ifdef __APPLE__
#define STAFF_LIBRARY_PREFIX "lib"
#define STAFF_LIBRARY_EXT ".dylib"
#define STAFF_LIBRARY_VEREXT "." LIBRARY_VERSION STAFF_LIBRARY_EXT
#else
#define STAFF_LIBRARY_PREFIX "lib"
#define STAFF_LIBRARY_EXT ".so"
#define STAFF_LIBRARY_VEREXT STAFF_LIBRARY_EXT "." LIBRARY_VERSION
#endif
#endif

namespace staff
{

  //! dynamic library
  class STAFF_UTILS_EXPORT DynamicLibrary
  {
  public:
    //! constructor
    DynamicLibrary();

    //! destructor
    ~DynamicLibrary();

    //! load dynamic library
    /*! \param  sLibName - library name
        \param  bRawName - true, given full library name with extension
        */
    void Load(const std::string& sLibName, bool bRawName = false);

    //! get library name
    /*! \return library name
    */
    const std::string& GetName() const;

    //! get pointer to library symbol
    /*! \param  sSymName - symbol name
        \return pointer to symbol
    */
    void* GetSymbol(const std::string& sSymName) const;

    //! unload library
    void UnloadLibrary();

  private:
    DynamicLibrary(const DynamicLibrary&);
    DynamicLibrary& operator=(const DynamicLibrary&);

  private:
    void*        m_pDynLib; //!< library handle
    std::string  m_sName;   //!< library name
  };

} // namespace staff

#endif // _STAFF_UTILS_DYNAMICLIBRARY_H_
