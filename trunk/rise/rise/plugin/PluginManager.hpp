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

#include <rise/string/String.h>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/plugin/PluginExport.h>
#include "PluginManager.h"

namespace rise
{
  namespace plugin
  {
    template<typename TPluginBaseClass> 
    TPluginBaseClass* CPluginManager<TPluginBaseClass>::LoadPlugin( const CString& sPluginName, bool bRawName /*= false*/ )
    {
      TPluginBaseClass* pPlugin = NULL;
      PDynamicLibrary pDynLib(new CDynamicLibrary);
      pDynLib->Load(sPluginName, bRawName);

#ifdef WIN32      
      typedef TPluginBaseClass* (*PRiseGetPluginAddress)();
      PRiseGetPluginAddress CallFun = reinterpret_cast<PRiseGetPluginAddress>(pDynLib->GetSymbol("RiseGetPluginAddress"));
      pPlugin = reinterpret_cast<TPluginBaseClass*>(CallFun());
#else
      pPlugin = reinterpret_cast<TPluginBaseClass*>(pDynLib->GetSymbol(RISE_PLUGIN_EXPORTED_SYMBOL_STR));
#endif

      RISE_ASSERTES(pPlugin != NULL, rise::CFileOpenException, "Error while getting plugin object");

      m_mPlugins[sPluginName] = pPlugin;
      m_mDynLibs[sPluginName] = pDynLib;

      return pPlugin;
    }

    template<typename TPluginBaseClass> 
    TPluginBaseClass* CPluginManager<TPluginBaseClass>::GetPlugin(const CString& sPluginName)
    {
      TPluginIterator itPlugin = m_mPlugins.find(sPluginName);
      RISE_ASSERTES(itPlugin != m_mPlugins.end(), rise::CLogicNoItemException, "Plugin \'" + sPluginName + "\' is not loaded");
      return itPlugin->second;
    }

    template<typename TPluginBaseClass> 
    void CPluginManager<TPluginBaseClass>::UnLoadPlugin( const CString& sPluginName )
    {
      TDynLibMap::iterator itPlugin = m_mDynLibs.find(sPluginName);
      RISE_ASSERTES(itPlugin != m_mDynLibs.end(), rise::CLogicNoItemException, "Plugin \'" + sPluginName + "\' is not loaded");
      m_mPlugins.erase(sPluginName);
      m_mDynLibs.erase(sPluginName);
    }

    template<typename TPluginBaseClass> 
    void CPluginManager<TPluginBaseClass>::UnloadAll()
    {
      m_mPlugins.clear();
      m_mDynLibs.clear();
    }

    template<typename TPluginBaseClass> 
    const typename CPluginManager<TPluginBaseClass>::TPluginMap& CPluginManager<TPluginBaseClass>::GetPluginMap() const
    {
      return m_mPlugins;
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginIterator CPluginManager<TPluginBaseClass>::PluginBegin()
    {
      return m_mPlugins.begin();
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginIterator CPluginManager<TPluginBaseClass>::PluginEnd()
    {
      return m_mPlugins.end();
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginConstIterator CPluginManager<TPluginBaseClass>::PluginBegin() const
    {
      return m_mPlugins.begin();
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginConstIterator CPluginManager<TPluginBaseClass>::PluginEnd() const
    {
      return m_mPlugins.end();
    }

  } // namespace plugin
} // namespace rise
