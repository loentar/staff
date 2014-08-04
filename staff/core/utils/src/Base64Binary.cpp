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
 *  Please, visit http://code.google.com/p/staff for more information.
 */

#include "Exception.h"
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
    static const byte baDecodeTable[] =
    {
      63, 0, 0, 0, 64, // +    /
      53, 54, 55, 56, 57, 58, 59, 60, 61, 62, // digits
      0, 0, 0, 0, 0, 0, 0,  // garbage
      1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26, // UPPER
      0, 0, 0, 0, 0, 0, // garbage
      27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52 // lower
    };

    unsigned long ulResultingSize = sEncodedData.size() * 3 / 4;

    rDecodedData.Set(ulResultingSize);

    char* pBuffOut = rDecodedData.GetData();
    STAFF_DEBUG_ASSERT(pBuffOut, "Internal error");

    char* pOut = pBuffOut;
    char* pBuffEnd = pOut + ulResultingSize;
    char baTmp[4];
    unsigned char btPos = 0;

    for (const char* szPos = sEncodedData.c_str(); *szPos; ++szPos)
    {
      if (*szPos >= '+' && *szPos <= 'z')
      {
        if (btPos == 4)
        {
          STAFF_DEBUG_ASSERT(pOut <= pBuffEnd + 3, "output buffer too small");
          *pOut = (baTmp[0] << 2 | baTmp[1] >> 4);
          *(++pOut) =   (baTmp[1] << 4 | baTmp[2] >> 2);
          *(++pOut) = (((baTmp[2] << 6) & 0xc0) | baTmp[3]);
          ++pOut;
          btPos = 0;
        }
        if (baDecodeTable[*szPos - '+'] != 0)
        {
          baTmp[btPos] = baDecodeTable[*szPos - '+'] - 1;
          ++btPos;
        }
      }
    }
    if (btPos >= 1)
    {
      STAFF_DEBUG_ASSERT(pOut <= pBuffEnd, "output buffer too small");
      *pOut = (baTmp[0] << 2 | (btPos > 1 ? (baTmp[1] >> 4) : 0));
      ++pOut;
      if (btPos > 2)
      {
        STAFF_DEBUG_ASSERT(pOut <= pBuffEnd + 1, "output buffer too small");
        *pOut = (baTmp[1] << 4 | baTmp[2] >> 2);
        ++pOut;
        if (btPos > 3)
        {
          STAFF_DEBUG_ASSERT(pOut <= pBuffEnd + 2, "output buffer too small");
          *pOut = (((baTmp[2] << 6) & 0xc0) | baTmp[3]);
          ++pOut;
        }
      }
    }

    rDecodedData.SetSize(pOut - pBuffOut);
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
    const unsigned char* pData = reinterpret_cast<const unsigned char*>(pBinaryData);
    static const char sEncodeTable[65] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    STAFF_ASSERT_PARAM(pData);

    unsigned long ulRest = ulDataSize % 3;
    unsigned long ulPos = 0;

    sEncodedData.resize(ulDataSize * 4 / 3 + 3);

    std::string::iterator itRes = sEncodedData.begin();

    for (; ulPos < ulDataSize - ulRest; ulPos += 3)
    {
      *itRes = sEncodeTable[ pData[ulPos] >> 2 ];
      *(++itRes) = sEncodeTable[ ((pData[ulPos] & 0x03) << 4)
          | ((pData[ulPos + 1] & 0xf0) >> 4) ];
      *(++itRes) = sEncodeTable[ ((pData[ulPos + 1] & 0x0f) << 2)
          | ((pData[ulPos + 2] & 0xc0) >> 6) ];
      *(++itRes) = sEncodeTable[ pData[ulPos + 2] & 0x3f ];
      ++itRes;
    }

    if (ulRest > 0)
    {
      *itRes = sEncodeTable[ pData[ulPos] >> 2 ];
      *(++itRes) = sEncodeTable[ ((pData[ulPos] & 0x03) << 4)
          | (ulRest > 1 ? ((pData[ulPos + 1] & 0xf0) >> 4) : 0) ];
      *(++itRes) = ulRest > 1 ? sEncodeTable[ ((pData[ulPos + 1] & 0x0f) << 2)
                                  | (ulRest > 2 ? ((pData[ulPos + 2] & 0xc0) >> 6) : 0) ] : '=';
      *(++itRes) = ulRest > 2 ? sEncodeTable[ pData[ulPos + 2] & 0x3f ] : '=';
    }

    if (ulRest != 0)
    {
      *itRes = '=';
      ++itRes;
    }

    if (itRes != sEncodedData.end())
    {
      sEncodedData.erase(itRes, sEncodedData.end());
    }
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
