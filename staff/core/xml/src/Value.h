/*
 *  Copyright 2012 Utkin Dmitry
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

#ifndef _STAFF_XML_VALUE_H_
#define _STAFF_XML_VALUE_H_

#include <string>
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  typedef char byte;
  typedef unsigned char unsignedByte;

  //! xml value
  class STAFF_XML_EXPORT Value
  {
  public:
    //! default constructor
    Value();

    //! initializing constructor
    /*! \param  rValue - value
        */
    explicit Value(const Value& rValue);

    //! initializing constructor
    /*! \param  sValue - value
        */
    Value(const std::string& sValue);

    //! initializing constructor
    /*! \param  szValue - value
    */
    Value(const char* szValue);

    //! initializing constructor
    /*! \param  fValue - value
    */
    Value(float fValue);

    //! initializing constructor
    /*! \param  dValue - value
        */
    Value(double dValue);

    //! initializing constructor
    /*! \param  btValue - value
    */
    Value(byte btValue);

    //! initializing constructor
    /*! \param  nValue - value
        */
    Value(int nValue);

    //! initializing constructor
    /*! \param  shValue - value
    */
    Value(short shValue);

    //! initializing constructor
    /*! \param  lValue - value
        */
    Value(long lValue);

    //! initializing constructor
    /*! \param  llValue - value
    */
    Value(long long llValue);

    //! initializing constructor
    /*! \param  ubtValue - value
    */
    Value(unsignedByte ubtValue);

    //! initializing constructor
    /*! \param  unValue - value
    */
    Value(unsigned int unValue);

    //! initializing constructor
    /*! \param  ushValue - value
    */
    Value(unsigned short ushValue);

    //! initializing constructor
    /*! \param  ulValue - value
    */
    Value(unsigned long ulValue);

    //! initializing constructor
    /*! \param  ullValue - value
    */
    Value(unsigned long long ullValue);

    //! initializing constructor
    /*! \param  bValue - value
        */
    Value(bool bValue);


    //! copy operator
    /*! \param  rValue - value
        */
    Value& operator=(const Value& rValue);

    //! copy operator
    /*! \param  sValue - value
        */
    Value& operator=(const std::string& sValue);

    //! copy operator
    /*! \param  szValue - value
        */
    Value& operator=(const char* szValue);

    //! copy operator
    /*! \param  fValue - value
        */
    Value& operator=(float fValue);

    //! copy operator
    /*! \param  dValue - value
        */
    Value& operator=(double dValue);

    //! copy operator
    /*! \param  btValue - value
    */
    Value& operator=(byte btValue);

    //! copy operator
    /*! \param  nValue - value
    */
    Value& operator=(int nValue);

    //! copy operator
    /*! \param  shValue - value
    */
    Value& operator=(short shValue);

    //! copy operator
    /*! \param  lValue - value
    */
    Value& operator=(long lValue);

    //! copy operator
    /*! \param  llValue - value
    */
    Value& operator=(long long llValue);

    //! copy operator
    /*! \param  ubtValue - value
        */
    Value& operator=(unsignedByte ubtValue);

    //! copy operator
    /*! \param  unValue - value
        */
    Value& operator=(unsigned int unValue);

    //! copy operator
    /*! \param  ushValue - value
        */
    Value& operator=(unsigned short ushValue);

    //! copy operator
    /*! \param  ulValue - value
        */
    Value& operator=(unsigned long ulValue);

    //! copy operator
    /*! \param  ulValue - value
    */
    Value& operator=(unsigned long long ulValue);

    //! copy operator
    /*! \param  bValue - value
        */
    Value& operator=(bool bValue);


    //! const value cast operator
    /*! \return casted const value
        */
    operator const std::string&() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator float() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator double() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator byte() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator int() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator short() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator long long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsignedByte() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned int() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned short() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator unsigned long long() const;

    //! const value cast operator
    /*! \return casted const value
        */
    operator bool() const;


    //! value cast operator
    /*! \return value
        */
    operator std::string&();


    //! explicit conversion to const string
    /*! \return const string value
        */
    const std::string& AsString() const;

    //! explicit conversion to string
    /*! \return string value
        */
    std::string& AsString();

    //! test target value for equality with specified value
    /*! \param  rValue - other value
        \return true, if values are equals
        */
    bool operator==(const Value& rValue) const;

  private:
    std::string m_sValue;         //!<  string value
  };

}
}

#endif // _STAFF_XML_VALUE_H_
