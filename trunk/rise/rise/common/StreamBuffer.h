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

#ifndef _StreamBuffer_h_
#define _StreamBuffer_h_

#include <rise/common/types.h>
#include <rise/common/ByteOrder.h>

namespace rise
{
  //!        stream buffer
  class RISE_EXPORT CStreamBuffer
  {
  public:
    enum EFlags  //! flags
    {
      EF_NONE = 0,                //!< none
      EF_STR_NO_LEADING_ZERO = 1  //!< disable '\\0' adding while string serializing
    };

  public:
    //!         constructor
    CStreamBuffer();

    //!         copying constructor
    /*! \param  rBuffer - source stream buffer
    */
    CStreamBuffer(const CStreamBuffer& rBuffer);

    //!         destructor
    ~CStreamBuffer();

    //!         copy operator
    /*! \param  rBuffer - source stream buffer
        \return *this
    */
    CStreamBuffer& operator=(const CStreamBuffer& rBuffer);

    //!         set flags
    /*! \sa     EFlags
        \param  nFlags - flags
    */
    void SetFlags(int nFlags);

    //!         set default flags (for newly created stream buffers)
    /*! \sa     EFlags
        \param  nFlags - flags
    */
    static void SetDefaultFlags(int nFlags);

    //!         get flags
    /*! \return flags
    */
    int GetFlags() const;

    //!         get default flags
    /*! \return default flags
    */
    static int GetDefaultFlags();

    //!         set byte order
    /*! \param  eByteOrder - byte order
    */
    void SetByteOrder(CByteOrder::EByteOrder eByteOrder = CByteOrder::GetDefaultByteOrder());

    //!         set default byte order (for all newly created stream buffers)
    /*! \param  eByteOrder - byte order
    */
    static void SetDefaultByteOrder(CByteOrder::EByteOrder eByteOrder = CByteOrder::GetDefaultByteOrder());

    //!         get byte order
    /*! \return byte order
    */
    CByteOrder::EByteOrder GetByteOrder() const;

    //!         get default byte order
    /*! \return default byte order
    */
    static CByteOrder::EByteOrder GetDefaultByteOrder();

    //!         put data to stream buffer
    /*! \param  pData - pointer to data
        \param  ulDataSize - data size
        \return *this
    */
    CStreamBuffer& Put(PCBuffer pData, TSize ulDataSize);

    //!         put data to stream buffer
    /*! \param  rData - stream buffer
        \param  ulDataSize - data size
        \return *this
    */
    CStreamBuffer& Put(const CStreamBuffer& rData, TSize ulDataSize);

    //!         put ansi string to buffer
    /*! \param  sData - ansi string
        \return *this
    */
    CStreamBuffer& Put(CStringA& sData);

    //!         put wide string to buffer
    /*! \param  sData - wide string
        \return *this
    */
    CStreamBuffer& Put(CStringW& sData);

    //!         get data from buffer
    /*! \param  pData - buffer to put data
        \param  ulDataSize - data size
        \return *this
    */
    CStreamBuffer& Get(PBuffer pData, TSize ulDataSize);

    //!         get data from buffer
    /*! \param  rData - buffer to put data
        \param  ulDataSize - data size
        \return *this
    */
    CStreamBuffer& Get(CStreamBuffer& rData, TSize ulDataSize);

    //!         get ansi string from buffer
    /*! \param  sData - ansi string
        \param  ulDataSize - chars count
        \return *this
    */
    CStreamBuffer& Get(CStringA& sData, TSize ulDataSize);

    //!         get wide string from buffer
    /*! \param  sData - wide string
        \param  ulDataSize - chars count
        \return *this
    */
    CStreamBuffer& Get(CStringW& sData, TSize ulDataSize);

    //!         get ansi string from buffer
    /*! \param  sData - ansi string
        \return *this
    */
    CStreamBuffer& Get(CStringA& sData);

    //!         get wide string from buffer
    /*! \param  sData - wide string
        \return *this
    */
    CStreamBuffer& Get(CStringW& sData);

    //!         skip n bytes
    /*! \param  ulDataSize - bytes count
        \return *this
        */
    CStreamBuffer& Ignore(TSize ulDataSize);

    //!         put data to buffer with offset
    /*! \param  pData - data
        \param  ulDataSize - data size
        \param  ulOffset - offset
        \return *this
    */
    CStreamBuffer& WriteBuffer(PCBuffer pData, TSize ulDataSize, TSize ulOffset);

    //!         put data to buffer with offset
    /*! \param  rsbData - data buffer
        \param  ulOffset - offset
        \return *this
    */
    CStreamBuffer& WriteBuffer(const CStreamBuffer& rsbData, TSize ulOffset);

    //!         reset data
    void Reset();

    //!         buffer normalization (after put and get ops)
    void Normalize();

    //!         get raw pointer to data
    /*! \return pointer to data, NULL, if buffer is empty
    */
    PBuffer GetData();

    //!         get raw pointer to data
    /*! \return pointer to data, NULL, if buffer is empty
    */
    PCBuffer GetData() const;
   
    //!         get raw pointer to data
    /*! \return pointer to data, NULL, if buffer is empty
    */
    PBuffer GetBuffer();

    //!         get raw pointer to data
    /*! \return pointer to data, NULL, if buffer is empty
    */
    PCBuffer GetBuffer() const;

    //!         get data size
    /*! \return data size
    */
    TSize GetSize() const;

    //!         get buffer size
    /*! \return buffer size
    */
    TSize GetBufferSize() const;

    //!         get read offset
    /*! \return read offset
    */
    TSize GetROffset() const;

    //!         get write offset
    /*! \return write offset
    */
    TSize GetWOffset() const;
    
    //!         grow buffer
    /*! \param  ulSize - grow size
        \return pointer to newly allocated data
    */
    PBuffer Grow(const TSize ulSize);

    //!         resize data
    /*! \param  ulSize - new data size
    */
    void Resize(const TSize ulSize);

    //!         allocate ulSize bytes for buffer, if needed
    /*! \param  ulSize - new buffer size
    */
    void Reserve(const TSize ulSize);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(char& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(unsigned char& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(short& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(unsigned short& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(int& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(unsigned int& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(long& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(unsigned long& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(float& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(double& tData);  

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(CStringA& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(CStringW& tData);

    //!         serializing operator
    /*! \param  tData - reference to serializing object
        \return *this
    */
    CStreamBuffer& operator>>(CStreamBuffer& tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const char tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const unsigned char tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const short tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const unsigned short tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const int tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const unsigned int tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const long tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const unsigned long tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const float tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const double tData);  

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const CStringA& tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const CStringW& tData);

    //!         deserializing operator
    /*! \param  tData - reference to deserializing object
        \return *this
    */
    CStreamBuffer& operator<<(const CStreamBuffer& tData);

  private:
    PBuffer m_pucBegin;                               //!< pointer to buffer begin
    TSize m_ulROffset;                                //!< read offset
    TSize m_ulWOffset;                                //!< write offset
    TSize m_ulSize;                                   //!< data size
    TSize m_ulBufferSize;                             //!< buffer size
    int   m_nFlags;                                   //!< flags
    CByteOrder::EByteOrder m_eByteOrder;              //!< byte order
    static int m_nFlagsStatic;                        //!< default flags
    static CByteOrder::EByteOrder m_eByteOrderStatic; //!< default byte order
  };

} // namespace rise

#endif // _StreamBuffer_h_
