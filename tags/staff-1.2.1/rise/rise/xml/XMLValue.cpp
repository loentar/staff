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

#include "XMLValue.h"

namespace rise
{
  namespace xml
  {
    CXMLValue::CXMLValue() 
    {
    }

    CXMLValue::CXMLValue(const CXMLValue& rValue):
      m_sValue(rValue.m_sValue)
    {
    }

    CXMLValue::CXMLValue(const CString& rValue):
      m_sValue(rValue)
    {
    }


    CXMLValue& CXMLValue::operator=(const CXMLValue& rValue)
    {
      m_sValue = rValue.m_sValue;
      return *this;
    }


    CXMLValue& CXMLValue::operator=(const CString& rValue)
    {
      m_sValue = rValue;
      return *this;
    }

    CXMLValue& CXMLValue::operator=(const TChar* rValue)
    {
      m_sValue = rValue;
      return *this;
    }

    bool CXMLValue::operator==(const CXMLValue& rXMLValue) const
    {
      return m_sValue == rXMLValue.m_sValue;
    }

    bool CXMLValue::operator==( const TChar* szValue ) const
    {
      return m_sValue == szValue;
    }

    bool CXMLValue::operator==(const CString& rValue) const
    {
      return m_sValue == rValue;
    }


    bool CXMLValue::operator!=(const CXMLValue& rXMLValue) const
    {
      return m_sValue != rXMLValue.m_sValue;
    }

    bool CXMLValue::operator!=(const CString& rValue) const
    {
      return m_sValue != rValue;
    }

    bool CXMLValue::operator!=( const TChar* szValue ) const
    {
      return m_sValue != szValue;
    }


    CXMLValue::operator const CString&() const
    {
      return m_sValue;
    }

    CXMLValue::operator CString&()
    {
      return m_sValue;
    }

    CString& CXMLValue::AsString()
    {
      return m_sValue;
    }

    const CString& CXMLValue::AsString() const
    {
      return m_sValue;
    }

    int CXMLValue::AsInt() const
    {
      int nValue = 0;
      return ::rise::FromStr(m_sValue, nValue);
    }

    float CXMLValue::AsFloat() const
    {
      float fValue = 0;
      return ::rise::FromStr(m_sValue, fValue);
    }

    double CXMLValue::AsDouble() const
    {
      double dValue = 0;
      return ::rise::FromStr(m_sValue, dValue);
    }


    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLValue& rXMLValue)
    {      
      rStream.WriteString(rXMLValue.AsString());
      return rStream;
    }

    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLValue& rXMLValue)
    {
      rStream.ReadString(rXMLValue.AsString());
      return rStream;
    }


    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLValue& rXMLValue)
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLValue);
    }

    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLValue& rXMLValue)
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLValue);
    }


  } // namespace xml
} // namespace rise
