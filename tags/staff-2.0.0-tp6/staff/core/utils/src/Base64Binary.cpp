/*
 *  Copyright 2011 Utkin Dmitry
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
 */

#include <rise/common/StreamBuffer.h>
#include <rise/common/DataEncoder.h>
#include <rise/common/ExceptionTemplate.h>
#include "ByteArray.h"
#include "Base64Binary.h"

namespace staff
{

  Base64Binary::Base64Binary()
  {
  }

  Base64Binary::Base64Binary(const ByteArray& rByteArray)
  {
    Encode(rByteArray, m_sEncodedData);
  }

  Base64Binary::Base64Binary(const byte* pBinaryData, unsigned long ulDataSize)
  {
    Encode(pBinaryData, ulDataSize, m_sEncodedData);
  }

  Base64Binary::Base64Binary(const std::string& sEncodedData)
  {
    m_sEncodedData = sEncodedData;
  }


  ByteArray Base64Binary::Decode(const std::string& sEncodedData)
  {
    ByteArray baResult;
    Decode(sEncodedData, baResult);
    return baResult;
  }

  void Base64Binary::Decode(const std::string& sEncodedData, ByteArray& rDecodedData)
  {
    rise::CStreamBuffer sbDecodedData;
    rise::CBase64Encoder::Decode(sEncodedData, sbDecodedData);
    rDecodedData.Set(sbDecodedData.GetSize());
    sbDecodedData.Get(reinterpret_cast<rise::PBuffer>(rDecodedData.GetData()), rDecodedData.GetSize());
  }


  std::string Base64Binary::Encode(const ByteArray& rBinaryData)
  {
    std::string sResult;
    Encode(rBinaryData.GetData(), rBinaryData.GetSize(), sResult);
    return sResult;
  }

  std::string Base64Binary::Encode(const byte* pBinaryData, unsigned long ulDataSize)
  {
    std::string sResult;
    Encode(pBinaryData, ulDataSize, sResult);
    return sResult;
  }

  void Base64Binary::Encode(const ByteArray& rBinaryData, std::string& sEncodedData)
  {
    Encode(rBinaryData.GetData(), rBinaryData.GetSize(), sEncodedData);
  }

  void Base64Binary::Encode(const byte* pBinaryData, unsigned long ulDataSize, std::string& sEncodedData)
  {
    rise::CBase64Encoder::Encode(reinterpret_cast<rise::PCBuffer>(pBinaryData), ulDataSize, sEncodedData);
  }


  ByteArray Base64Binary::GetBytes() const
  {
    ByteArray baResult;
    Decode(m_sEncodedData, baResult);
    return baResult;
  }

  void Base64Binary::GetBytes(ByteArray& rDecodedData) const
  {
    Decode(m_sEncodedData, rDecodedData);
  }


  const std::string& Base64Binary::ToString() const
  {
    return m_sEncodedData;
  }


  Base64Binary::operator const std::string&() const
  {
    return m_sEncodedData;
  }

  Base64Binary::operator std::string&()
  {
    return m_sEncodedData;
  }

  Base64Binary::operator ByteArray() const
  {
    ByteArray baResult;
    Decode(m_sEncodedData, baResult);
    return baResult;
  }

  Base64Binary& Base64Binary::operator=(const std::string& sData)
  {
    m_sEncodedData = sData;
    return *this;
  }

  Base64Binary& Base64Binary::operator=(const ByteArray& rData)
  {
    Encode(rData.GetData(), rData.GetSize(), m_sEncodedData);
    return *this;
  }

}
