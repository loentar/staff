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

#include <wchar.h>
#include <rise/common/ByteOrder.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include "StreamBuffer.h"


namespace rise
{
  int CStreamBuffer::m_nFlagsStatic = CStreamBuffer::EF_NONE; // флаги io по умолчанию
  CByteOrder::EByteOrder CStreamBuffer::m_eByteOrderStatic = CByteOrder::GetDefaultByteOrder(); // кодировка по умолчанию

  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CStreamBuffer
  //    DESCRIPTION:    default constructor
  //    PARAMETRS:      none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer::CStreamBuffer():
    m_pucBegin(NULL),
    m_ulROffset(0),
    m_ulWOffset(0),
    m_ulSize(0),
    m_ulBufferSize(0),
    m_nFlags(m_nFlagsStatic),
    m_eByteOrder(m_eByteOrderStatic)
  {
  }

  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CStreamBuffer
  //    DESCRIPTION:    копирующий конструктор
  //    PARAMETRS:      none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer::CStreamBuffer(const CStreamBuffer& rBuffer)
  {
    operator=(rBuffer);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    DESTRUCTOR:     ~CStreamBuffer
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer::~CStreamBuffer()
  {
    if (m_pucBegin != NULL)
      delete[] m_pucBegin;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       =
  //    DESCRIPTION:    оператор копирования
  //    PARAMETRS:      rBuffer - исходный буфер
  //    RETURN:         ссылка на текущий буфер
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer& CStreamBuffer::operator=(const CStreamBuffer& rBuffer)
  {
    Reset();
    Put(rBuffer.GetBuffer(), rBuffer.GetBufferSize());
    m_eByteOrder = rBuffer.m_eByteOrder;
    m_ulROffset = rBuffer.m_ulROffset;
    m_ulWOffset = rBuffer.m_ulWOffset;
    m_ulSize = rBuffer.m_ulSize;
    m_nFlags = rBuffer.m_nFlags;
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       SetFlags
  //    DESCRIPTION:    установка флагов ввода вывода
  //    PARAMETRS:      nFlags
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        см. EFlags
  //////////////////////////////////////////////////////////////////////////////
  void CStreamBuffer::SetFlags(int nFlags)
  {
    m_nFlags = nFlags;
  }

  void CStreamBuffer::SetDefaultFlags(int nFlags)
  {
    m_nFlagsStatic = nFlags;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetFlags
  //    DESCRIPTION:    получение флагов ввода вывода
  //    PARAMETRS:      none
  //    RETURN:         флаги
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int CStreamBuffer::GetFlags() const
  {
    return m_nFlags;
  }

  int CStreamBuffer::GetDefaultFlags()
  {
    return m_nFlagsStatic;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       SetByteOrder
  //    DESCRIPTION:    установить порядок байт
  //    PARAMETRS:      eByteOrder - порядок байт
  //    RETURN:         void
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CStreamBuffer::SetByteOrder(CByteOrder::EByteOrder eByteOrder /*= __BYTE_ORDER*/)
  {
    m_eByteOrder = eByteOrder;
  }

  void CStreamBuffer::SetDefaultByteOrder(CByteOrder::EByteOrder eByteOrder /*= __BYTE_ORDER*/)
  {
    m_eByteOrderStatic = eByteOrder;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetByteOrder
  //    DESCRIPTION:    получить порядок байт
  //    PARAMETRS:      none
  //    RETURN:         порядок байт
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CByteOrder::EByteOrder CStreamBuffer::GetByteOrder() const
  {
    return m_eByteOrder;
  }

  CByteOrder::EByteOrder CStreamBuffer::GetDefaultByteOrder()
  {
    return m_eByteOrderStatic;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Put
  //    DESCRIPTION:    поместить в буфер данные
  //    PARAMETRS:      pData - указатель на данные
  //                    ulDataSize - размер данных
  //    RETURN:         ссылка на обьект буфера
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer& CStreamBuffer::Put(PCBuffer pData, TSize ulDataSize)
  {
    if (ulDataSize == 0)
      return *this;

    memcpy(Grow(ulDataSize), pData, ulDataSize);
    return *this;
  }

  CStreamBuffer& CStreamBuffer::Put(const CStreamBuffer& rData, TSize ulDataSize)
  {
    RISE_ASSERTES((rData.GetSize() >= ulDataSize) && (rData.GetData() != NULL), rise::CLogicSizeException, 
      "недостаточно данных в исходном буфере");
    return Put(rData.GetData(), ulDataSize);
  }

  CStreamBuffer& CStreamBuffer::Put(CStringA& sData)
  {
    return Put(reinterpret_cast<PCBuffer>(sData.c_str()), sData.size() * sizeof(TCharA));
  }

  CStreamBuffer& CStreamBuffer::Put(CStringW& sData)
  {
    return Put(reinterpret_cast<PCBuffer>(sData.c_str()), sData.size() * sizeof(TCharW));
  }


  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Get
  //    DESCRIPTION:    прочесть из буфера данные
  //    PARAMETRS:      pData - указатель на данные
  //                    ulDataSize - размер данных
  //    RETURN:         ссылка на обьект буфера
  //    EXCEPTIONS:     CLogicSizeException - недостаточно данных для чтения
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer& CStreamBuffer::Get(PBuffer pData, TSize ulDataSize)
  {
    if (ulDataSize == 0)
      return *this;

    RISE_ASSERTES((m_ulROffset + ulDataSize) <= m_ulBufferSize, rise::CLogicSizeException, "попытка чтения за пределами буфера: (требуется: " + 
                ToStr(m_ulROffset + ulDataSize) + "/имеется: " + ToStr(m_ulBufferSize) + ")");
    RISE_ASSERTES((m_ulSize >= ulDataSize) && (m_pucBegin != NULL), rise::CLogicSizeException, "недостаточно данных в буфере: (требуется: " +
                ToStr(m_ulROffset + ulDataSize) + "/имеется: " + ToStr(m_ulBufferSize) + ")");
    
    memcpy(pData, m_pucBegin + m_ulROffset, ulDataSize);
    m_ulROffset += ulDataSize;
    m_ulSize -= ulDataSize;
    return *this;
  }

  CStreamBuffer& CStreamBuffer::Get(CStreamBuffer& rData, TSize ulDataSize)
  {
    rData.Reset();
    rData.Resize(ulDataSize);

    return Get(rData.GetData(), ulDataSize);
  }

  CStreamBuffer& CStreamBuffer::Get(CStringA& sData, TSize ulDataSize)
  {
    if (ulDataSize == 0)
      return *this;

    TSize ulRealDataSize = ulDataSize * sizeof(TCharA);

    RISE_ASSERTES((m_ulROffset + ulRealDataSize) <= m_ulBufferSize, rise::CLogicSizeException, "попытка чтения за пределами буфера");
    RISE_ASSERTES((m_ulSize >= ulRealDataSize) && (m_pucBegin != NULL), rise::CLogicSizeException, "недостаточно данных в буфере");

    sData.assign(reinterpret_cast<TCharA*>(GetData()), ulDataSize);

    m_ulROffset += ulRealDataSize;
    m_ulSize -= ulRealDataSize;
    return *this;
  }

  CStreamBuffer& CStreamBuffer::Get(CStringW& sData, TSize ulDataSize)
  {
    if (ulDataSize == 0)
      return *this;

    TSize ulRealDataSize = ulDataSize * sizeof(TCharW);

    RISE_ASSERTES((m_ulROffset + ulRealDataSize) <= m_ulBufferSize, rise::CLogicSizeException, "попытка чтения за пределами буфера");
    RISE_ASSERTES((m_ulSize >= ulRealDataSize) && (m_pucBegin != NULL), rise::CLogicSizeException, "недостаточно данных в буфере");

    sData.assign(reinterpret_cast<TCharW*>(GetData()), ulDataSize);

    m_ulROffset += ulRealDataSize;
    m_ulSize -= ulRealDataSize;
    return *this;
  }

  CStreamBuffer& CStreamBuffer::Get(CStringA& sData)
  {
    sData.erase();

    if(m_pucBegin == NULL || GetSize() == 0)
      return *this;

    TSize ulCharCount = m_ulSize / sizeof(TCharA);
    TSize ulSize = ulCharCount * sizeof(TCharA);

    sData.assign(reinterpret_cast<TCharA*>(GetData()), ulCharCount);

    m_ulROffset += ulSize;
    m_ulSize -= ulSize;
    return *this;
  }

  CStreamBuffer& CStreamBuffer::Get(CStringW& sData)
  {
    sData.erase();

    if(m_pucBegin == NULL || GetSize() == 0)
      return *this;

    TSize ulCharCount = m_ulSize / sizeof(TCharW);
    TSize ulSize = ulCharCount * sizeof(TCharW);

    sData.assign(reinterpret_cast<TCharW*>(GetData()), ulCharCount);

    m_ulROffset += ulSize;
    m_ulSize -= ulSize;
    return *this;
  }
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Ignore
  //    DESCRIPTION:    пропустить чтение N байт
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer& CStreamBuffer::Ignore(TSize ulDataSize)
  {
    if (ulDataSize == 0)
      return *this;

    RISE_ASSERTES((m_ulROffset + ulDataSize) <= m_ulBufferSize, rise::CLogicSizeException, "попытка игнорирования данных за пределами буфера: (требуется: " + 
                ToStr(m_ulROffset + ulDataSize) + "/имеется: " + ToStr(m_ulBufferSize) + ")");
    RISE_ASSERTES((m_ulSize >= ulDataSize) && (m_pucBegin != NULL), rise::CLogicSizeException, "недостаточно данных в буфере для игнорирования: (требуется: " +
                ToStr(m_ulROffset + ulDataSize) + "/имеется: " + ToStr(m_ulBufferSize) + ")");

    m_ulROffset += ulDataSize;
    m_ulSize -= ulDataSize;
    return *this;
  }


  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       WriteBuffer
  //    DESCRIPTION:    Записать данные в буфер со смещением
  //    PARAMETRS:      pData - указатель на буфер данных
  //                    ulDataSize - размер данных
  //                    rsbData - буфер данных
  //                    ulOffset - смещение относительно начало буфера
  //    RETURN:         ссылка на обьект буфера
  //    EXCEPTIONS:     CLogicSizeException - несоответствие размеров буферов
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CStreamBuffer& CStreamBuffer::WriteBuffer(PCBuffer pData, TSize ulDataSize, TSize ulOffset)
  {
    if (ulDataSize == 0)
      return *this;

    RISE_ASSERTES((m_ulROffset + ulOffset + ulDataSize) <= m_ulBufferSize, rise::CLogicSizeException, "попытка записи за пределами буфера");
    RISE_ASSERTES((m_ulSize >= (ulOffset + ulDataSize)) && (m_pucBegin != NULL), rise::CLogicSizeException, "недостаточно данных в буфере");

    memcpy(m_pucBegin + ulOffset, pData, ulDataSize);

    return *this;
  }

  CStreamBuffer& CStreamBuffer::WriteBuffer(const CStreamBuffer& rsbData, TSize ulOffset)
  {
    return WriteBuffer(rsbData.GetData(), rsbData.GetSize(), ulOffset);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Reset
  //    DESCRIPTION:    сброс данных
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CStreamBuffer::Reset()
  {
    m_ulROffset = 0;
    m_ulWOffset = 0;
    m_ulSize = 0;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Normalize
  //    DESCRIPTION:    нормализация(перенос блока данных в начало буфера)
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CStreamBuffer::Normalize()
  {
    if(m_pucBegin == NULL || m_ulROffset == 0)
      return;

    memcpy(m_pucBegin, m_pucBegin + m_ulROffset, m_ulSize);
    m_ulWOffset -= m_ulROffset;
    m_ulROffset = 0;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetData
  //    DESCRIPTION:    получить указатель на начало данных
  //    PARAMETRS:      none
  //    RETURN:         указатель на начало данных, NULL - буфер пуст
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  PBuffer CStreamBuffer::GetData()
  {
    return (m_pucBegin == NULL) ? NULL : m_pucBegin + m_ulROffset;
  }

  PCBuffer CStreamBuffer::GetData() const
  {
    return (m_pucBegin == NULL) ? NULL : m_pucBegin + m_ulROffset;
  }


  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetBuffer
  //    DESCRIPTION:    получить указатель на начало буфера
  //    PARAMETRS:      none
  //    RETURN:         указатель на начало буфера, NULL - буфер пуст
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  PBuffer CStreamBuffer::GetBuffer()
  {
    return m_pucBegin;
  }
  PCBuffer CStreamBuffer::GetBuffer() const
  {
    return m_pucBegin;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetSize
  //    DESCRIPTION:    получение текущего размера данных в буфере
  //    PARAMETRS:      none
  //    RETURN:         текущий размер данных в буфере
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  TSize CStreamBuffer::GetSize() const
  {
    return m_ulSize;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetBufferSize
  //    DESCRIPTION:    получить размер всего буфера
  //    PARAMETRS:      none
  //    RETURN:         размер всего буфера
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  TSize CStreamBuffer::GetBufferSize() const
  {
    return m_ulBufferSize;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetROffset
  //    DESCRIPTION:    получить смещение чтения относительно начала буфера
  //    PARAMETRS:      none
  //    RETURN:         смещение чтения
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  TSize CStreamBuffer::GetROffset() const
  {
    return m_ulROffset;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetWOffset
  //    DESCRIPTION:    получить смещение записи относительно начала буфера
  //    PARAMETRS:      none
  //    RETURN:         смещение записи
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  TSize CStreamBuffer::GetWOffset() const
  {
    return m_ulWOffset;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Grow
  //    DESCRIPTION:    увеличить размер данных в буфере
  //    PARAMETRS:      ulSize - размер на который необходимо увеличить буфер
  //    RETURN:         указатель на начало выделенных данных
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  PBuffer CStreamBuffer::Grow(const TSize ulSize)
  {
    ulong ulSizeOld = m_ulSize;
    Resize(m_ulSize + ulSize);
    return m_pucBegin + ulSizeOld;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Resize
  //    DESCRIPTION:    изменение размера данных
  //    PARAMETRS:      ulSize - новый размер данных
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CStreamBuffer::Resize(const TSize ulSize)
  {
    Reserve(ulSize);
    m_ulWOffset += ulSize - m_ulSize;
    m_ulSize = ulSize;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Reserve
  //    DESCRIPTION:    при необходимости, выделить как минимум ulSize под буфер
  //    PARAMETRS:      ulSize - размер буфера
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CStreamBuffer::Reserve(const TSize ulSize)
  {
    if (ulSize > m_ulBufferSize)
    {
      TSize ulNewMaxSize = ulSize;
      PBuffer pucBeginNew = new TBuffer[ulNewMaxSize];
//      std::cout << "\nRESERVE NEW: " << ulNewMaxSize << std::endl;

      if (m_pucBegin != NULL)
      {
        memcpy(pucBeginNew, m_pucBegin, m_ulBufferSize);
        delete[] m_pucBegin;
      }

      m_pucBegin = pucBeginNew;
      m_ulBufferSize = ulNewMaxSize;
    }
  }


#define CStreamBuffer_DECLARE_SERIAL(TDATA) \
  CStreamBuffer& CStreamBuffer::operator>>(TDATA& tData ) \
  { \
    Get(reinterpret_cast<PBuffer>(&tData), sizeof(tData)); \
    tData = CByteOrder::ToByteOrder(tData, m_eByteOrder); \
    return *this; \
  } \
  CStreamBuffer& CStreamBuffer::operator<<(const TDATA tData ) \
  { \
    TDATA tDataTmp = CByteOrder::ToByteOrder(tData, m_eByteOrder); \
    return Put(reinterpret_cast<PCBuffer>(&tDataTmp), sizeof(tData)); \
  }

  CStreamBuffer_DECLARE_SERIAL(char)
  CStreamBuffer_DECLARE_SERIAL(unsigned char)
  CStreamBuffer_DECLARE_SERIAL(short)
  CStreamBuffer_DECLARE_SERIAL(unsigned short)
  CStreamBuffer_DECLARE_SERIAL(int)
  CStreamBuffer_DECLARE_SERIAL(unsigned int)
  CStreamBuffer_DECLARE_SERIAL(long)
  CStreamBuffer_DECLARE_SERIAL(unsigned long)
  CStreamBuffer_DECLARE_SERIAL(float)
  CStreamBuffer_DECLARE_SERIAL(double)  

  CStreamBuffer& CStreamBuffer::operator>>(CStringA& tData )
  {
    RISE_ASSERTES((m_ulSize > 0) && (m_pucBegin != NULL), rise::CLogicSizeException, "недостаточно данных в буфере");

    TSize nReadSize = 0;
    PBuffer pBufferEnd = reinterpret_cast<PBuffer>(memchr(GetData(), '\0', GetSize() / sizeof(TCharA)));

    if (pBufferEnd == NULL)
      pBufferEnd = GetData() + GetSize();

    nReadSize = (pBufferEnd - GetData()) / sizeof(TCharA);

    tData.assign(reinterpret_cast<TCharA*>(GetData()), nReadSize);
    Ignore((nReadSize + 1) * sizeof(TCharA));
    return *this;
  }

  CStreamBuffer& CStreamBuffer::operator>>(CStringW& tData )
  {
    RISE_ASSERTES((m_ulSize > 0) && (m_pucBegin != NULL), rise::CLogicSizeException, "недостаточно данных в буфере");
    TSize nReadSize = 0;
    PBuffer pBufferEnd = 
      reinterpret_cast<PBuffer>(wmemchr(reinterpret_cast<TCharW*>(GetData()), L'\0', GetSize() / sizeof(TCharW)));

    if (pBufferEnd == NULL)
      pBufferEnd = GetData() + GetSize();

    nReadSize = (pBufferEnd - GetData()) / sizeof(TCharW);

    tData.assign(reinterpret_cast<TCharW*>(GetData()), nReadSize);
    Ignore((nReadSize + 1) * sizeof(TCharW));
    return *this;
  }

  CStreamBuffer& CStreamBuffer::operator<<(const CStringA& tData )
  {
    int nAddWidth = (m_nFlags & EF_STR_NO_LEADING_ZERO) != 0 ? 0 : 1;
    return Put(reinterpret_cast<PCBuffer>(tData.c_str()), tData.size() * sizeof(TCharA) + nAddWidth);
  }

  CStreamBuffer& CStreamBuffer::operator<<(const CStringW& tData )
  {
    int nAddWidth = (m_nFlags & EF_STR_NO_LEADING_ZERO) != 0 ? 0 : 2;
    return Put(reinterpret_cast<PCBuffer>(tData.c_str()), tData.size() * sizeof(TCharW) + nAddWidth);
  }

  CStreamBuffer& CStreamBuffer::operator>>(CStreamBuffer& tData)
  {
    tData.Put(GetBuffer(), GetSize());
    Reset();
    return *this;
  }

  CStreamBuffer& CStreamBuffer::operator<<(const CStreamBuffer& tData)
  {
    Put(tData.GetBuffer(), tData.GetSize());
    return *this;
  }


} // namespace rise
