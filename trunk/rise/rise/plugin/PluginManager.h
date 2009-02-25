#ifndef _PluginManager_h_
#define _PluginManager_h_

#include <map>
#include <rise/os/osdllexport.h>
#include <rise/string/String.h>
#include "DynamicLibrary.h"

namespace rise
{
  namespace plugin
  {
    //!        Менеджер подключаемых модулей
    template<typename TPluginBaseClass>
    class CPluginManager
    {
    public:
      typedef std::map<CString, TPluginBaseClass*> TPluginMap;           //! ассоциативный список подключаемых модулей
      typedef typename TPluginMap::iterator TPluginIterator;             //! итератор по модулям
      typedef typename TPluginMap::const_iterator TPluginConstIterator;  //! константный итератор по модулям

    public:
      //!        Загрузка подгружаемого модуля
      /*
         \param  sPluginName - имя подгружаемого модуля
         \return указатель на обьект модуля
      */
      TPluginBaseClass* LoadPlugin(const CString& sPluginName, bool bRawName = false);

      //!        Получение указателя на обьект подгружаемого модуля
      /*
         \param  sPluginName - имя подгружаемого модуля
         \return указатель на обьект модуля
      */
      TPluginBaseClass* GetPlugin(const CString& sPluginName);

      //!        Выгрузка подгружаемого модуля
      /*
         \param  sPluginName - подгружаемого модуля
      */
      void UnLoadPlugin(const CString& sPluginName);

      //!        Выгрузка
      void UnloadAll();

      //!        получить список загруженных модулей
      /*
         \return список загруженных модулей
      */
      const TPluginMap& GetPluginMap() const;
      
      //!        получить итератор на первый подключаемый модуль
      /*
         \return итератор на первый подключаемый модуль
      */
      TPluginIterator PluginBegin();

      //!        получить итератор на следующий ща последним подключаемый модуль
      /*
         \return итератор на первый подключаемый модуль
      */
      TPluginIterator PluginEnd();

      //!        получить константный итератор на первый подключаемый модуль
      /*
         \return константный итератор на первый подключаемый модуль
      */
      TPluginConstIterator PluginBegin() const;

      //!        получить константный итератор на следующий ща последним подключаемый модуль
      /*
         \return константный итератор на первый подключаемый модуль
      */
      TPluginConstIterator PluginEnd() const;

    private:
      typedef std::map<CString, CDynamicLibrary*> TDynLibMap; //! ассоциативный список библиотек

    private:
      TPluginMap m_lsPluginMap;  //! список модулей для пользователя
      TDynLibMap m_lsDynLibMap;  //! список динамических библиотек
    };
  } // namespace plugin
} // namespace rise

#include "PluginManager.hpp"

#endif // _PluginManager_h_
