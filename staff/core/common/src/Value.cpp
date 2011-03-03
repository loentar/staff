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

#include <rise/string/String.h>
#include "DataObject.h"
#include "Value.h"

namespace staff
{
  CValue::CValue():
    m_eType(ET_NOTINIT), m_ePrevType(ET_NOTINIT), m_bChanged(false), m_pDataObject(NULL)
  {
  }

  CValue::CValue(const std::string& sValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(sValue);
  }

  CValue::CValue(const char* szValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(szValue);
  }

  CValue::CValue(float fValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(fValue);
  }

  CValue::CValue(double dValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(dValue);
  }

  CValue::CValue(byte btValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(btValue);
  }

  CValue::CValue(int nValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(nValue);
  }

  CValue::CValue(short shValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(shValue);
  }

  CValue::CValue(long lValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(lValue);
  }

  CValue::CValue(long long llValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(llValue);
  }

  CValue::CValue(unsignedByte btValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(btValue);
  }

  CValue::CValue(unsigned int unValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(unValue);
  }

  CValue::CValue(unsigned short ushValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(ushValue);
  }

  CValue::CValue(unsigned long ulValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(ulValue);
  }

  CValue::CValue(unsigned long long ullValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(ullValue);
  }

  CValue::CValue(bool bValue):
    m_bChanged(false), m_pDataObject(NULL)
  {
    operator=(bValue);
  }

  CValue::CValue( CDataObject* pDataObject ):
    m_bChanged(false), m_pDataObject(pDataObject)
  {
    if(m_pDataObject)
    {
      m_eType = ET_TEXT;
      m_ePrevType = m_eType;
      m_sValue = m_pDataObject->GetText();
    }
  }


  CValue& CValue::operator=( const std::string& sValue )
  {
    m_bChanged = true;
    m_eType = ET_TEXT;
    m_ePrevType = m_eType;
    m_sValue = sValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( const char* szValue )
  {
    m_bChanged = true;
    m_eType = ET_TEXT;
    m_ePrevType = m_eType;
    if (szValue == NULL)
      m_sValue = "";
    else
      m_sValue = szValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( float fValue )
  {
    m_bChanged = true;
    m_eType = ET_FLOAT;
    m_ePrevType = m_eType;
    m_uValue.fValue = fValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( double dValue )
  {
    m_bChanged = true;
    m_eType = ET_DOUBLE;
    m_ePrevType = m_eType;
    m_uValue.dValue = dValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( byte btValue )
  {
    m_bChanged = true;
    m_eType = ET_BYTE;
    m_ePrevType = m_eType;
    m_uValue.btValue = btValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( int nValue )
  {
    m_bChanged = true;
    m_eType = ET_INT;
    m_ePrevType = m_eType;
    m_uValue.nValue = nValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( short shValue )
  {
    m_bChanged = true;
    m_eType = ET_SHORT;
    m_ePrevType = m_eType;
    m_uValue.shValue = shValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( long lValue )
  {
    m_bChanged = true;
    m_eType = ET_LONG;
    m_ePrevType = m_eType;
    m_uValue.lValue = lValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( long long llValue )
  {
    m_bChanged = true;
    m_eType = ET_LONGLONG;
    m_ePrevType = m_eType;
    m_uValue.llValue = llValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( unsignedByte ubtValue )
  {
    m_bChanged = true;
    m_eType = ET_UBYTE;
    m_ePrevType = m_eType;
    m_uValue.ubtValue = ubtValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( unsigned int unValue )
  {
    m_bChanged = true;
    m_eType = ET_UINT;
    m_ePrevType = m_eType;
    m_uValue.unValue = unValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( unsigned short ushValue )
  {
    m_bChanged = true;
    m_eType = ET_USHORT;
    m_ePrevType = m_eType;
    m_uValue.ushValue = ushValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( unsigned long ulValue )
  {
    m_bChanged = true;
    m_eType = ET_ULONG;
    m_ePrevType = m_eType;
    m_uValue.ulValue = ulValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( unsigned long long ullValue )
  {
    m_bChanged = true;
    m_eType = ET_ULONGLONG;
    m_ePrevType = m_eType;
    m_uValue.ullValue = ullValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( bool bValue )
  {
    m_bChanged = true;
    m_eType = ET_BOOL;
    m_ePrevType = m_eType;
    m_uValue.bValue = bValue;
    Flush();
    return *this;
  }

  CValue& CValue::operator=( const CValue& rValue )
  {
    m_eType = rValue.m_eType;
    m_ePrevType = rValue.m_ePrevType;
    if(rValue.m_pDataObject != NULL)
      m_pDataObject = rValue.m_pDataObject;
    m_bChanged = rValue.m_bChanged;
    if (m_eType == ET_TEXT)
    {
      m_sValue = rValue.m_sValue;
    } else
    {
      m_uValue = rValue.m_uValue;
    }

    return *this;
  }


  CValue::operator std::string&()
  {
    m_bChanged = true;
    SyncTo(ET_TEXT);
    return m_sValue;
  }

  CValue::operator float&()
  {
    m_bChanged = true;
    SyncTo(ET_FLOAT);
    return m_uValue.fValue;
  }

  CValue::operator double&()
  {
    m_bChanged = true;
    SyncTo(ET_DOUBLE);
    return m_uValue.dValue;
  }

  CValue::operator byte&()
  {
    m_bChanged = true;
    SyncTo(ET_BYTE);
    return m_uValue.btValue;
  }

  CValue::operator int&()
  {
    m_bChanged = true;
    SyncTo(ET_INT);
    return m_uValue.nValue;
  }

  CValue::operator short&()
  {
    m_bChanged = true;
    SyncTo(ET_SHORT);
    return m_uValue.shValue;
  }

  CValue::operator long&()
  {
    m_bChanged = true;
    SyncTo(ET_LONG);
    return m_uValue.lValue;
  }

  CValue::operator long long&()
  {
    m_bChanged = true;
    SyncTo(ET_LONGLONG);
    return m_uValue.llValue;
  }

  CValue::operator unsignedByte&()
  {
    m_bChanged = true;
    SyncTo(ET_UBYTE);
    return m_uValue.ubtValue;
  }

  CValue::operator unsigned int&()
  {
    m_bChanged = true;
    SyncTo(ET_UINT);
    return m_uValue.unValue;
  }

  CValue::operator unsigned short&()
  {
    m_bChanged = true;
    SyncTo(ET_USHORT);
    return m_uValue.ushValue;
  }

  CValue::operator unsigned long&()
  {
    m_bChanged = true;
    SyncTo(ET_ULONG);
    return m_uValue.ulValue;
  }

  CValue::operator unsigned long long&()
  {
    m_bChanged = true;
    SyncTo(ET_ULONGLONG);
    return m_uValue.ullValue;
  }

  CValue::operator bool&()
  {
    m_bChanged = true;
    SyncTo(ET_BOOL);
    return m_uValue.bValue;
  }


  CValue::operator const std::string&() const
  {
    SyncTo(ET_TEXT);
    return m_sValue;
  }

  CValue::operator const float() const
  {
    SyncTo(ET_FLOAT);
    return m_uValue.fValue;
  }

  CValue::operator const double() const
  {
    SyncTo(ET_DOUBLE);
    return m_uValue.dValue;
  }

  CValue::operator const byte() const
  {
    SyncTo(ET_BYTE);
    return m_uValue.btValue;
  }

  CValue::operator const int() const
  {
    SyncTo(ET_INT);
    return m_uValue.nValue;
  }

  CValue::operator const short() const
  {
    SyncTo(ET_SHORT);
    return m_uValue.shValue;
  }

  CValue::operator const long() const
  {
    SyncTo(ET_LONG);
    return m_uValue.lValue;
  }

  CValue::operator const long long() const
  {
    SyncTo(ET_LONGLONG);
    return m_uValue.llValue;
  }

  CValue::operator const unsignedByte() const
  {
    SyncTo(ET_UBYTE);
    return m_uValue.ubtValue;
  }

  CValue::operator const unsigned int() const
  {
    SyncTo(ET_UINT);
    return m_uValue.unValue;
  }

  CValue::operator const unsigned short() const
  {
    SyncTo(ET_USHORT);
    return m_uValue.ushValue;
  }

  CValue::operator const unsigned long() const
  {
    SyncTo(ET_ULONG);
    return m_uValue.ulValue;
  }

  CValue::operator const unsigned long long() const
  {
    SyncTo(ET_ULONGLONG);
    return m_uValue.ullValue;
  }

  CValue::operator const bool() const
  {
    SyncTo(ET_BOOL);
    return m_uValue.bValue;
  }


  void CValue::Sync( EType eTypeFrom, EType eTypeTo ) const
  {
    switch(eTypeTo)
    {
    case ET_TEXT:
        switch(eTypeFrom)
        {
        case ET_TEXT:
          break;
        case ET_FLOAT:
          rise::ToStr(m_uValue.fValue, m_sValue); 
          break;
        case ET_DOUBLE:
          rise::ToStr(m_uValue.dValue, m_sValue); 
          break;
        case ET_BYTE:
          rise::ToStr(m_uValue.btValue, m_sValue); 
          break;
        case ET_INT:
          rise::ToStr(m_uValue.nValue, m_sValue); 
          break;
        case ET_SHORT:
          rise::ToStr(m_uValue.shValue, m_sValue); 
          break;
        case ET_LONG:
          rise::ToStr(m_uValue.lValue, m_sValue); 
          break;
        case ET_LONGLONG:
          rise::ToStr(m_uValue.llValue, m_sValue); 
          break;
        case ET_UBYTE:
          rise::ToStr(m_uValue.ubtValue, m_sValue); 
          break;
        case ET_UINT:
          rise::ToStr(m_uValue.unValue, m_sValue); 
          break;
        case ET_USHORT:
          rise::ToStr(m_uValue.ushValue, m_sValue); 
          break;
        case ET_ULONG:
          rise::ToStr(m_uValue.ulValue, m_sValue); 
          break;
        case ET_ULONGLONG:
          rise::ToStr(m_uValue.ullValue, m_sValue); 
          break;
        case ET_BOOL:
          m_sValue = m_uValue.bValue ? "true" : "false"; 
          break;
        case ET_NOTINIT:
        case ET_UNKNOWN:
          break;
        }
        break;
      
    case ET_FLOAT:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.fValue); 
        break;
      case ET_FLOAT:
        break;
      case ET_DOUBLE:
        m_uValue.fValue = static_cast<float>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.fValue = static_cast<float>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.fValue = static_cast<float>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.fValue = static_cast<float>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.fValue = static_cast<float>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.fValue = static_cast<float>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.fValue = static_cast<float>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.fValue = static_cast<float>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.fValue = static_cast<float>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.fValue = static_cast<float>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.fValue = static_cast<float>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.fValue = m_uValue.bValue ? 1.0f : 0.0f;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;
      
    case ET_DOUBLE:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.dValue); 
        break;
      case ET_FLOAT:
        m_uValue.dValue = static_cast<double>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        break;
      case ET_BYTE:
        m_uValue.dValue = static_cast<double>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.dValue = static_cast<double>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.dValue = static_cast<double>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.dValue = static_cast<double>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.dValue = static_cast<double>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.dValue = static_cast<double>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.dValue = static_cast<double>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.dValue = static_cast<double>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.dValue = static_cast<double>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.dValue = static_cast<double>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.dValue = m_uValue.bValue ? 1.0 : 0.0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;
      
    case ET_BYTE:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        m_uValue.btValue = m_sValue.size() > 0 ? m_sValue[0] : 0;
        break;
      case ET_FLOAT:
        m_uValue.btValue = static_cast<byte>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.btValue = static_cast<byte>(m_uValue.dValue);
        break;
      case ET_BYTE:
        break;
      case ET_INT:
        m_uValue.btValue = static_cast<byte>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.btValue = static_cast<byte>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.btValue = static_cast<byte>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.btValue = static_cast<byte>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.btValue = static_cast<byte>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.btValue = static_cast<byte>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.btValue = static_cast<byte>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.btValue = static_cast<byte>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.btValue = static_cast<byte>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.btValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;
      
    case ET_INT:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.nValue); 
        break;
      case ET_FLOAT:
        m_uValue.nValue = static_cast<int>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.nValue = static_cast<int>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.nValue = static_cast<int>(m_uValue.btValue);
        break;
      case ET_INT:
        break;
      case ET_SHORT:
        m_uValue.nValue = static_cast<int>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.nValue = static_cast<int>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.nValue = static_cast<int>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.nValue = static_cast<int>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.nValue = static_cast<int>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.nValue = static_cast<int>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.nValue = static_cast<int>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.nValue = static_cast<int>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.nValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;
      
    case ET_SHORT:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.shValue); 
        break;
      case ET_FLOAT:
        m_uValue.shValue = static_cast<short>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.shValue = static_cast<short>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.shValue = static_cast<short>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.shValue = static_cast<short>(m_uValue.nValue);
        break;
      case ET_SHORT:
        break;
      case ET_LONG:
        m_uValue.shValue = static_cast<short>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.shValue = static_cast<short>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.shValue = static_cast<short>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.shValue = static_cast<short>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.shValue = static_cast<short>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.shValue = static_cast<short>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.shValue = static_cast<short>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.shValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_LONG:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.lValue); 
        break;
      case ET_FLOAT:
        m_uValue.lValue = static_cast<long>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.lValue = static_cast<long>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.lValue = static_cast<long>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.lValue = static_cast<long>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.lValue = static_cast<long>(m_uValue.shValue);
        break;
      case ET_LONG:
        break;
      case ET_LONGLONG:
        m_uValue.lValue = static_cast<long>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.lValue = static_cast<long>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.lValue = static_cast<long>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.lValue = static_cast<long>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.lValue = static_cast<long>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.lValue = static_cast<long>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.lValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_LONGLONG:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.llValue); 
        break;
      case ET_FLOAT:
        m_uValue.llValue = static_cast<long long>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.llValue = static_cast<long long>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.llValue = static_cast<long long>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.llValue = static_cast<long long>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.llValue = static_cast<long long>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.llValue = static_cast<long long>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        break;
      case ET_UBYTE:
        m_uValue.llValue = static_cast<long long>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.llValue = static_cast<long long>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.llValue = static_cast<long long>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.llValue = static_cast<long long>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.llValue = static_cast<long long>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.llValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_UBYTE:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        m_sValue = m_uValue.ubtValue;
        break;
      case ET_FLOAT:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.ubtValue = static_cast<unsignedByte>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.ubtValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_UINT:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.unValue); 
        break;
      case ET_FLOAT:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        break;
      case ET_USHORT:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.unValue = static_cast<unsigned int>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.unValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;
      
    case ET_USHORT:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.ushValue); 
        break;
      case ET_FLOAT:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.unValue);
        break;
      case ET_USHORT:
        break;
      case ET_ULONG:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        m_uValue.ushValue = static_cast<unsigned short>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.ushValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_ULONG:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.ulValue); 
        break;
      case ET_FLOAT:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        break;
      case ET_ULONGLONG:
        m_uValue.ulValue = static_cast<unsigned long>(m_uValue.ullValue);
        break;
      case ET_BOOL:
        m_uValue.ulValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_ULONGLONG:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        rise::FromStr(m_sValue, m_uValue.ullValue); 
        break;
      case ET_FLOAT:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.fValue);
        break;
      case ET_DOUBLE:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.dValue);
        break;
      case ET_BYTE:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.btValue);
        break;
      case ET_INT:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.nValue);
        break;
      case ET_SHORT:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.shValue);
        break;
      case ET_LONG:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.lValue);
        break;
      case ET_LONGLONG:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.llValue);
        break;
      case ET_UBYTE:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.ubtValue);
        break;
      case ET_UINT:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.unValue);
        break;
      case ET_USHORT:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.ushValue);
        break;
      case ET_ULONG:
        m_uValue.ullValue = static_cast<unsigned long long>(m_uValue.ulValue);
        break;
      case ET_ULONGLONG:
        break;
      case ET_BOOL:
        m_uValue.ullValue = m_uValue.bValue ? 1 : 0;
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_BOOL:
      switch(eTypeFrom)
      {
      case ET_TEXT:
        m_uValue.bValue = m_sValue == "True" || m_sValue == "true" || m_sValue == "TRUE" || m_sValue == "1";
        break;
      case ET_FLOAT:
        m_uValue.bValue = m_uValue.fValue != 0;
        break;
      case ET_DOUBLE:
        m_uValue.bValue = m_uValue.dValue != 0;
        break;
      case ET_BYTE:
        m_uValue.bValue = m_uValue.btValue != 0;
        break;
      case ET_INT:
        m_uValue.bValue = m_uValue.nValue != 0;
        break;
      case ET_SHORT:
        m_uValue.bValue = m_uValue.shValue != 0;
        break;
      case ET_LONG:
        m_uValue.bValue = m_uValue.lValue != 0;
        break;
      case ET_LONGLONG:
        m_uValue.bValue = m_uValue.llValue != 0;
        break;
      case ET_UBYTE:
        m_uValue.bValue = m_uValue.ubtValue != 0;
        break;
      case ET_UINT:
        m_uValue.bValue = m_uValue.unValue != 0;
        break;
      case ET_USHORT:
        m_uValue.bValue = m_uValue.ushValue != 0;
        break;
      case ET_ULONG:
        m_uValue.bValue = m_uValue.ulValue != 0;
        break;
      case ET_ULONGLONG:
        m_uValue.bValue = m_uValue.ullValue != 0;
        break;
      case ET_BOOL:
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      break;

    case ET_NOTINIT:
    case ET_UNKNOWN:
      break;
    }
  }

  void CValue::Sync() const
  {
    if (m_ePrevType == m_eType)
      return;

    Sync(m_ePrevType, m_eType);
    m_ePrevType = m_eType;
  }

  void CValue::SyncTo( EType eTypeTo ) const
  {
    Sync();
    Sync(m_eType, eTypeTo);
    m_ePrevType = eTypeTo;
  }

  const CValue::EType CValue::GetType() const
  {
    return m_eType;
  }

  const std::string& CValue::AsString() const
  {
    return operator const std::string&();
  }

  std::string& CValue::AsString()
  {
    return operator std::string&();
  }

  bool CValue::IsNumber() const
  {
    return IsNumber(m_eType);
  }

  bool CValue::IsNumber( EType eType ) const
  {
    return eType >= ET_FLOAT && eType <= ET_BOOL;
  }

  bool CValue::operator==( const CValue& rValue ) const
  {
    if(m_eType != rValue.m_eType)
      return false;

    switch(m_eType)
    {
    case ET_TEXT:
      if (m_sValue == rValue.m_sValue)
        return true;
      break;
    case ET_FLOAT:
      if (m_uValue.fValue == rValue.m_uValue.fValue)
        return true;
      break;
    case ET_DOUBLE:
      if (m_uValue.dValue == rValue.m_uValue.dValue)
        return true;
      break;
    case ET_BYTE:
      if (m_uValue.btValue == rValue.m_uValue.btValue)
        return true;
      break;
    case ET_INT:
      if (m_uValue.nValue == rValue.m_uValue.nValue)
        return true;
      break;
    case ET_SHORT:
      if (m_uValue.shValue == rValue.m_uValue.shValue)
        return true;
      break;
    case ET_LONG:
      if (m_uValue.lValue == rValue.m_uValue.lValue)
        return true;
      break;
    case ET_LONGLONG:
      if (m_uValue.llValue == rValue.m_uValue.llValue)
        return true;
      break;
    case ET_UBYTE:
      if (m_uValue.ubtValue == rValue.m_uValue.ubtValue)
        return true;
      break;
    case ET_UINT:
      if (m_uValue.unValue == rValue.m_uValue.unValue)
        return true;
      break;
    case ET_USHORT:
      if (m_uValue.ushValue == rValue.m_uValue.ushValue)
        return true;
      break;
    case ET_ULONG:
      if (m_uValue.ulValue == rValue.m_uValue.ulValue)
        return true;
      break;
    case ET_ULONGLONG:
      if (m_uValue.ullValue == rValue.m_uValue.ullValue)
        return true;
      break;
    case ET_BOOL:
      if (m_uValue.bValue == rValue.m_uValue.bValue)
        return true;
      break;
    case ET_NOTINIT:
    case ET_UNKNOWN:
      return true;
      break;
    }

    return false;
  }

  CValue::~CValue()
  {
    Flush();
  }

  void CValue::Flush()
  {
    if (m_bChanged && m_pDataObject != NULL)
    {
      switch(m_eType)
      {
      case ET_TEXT:
        break;
      case ET_FLOAT:
        rise::ToStr(m_uValue.fValue, m_sValue); 
        break;
      case ET_DOUBLE:
        rise::ToStr(m_uValue.dValue, m_sValue); 
        break;
      case ET_BYTE:
        rise::ToStr(static_cast<short>(m_uValue.btValue), m_sValue); 
        break;
      case ET_INT:
        rise::ToStr(m_uValue.nValue, m_sValue); 
        break;
      case ET_SHORT:
        rise::ToStr(m_uValue.shValue, m_sValue); 
        break;
      case ET_LONG:
        rise::ToStr(m_uValue.lValue, m_sValue); 
        break;
      case ET_LONGLONG:
        rise::ToStr(m_uValue.llValue, m_sValue); 
        break;
      case ET_UBYTE:
        rise::ToStr(static_cast<unsigned short>(m_uValue.ubtValue), m_sValue); 
        break;
      case ET_UINT:
        rise::ToStr(m_uValue.unValue, m_sValue); 
        break;
      case ET_USHORT:
        rise::ToStr(m_uValue.ushValue, m_sValue); 
        break;
      case ET_ULONG:
        rise::ToStr(m_uValue.ulValue, m_sValue); 
        break;
      case ET_ULONGLONG:
        rise::ToStr(m_uValue.ullValue, m_sValue); 
        break;
      case ET_BOOL:
        m_sValue = m_uValue.bValue ? "true" : "false"; 
        break;
      case ET_NOTINIT:
      case ET_UNKNOWN:
        break;
      }
      m_pDataObject->SetText(m_sValue);
    }
  }
}
