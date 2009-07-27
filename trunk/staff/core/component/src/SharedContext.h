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

#ifndef _SHAREDCONTEXT_H_
#define _SHAREDCONTEXT_H_

#include "staffcomponentexport.h"
#include <rise/string/String.h>
#include <rise/common/MutablePtr.h>
#include <map>
#include "CompositeComponent.h"

namespace staff
{
  class CService;
  class CComponent;
  typedef rise::CMutablePtr<CCompositeComponent> PCompositeComponent; //!< Указатель на на обьект составного компонента
  typedef std::map<rise::CString, PCompositeComponent> TCompositeComponentMap;  //!< Ассоциативный список обьектов составных компонентов

  //!  общий контекст
  class STAFF_COMPONENT_EXPORT CSharedContext
  {
  public:
    //!         получить общий контекст
    /*! \return общий контекст
    */
    static CSharedContext& Inst();
    
    //!         добавить частный компонент
    /*! \param  pComponent - указатель на компонент
        */
    void AddComponent(CComponent* pComponent);

    //!         получить составной компонент по имени
    /*! \param  sName - имя компонента
        \return указатель на составной компонет, NULL если компонент не найден
        */
    const CCompositeComponent* GetComponent(const rise::CString& sName) const;
    
    //!         получить составной компонент по имени
    /*! \param  sName - имя компонента
        \return указатель на составной компонет, NULL если компонент не найден
        */
    CCompositeComponent* GetComponent(const rise::CString& sName);
    
    //!         получить ассоциативный массив составных компонентов
    /*! \return ассоциативный массив составных компонентов
    */
    const TCompositeComponentMap& GetComponents() const;

    //!         получить сервис по имени
    /*! \param  sName - полное имя сервиса(включающее имя компонента)
        \return указатель на сервис, NULL если сервис не найден
        */
    const CService* GetService(const rise::CString& sName) const;

    //!         получить сервис по имени
    /*! \param  sName - полное имя сервиса(включающее имя компонента)
        \return указатель на сервис, NULL если сервис не найден
        */
    CService* GetService(const rise::CString& sName);

    //!         получить полный список сервисов
    /*! \return список сервисов
        */
    TServiceMap GetServices() const;

    //!         очистить список компонентов
    void Clear();

  protected:
    //!         защищенный конструктор
    CSharedContext();
    
    //!         защищенный деструктор
    virtual ~CSharedContext();

  private:
    TCompositeComponentMap m_mComponents;
    static CSharedContext* m_pInst;
  };
}

#endif // _SHAREDCONTEXT_H_
