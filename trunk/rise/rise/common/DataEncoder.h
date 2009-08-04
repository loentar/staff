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

#ifndef _DATAENCODER_H_
#define _DATAENCODER_H_

#include <rise/common/containertypes.h>
#include <rise/os/osdllexport.h>
#include <rise/string/String.h>

namespace rise
{
  //! base64 encoder/decoder
  class RISE_EXPORT CBase64Encoder
  {
  public:
    //!         encode byte array to base64
    /*! \param  baIn - source array
        \param  asOut - resulting base64 string
        \param  nRowWidth - row width
      */
    static void Encode(const CByteArray& baIn, CStringA& asOut, int nRowWidth = 19);

    //!         decode base64 string to byte array
    /*! \param  asIn - source base64 string
        \param  baOut - resulting byte array
      */
    static void Decode(const CStringA& asIn, CByteArray& baOut);


    //!         encode byte buffer to base64 string
    /*
        \param  pBuffIn - source buffer
        \param  tBufferSize - buffer size in bytes
        \param  asOut - resulting base64 string
        \param  nRowWidth - row width
      */
    static void Encode(PCBuffer pBuffIn, TSize tBufferSize, CStringA& asOut, int nRowWidth = 19); // throw

    //!         encode base64 string to byte buffer 
    /*! \param  asIn - source base64 string
        \param  pBuffOut - resulting buffer
        \param  tBufferSize - (i/o)buffer size in bytes
      */
    static void Decode(const CStringA& asIn, PBuffer pBuffOut, TSize& tBufferSize); // throw
  
  private:
    static const CStringA m_sEncodeTable;             //!< encoding table
    static const byte m_baDecodeTable['z' - '+' + 1]; //!< decoding table
  };
}

#endif // _DATAENCODER_H_
