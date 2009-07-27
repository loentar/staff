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

#ifndef _EXCODES_H_
#define _EXCODES_H_

namespace rise
{
  enum EXCLASS  //! классы исключений
  {
    EXCFILE,     //!< файловые исключения
    EXCLOGIC,    //!< логические исключения
    EXCINTERNAL, //!< внутренние исключения
    EXCXML,      //!< XML исключения
    EXCLAST
  };

  enum EXCODE  //! коды исключений
  {
    EXCGENERIC,        //!< стандартное

    // файловые исключения
    EXCREATE,          //!< создание
    EXDELETE,          //!< удаление
    EXOPEN,            //!< открытие
    EXCLOSE,           //!< закрытие
    EXREAD,            //!< чтение
    EXWRITE,           //!< запись
    EXIO,              //!< установка режима ввода/вывода

    // логические исключения
    EXNOINIT = 32,     //!< не произведена инициализация
    EXALREADYEXISTS,   //!< попытка повторной инициализации/создания
    EXCONTINUE,        //!< продолжить операцию
    EXSIZE,            //!< неверный размер
    EXNOITEM,          //!< заданный элемент не найден
    EXTIMEOUT,         //!< истекло время ожидания
    EXNOPERMISSION,    //!< нет прав
    EXFORMAT,          //!< неверный формат
    EXPARAM,           //!< неверный параметр

    // внутренние исключения
    EXINTERNAL = 64,   //!< внутренняя ошибка
    EXNOMEM,           //!< ошибка при выделении памяти
    EXASSERT,          //!< проверка утверждения
    EXNOTIMPLEMENTED,  //!< не реализовано
    EXTEMPDECISION     //!< временное решение
  };

} // namespace rise

#endif // #ifndef _EXCODES_H_
