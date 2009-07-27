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

#ifndef _QNAME_H_
#define _QNAME_H_

#include "staffcommonexport.h"
#include <string>

typedef struct axutil_qname axutil_qname_t;

namespace staff
{

  //! составное имя - обертка для axutil_qname
  class STAFF_COMMON_EXPORT CQName
  {
  public:
    //!         конструктор по умолчанию
    CQName();

    //!         конструктор 
    explicit CQName(axutil_qname_t* pQName);

    //!         конструктор передачи владения
    CQName(CQName& rstQName);

    //!         инициализирующий конструктор
    /*! \param  sLocalPart - локальная часть (может быть в виде local или prefix:local)
        \param  sNamespaceURI - URI пространства имен
        */
    CQName(const std::string& sLocalPart, const std::string& sNamespaceUri);

    //!         инициализирующий конструктор
    /*! \param  sLocalPart - локальная часть
        \param  sNamespaceURI - URI пространства имен
        \param  sPrefix - префикс
        */
    CQName(const std::string& sLocalPart, const std::string& sNamespaceUri, const std::string& sPrefix);

    //!         деструктор
    ~CQName();

    //!         оператор передачи владения
    /*! \param  rQName - исходный 
        \return ссылка на текущее составное имя
        */
    CQName& operator=( CQName& rQName );

    //!         оператор передачи владения
    /*! \param  pQName - составное имя AxiOM
        \return ссылка на текущее составное имя
        */
    CQName& operator=( axutil_qname_t* pQName );

    //!         оператор проверки на эквивалентность
    /*! 
        на эквивалентность проверяются только локальная часть и URI
        \param  rstQName - составное имя
        \return true - текущее составное имя эквивалентно заданному
        */
    bool operator==(const CQName& rstQName) const;

    //!         оператор проверки на не эквивалентность
    /*! \param  rstQName - составное имя
        \return true - текущее составное имя не эквивалентно заданному
        */
    bool operator!=(const CQName& rstQName) const;

    //!         получить локальную часть
    /*! \return локальная часть
    */
    std::string GetLocalPart() const;
    
    //!         получить URI пространства имен
    /*! \return URI пространства имен
    */
    std::string GetNamespaceUri() const;
    
    //!         получить префикс
    /*! \return префикс
    */
    std::string GetPrefix() const;

    // create/delete
    
    //!         создание составного имени
    /*! устанавливается флаг владения
        \param  sLocalPart - локальная часть
        \param  sNamespaceUri - URI пространства имен 
        \param  sPrefix - префикс
        \return ссылка на текущий обьект составного имени
        */
    CQName& Create(const std::string& sLocalPart, const std::string& sNamespaceUri, const std::string& sPrefix = "");

    //!         освободить составное имя(игнорируя владение)
    void Free();

    //!         присоединиться к обьекту axutils_qname
    /*! \param  pQName - указатель на обьект axutils_qname
        \return ссылка на текущий обьект составного имени
        */
    CQName& Attach(axutil_qname_t* pQName);

    //!         отсоединиться от обьекта axutils_qname
    /*! если установлен флаг владения, объект axutils_qname будет освобожден */
    void Detach();

    //!         создать копию обьекта из указанного составного имени
    /*! \param  rQName - ссылка на исходный объект
        \return ссылка на текущий обьект составного имени
        */
    CQName& Clone(const CQName& rQName);
    
    //!         создать копию обьекта составного имени
    /*! \return копия обьекта составного имени
    */
    CQName Clone();

    //!         оператор преобразования в строку
    /*! \return prefix:localPart
    */
    operator std::string() const;
  
  private:
    bool m_bOwner;
    axutil_qname_t* m_pAxutilQName;
  };

} // namespace staff

#endif // _QNAME_H_
