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

#ifndef _STAFF_UTILS_HEXBINARY_H_
#define _STAFF_UTILS_HEXBINARY_H_

#include <string>
#include "staffutilsexport.h"

namespace staff
{
  typedef char byte;
  class STAFF_UTILS_EXPORT ByteArray;

  //! class HexBinary
  class STAFF_UTILS_EXPORT HexBinary
  {
  public:
    //! default constructor
    HexBinary();

    //! initializing constructor
    /*! encode data while construction
        \param rByteArray - binary data
      */
    HexBinary(const ByteArray& rByteArray);

    //! initializing constructor
    /*! encode data while construction
        \param pBinaryData - pointer to binary data
        \param ulDataSize - binary data length
      */
    HexBinary(const byte* pBinaryData, unsigned long ulDataSize);

    //! initializing constructor
    /*! copies encoded data
        \param sEncodedData - encoded data
      */
    HexBinary(const std::string& sEncodedData);


    //! decode data to buffer
    /*! may throw exception if data is in invalid format
        \param sEncodedData - encoded data
        \return decoded data
      */
    static ByteArray Decode(const std::string& sEncodedData);

    //! decode data to buffer
    /*! may throw exception if data is in invalid format
        \param sEncodedData - encoded data
        \param rDecodedData - decoded data output
      */
    static void Decode(const std::string& sEncodedData, ByteArray& rDecodedData);


    //! encode data
    /*! \param rBinaryData - binary data
        \return encoded data
      */
    static std::string Encode(const ByteArray& rBinaryData);

    //! encode data
    /*! \param pBinaryData - binary data
        \param ulDataSize - data size
        \return encoded data
      */
    static std::string Encode(const byte* pBinaryData, unsigned long ulDataSize);

    //! encode data
    /*! \param rBinaryData - binary data
        \param sEncodedData - encoded data output
      */
    static void Encode(const ByteArray& rBinaryData, std::string& sEncodedData);

    //! encode data
    /*! \param pBinaryData - binary data
        \param ulDataSize - data size
        \param sEncodedData - encoded data output
      */
    static void Encode(const byte* pBinaryData, unsigned long ulDataSize, std::string& sEncodedData);


    //! decode and get data
    /*! may throw exception if data is in invalid format
        \return decoded data
      */
    ByteArray GetBytes() const;

    //! decode and get data
    /*! may throw exception if data is in invalid format
        \param rDecodedData - decoded data
      */
    void GetBytes(ByteArray& rDecodedData) const;


    //! get encoded data
    /*! \return encoded data
      */
    const std::string& ToString() const;


    // compatability operators
    operator const std::string&() const;
    // may throw exception if data is in invalid format
    operator ByteArray() const;
    HexBinary& operator=(const std::string& sData);
    HexBinary& operator=(const ByteArray& rData);

  private:
    std::string m_sEncodedData; //!< encoded data
  };

}

#endif // _STAFF_UTILS_HEXBINARY_H_
