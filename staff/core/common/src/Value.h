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
  //! Value
  class STAFF_COMMON_EXPORT Value
  {
  public:
    enum Type      //!  value type
    {
      TypeUnknown,   //!<  unknown
      TypeText,      //!<  text

      TypeFloat,     //!<  float
      TypeDouble,    //!<  double
      TypeLongDouble,//!<  long double

      TypeByte,      //!<  byte
      TypeInt,       //!<  int
      TypeShort,     //!<  short
      TypeLong,      //!<  long
      TypeLongLong,  //!<  long long

      TypeUByte,     //!<  unsigned byte
      TypeUInt,      //!<  unsigned int
      TypeUShort,    //!<  unsigned short
      TypeULong,     //!<  unsigned long
      TypeULongLong, //!<  unsigned long long

      TypeBool       //!<  bool
    };

  public:
    //! default constructor
    Value();

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
    /*! \param  dValue - value
        */
    Value(long double ldValue);

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

    //! destructor
    ~Value();

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
    /*! \param  dValue - value
        */
    Value& operator=(long double dValue);

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
    operator long double() const;

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
    /*! \return casted value
        */
    operator std::string&();

    //! value cast operator
    /*! \return casted value
        */
    operator float&();

    //! value cast operator
    /*! \return casted value
        */
    operator double&();

    //! value cast operator
    /*! \return casted value
        */
    operator long double&();

    //! value cast operator
    /*! \return casted value
        */
    operator byte&();

    //! value cast operator
    /*! \return casted value
        */
    operator int&();

    //! value cast operator
    /*! \return casted value
        */
    operator short&();

    //! value cast operator
    /*! \return casted value
        */
    operator long&();

    //! value cast operator
    /*! \return casted value
        */
    operator long long&();

    //! value cast operator
    /*! \return casted value
        */
    operator unsignedByte&();

    //! value cast operator
    /*! \return casted value
        */
    operator unsigned int&();

    //! value cast operator
    /*! \return casted value
        */
    operator unsigned short&();

    //! value cast operator
    /*! \return casted value
        */
    operator unsigned long&();

    //! value cast operator
    /*! \return casted value
        */
    operator unsigned long long&();

    //! value cast operator
    /*! \return casted value
        */
    operator bool&();

    //! explicit conversion to const string
    /*! \return const string value
        */
    const std::string& AsString() const;

    //! explicit conversion to string
    /*! \return string value
        */
    std::string& AsString();

    //! get stored value type
    /*! \return stored value type
        */
    Type GetType() const;

    //! is stored type number
    /*! \return true - stored type is number
    */
    bool IsNumber() const;

    //! is given type number
    /*! \param  eType - type
        \return true - given type is number
        */
    bool IsNumber(Type eType) const;

    //! test target value for equality with specified value
    /*! \param  rValue - other value
        \return true, if values are equals
        */
    bool operator==(const Value& rValue) const;

  private:
    union UValue //!  value storing union
    {
      bool                bValue;   //!<  bool               
                                          
      float               fValue;   //!<  float              
      double              dValue;   //!<  double             
      long double         ldValue;  //!<  long double
                                          
      byte                btValue;  //!<  byte               
      int                 nValue;   //!<  int                
      short               shValue;  //!<  short              
      long                lValue;   //!<  long               
      long long           llValue;  //!<  long long          
                                          
      unsignedByte        ubtValue; //!<  unsignedByte       
      unsigned int        unValue;  //!<  unsigned int       
      unsigned short      ushValue; //!<  unsigned short     
      unsigned long       ulValue;  //!<  unsigned long      
      unsigned long long  ullValue; //!<  unsigned long long 
    };

  private:
    //! synchronize types
    /*! \param  eTypeFrom - source type
        \param  eTypeTo - destination type
        */
    void Sync(Type eTypeFrom, Type eTypeTo) const;

    //! synchronize type to
    /*! \param  eTypeTo - convert to type
        */
    void SyncTo(Type eTypeTo) const;

  private:
    mutable Type m_eType;                //!<  current type
    mutable Type m_ePrevType;            //!<  previous type

    mutable std::string m_sValue;         //!<  string value
    mutable UValue m_uValue;              //!<  numeral value
    bool m_bChanged;                      //!<  is value was changed
  };

}

#endif // _VALUE_H_
