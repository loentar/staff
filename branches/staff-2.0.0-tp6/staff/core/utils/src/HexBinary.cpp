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

#include <rise/common/ExceptionTemplate.h>
#include "ByteArray.h"
#include "HexBinary.h"

namespace staff
{

  HexBinary::HexBinary()
  {
  }

  HexBinary::HexBinary(const ByteArray& rByteArray)
  {
    Encode(rByteArray, m_sEncodedData);
  }

  HexBinary::HexBinary(const byte* pBinaryData, unsigned long ulDataSize)
  {
    Encode(pBinaryData, ulDataSize, m_sEncodedData);
  }

  HexBinary::HexBinary(const std::string& sEncodedData)
  {
    m_sEncodedData = sEncodedData;
  }


  ByteArray HexBinary::Decode(const std::string& sEncodedData)
  {
    ByteArray baResult;
    Decode(sEncodedData, baResult);
    return baResult;
  }

  inline byte HexToByte(char chHex)
  {
    if (chHex >= '0' && chHex <= '9')
    {
      return (chHex - '0');
    }
    else
    if (chHex >= 'A' && chHex <= 'F')
    {
      return (chHex - 'A' + 0x0a);
    }
    else
    if (chHex >= 'a' && chHex <= 'f')
    {
      return (chHex - 'a' + 0x0a);
    }

    RISE_THROWS(rise::CLogicFormatException, "Invalid char [" + std::string(chHex, 1) + "] in hex string");
  }

  void HexBinary::Decode(const std::string& sEncodedData, ByteArray& rDecodedData)
  {
    RISE_ASSERTS(!(sEncodedData.size() % 2), "buffer has odd size");

    const unsigned long ulEncodedDataSize = sEncodedData.size();

    const unsigned long ulDecodedDataSize = ulEncodedDataSize / 2;
    rDecodedData.Set(ulDecodedDataSize);
    byte* pDecodedData = rDecodedData.GetData();
    const char* szEncodedData = sEncodedData.c_str();

    for (unsigned long ulIndex = 0; ulIndex < ulDecodedDataSize; ++ulIndex, ++pDecodedData)
    {
      *pDecodedData = HexToByte(*szEncodedData) << 4;
      ++szEncodedData;
      *pDecodedData |= HexToByte(*szEncodedData);
      ++szEncodedData;
    }
  }


  std::string HexBinary::Encode(const ByteArray& rBinaryData)
  {
    std::string sResult;
    Encode(rBinaryData.GetData(), rBinaryData.GetSize(), sResult);
    return sResult;
  }

  std::string HexBinary::Encode(const byte* pBinaryData, unsigned long ulDataSize)
  {
    std::string sResult;
    Encode(pBinaryData, ulDataSize, sResult);
    return sResult;
  }

  void HexBinary::Encode(const ByteArray& rBinaryData, std::string& sEncodedData)
  {
    Encode(rBinaryData.GetData(), rBinaryData.GetSize(), sEncodedData);
  }

#define STAFF_BIN_TO_HEX(data) ((data > 0x09) ? ('A' + data - 0x0A) : (data + '0'))

  void HexBinary::Encode(const byte* pBinaryData, unsigned long ulDataSize, std::string& sEncodedData)
  {
    byte btTmp;
    RISE_ASSERTP(pBinaryData);
    sEncodedData.resize(ulDataSize * 2);
    for (unsigned long ulIndex = 0; ulIndex < (ulDataSize * 2); ulIndex += 2, ++pBinaryData)
    {
      btTmp = ((*pBinaryData) >> 4) & 0x0f;
      sEncodedData[ulIndex] = STAFF_BIN_TO_HEX(btTmp);
      btTmp = (*pBinaryData) & 0x0f;
      sEncodedData[ulIndex + 1] = STAFF_BIN_TO_HEX(btTmp);
    }
  }


  ByteArray HexBinary::GetBytes() const
  {
    ByteArray baResult;
    Decode(m_sEncodedData, baResult);
    return baResult;
  }

  void HexBinary::GetBytes(ByteArray& rDecodedData) const
  {
    Decode(m_sEncodedData, rDecodedData);
  }


  const std::string& HexBinary::ToString() const
  {
    return m_sEncodedData;
  }


  HexBinary::operator const std::string&() const
  {
    return m_sEncodedData;
  }

  HexBinary::operator std::string&()
  {
    return m_sEncodedData;
  }

  HexBinary::operator ByteArray() const
  {
    ByteArray baResult;
    Decode(m_sEncodedData, baResult);
    return baResult;
  }

  HexBinary& HexBinary::operator=(const std::string& sData)
  {
    m_sEncodedData = sData;
    return *this;
  }

  HexBinary& HexBinary::operator=(const ByteArray& rData)
  {
    Encode(rData.GetData(), rData.GetSize(), m_sEncodedData);
    return *this;
  }

}
