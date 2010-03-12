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

  //! Value
  class STAFF_COMMON_EXPORT CValue
  {
  public:
    enum EType      //!  value type
    {
      ET_NOTINIT,   //!<  not initialized
      ET_UNKNOWN,   //!<  unknown
      ET_TEXT,      //!<  text

      ET_FLOAT,     //!<  float
      ET_DOUBLE,    //!<  double

      ET_BYTE,      //!<  byte
      ET_INT,       //!<  int
      ET_SHORT,     //!<  short
      ET_LONG,      //!<  long
      ET_LONGLONG,  //!<  long long

      ET_UBYTE,     //!<  unsigned byte
      ET_UINT,      //!<  unsigned int
      ET_USHORT,    //!<  unsigned short
      ET_ULONG,     //!<  unsigned long
      ET_ULONGLONG, //!<  unsigned long long

      ET_BOOL       //!<  bool
    };

  public:
    //!         default constructor
    CValue();

    //!         initializing constructor
    /*! initialization from DataObject's value
        \param  pDataObject - DataObject
        */
    CValue(CDataObject* pDataObject);
    
    //!         initializing constructor
    /*! \param  sValue - value
        */
    CValue(const std::string& sValue);

    //!         initializing constructor
    /*! \param  szValue - value
    */
    CValue(const char* szValue);

    //!         initializing constructor
    /*! \param  fValue - value
    */
    CValue(float fValue);

    //!         initializing constructor
    /*! \param  dValue - value
        */
    CValue(double dValue);

    //!         initializing constructor
    /*! \param  btValue - value
    */
    CValue(byte btValue);

    //!         initializing constructor
    /*! \param  nValue - value
        */
    CValue(int nValue);
    
    //!         initializing constructor
    /*! \param  shValue - value
    */
    CValue(short shValue);

    //!         initializing constructor
    /*! \param  lValue - value
        */
    CValue(long lValue);
    
    //!         initializing constructor
    /*! \param  llValue - value
    */
    CValue(long long llValue);

    //!         initializing constructor
    /*! \param  ubtValue - value
    */
    CValue(unsignedByte ubtValue);

    //!         initializing constructor
    /*! \param  unValue - value
    */
    CValue(unsigned int unValue);

    //!         initializing constructor
    /*! \param  ushValue - value
    */
    CValue(unsigned short ushValue);

    //!         initializing constructor
    /*! \param  ulValue - value
    */
    CValue(unsigned long ulValue);

    //!         initializing constructor
    /*! \param  ullValue - value
    */
    CValue(unsigned long long ullValue);

    //!         initializing constructor
    /*! \param  bValue - value
        */
    CValue(bool bValue);

    //!         destructor
    ~CValue();

    //!         copy operator
    /*! \param  rValue - value
        */
    CValue& operator=(const CValue& rValue);

    //!         copy operator
    /*! \param  sValue - value
        */
    CValue& operator=(const std::string& sValue);

    //!         copy operator
    /*! \param  szValue - value
        */
    CValue& operator=(const char* szValue);

    //!         copy operator
    /*! \param  fValue - value
        */
    CValue& operator=(float fValue);

    //!         copy operator
    /*! \param  dValue - value
        */
    CValue& operator=(double dValue);

    //!         copy operator
    /*! \param  btValue - value
    */
    CValue& operator=(byte btValue);

    //!         copy operator
    /*! \param  nValue - value
    */
    CValue& operator=(int nValue);

    //!         copy operator
    /*! \param  shValue - value
    */
    CValue& operator=(short shValue);

    //!         copy operator
    /*! \param  lValue - value
    */
    CValue& operator=(long lValue);

    //!         copy operator
    /*! \param  llValue - value
    */
    CValue& operator=(long long llValue);

    //!         copy operator
    /*! \param  ubtValue - value
        */
    CValue& operator=(unsignedByte ubtValue);

    //!         copy operator
    /*! \param  unValue - value
        */
    CValue& operator=(unsigned int unValue);

    //!         copy operator
    /*! \param  ushValue - value
        */
    CValue& operator=(unsigned short ushValue);

    //!         copy operator
    /*! \param  ulValue - value
        */
    CValue& operator=(unsigned long ulValue);

    //!         copy operator
    /*! \param  ulValue - value
    */
    CValue& operator=(unsigned long long ulValue);

    //!         copy operator
    /*! \param  bValue - value
        */
    CValue& operator=(bool bValue);


    //!         const value cast operator
    /*! \return casted const value
        */
    operator const std::string&() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const float() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const double() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const byte() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const int() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const short() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const long() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const long long() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const unsignedByte() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const unsigned int() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const unsigned short() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const unsigned long() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const unsigned long long() const;

    //!         const value cast operator
    /*! \return casted const value
        */
    operator const bool() const;


    //!         value cast operator
    /*! \return casted value
        */
    operator std::string&();

    //!         value cast operator
    /*! \return casted value
        */
    operator float&();

    //!         value cast operator
    /*! \return casted value
        */
    operator double&();

    //!         value cast operator
    /*! \return casted value
        */
    operator byte&();

    //!         value cast operator
    /*! \return casted value
        */
    operator int&();

    //!         value cast operator
    /*! \return casted value
        */
    operator short&();

    //!         value cast operator
    /*! \return casted value
        */
    operator long&();

    //!         value cast operator
    /*! \return casted value
        */
    operator long long&();

    //!         value cast operator
    /*! \return casted value
        */
    operator unsignedByte&();

    //!         value cast operator
    /*! \return casted value
        */
    operator unsigned int&();

    //!         value cast operator
    /*! \return casted value
        */
    operator unsigned short&();

    //!         value cast operator
    /*! \return casted value
        */
    operator unsigned long&();

    //!         value cast operator
    /*! \return casted value
        */
    operator unsigned long long&();

    //!         value cast operator
    /*! \return casted value
        */
    operator bool&();

    //!         explicit conversion to const string
    /*! \return const string value
        */
    const std::string& AsString() const;

    //!         explicit conversion to string
    /*! \return string value
        */
    std::string& AsString();

    //!         get stored value type
    /*! \return stored value type
        */
    const EType GetType() const;

    //!         is stored type number
    /*! \return true - stored type is number
    */
    bool IsNumber() const;

    //!         is given type number
    /*! \param  eType - type
        \return true - given type is number
        */
    bool IsNumber(EType eType) const;

    //!         test target value for equality with specified value
    /*! \param  rValue - other value
        \return true, if values are equals
        */
    bool operator==(const CValue& rValue) const;

    //!         force type conversion
    void Flush();

  private:
    union UValue //!  value storing union
    {
      bool                bValue;   //!<  bool               
                                          
      float               fValue;   //!<  float              
      double              dValue;   //!<  double             
                                          
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
    //!         synchronize types
    /*! \param  eTypeFrom - source type
        \param  eTypeTo - destination type
        */
    void Sync( EType eTypeFrom, EType eTypeTo ) const;

    //!         synchronize types
    void Sync() const;
    
    //!         synchronize type to
    /*! \param  eTypeTo - convert to type
        */
    void SyncTo( EType eTypeTo ) const;

  private:
    mutable EType m_eType;                //!<  current type
    mutable EType m_ePrevType;            //!<  previous type

    mutable std::string m_sValue;         //!<  string value
    mutable UValue m_uValue;              //!<  numeral value
    bool m_bChanged;                      //!<  is value was changed
    mutable CDataObject* m_pDataObject;   //!<  bound DataObject
  };
}

#endif // _VALUE_H_
