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

#ifndef _DYNAMICLIBRARY_H_
#define _DYNAMICLIBRARY_H_

#include <string>
#include <rise/os/osdllexport.h>
#include <rise/os/osdl.h>

namespace rise
{
  namespace plugin
  {
    //!        класс работы с динамическими библиотеками
    class RISE_EXPORT CDynamicLibrary
    {
    public:
      CDynamicLibrary();

      virtual ~CDynamicLibrary();
      
      //!        подгрузка динамической библиотеки
      virtual void Load(const CString& sLibName, bool bRawName = false);

      //!        получение имени
      /*
         \return имя библиотеки
      */
      const CString& GetName() const;

      //!        получение указателя на символ в библиотеке
      /*
           CLogicNoItemException - указанный символ не найден
         \param  sSymName - имя символа
         \return указатель на функцию, NULL при ошибке
      */
      PLibSymbol GetSymbol(const CString& sSymName) const;

      //!        выгрузка библиотеки
      /*         CFileCloseException
      */
      void UnloadLibrary();

    private:
      HDynamicLib  m_hDynLib; //! дескриптор модуля
      CString      m_sName;   //! имя модуля
    };
  } //  namespace plugin
} // namespace rise

#endif // _DYNAMICLIBRARY_H_
