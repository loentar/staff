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

#include <staff/utils/tostring.h>
#include <staff/utils/fromstring.h>
#include "Value.h"

namespace staff
{
namespace xml
{

  Value::Value()
  {
  }

  Value::Value(const Value& rValue):
    m_sValue(rValue.m_sValue)
  {
  }

  Value::Value(const std::string& sValue):
    m_sValue(sValue)
  {
  }

  Value::Value(const char* szValue):
    m_sValue(szValue ? szValue : "")
  {
  }

  Value::Value(float fValue):
    m_sValue(ToString(fValue))
  {
  }

  Value::Value(double dValue):
    m_sValue(ToString(dValue))
  {
  }

  Value::Value(byte btValue):
    m_sValue(ToString(btValue))
  {
  }

  Value::Value(int nValue):
    m_sValue(ToString(nValue))
  {
  }

  Value::Value(short shValue):
    m_sValue(ToString(shValue))
  {
  }

  Value::Value(long lValue):
    m_sValue(ToString(lValue))
  {
  }

  Value::Value(long long llValue):
    m_sValue(ToString(llValue))
  {
  }

  Value::Value(unsignedByte btValue):
    m_sValue(ToString(btValue))
  {
  }

  Value::Value(unsigned int unValue):
    m_sValue(ToString(unValue))
  {
  }

  Value::Value(unsigned short ushValue):
    m_sValue(ToString(ushValue))
  {
  }

  Value::Value(unsigned long ulValue):
    m_sValue(ToString(ulValue))
  {
  }

  Value::Value(unsigned long long ullValue):
    m_sValue(ToString(ullValue))
  {
  }

  Value::Value(bool bValue):
    m_sValue(bValue ? "true" : "false")
  {
  }


  Value& Value::operator=(const Value& rValue)
  {
    m_sValue = rValue.m_sValue;
    return *this;
  }


  Value& Value::operator=(const std::string& sValue)
  {
    m_sValue = sValue;
    return *this;
  }

  Value& Value::operator=(const char* szValue)
  {
    m_sValue = szValue ? szValue : "";
    return *this;
  }

  Value& Value::operator=(float fValue)
  {
    ToString(fValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(double dValue)
  {
    ToString(dValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(byte btValue)
  {
    ToString(btValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(int nValue)
  {
    ToString(nValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(short shValue)
  {
    ToString(shValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(long lValue)
  {
    ToString(lValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(long long llValue)
  {
    ToString(llValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(unsignedByte ubtValue)
  {
    ToString(ubtValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(unsigned int unValue)
  {
    ToString(unValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(unsigned short ushValue)
  {
    ToString(ushValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(unsigned long ulValue)
  {
    ToString(ulValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(unsigned long long ullValue)
  {
    ToString(ullValue, m_sValue);
    return *this;
  }

  Value& Value::operator=(bool bValue)
  {
    m_sValue = bValue ? "true" : "false";
    return *this;
  }


  Value::operator const std::string&() const
  {
    return m_sValue;
  }

  Value::operator float() const
  {
    float fValue = 0;
    FromString(m_sValue, fValue);
    return fValue;
  }

  Value::operator double() const
  {
    float dValue = 0;
    FromString(m_sValue, dValue);
    return dValue;
  }

  Value::operator byte() const
  {
    float btValue = 0;
    FromString(m_sValue, btValue);
    return btValue;
  }

  Value::operator int() const
  {
    float nValue = 0;
    FromString(m_sValue, nValue);
    return nValue;
  }

  Value::operator short() const
  {
    float shValue = 0;
    FromString(m_sValue, shValue);
    return shValue;
  }

  Value::operator long() const
  {
    float lValue = 0;
    FromString(m_sValue, lValue);
    return lValue;
  }

  Value::operator long long() const
  {
    float llValue = 0;
    FromString(m_sValue, llValue);
    return llValue;
  }

  Value::operator unsignedByte() const
  {
    float ubtValue = 0;
    FromString(m_sValue, ubtValue);
    return ubtValue;
  }

  Value::operator unsigned int() const
  {
    float unValue = 0;
    FromString(m_sValue, unValue);
    return unValue;
  }

  Value::operator unsigned short() const
  {
    float ushValue = 0;
    FromString(m_sValue, ushValue);
    return ushValue;
  }

  Value::operator unsigned long() const
  {
    float ulValue = 0;
    FromString(m_sValue, ulValue);
    return ulValue;
  }

  Value::operator unsigned long long() const
  {
    float ullValue = 0;
    FromString(m_sValue, ullValue);
    return ullValue;
  }

  Value::operator bool() const
  {
    return m_sValue == "true" || m_sValue == "1";
  }


  Value::operator std::string&()
  {
    return m_sValue;
  }

  const std::string& Value::AsString() const
  {
    return m_sValue;
  }

  std::string& Value::AsString()
  {
    return m_sValue;
  }

  bool Value::operator==(const Value& rValue) const
  {
    return m_sValue == rValue.m_sValue;
  }


}
}
