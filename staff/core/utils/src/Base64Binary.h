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

#ifndef _STAFF_UTILS_BASE64BINARY_H_
#define _STAFF_UTILS_BASE64BINARY_H_

#include <string>
#include "staffutilsexport.h"

namespace staff
{
  typedef char byte;
  class STAFF_UTILS_EXPORT ByteArray;

  //! class Base64Binary
  class STAFF_UTILS_EXPORT Base64Binary
  {
  public:
    //! default constructor
    Base64Binary();

    //! initializing constructor
    /*! encode data while construction
        \param rByteArray - binary data
      */
    Base64Binary(const ByteArray& rByteArray);

    //! initializing constructor
    /*! encode data while construction
        \param pBinaryData - pointer to binary data
        \param ulDataSize - binary data length
      */
    Base64Binary(const byte* pBinaryData, unsigned long ulDataSize);

    //! initializing constructor
    /*! copies encoded data
        \param sEncodedData - encoded data
      */
    Base64Binary(const std::string& sEncodedData);


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
    /*! \return decoded data
      */
    ByteArray GetBytes() const;

    //! decode and get data
    /*! \param rDecodedData - decoded data
      */
    void GetBytes(ByteArray& rDecodedData) const;


    //! get encoded data
    /*! \return encoded data
      */
    const std::string& ToString() const;


    // compatability operators
    operator const std::string&() const;
    operator std::string&();
    operator ByteArray() const;
    Base64Binary& operator=(const std::string& sData);
    Base64Binary& operator=(const ByteArray& rData);

  private:
    std::string m_sEncodedData; //!< encoded data
  };

}

#endif // _STAFF_UTILS_BASE64BINARY_H_
