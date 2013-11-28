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

#include <staff/utils/tostring.h>
#include <staff/utils/fromstring.h>
#include "Value.h"

namespace staff
{
  Value::Value():
    m_eType(TypeUnknown), m_ePrevType(TypeUnknown), m_bChanged(false)
  {
  }

  Value::Value(const std::string& sValue):
    m_bChanged(false)
  {
    operator=(sValue);
  }

  Value::Value(const char* szValue):
    m_bChanged(false)
  {
    operator=(szValue);
  }

  Value::Value(float fValue):
    m_bChanged(false)
  {
    operator=(fValue);
  }

  Value::Value(double dValue):
    m_bChanged(false)
  {
    operator=(dValue);
  }

  Value::Value(long double ldValue):
    m_bChanged(false)
  {
    operator=(ldValue);
  }

  Value::Value(byte btValue):
    m_bChanged(false)
  {
    operator=(btValue);
  }

  Value::Value(int nValue):
    m_bChanged(false)
  {
    operator=(nValue);
  }

  Value::Value(short shValue):
    m_bChanged(false)
  {
    operator=(shValue);
  }

  Value::Value(long lValue):
    m_bChanged(false)
  {
    operator=(lValue);
  }

  Value::Value(long long llValue):
    m_bChanged(false)
  {
    operator=(llValue);
  }

  Value::Value(unsignedByte btValue):
    m_bChanged(false)
  {
    operator=(btValue);
  }

  Value::Value(unsigned int unValue):
    m_bChanged(false)
  {
    operator=(unValue);
  }

  Value::Value(unsigned short ushValue):
    m_bChanged(false)
  {
    operator=(ushValue);
  }

  Value::Value(unsigned long ulValue):
    m_bChanged(false)
  {
    operator=(ulValue);
  }

  Value::Value(unsigned long long ullValue):
    m_bChanged(false)
  {
    operator=(ullValue);
  }

  Value::Value(bool bValue):
    m_bChanged(false)
  {
    operator=(bValue);
  }


  Value& Value::operator=(const std::string& sValue)
  {
    m_bChanged = true;
    m_eType = TypeText;
    m_ePrevType = m_eType;
    m_sValue = sValue;
    return *this;
  }

  Value& Value::operator=(const char* szValue)
  {
    m_bChanged = true;
    m_eType = TypeText;
    m_ePrevType = m_eType;
    if (szValue == NULL)
      m_sValue = "";
    else
      m_sValue = szValue;
    return *this;
  }

  Value& Value::operator=(float fValue)
  {
    m_bChanged = true;
    m_eType = TypeFloat;
    m_ePrevType = m_eType;
    m_uValue.fValue = fValue;
    return *this;
  }

  Value& Value::operator=(double dValue)
  {
    m_bChanged = true;
    m_eType = TypeDouble;
    m_ePrevType = m_eType;
    m_uValue.dValue = dValue;
    return *this;
  }

  Value& Value::operator=(long double ldValue)
  {
    m_bChanged = true;
    m_eType = TypeLongDouble;
    m_ePrevType = m_eType;
    m_uValue.ldValue = ldValue;
    return *this;
  }

  Value& Value::operator=(byte btValue)
  {
    m_bChanged = true;
    m_eType = TypeByte;
    m_ePrevType = m_eType;
    m_uValue.btValue = btValue;
    return *this;
  }

  Value& Value::operator=(int nValue)
  {
    m_bChanged = true;
    m_eType = TypeInt;
    m_ePrevType = m_eType;
    m_uValue.nValue = nValue;
    return *this;
  }

  Value& Value::operator=(short shValue)
  {
    m_bChanged = true;
    m_eType = TypeShort;
    m_ePrevType = m_eType;
    m_uValue.shValue = shValue;
    return *this;
  }

  Value& Value::operator=(long lValue)
  {
    m_bChanged = true;
    m_eType = TypeLong;
    m_ePrevType = m_eType;
    m_uValue.lValue = lValue;
    return *this;
  }

  Value& Value::operator=(long long llValue)
  {
    m_bChanged = true;
    m_eType = TypeLongLong;
    m_ePrevType = m_eType;
    m_uValue.llValue = llValue;
    return *this;
  }

  Value& Value::operator=(unsignedByte ubtValue)
  {
    m_bChanged = true;
    m_eType = TypeUByte;
    m_ePrevType = m_eType;
    m_uValue.ubtValue = ubtValue;
    return *this;
  }

  Value& Value::operator=(unsigned int unValue)
  {
    m_bChanged = true;
    m_eType = TypeUInt;
    m_ePrevType = m_eType;
    m_uValue.unValue = unValue;
    return *this;
  }

  Value& Value::operator=(unsigned short ushValue)
  {
    m_bChanged = true;
    m_eType = TypeUShort;
    m_ePrevType = m_eType;
    m_uValue.ushValue = ushValue;
    return *this;
  }

  Value& Value::operator=(unsigned long ulValue)
  {
    m_bChanged = true;
    m_eType = TypeULong;
    m_ePrevType = m_eType;
    m_uValue.ulValue = ulValue;
    return *this;
  }

  Value& Value::operator=(unsigned long long ullValue)
  {
    m_bChanged = true;
    m_eType = TypeULongLong;
    m_ePrevType = m_eType;
    m_uValue.ullValue = ullValue;
    return *this;
  }

  Value& Value::operator=(bool bValue)
  {
    m_bChanged = true;
    m_eType = TypeBool;
    m_ePrevType = m_eType;
    m_uValue.bValue = bValue;
    return *this;
  }

  Value& Value::operator=(const Value& rValue)
  {
    m_eType = rValue.m_eType;
    m_ePrevType = rValue.m_ePrevType;
    m_bChanged = rValue.m_bChanged;
    if (m_eType == TypeText)
    {
      m_sValue = rValue.m_sValue;
    } else
    {
      m_uValue = rValue.m_uValue;
    }

    return *this;
  }


  Value::operator std::string&()
  {
    m_bChanged = true;
    SyncTo(TypeText);
    return m_sValue;
  }

  Value::operator float&()
  {
    m_bChanged = true;
    SyncTo(TypeFloat);
    return m_uValue.fValue;
  }

  Value::operator double&()
  {
    m_bChanged = true;
    SyncTo(TypeDouble);
    return m_uValue.dValue;
  }

  Value::operator long double&()
  {
    m_bChanged = true;
    SyncTo(TypeLongDouble);
    return m_uValue.ldValue;
  }

  Value::operator byte&()
  {
    m_bChanged = true;
    SyncTo(TypeByte);
    return m_uValue.btValue;
  }

  Value::operator int&()
  {
    m_bChanged = true;
    SyncTo(TypeInt);
    return m_uValue.nValue;
  }

  Value::operator short&()
  {
    m_bChanged = true;
    SyncTo(TypeShort);
    return m_uValue.shValue;
  }

  Value::operator long&()
  {
    m_bChanged = true;
    SyncTo(TypeLong);
    return m_uValue.lValue;
  }

  Value::operator long long&()
  {
    m_bChanged = true;
    SyncTo(TypeLongLong);
    return m_uValue.llValue;
  }

  Value::operator unsignedByte&()
  {
    m_bChanged = true;
    SyncTo(TypeUByte);
    return m_uValue.ubtValue;
  }

  Value::operator unsigned int&()
  {
    m_bChanged = true;
    SyncTo(TypeUInt);
    return m_uValue.unValue;
  }

  Value::operator unsigned short&()
  {
    m_bChanged = true;
    SyncTo(TypeUShort);
    return m_uValue.ushValue;
  }

  Value::operator unsigned long&()
  {
    m_bChanged = true;
    SyncTo(TypeULong);
    return m_uValue.ulValue;
  }

  Value::operator unsigned long long&()
  {
    m_bChanged = true;
    SyncTo(TypeULongLong);
    return m_uValue.ullValue;
  }

  Value::operator bool&()
  {
    m_bChanged = true;
    SyncTo(TypeBool);
    return m_uValue.bValue;
  }


  Value::operator const std::string&() const
  {
    SyncTo(TypeText);
    return m_sValue;
  }

  Value::operator float() const
  {
    SyncTo(TypeFloat);
    return m_uValue.fValue;
  }

  Value::operator double() const
  {
    SyncTo(TypeDouble);
    return m_uValue.dValue;
  }

  Value::operator long double() const
  {
    SyncTo(TypeLongDouble);
    return m_uValue.ldValue;
  }

  Value::operator byte() const
  {
    SyncTo(TypeByte);
    return m_uValue.btValue;
  }

  Value::operator int() const
  {
    SyncTo(TypeInt);
    return m_uValue.nValue;
  }

  Value::operator short() const
  {
    SyncTo(TypeShort);
    return m_uValue.shValue;
  }

  Value::operator long() const
  {
    SyncTo(TypeLong);
    return m_uValue.lValue;
  }

  Value::operator long long() const
  {
    SyncTo(TypeLongLong);
    return m_uValue.llValue;
  }

  Value::operator unsignedByte() const
  {
    SyncTo(TypeUByte);
    return m_uValue.ubtValue;
  }

  Value::operator unsigned int() const
  {
    SyncTo(TypeUInt);
    return m_uValue.unValue;
  }

  Value::operator unsigned short() const
  {
    SyncTo(TypeUShort);
    return m_uValue.ushValue;
  }

  Value::operator unsigned long() const
  {
    SyncTo(TypeULong);
    return m_uValue.ulValue;
  }

  Value::operator unsigned long long() const
  {
    SyncTo(TypeULongLong);
    return m_uValue.ullValue;
  }

  Value::operator bool() const
  {
    SyncTo(TypeBool);
    return m_uValue.bValue;
  }


  void Value::Sync(Type eTypeFrom, Type eTypeTo) const
  {
    switch(eTypeTo)
    {
    case TypeText:
        switch(eTypeFrom)
        {
        case TypeText:
          break;
        case TypeFloat:
          ToString(m_uValue.fValue, m_sValue);
          break;
        case TypeDouble:
          ToString(m_uValue.dValue, m_sValue);
          break;
        case TypeLongDouble:
          ToString(m_uValue.ldValue, m_sValue);
          break;
        case TypeByte:
          ToString(m_uValue.btValue, m_sValue);
          break;
        case TypeInt:
          ToString(m_uValue.nValue, m_sValue);
          break;
        case TypeShort:
          ToString(m_uValue.shValue, m_sValue);
          break;
        case TypeLong:
          ToString(m_uValue.lValue, m_sValue);
          break;
        case TypeLongLong:
          ToString(m_uValue.llValue, m_sValue);
          break;
        case TypeUByte:
          ToString(m_uValue.ubtValue, m_sValue);
          break;
        case TypeUInt:
          ToString(m_uValue.unValue, m_sValue);
          break;
        case TypeUShort:
          ToString(m_uValue.ushValue, m_sValue);
          break;
        case TypeULong:
          ToString(m_uValue.ulValue, m_sValue);
          break;
        case TypeULongLong:
          ToString(m_uValue.ullValue, m_sValue);
          break;
        case TypeBool:
          m_sValue = m_uValue.bValue ? "true" : "false"; 
          break;
        case TypeUnknown:
          break;
        }
        break;
      
    case TypeFloat:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.fValue);
        break;
      case TypeFloat:
        break;
      case TypeDouble:
        m_uValue.fValue = static_cast<float>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.fValue = static_cast<float>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.fValue = static_cast<float>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.fValue = static_cast<float>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.fValue = static_cast<float>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.fValue = static_cast<float>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.fValue = static_cast<float>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.fValue = static_cast<float>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.fValue = static_cast<float>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.fValue = static_cast<float>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.fValue = static_cast<float>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.fValue = static_cast<float>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.fValue = m_uValue.bValue ? 1.0f : 0.0f;
        break;
      case TypeUnknown:
        break;
      }
      break;
      
    case TypeDouble:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.dValue);
        break;
      case TypeFloat:
        m_uValue.dValue = static_cast<double>(m_uValue.fValue);
        break;
      case TypeDouble:
        break;
      case TypeLongDouble:
        m_uValue.dValue = static_cast<double>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.dValue = static_cast<double>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.dValue = static_cast<double>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.dValue = static_cast<double>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.dValue = static_cast<double>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.dValue = static_cast<double>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.dValue = static_cast<double>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.dValue = static_cast<double>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.dValue = static_cast<double>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.dValue = static_cast<double>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.dValue = static_cast<double>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.dValue = m_uValue.bValue ? 1.0 : 0.0;
        break;
      case TypeUnknown:
        break;
      }
      break;
      
    case TypeLongDouble:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.ldValue);
        break;
      case TypeFloat:
        m_uValue.ldValue = static_cast<long double>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.ldValue = static_cast<long double>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        break;
      case TypeByte:
        m_uValue.ldValue = static_cast<long double>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.ldValue = static_cast<long double>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.ldValue = static_cast<long double>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.ldValue = static_cast<long double>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.ldValue = static_cast<long double>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.ldValue = static_cast<long double>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.ldValue = static_cast<long double>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.ldValue = static_cast<long double>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.ldValue = static_cast<long double>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.ldValue = static_cast<long double>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.ldValue = m_uValue.bValue ? 1.0 : 0.0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeByte:
      switch(eTypeFrom)
      {
      case TypeText:
        m_uValue.btValue = m_sValue.size() > 0 ? m_sValue[0] : 0;
        break;
      case TypeFloat:
        m_uValue.btValue = static_cast<byte>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.btValue = static_cast<byte>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.btValue = static_cast<byte>(m_uValue.ldValue);
        break;
      case TypeByte:
        break;
      case TypeInt:
        m_uValue.btValue = static_cast<byte>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.btValue = static_cast<byte>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.btValue = static_cast<byte>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.btValue = static_cast<byte>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.btValue = static_cast<byte>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.btValue = static_cast<byte>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.btValue = static_cast<byte>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.btValue = static_cast<byte>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.btValue = static_cast<byte>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.btValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;
      
    case TypeInt:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.nValue);
        break;
      case TypeFloat:
        m_uValue.nValue = static_cast<int>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.nValue = static_cast<int>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.nValue = static_cast<int>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.nValue = static_cast<int>(m_uValue.btValue);
        break;
      case TypeInt:
        break;
      case TypeShort:
        m_uValue.nValue = static_cast<int>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.nValue = static_cast<int>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.nValue = static_cast<int>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.nValue = static_cast<int>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.nValue = static_cast<int>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.nValue = static_cast<int>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.nValue = static_cast<int>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.nValue = static_cast<int>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.nValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;
      
    case TypeShort:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.shValue);
        break;
      case TypeFloat:
        m_uValue.shValue = static_cast<short>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.shValue = static_cast<short>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.shValue = static_cast<short>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.shValue = static_cast<short>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.shValue = static_cast<short>(m_uValue.nValue);
        break;
      case TypeShort:
        break;
      case TypeLong:
        m_uValue.shValue = static_cast<short>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.shValue = static_cast<short>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.shValue = static_cast<short>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.shValue = static_cast<short>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.shValue = static_cast<short>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.shValue = static_cast<short>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.shValue = static_cast<short>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.shValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeLong:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.lValue);
        break;
      case TypeFloat:
        m_uValue.lValue = static_cast<long>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.lValue = static_cast<long>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.lValue = static_cast<long>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.lValue = static_cast<long>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.lValue = static_cast<long>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.lValue = static_cast<long>(m_uValue.shValue);
        break;
      case TypeLong:
        break;
      case TypeLongLong:
        m_uValue.lValue = static_cast<long>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.lValue = static_cast<long>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.lValue = static_cast<long>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.lValue = static_cast<long>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.lValue = static_cast<long>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.lValue = static_cast<long>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.lValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeLongLong:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.llValue);
        break;
      case TypeFloat:
        m_uValue.llValue = static_cast<long long>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.llValue = static_cast<long long>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.llValue = static_cast<long long>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.llValue = static_cast<long long>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.llValue = static_cast<long long>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.llValue = static_cast<long long>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.llValue = static_cast<long long>(m_uValue.lValue);
        break;
      case TypeLongLong:
        break;
      case TypeUByte:
        m_uValue.llValue = static_cast<long long>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.llValue = static_cast<long long>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.llValue = static_cast<long long>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.llValue = static_cast<long long>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.llValue = static_cast<long long>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.llValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeUByte:
      switch(eTypeFrom)
      {
      case TypeText:
        m_sValue = m_uValue.ubtValue;
        break;
      case TypeFloat:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.ubtValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeUInt:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.unValue);
        break;
      case TypeFloat:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        break;
      case TypeUShort:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.unValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;
      
    case TypeUShort:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.ushValue);
        break;
      case TypeFloat:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.unValue);
        break;
      case TypeUShort:
        break;
      case TypeULong:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.ushValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeULong:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.ulValue);
        break;
      case TypeFloat:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.ushValue);
        break;
      case TypeULong:
        break;
      case TypeULongLong:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.ullValue);
        break;
      case TypeBool:
        m_uValue.ulValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeULongLong:
      switch(eTypeFrom)
      {
      case TypeText:
        FromString(m_sValue, m_uValue.ullValue);
        break;
      case TypeFloat:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.fValue);
        break;
      case TypeDouble:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.dValue);
        break;
      case TypeLongDouble:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.ldValue);
        break;
      case TypeByte:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.btValue);
        break;
      case TypeInt:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.nValue);
        break;
      case TypeShort:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.shValue);
        break;
      case TypeLong:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.lValue);
        break;
      case TypeLongLong:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.llValue);
        break;
      case TypeUByte:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.ubtValue);
        break;
      case TypeUInt:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.unValue);
        break;
      case TypeUShort:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.ushValue);
        break;
      case TypeULong:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.ulValue);
        break;
      case TypeULongLong:
        break;
      case TypeBool:
        m_uValue.ullValue = m_uValue.bValue ? 1 : 0;
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeBool:
      switch(eTypeFrom)
      {
      case TypeText:
        m_uValue.bValue = m_sValue == "True" || m_sValue == "true" || m_sValue == "TRUE" || m_sValue == "1";
        break;
      case TypeFloat:
        m_uValue.bValue = m_uValue.fValue != 0;
        break;
      case TypeDouble:
        m_uValue.bValue = m_uValue.dValue != 0;
        break;
      case TypeLongDouble:
        m_uValue.bValue = m_uValue.ldValue != 0;
        break;
      case TypeByte:
        m_uValue.bValue = m_uValue.btValue != 0;
        break;
      case TypeInt:
        m_uValue.bValue = m_uValue.nValue != 0;
        break;
      case TypeShort:
        m_uValue.bValue = m_uValue.shValue != 0;
        break;
      case TypeLong:
        m_uValue.bValue = m_uValue.lValue != 0;
        break;
      case TypeLongLong:
        m_uValue.bValue = m_uValue.llValue != 0;
        break;
      case TypeUByte:
        m_uValue.bValue = m_uValue.ubtValue != 0;
        break;
      case TypeUInt:
        m_uValue.bValue = m_uValue.unValue != 0;
        break;
      case TypeUShort:
        m_uValue.bValue = m_uValue.ushValue != 0;
        break;
      case TypeULong:
        m_uValue.bValue = m_uValue.ulValue != 0;
        break;
      case TypeULongLong:
        m_uValue.bValue = m_uValue.ullValue != 0;
        break;
      case TypeBool:
        break;
      case TypeUnknown:
        break;
      }
      break;

    case TypeUnknown:
      break;
    }
  }

  void Value::SyncTo(Type eTypeTo) const
  {
    Sync(m_eType, eTypeTo);
    m_ePrevType = eTypeTo;
  }

  Value::Type Value::GetType() const
  {
    return m_eType;
  }

  const std::string& Value::AsString() const
  {
    SyncTo(TypeText);
    return m_sValue;
  }

  std::string& Value::AsString()
  {
    SyncTo(TypeText);
    return m_sValue;
  }

  bool Value::IsNumber() const
  {
    return IsNumber(m_eType);
  }

  bool Value::IsNumber(Type eType) const
  {
    return eType >= TypeFloat && eType <= TypeBool;
  }

  bool Value::operator==(const Value& rValue) const
  {
    if(m_eType != rValue.m_eType)
      return false;

    switch(m_eType)
    {
    case TypeText:
      if (m_sValue == rValue.m_sValue)
        return true;
      break;
    case TypeFloat:
      if (m_uValue.fValue == rValue.m_uValue.fValue)
        return true;
      break;
    case TypeDouble:
      if (m_uValue.dValue == rValue.m_uValue.dValue)
        return true;
      break;
    case TypeLongDouble:
      if (m_uValue.ldValue == rValue.m_uValue.ldValue)
        return true;
      break;
    case TypeByte:
      if (m_uValue.btValue == rValue.m_uValue.btValue)
        return true;
      break;
    case TypeInt:
      if (m_uValue.nValue == rValue.m_uValue.nValue)
        return true;
      break;
    case TypeShort:
      if (m_uValue.shValue == rValue.m_uValue.shValue)
        return true;
      break;
    case TypeLong:
      if (m_uValue.lValue == rValue.m_uValue.lValue)
        return true;
      break;
    case TypeLongLong:
      if (m_uValue.llValue == rValue.m_uValue.llValue)
        return true;
      break;
    case TypeUByte:
      if (m_uValue.ubtValue == rValue.m_uValue.ubtValue)
        return true;
      break;
    case TypeUInt:
      if (m_uValue.unValue == rValue.m_uValue.unValue)
        return true;
      break;
    case TypeUShort:
      if (m_uValue.ushValue == rValue.m_uValue.ushValue)
        return true;
      break;
    case TypeULong:
      if (m_uValue.ulValue == rValue.m_uValue.ulValue)
        return true;
      break;
    case TypeULongLong:
      if (m_uValue.ullValue == rValue.m_uValue.ullValue)
        return true;
      break;
    case TypeBool:
      if (m_uValue.bValue == rValue.m_uValue.bValue)
        return true;
      break;
    case TypeUnknown:
      return true;
      break;
    }

    return false;
  }

  Value::~Value()
  {
  }
}
