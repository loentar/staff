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

#ifndef _VALUE_H_
#define _VALUE_H_

#include "staffcommonexport.h"
#include <string>
#include "WsdlTypes.h"

namespace staff
{
  class CDataObject;

  //! класс "Значение"
  /*! предназначен для прозрачного хранения и преобразования перечислимых и строковых типов данных */
  class STAFF_COMMON_EXPORT CValue
  {
  public:
    enum EType      //!  тип значения
    {
      ET_NOTINIT,   //!<  не проинициализирован
      ET_UNKNOWN,   //!<  тип не известен
      ET_TEXT,      //!<  текст

      // числовые типы
      ET_FLOAT,     //!<  число с плавающей точкой
      ET_DOUBLE,    //!<  двойное число с плавающей точкой

      ET_BYTE,      //!<  byte
      ET_INT,       //!<  целое
      ET_SHORT,     //!<  короткое
      ET_LONG,      //!<  длинное
      ET_LONGLONG,  //!<  двойное длинное

      ET_UBYTE,     //!<  unsigned byte
      ET_UINT,      //!<  беззнаковое целое
      ET_USHORT,    //!<  беззнаковое короткое
      ET_ULONG,     //!<  беззнаковое длинное
      ET_ULONGLONG, //!<  беззнаковое двойное длинное

      ET_BOOL       //!<  логическая переменная
    };

  public:
    //!         конструктор по умолчанию
    CValue();

    //!         конструктор инициализации от объекта данных
    /*! \param  pDataObject - указатель на объект данных
        */
    CValue(CDataObject* pDataObject);
    
    //!         инициализирующий конструктор
    /*! \param  sValue - значение
        */
    CValue(const std::string& sValue);

    //!         инициализирующий конструктор
    /*! \param  szValue - значение
    */
    CValue(const char* szValue);

    //!         инициализирующий конструктор
    /*! \param  fValue - значение
    */
    CValue(float fValue);

    //!         инициализирующий конструктор
    /*! \param  dValue - значение
        */
    CValue(double dValue);

    //!         инициализирующий конструктор
    /*! \param  nValue - значение
    */
    CValue(byte btValue);

    //!         инициализирующий конструктор
    /*! \param  nValue - значение
        */
    CValue(int nValue);
    
    //!         инициализирующий конструктор
    /*! \param  shValue - значение
    */
    CValue(short shValue);

    //!         инициализирующий конструктор
    /*! \param  lValue - значение
        */
    CValue(long lValue);
    
    //!         инициализирующий конструктор
    /*! \param  llValue - значение
    */
    CValue(long long llValue);

    //!         инициализирующий конструктор
    /*! \param  unValue - значение
    */
    CValue(unsignedByte ubtValue);

    //!         инициализирующий конструктор
    /*! \param  unValue - значение
    */
    CValue(unsigned int unValue);

    //!         инициализирующий конструктор
    /*! \param  ushValue - значение
    */
    CValue(unsigned short ushValue);

    //!         инициализирующий конструктор
    /*! \param  ulValue - значение
    */
    CValue(unsigned long ulValue);

    //!         инициализирующий конструктор
    /*! \param  ullValue - значение
    */
    CValue(unsigned long long ullValue);

    //!         инициализирующий конструктор
    /*! \param  bValue - значение
        */
    CValue(bool bValue);

    ~CValue();

    //!         оператор копирования
    /*! \param  bValue - значение
        */
    CValue& operator=(const CValue& rValue);

    //!         оператор копирования
    /*! \param  sValue - значение
        */
    CValue& operator=(const std::string& sValue);

    //!         оператор копирования
    /*! \param  szValue - значение
        */
    CValue& operator=(const char* szValue);

    //!         оператор копирования
    /*! \param  fValue - значение
        */
    CValue& operator=(float fValue);

    //!         оператор копирования
    /*! \param  dValue - значение
        */
    CValue& operator=(double dValue);

    //!         оператор копирования
    /*! \param  btValue - значение
    */
    CValue& operator=(byte btValue);

    //!         оператор копирования
    /*! \param  nValue - значение
    */
    CValue& operator=(int nValue);

    //!         оператор копирования
    /*! \param  shValue - значение
    */
    CValue& operator=(short shValue);

    //!         оператор копирования
    /*! \param  lValue - значение
    */
    CValue& operator=(long lValue);

    //!         оператор копирования
    /*! \param  llValue - значение
    */
    CValue& operator=(long long llValue);

    //!         оператор копирования
    /*! \param  ubtValue - значение
        */
    CValue& operator=(unsignedByte ubtValue);

    //!         оператор копирования
    /*! \param  unValue - значение
        */
    CValue& operator=(unsigned int unValue);

    //!         оператор копирования
    /*! \param  ushValue - значение
        */
    CValue& operator=(unsigned short ushValue);

    //!         оператор копирования
    /*! \param  ulValue - значение
        */
    CValue& operator=(unsigned long ulValue);

    //!         оператор копирования
    /*! \param  ulValue - значение
    */
    CValue& operator=(unsigned long long ulValue);

    //!         оператор копирования
    /*! \param  bValue - значение
        */
    CValue& operator=(bool bValue);


    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const std::string&() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const float() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const double() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const byte() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const int() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const short() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const long() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const long long() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const unsignedByte() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const unsigned int() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const unsigned short() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const unsigned long() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const unsigned long long() const;

    //!         const-оператор приведения к типу
    /*! \return const-значение приведенное к типу
        */
    operator const bool() const;


    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator std::string&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator float&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator double&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator byte&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator int&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator short&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator long&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator long long&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator unsignedByte&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator unsigned int&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator unsigned short&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator unsigned long&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator unsigned long long&();

    //!         оператор приведения к типу
    /*! \return значение приведенное к типу
        */
    operator bool&();

    //!         явное получение const-строкового значение
    /*! \return const-строковое значение
        */
    const std::string& AsString() const;

    //!         явное получение строкового значение
    /*! \return строковое значение
        */
    std::string& AsString();

    //!         получить тип значения
    /*! \return тип значения
        */
    const EType GetType() const;

    //!         является ли хранимый тип - числовой
    /*! \return true - хранимый тип - числовой
    */
    bool IsNumber() const;

    //!         является ли указанный тип - числовой
    /*! \param  eType - типп
        \return true - указанный тип - числовой
        */
    bool IsNumber(EType eType) const;

    bool operator==(const CValue& rValue) const;

    void Flush();

  private:
    union UValue
    {
      bool                bValue;

      float               fValue;
      double              dValue;
      
      byte                btValue;
      int                 nValue;
      short               shValue;
      long                lValue;
      long long           llValue;

      unsignedByte        ubtValue;
      unsigned int        unValue;
      unsigned short      ushValue;
      unsigned long       ulValue;
      unsigned long long  ullValue;
    };

  private:
    void Sync( EType eTypeFrom, EType eTypeTo ) const;
    void Sync() const;
    void SyncTo( EType eTypeTo ) const;

  private:
    mutable EType m_eType;
    mutable EType m_ePrevType;

    mutable std::string  m_sValue;
    mutable UValue m_uValue;
    bool m_bChanged;
    mutable CDataObject* m_pDataObject;
  };
}

#endif // _VALUE_H_
