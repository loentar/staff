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

#ifndef _COMPONENTCONFIG_H_
#define _COMPONENTCONFIG_H_

#include "staffcomponentexport.h"
#include <rise/string/String.h>

namespace rise
{
  namespace xml
  {
    class CXMLNode;
  }
}


namespace staff
{
  //!  конфигурация компонента
  class STAFF_COMPONENT_EXPORT CComponentConfig
  {
  public:
    //!         деструктор
    ~CComponentConfig();

    //!         получить имя компонента
    /*! \return имя компонента
    */
    const rise::CString& GetComponent() const;

    //!         получть имя конфигурационного файла
    /*! \return имя конфигурационного файла
    */
    const rise::CString& GetConfig() const;

    //!         перезагрузить конфигурацию
    /*! \param  bCreate - создать новую конфигурацию при ошибке загрузки
        \return конфигурация
        */
    rise::xml::CXMLNode& ReloadConfig(bool bCreate = false);
    
    //!         сохранить конфигурацию
    void SaveConfig();

    //!         получить конфигурацию
    /*! \return конфигурация
    */
    rise::xml::CXMLNode& Config();
    
    //!         получить конфигурацию
    /*! \return конфигурация
    */
    const rise::xml::CXMLNode& Config() const;

  protected:
    //!         защищенный конструктор
    CComponentConfig();

    //!         инициализация
    /*! внутренняя функция
        */
    void Init(const rise::CString& sComponent, const rise::CString& sConfig, bool bCreate);

  private:
    //!         запрет копирования
    CComponentConfig& operator=(const CComponentConfig&);

    //!         запрет копирования
    CComponentConfig(const CComponentConfig&);

    class CComponentConfigImpl;
    CComponentConfigImpl* m_pImpl;
    friend class CComponentConfigManager;
  };
}


#endif // _COMPONENTCONFIG_H_
