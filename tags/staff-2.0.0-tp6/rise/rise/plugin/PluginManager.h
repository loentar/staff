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

#ifndef _PluginManager_h_
#define _PluginManager_h_

#include <map>
#include <rise/os/osdllexport.h>
#include <rise/string/String.h>
#include <rise/common/MutablePtr.h>
#include "DynamicLibrary.h"

namespace rise
{
  //!        plugin and dynamic library classes
  namespace plugin
  {
    //!        Plugin manager
    template<typename TPluginBaseClass>
    class CPluginManager
    {
    public:
      //! plugin map
      typedef std::map<CString, TPluginBaseClass*> TPluginMap;
      //! plugin iterator
      typedef typename TPluginMap::iterator TPluginIterator;
      //! const plugin iterator
      typedef typename TPluginMap::const_iterator TPluginConstIterator;

    public:
      //!         load plugin
      /*! \param  sPluginName - plugin name
          \param  bRawName - use raw(platform specific) library name
          \return pointer to plugin main class
      */
      TPluginBaseClass* LoadPlugin(const CString& sPluginName, bool bRawName = false);

      //!         get pointer to plugin main class
      /*! \param  sPluginName - plugin name
          \return pointer to plugin main class
      */
      TPluginBaseClass* GetPlugin(const CString& sPluginName);

      //!         unload plugin
      /*! \param  sPluginName - plugin name
      */
      void UnLoadPlugin(const CString& sPluginName);

      //!         unload all plugins with given this type
      void UnloadAll();

      //!         get loaded plugin map
      /*! \return loaded plugin map
      */
      const TPluginMap& GetPluginMap() const;
      
      //!         get iterator to first plugin
      /*! \return iterator to first plugin
      */
      TPluginIterator PluginBegin();

      //!         get iterator to next by last plugin
      /*! \return iterator to next by last plugin
      */
      TPluginIterator PluginEnd();

      //!         get const iterator to first plugin
      /*! \return const iterator to first plugin
      */
      TPluginConstIterator PluginBegin() const;

      //!         get const iterator to next by last plugin
      /*! \return const iterator to next by last plugin
      */
      TPluginConstIterator PluginEnd() const;

    private:
      //! counted pointer to dynamic lib
      typedef rise::CMutablePtr<CDynamicLibrary> PDynamicLibrary;

      //! dynamic library map type
      typedef std::map<CString, PDynamicLibrary> TDynLibMap;

    private:
      TPluginMap m_mPlugins;  //!< plugin map
      TDynLibMap m_mDynLibs;  //!< dynamic library map
    };
  } // namespace plugin
} // namespace rise

#include "PluginManager.hpp"

#endif // _PluginManager_h_
