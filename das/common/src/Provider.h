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
#include <map>
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
  namespace das
  {

    class IExecutor;
    typedef rise::CMutablePtr<IExecutor> PExecutor;

    //! string list
    typedef std::list<std::string> StringList;


    //! DAS provider interface
    class IProvider
    {
    public:
      //! destructor
      virtual ~IProvider() {}

      //! initialize provider
      /*! \param rConfig - provider's config
        */
      virtual void Init(const rise::xml::CXMLNode& rConfig) = 0;

      //! deinitialize provider
      virtual void Deinit() = 0;

      //! get provider name
      /*! \return provider name
        */
      virtual const std::string& GetName() const = 0;

      //! get provider description
      /*! \return provider description
        */
      virtual const std::string& GetDescr() const = 0;

      //! create new executor object to execute query
      /*! \return new executor
        */
      virtual PExecutor GetExecutor() = 0;
    };

    //! pointer to provider wrapper
    typedef rise::CMutablePtr<IProvider> PProvider;

    //! map of providers
    typedef std::map<std::string, PProvider> ProvidersMap;

    //! providers
    struct Providers
    {
      std::string sDefaultId;  //!< default provider id
      ProvidersMap mProviders; //!< map of providers
    };


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
