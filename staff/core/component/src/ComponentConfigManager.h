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

#ifndef _CCOMPONENTCONFIGMANAGER_H_
#define _CCOMPONENTCONFIGMANAGER_H_

#include "staffcomponentexport.h"
#include <rise/string/String.h>

namespace staff
{
  class CComponentConfig;

  //! хранитель конфигураций компонентов
  class STAFF_COMPONENT_EXPORT CComponentConfigManager
  {
  public:
    //!         получить экземпляр
    /*! \return экземпляр
    */
    static CComponentConfigManager& Inst();

    //!         получить(и загрузить) конфигурацию компонента
    /*! \param  sComponent - имя компонента
        \param  sConfig - имя конфигурационного файла
        \param  bCreate - создать новую конфиграцию при ошибке загрузки
        \return конфигурация компонента
        */
    CComponentConfig& GetComponentConfig(const rise::CString& sComponent, const rise::CString& sConfig = "config.xml", 
      bool bCreate = false);
  
  protected:
    CComponentConfigManager();
    
    ~CComponentConfigManager();

  private:
    static CComponentConfigManager* m_pInst;
    class CComponentConfigManagerImpl;
    CComponentConfigManagerImpl* m_pImpl;
  };
}

#endif // _CCOMPONENTCONFIGMANAGER_H_
