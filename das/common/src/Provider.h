/*
 *  Copyright 2010 Utkin Dmitry
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

#ifndef _PROVIDER_H_
#define _PROVIDER_H_

#include <string>
#include <list>
#include <staff/codegen/Interface.h>
#include "staffdascommonexport.h"

namespace rise
{
  template <typename Type> class CMutablePtr;
  namespace xml
  {
    class CXMLNode;
  }
}

namespace staff
{
  class CDataObject;

  namespace das
  {
    class DataSource;
    struct Type;

    typedef std::list<std::string> StringList; //!< string list

    //! DAS provider wrapper
    class STAFF_DAS_COMMON_EXPORT IProvider
    {
    public:
      virtual ~IProvider() {}

      //! initialize
      /*! \param rDataSource - data source
        */
      virtual void Init(const DataSource& rDataSource) = 0;

      //! deinitialize
      virtual void Deinit() = 0;

      //! get provider name
      /*! \return provider name
        */
      virtual const std::string& GetName() const = 0;

      //! get provider description
      /*! \return provider description
        */
      virtual const std::string& GetDescr() const = 0;

      //! execute query
      /*! \param  sExecute - execute string(query)
          \param  rstReturnType - resulting type
          \param  rdoResult - query result
          */
      virtual void Invoke(const std::string& sExecute, const Type& rstReturnType, staff::CDataObject& rdoResult) = 0;
    };

    //! pointer to provider wrapper
    typedef rise::CMutablePtr<IProvider> PProvider;

    //! provider allocator
    class ProviderAllocator
    {
    public:
      //! virtual destructor
      virtual ~ProviderAllocator() {}

      //! create new provider instance
      /*! \return new provider instance
        */
      virtual PProvider Allocate(const std::string& sName) = 0;

      //! get providers list
      /*! \param rlsProvidersList - resulting providers list
        */
      virtual void GetProvidersList(StringList& rlsProvidersList) = 0;
    };

  }
}

#endif // _PROVIDER_H_
