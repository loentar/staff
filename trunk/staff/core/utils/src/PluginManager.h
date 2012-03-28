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

#ifndef _STAFF_UTILS_PLUGINMANAGER_H_
#define _STAFF_UTILS_PLUGINMANAGER_H_

#include <map>
#include "SharedPtr.h"
#include "Exception.h"
#include "PluginExport.h"
#include "DynamicLibrary.h"

namespace staff
{

  //! plugin manager
  template <typename IPlugin>
  class PluginManager
  {
  public:
    //! plugin map
    typedef std::map<std::string, IPlugin*> PluginMap;

  public:
    //! load plugin
    /*! \param  sName - plugin name
        \param  bRawName - use raw(platform specific) library name
        \return pointer to plugin object
    */
    inline IPlugin* Load(const std::string& sName, bool bRawName = false)
    {
      PDynamicLibrary pDynLib(new DynamicLibrary);
      pDynLib->Load(sName, bRawName);

#ifdef WIN32
      typedef IPlugin* (*PStaffGetPluginAddress)();
      PStaffGetPluginAddress pfnGetPluginAddress = reinterpret_cast<PStaffGetPluginAddress>(
            pDynLib->GetSymbol("StaffGetPluginAddress"));
      STAFF_ASSERT(pfnGetPluginAddress, "Error while getting StaffGetPluginAddress");
      IPlugin* pPlugin = reinterpret_cast<IPlugin*>(pfnGetPluginAddress());
#else
      IPlugin* pPlugin = reinterpret_cast<IPlugin*>(pDynLib->GetSymbol(STAFF_PLUGIN_EXPORTED_SYMBOL_STR));
#endif

      STAFF_ASSERT(pPlugin, "Error while getting plugin object");

      m_mPlugins[sName] = pPlugin;
      m_mDynLibs[sName] = pDynLib;

      return pPlugin;
    }

    //! get pointer to plugin object
    /*! \param  sName - plugin name
        \return pointer to plugin object
    */
    inline IPlugin* Get(const std::string& sName)
    {
      typename PluginMap::iterator itPlugin = m_mPlugins.find(sName);
      STAFF_ASSERT(itPlugin != m_mPlugins.end(), "Plugin \'" + sName + "\' is not loaded");
      return itPlugin->second;
    }

    //! unload plugin
    /*! \param  sName - plugin name
    */
    inline void Unload(const std::string& sName)
    {
      typename DynLibMap::iterator itPlugin = m_mDynLibs.find(sName);
      STAFF_ASSERT(itPlugin != m_mDynLibs.end(), "Plugin \'" + sName + "\' is not loaded");
      m_mPlugins.erase(sName);
      m_mDynLibs.erase(sName);
    }

    //! unload all plugins
    inline void UnloadAll()
    {
      m_mPlugins.clear();
      m_mDynLibs.clear();
    }

    //! get loaded plugin map
    /*! \return loaded plugin map
    */
    inline const PluginMap& GetPlugins() const
    {
      return m_mPlugins;
    }

  private:
    //! counted pointer to dynamic lib
    typedef SharedPtr<DynamicLibrary> PDynamicLibrary;

    //! dynamic library map type
    typedef std::map<std::string, PDynamicLibrary> DynLibMap;

  private:
    PluginMap m_mPlugins;  //!< plugin map
    DynLibMap m_mDynLibs; //!< dynamic library map
  };

}

#endif // _STAFF_UTILS_PLUGINMANAGER_H_
