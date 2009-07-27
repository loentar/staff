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
#include <rise/common/ExceptionTemplate.h>
#include "DataEncoder.h"

namespace rise
{

  void CBase64Encoder::Encode( const CByteArray& baIn, CStringA& asOut, int nRowWidth /*= 19*/ )
  {
    TSize stRest = baIn.size() % 3;
    TSize stInPos;
    CStringA::iterator iterOut;
    asOut.resize( baIn.size() * 4 / 3 + baIn.size() * 4 / 3 / nRowWidth + stRest + 1 );

    iterOut = asOut.begin();

    for ( stInPos = 0; stInPos < baIn.size() - stRest; stInPos += 3 )
    {
      if ( (stInPos / 3) % nRowWidth == 0 && stInPos != 0 )
      {
        *iterOut = '\n';
        ++iterOut;
      }

      *iterOut     = m_sEncodeTable[   baIn[stInPos + 0] >> 2 ];
      *(++iterOut) = m_sEncodeTable[ ((baIn[stInPos + 0] & 0x03) << 4) | ((baIn[stInPos + 1] & 0xf0) >> 4) ];
      *(++iterOut) = m_sEncodeTable[ ((baIn[stInPos + 1] & 0x0f) << 2) | ((baIn[stInPos + 2] & 0xc0) >> 6) ];
      *(++iterOut) = m_sEncodeTable[   baIn[stInPos + 2] & 0x3f ];
      ++iterOut;
    }

    if (stRest > 0)
    {
      *iterOut = m_sEncodeTable[ baIn[stInPos] >> 2 ];
      *(++iterOut) = m_sEncodeTable[ ((baIn[stInPos] & 0x03) << 4) | (stRest > 1 ? ((baIn[stInPos + 1] & 0xf0) >> 4) : 0) ];
      *(++iterOut) = stRest > 1 ? m_sEncodeTable[ ((baIn[stInPos + 1] & 0x0f) << 2) | (stRest > 2 ? ((baIn[stInPos + 2] & 0xc0) >> 6) : 0) ] : '=';
      *(++iterOut) = stRest > 2 ? m_sEncodeTable[ baIn[stInPos + 2] & 0x3f ] : '=';
      ++iterOut;
    }
    asOut.erase(iterOut, asOut.end());
  }

  void CBase64Encoder::Encode( PCBuffer pBuffIn, TSize tBufferSize, CStringA& sOut, int nRowWidth /*= 19*/ )
  {
    RISE_ASSERTP(pBuffIn);

    TSize stRest = tBufferSize % 3;
    TSize stInPos;
    
    sOut.resize( tBufferSize * 4 / 3 + tBufferSize * 4 / 3 / nRowWidth + stRest + 1 );

    CStringA::iterator iterOut = sOut.begin();

    for ( stInPos = 0; stInPos < tBufferSize - stRest; stInPos += 3 )
    {
      if ( (stInPos / 3) % nRowWidth == 0 && stInPos != 0)
      {
        *iterOut = '\n';
        ++iterOut;
      }

      *iterOut = m_sEncodeTable[ pBuffIn[stInPos] >> 2 ];
      *(++iterOut) = m_sEncodeTable[ ((pBuffIn[stInPos] & 0x03) << 4) | ((pBuffIn[stInPos + 1] & 0xf0) >> 4) ];
      *(++iterOut) = m_sEncodeTable[ ((pBuffIn[stInPos + 1] & 0x0f) << 2) | ((pBuffIn[stInPos + 2] & 0xc0) >> 6) ];
      *(++iterOut) = m_sEncodeTable[ pBuffIn[stInPos + 2] & 0x3f ];
      ++iterOut;
    }

    if (stRest > 0)
    {
      *iterOut = m_sEncodeTable[ pBuffIn[stInPos] >> 2 ];
      *(++iterOut) = m_sEncodeTable[ ((pBuffIn[stInPos] & 0x03) << 4) | (stRest > 1 ? ((pBuffIn[stInPos + 1] & 0xf0) >> 4) : 0) ];
      *(++iterOut) = stRest > 1 ? m_sEncodeTable[ ((pBuffIn[stInPos + 1] & 0x0f) << 2) | (stRest > 2 ? ((pBuffIn[stInPos + 2] & 0xc0) >> 6) : 0) ] : '=';
      *(++iterOut) = stRest > 2 ? m_sEncodeTable[ pBuffIn[stInPos + 2] & 0x3f ] : '=';
      ++iterOut;
    }
    sOut.erase(iterOut, sOut.end());
  }

  void CBase64Encoder::Decode( const CStringA& asIn, CByteArray& baOut )
  {
    TSize stBlocks = asIn.size() / 4;
    CByteArray::iterator iterOut;
    TCharA baTmp[4];
    byte bPos = 0;
    
    baOut.resize( 3 * stBlocks );
    iterOut = baOut.begin();

    for ( CStringA::const_iterator iterIn = asIn.begin(); iterIn != asIn.end(); ++iterIn )
    {
      if( *iterIn >= '+' && *iterIn <= 'z' )
      {
        if ( bPos == 4 )
        {
          *iterOut = (baTmp[0] << 2 | baTmp[1] >> 4);
          *(++iterOut) =   (baTmp[1] << 4 | baTmp[2] >> 2);
          *(++iterOut) = (((baTmp[2] << 6) & 0xc0) | baTmp[3]);
          ++iterOut;
          bPos = 0;
        }
        if ( m_baDecodeTable[*iterIn - '+'] != 0 )
        {
          baTmp[bPos] = m_baDecodeTable[*iterIn - '+'] - 1;
          ++bPos;
        }
      }
    }
    if ( bPos >= 1 )
    {
      *iterOut = (baTmp[0] << 2 | (bPos > 1 ? (baTmp[1] >> 4) : 0) );
      ++iterOut;
      if ( bPos > 2 )
      {
        *iterOut =   (baTmp[1] << 4 | baTmp[2] >> 2);
        ++iterOut;
        if ( bPos > 3 )
        {
          *iterOut = (((baTmp[2] << 6) & 0xc0) | baTmp[3]);
          ++iterOut;
        }
      }
    }
    baOut.erase(iterOut, baOut.end());
  }

  void CBase64Encoder::Decode( const CStringA& asIn, PBuffer pBuffOut, TSize& tBufferSize )
  {
    RISE_ASSERTP(pBuffOut);
    PBuffer pOut = pBuffOut;
    PBuffer pBuffEnd = pBuffOut + tBufferSize;
    TCharA baTmp[4];
    byte bPos = 0;

    for ( CStringA::const_iterator iterIn = asIn.begin(); iterIn != asIn.end(); ++iterIn )
    {
      if( *iterIn >= '+' && *iterIn <= 'z' )
      {
        if ( bPos == 4 )
        {
          RISE_ASSERTES(pOut <= pBuffEnd + 3, CLogicSizeException, "output buffer too small");
          *pOut = (baTmp[0] << 2 | baTmp[1] >> 4);
          *(++pOut) =   (baTmp[1] << 4 | baTmp[2] >> 2);
          *(++pOut) = (((baTmp[2] << 6) & 0xc0) | baTmp[3]);
          ++pOut;
          bPos = 0;
        }
        if ( m_baDecodeTable[*iterIn - '+'] != 0 )
        {
          baTmp[bPos] = m_baDecodeTable[*iterIn - '+'] - 1;
          ++bPos;
        }
      }
    }
    if ( bPos >= 1 )
    {
      RISE_ASSERTES(pOut <= pBuffEnd, CLogicSizeException, "output buffer too small");
      *pOut = (baTmp[0] << 2 | (bPos > 1 ? (baTmp[1] >> 4) : 0) );
      ++pOut;
      if ( bPos > 2 )
      {
        RISE_ASSERTES(pOut <= pBuffEnd + 1, CLogicSizeException, "output buffer too small");
        *pOut =   (baTmp[1] << 4 | baTmp[2] >> 2);
        ++pOut;
        if ( bPos > 3 )
        {
          RISE_ASSERTES(pOut <= pBuffEnd + 2, CLogicSizeException, "output buffer too small");
          *pOut = (((baTmp[2] << 6) & 0xc0) | baTmp[3]);
          ++pOut;
        }
      }
    }
    tBufferSize = static_cast<TSize>(pOut - pBuffOut);
  }

  const CStringA CBase64Encoder::m_sEncodeTable = 
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  const byte CBase64Encoder::m_baDecodeTable[] = 
  {
    63, 0, 0, 0, 64, // +    /
    53, 54, 55, 56, 57, 58, 59, 60, 61, 62, // digits
    0, 0, 0, 0, 0, 0, 0,  // garbage
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26, // UPPER
    0, 0, 0, 0, 0, 0, // garbage
    27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52 // lower
  };

} // namespace rise
