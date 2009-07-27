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

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <rise/os/oscommon.h>
#include <rise/common/excodes.h>
#include <string>
#include <rise/string/String.h>

namespace rise 
{
  //!        абстрактный базовый класс исключений
  class RISE_EXPORT CException
  {
  public:
    //!        деструктор
    virtual ~CException() throw() {}
  
    //!        создание исключения
    /*
       \param  szFile - имя файла исходника
       \param  nLine - номер строки в файле исходника 
       \param  szFunction - имя функции в которой произошло исключение
       \param  eCode - код исключения
       \param  szDescr - описание исключения
       \param  szObject - объект исключения
    */
    virtual void Create(const CString& /*szFile*/, int /*nLine*/, const CString& /*szFunction*/, 
      const CString& /*szDescr*/ = "", const CString& /*szExpr*/ = "", 
      const CString& /*szObject*/ = "") throw() = 0;
  
    //!        получение файла в котором произошло исключение
    /*
       \return имя файла-исходника в котором произошло исключение
    */
    virtual const CString& GetFileName() const throw() = 0;

    //!        получение строки в файле в котором произошло исключение
    /*
       \return строка в файле в котором произошло исключение
    */
    virtual int GetLine() const throw() = 0;

    //!        получение имени функции в которой произошло исключение
    /*
       \return имя функции в которой произошло исключение
    */
    virtual const CString& GetFunction() const throw() = 0;

    //!        получение описания исключения
    /*
       \return описание исключения
    */
    virtual const CString& GetDescr() const throw() = 0;

    //!        получение выражения 
    /*
       \return выражение в результате которого возникло исключение
    */
    virtual const CString& GetExpr() const throw() = 0;

    //!        получение обьекта исключения
    /*
       \return обьекта исключения
    */
    virtual const CString& GetObjectName() const throw() = 0;
    
    //!        получение класса исключения
    /*
       \return код исключения
    */
    virtual EXCLASS GetClass() const throw() = 0;
  
    //!        получение кода исключения
    /*
       \return код исключения
    */
    virtual EXCODE GetCode() const throw() = 0;

    //!        получение полного описания исключения
    /*
       \sa SetFormat
       \param  sFormat - строка для форматирования
       \return описания исключения
    */
    virtual CString GetString(const CString& /*szFormat*/ = "") const throw() = 0;

  };  // class CException ///////////////////////////////////////////

}  // namespace rise

#endif // #ifndef _EXCEPTION_H_

