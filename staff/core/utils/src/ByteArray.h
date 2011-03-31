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

#ifndef _STAFF_UTILS_BYTEARRAY_H_
#define _STAFF_UTILS_BYTEARRAY_H_

#include <string>
#include "staffutilsexport.h"

namespace staff
{
  typedef char byte;

  //! class ByteArray
  class STAFF_UTILS_EXPORT ByteArray
  {
  public:
    //! default constructor
    ByteArray();

    //! construct object and allocate new buffer
    /*! \param ulDataSize - size of new buffer
      */
    ByteArray(unsigned long ulDataSize);

    //! construct object and set buffer
    /*! \param pBinaryData - pointer to already allocated data
        \param ulDataSize - data size
        \param bOwner - set ByteArray as owner to this buffer (owner frees data when destruct)
      */
    ByteArray(byte* pBinaryData, unsigned long ulDataSize, bool bOwner = true);

    //! allocate new buffer
    /*! \param ulDataSize - size of new buffer
      */
    void Set(unsigned long ulDataSize);

    //! set buffer
    /*! \param pBinaryData - pointer to already allocated data
        \param ulDataSize - data size
        \param bOwner - set ByteArray as owner to this buffer (owner frees data when destruct)
      */
    void Set(byte* pBinaryData, unsigned long ulDataSize, bool bOwner = true);

    //! set ByteArray as owner to this buffer (owner frees data when destruct)
    /*! \param bOwner - true - set as owner, false - unset
      */
    void SetOwner(bool bOwner = true);

    //! get ByteArray is owner or not
    /*! \return object is set as owner
      */
    bool IsOwner();

    //! release pointer
    /*! delete buffer, if owner
      */
    void Release();

    //! get stored data
    /*! \return stored data
      */
    byte* GetData();

    //! get stored data (const)
    /*! \return stored data (const)
      */
    const byte* GetData() const;

    //! get stored data size
    /*! \return stored data size
      */
    unsigned long GetSize() const;

    //! operator []
    /*! may throw
        \param ulPos - byte position
        \return ref to byte
      */
    byte& operator[](unsigned long ulPos);

    //! operator [] (const)
    /*! may throw
        \param ulPos - byte position
        \return byte
      */
    byte operator[](unsigned long ulPos) const;

    //! owner pass operator
    /*! set current object as owner, if other object was owner
        unset other object as owner
        \param rByteArray - other byte array
        \return reference to current object
      */
    ByteArray& operator=(ByteArray& rByteArray);

    //! operator ! (const)
    /*! \return true if ByteArray is not initialized
      */
    bool operator!() const;

  private:
    byte* m_pBinaryData; //!< data
    unsigned long m_ulDataSize; //!< data size
    bool m_bOwner; //!< is byte array owner
  };
}

#endif // _STAFF_UTILS_BYTEARRAY_H_
