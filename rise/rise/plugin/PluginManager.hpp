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

#include <memory>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/plugin/PluginExport.h>
#include "PluginManager.h"

namespace rise
{
  namespace plugin
  {
    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       LoadPlugin(...)
    //    DESCRIPTION:    Загрузка подгружаемого модуля
    //    PARAMETRS:      sPluginName - имя подгружаемого модуля
    //    RETURN:         идентификатор модуля
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    template<typename TPluginBaseClass> 
    TPluginBaseClass* CPluginManager<TPluginBaseClass>::LoadPlugin( const CString& sPluginName, bool bRawName /*= false*/ )
    {
      TPluginBaseClass* pPlugin = NULL;
      std::auto_ptr<CDynamicLibrary> pDynLib(new CDynamicLibrary);
      pDynLib->Load(sPluginName, bRawName);

#ifdef WIN32      
      typedef TPluginBaseClass* (*PRiseGetPluginAddress)();
      PRiseGetPluginAddress CallFun = static_cast<PRiseGetPluginAddress>(pDynLib->GetSymbol("RiseGetPluginAddress"));
      pPlugin = reinterpret_cast<TPluginBaseClass*>(CallFun());
#else
      pPlugin = reinterpret_cast<TPluginBaseClass*>(pDynLib->GetSymbol(RISE_PLUGIN_EXPORTED_SYMBOL_STR));
#endif

      RISE_ASSERTES(pPlugin != NULL, rise::CFileOpenException, "Ошибка получение адреса обьекта модуля");

      m_lsPluginMap[sPluginName] = pPlugin;
      m_lsDynLibMap[sPluginName] = pDynLib.release();

      return pPlugin;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetPlugin(...)
    //    DESCRIPTION:    Получение указателя на обьект подгружаемого модуля
    //    PARAMETRS:      sPluginName - имя подгружаемого модуля
    //    RETURN:         указатель на обьект модуля
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    template<typename TPluginBaseClass> 
    TPluginBaseClass* CPluginManager<TPluginBaseClass>::GetPlugin(const CString& sPluginName)
    {
      TPluginIterator itPlugin = m_lsPluginMap.find(sPluginName);
      RISE_ASSERTES(itPlugin != m_lsPluginMap.end(), rise::CLogicNoItemException, "Ошибка при выгрузке библиотеки...");
      return itPlugin->second;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       UnLoadPlugin(...)
    //    DESCRIPTION:    Выгрузка подгружаемого модуля
    //    PARAMETRS:      itPlugin - итератор модуля
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    template<typename TPluginBaseClass> 
    void CPluginManager<TPluginBaseClass>::UnLoadPlugin( const CString& sPluginName )
    {
      TDynLibMap::iterator itPlugin = m_lsDynLibMap.find(sPluginName);
      RISE_ASSERTES(itPlugin != m_lsDynLibMap.end(), rise::CLogicNoItemException, "Ошибка при выгрузке библиотеки...");
      itPlugin->second->UnloadLibrary();
      m_lsPluginMap.erase(sPluginName);
      m_lsDynLibMap.erase(sPluginName);
    }

    template<typename TPluginBaseClass> 
    void CPluginManager<TPluginBaseClass>::UnloadAll()
    {
      for(TDynLibMap::iterator itPlugin = m_lsDynLibMap.begin(); itPlugin != m_lsDynLibMap.end(); ++itPlugin)
        itPlugin->second->UnloadLibrary();
      m_lsPluginMap.clear();
      m_lsDynLibMap.clear();
    }

    template<typename TPluginBaseClass> 
    const typename CPluginManager<TPluginBaseClass>::TPluginMap& CPluginManager<TPluginBaseClass>::GetPluginMap() const
    {
      return m_lsPluginMap;
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginIterator CPluginManager<TPluginBaseClass>::PluginBegin()
    {
      return m_lsPluginMap.begin();
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginIterator CPluginManager<TPluginBaseClass>::PluginEnd()
    {
      return m_lsPluginMap.end();
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginConstIterator CPluginManager<TPluginBaseClass>::PluginBegin() const
    {
      return m_lsPluginMap.begin();
    }

    template<typename TPluginBaseClass> 
    typename CPluginManager<TPluginBaseClass>::TPluginConstIterator CPluginManager<TPluginBaseClass>::PluginEnd() const
    {
      return m_lsPluginMap.end();
    }

  } // namespace plugin
} // namespace rise
