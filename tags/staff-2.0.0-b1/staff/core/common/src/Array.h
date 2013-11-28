/*
 *  Copyright 2012 Utkin Dmitry
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


#ifndef _STAFF_COMMON_ARRAY_H_
#define _STAFF_COMMON_ARRAY_H_

#include <vector>
#include <string>
#include <staff/utils/tocstring.h>
#include <staff/utils/fromcstring.h>
#include <staff/utils/SharedPtr.h>

namespace staff
{

  //! SOAP Array implementation
  template <typename Type>
  class Array
  {
  public:
    //! constructor
    /*! creates empty array
     */
    Array()
    {
    }

    //! constructor
    /*! creates simple 1D array of given size
        \param  nSize - size of array
      */
    Array(unsigned nSize):
      m_vData(nSize)
    {
    }

    //! constructor
    /*! creates 2D array of given size
        \param  nSize1 - size of array
        \param  nSize2 - size of array
      */
    Array(unsigned nSize1, unsigned nSize2)
    {
      Resize(nSize1, nSize2);
    }

    //! constructor
    /*! creates 3D array of given size
        \param  nSize1 - size of array
        \param  nSize2 - size of array
        \param  nSize3 - size of array
      */
    Array(unsigned nSize1, unsigned nSize2, unsigned nSize3)
    {
      Resize(nSize1, nSize2, nSize3);
    }

    //! constructor
    /*! creates multidimensional array of given size
      */
    template <int nSize>
    Array(const unsigned (&anDimensions)[nSize])
    {
      Resize(anDimensions);
    }

    //! destructor
    virtual ~Array()
    {
    }


    //! access to 1D array element
    /*! if nPos is greater than array's size exception will thrown
        if array's element is empty it will be created
        \param  nPos - position
        \return reference to array item
      */
    Type& operator[](unsigned nPos)
    {
      STAFF_ASSERT(nPos < m_vData.size(), "Array index out of bounds");

      PtrType& rpType = m_vData[nPos];
      if (!rpType)
      {
        rpType = new Type;
      }

      return *rpType;
    }

    //! read-only access to 1D array element
    /*! if nPos is greater than array's size or array's element is empty
        exception will thrown
        \param  nPos - position
        \return const reference to array item
      */
    const Type& operator[](unsigned nPos) const
    {
      STAFF_ASSERT(nPos < m_vData.size(), "Array index out of bounds");

      const PtrType& rpType = m_vData[nPos];
      STAFF_ASSERT(rpType, "Array item is empty!");

      return *rpType;
    }

    //! access to 1D array element
    /*! if nPos is greater than array's size exception will thrown
        if array's element is empty it will be created
        \param  nPos - position
        \return reference to array item
      */
    Type& At(unsigned nPos)
    {
      return operator[](nPos);
    }

    //! read-only access to 1D array element
    /*! if nPos is greater than array's size or array's element is empty
        exception will thrown
        \param  nPos - position
        \return const reference to array item
      */
    const Type& At(unsigned nPos) const
    {
      return operator[](nPos);
    }

    //! access to 2D array element
    /*! \param  nPos1 - position
        \param  nPos2 - position
        \return reference to array item
      */
    Type& At(unsigned nPos1, unsigned nPos2)
    {
      STAFF_ASSERT(m_vDimensions.size() == 2, "Invalid array dimensions");
      return operator[](nPos1 * m_vDimensions[1] + nPos2);
    }

    //! read-only access to 2D array element
    /*! \param  nPos1 - position
        \param  nPos2 - position
        \return const reference to array item
      */
    const Type& At(unsigned nPos1, unsigned nPos2) const
    {
      STAFF_ASSERT(m_vDimensions.size() == 2, "Invalid array dimensions");
      return operator[](nPos1 * m_vDimensions[1] + nPos2);
    }

    //! access to 3D array element
    /*! \param  nPos1 - position
        \param  nPos2 - position
        \param  nPos3 - position
        \return reference to array item
      */
    Type& At(unsigned nPos1, unsigned nPos2, unsigned nPos3)
    {
      STAFF_ASSERT(m_vDimensions.size() == 3, "Invalid array dimensions");
      return operator[]((nPos1 * m_vDimensions[1] + nPos2) * m_vDimensions[2] + nPos3);
    }

    //! read-only access to 3D array element
    /*! \param  nPos1 - position
        \param  nPos2 - position
        \param  nPos3 - position
        \return const reference to array item
      */
    const Type& At(unsigned nPos1, unsigned nPos2, unsigned nPos3) const
    {
      STAFF_ASSERT(m_vDimensions.size() == 3, "Invalid array dimensions");
      return operator[]((nPos1 * m_vDimensions[2] + nPos2) * m_vDimensions[2] + nPos3);
    }

    //! access to multidimensional array element
    /*! \param  anPos - position
        \return reference to array item
      */
    template <int nSize>
    Type& At(const unsigned (&anPos)[nSize])
    {
      STAFF_ASSERT(nSize > 1, "Invalid dimension size");
      STAFF_ASSERT(m_vDimensions.size() == nSize, "Invalid array dimensions");

      unsigned nFlatPos = anPos[0];

      for (int nPos = 1; nPos < nSize; ++nPos)
      {
        nFlatPos = nFlatPos * m_vDimensions[nPos] + anPos[nPos];
      }

      return operator[](nFlatPos);
    }

    //! read-only access to multidimensional array element
    /*! \param  anPos - position
        \return const reference to array item
      */
    template <int nSize>
    const Type& At(const unsigned (&anPos)[nSize]) const
    {
      STAFF_ASSERT(nSize > 1, "Invalid dimension size");
      STAFF_ASSERT(m_vDimensions.size() == nSize, "Invalid array dimensions");

      unsigned nFlatPos = anPos[0];

      for (int nPos = 1; nPos < nSize; ++nPos)
      {
        nFlatPos = nFlatPos * m_vDimensions[nPos - 1] + anPos[nPos];
      }

      return operator[](nFlatPos);
    }

    //! access to multidimensional array element
    /*! \param  sPos - position in SOAP notation
        \return const reference to array item
      */
    Type& At(const std::string& sPos)
    {
      STAFF_ASSERT(sPos.size() > 2 && sPos[0] == '['
                   && sPos[sPos.size() - 1] == ']', "Invalid dimension string");

      std::string::size_type nStart = 1;
      std::string::size_type nEnd = sPos.find(',', nStart);
      unsigned nFlatPos = 0;

      if (nEnd == std::string::npos)
      {
        FromCString(sPos.substr(1, sPos.size() - 2).c_str(), nFlatPos);
      }
      else
      {
        unsigned nPos = 0;
        unsigned nIndex = 0;

        FromCString(sPos.substr(nStart, nEnd - nStart).c_str(), nFlatPos);
        nStart = nEnd + 1;

        while ((nEnd = sPos.find(',', nStart)) != std::string::npos)
        {
          FromCString(sPos.substr(nStart, nEnd - nStart).c_str(), nPos);
          nFlatPos = nFlatPos * m_vDimensions[nIndex] + nPos;
          nStart = nEnd + 1;
          ++nIndex;
        }
        FromCString(sPos.substr(nStart, sPos.size() - nStart - 1).c_str(), nPos);
        nFlatPos = nFlatPos * m_vDimensions[nIndex] + nPos;
      }

      return operator[](nFlatPos);
    }

    //! is array element empty?
    /*! \param  nPos - position
        \return true if element at array position is empty
      */
    bool IsEmpty(unsigned nPos) const
    {
      return (nPos >= m_vData.size()) || !m_vData[nPos];
    }

    //! sets array's element to empty
    /*! \param  nPos - position
      */
    void SetEmpty(unsigned nPos)
    {
      // increase size of sparse array
      if (m_vData.size() < nPos)
      {
        m_vData.resize(nPos);
      }
      else
      {
        m_vData[nPos].Free();
      }
    }

    //! is array empty (size of array = 0)?
    /*! \return true, if array is empty
      */
    bool IsEmpty() const
    {
      return m_vData.empty();
    }

    //! get size of array
    /*! \return size of array
      */
    unsigned GetSize() const
    {
      return m_vData.size();
    }

    //! get array dimensions
    /*! can be empty while working with 1D array
        \return array dimensions
      */
    const std::vector<unsigned>& GetDimensions() const
    {
      return m_vDimensions;
    }

    //! resize 1D array
    /*! \param  nSize - new array size
      */
    void Resize(unsigned nSize)
    {
      m_vDimensions.clear();
      m_vData.resize(nSize);
    }

    //! resize 2D array
    /*! \param  nSize1 - size
        \param  nSize2 - size
        \return reference to array item
      */
    void Resize(unsigned nSize1, unsigned nSize2)
    {
      m_vDimensions.resize(2);
      m_vDimensions[0] = nSize1;
      m_vDimensions[1] = nSize2;

      m_vData.resize(nSize1 * nSize2);
    }

    //! resize 3D array
    /*! \param  nSize1 - size
        \param  nSize2 - size
        \param  nSize3 - size
        \return reference to array item
      */
    void Resize(unsigned nSize1, unsigned nSize2, unsigned nSize3)
    {
      m_vDimensions.resize(3);
      m_vDimensions[0] = nSize1;
      m_vDimensions[1] = nSize2;
      m_vDimensions[2] = nSize3;

      m_vData.resize(nSize1 * nSize2 * nSize3);
    }

    //! resize multidimensional array
    /*! \param  anDimensions - new multidimensional array dimensions
      */
    template <int nSize>
    void Resize(const unsigned (&anDimensions)[nSize])
    {
      STAFF_ASSERT(nSize > 1, "Invalid dimension size");

      unsigned nDataSize = anDimensions[0];

      m_vDimensions.resize(nSize);
      m_vDimensions[0] = anDimensions[0];
      for (int nPos = 1; nPos < nSize; ++nPos)
      {
        m_vDimensions[nPos] = anDimensions[nPos];
        nDataSize *= anDimensions[nPos];
      }
      m_vData.resize(nDataSize);
    }

    //! resize array using soap notation
    /*! \param  sDimensions - new array dimensions
      */
    void Resize(const std::string& sDimensions)
    {
      STAFF_ASSERT(sDimensions.size() > 2 && sDimensions[0] == '['
                   && sDimensions[sDimensions.size() - 1] == ']', "Invalid dimension string");
      m_vDimensions.clear();

      std::string::size_type nStart = 1;
      std::string::size_type nEnd = nStart;
      unsigned nDataSize = 1;
      unsigned nDimension = 0;

      while ((nEnd = sDimensions.find(',', nStart)) != std::string::npos)
      {
        FromCString(sDimensions.substr(nStart, nEnd - nStart).c_str(), nDimension);
        m_vDimensions.push_back(nDimension);
        nDataSize *= nDimension;
        nStart = nEnd + 1;
      }
      FromCString(sDimensions.substr(nStart, sDimensions.size() - nStart - 1).c_str(), nDimension);
      if (!m_vDimensions.empty())
      {
        m_vDimensions.push_back(nDimension);
      }
      nDataSize *= nDimension;
      m_vData.resize(nDataSize);
    }

    //! reserve nSize items in array
    /*! \param  nSize - size
      */
    void Reserve(unsigned nSize)
    {
      m_vData.reserve(nSize);
    }

    //! is array sparse?
    /*! \return true if array is a sparse array
      */
    bool IsSparse() const
    {
      for (unsigned nPos = 0, nSize = m_vData.size(); nPos < nSize; ++nPos)
      {
        if (!m_vData[nPos])
        {
          return true;
        }
      }
      return false;
    }

    //! clear array
    void Clear()
    {
      m_vDimensions.clear();
      m_vData.clear();
    }

    //! get array dimensions as string
    /*! \return array dimensions
      */
    std::string GetDimensionsStr() const
    {
      std::string sDimensions;
      if (m_vDimensions.empty())
      {
        sDimensions = ToString(m_vData.size());
      }
      else
      {
        char szBuffer[32];
        ToCString(m_vDimensions[0], szBuffer, sizeof(szBuffer));
        sDimensions = szBuffer;
        for (unsigned nPos = 1; nPos < m_vDimensions.size(); ++nPos)
        {
          ToCString(m_vDimensions[nPos], szBuffer, sizeof(szBuffer));
          sDimensions.append(1, ',').append(szBuffer);
        }
      }

      return "[" + sDimensions + "]";
    }

    //! get position as string
    /*! \param  nPos - position
        \return position
      */
    std::string GetPositionStr(unsigned nPos) const
    {
      char szBuffer[32];
      std::string sPosition;
      if (m_vDimensions.empty())
      {
        ToCString(nPos, szBuffer, sizeof(szBuffer));
        sPosition = szBuffer;
      }
      else
      {
        unsigned nDimension = 0;
        unsigned nPosition = 0;
        for (unsigned nIndex = m_vDimensions.size() - 1;; --nIndex)
        {
          nDimension = m_vDimensions[nIndex];
          nPosition = nPos % nDimension;
          nPos = (nPos - nPosition) / nDimension;
          ToCString(nPosition, szBuffer, sizeof(szBuffer));
          sPosition.insert(0, szBuffer);
          if (!nIndex)
          {
            break;
          }
          sPosition.insert(0, 1, ',');
        }
      }

      return "[" + sPosition + "]";
    }

  private:
    typedef SharedPtr<Type> PtrType;
    std::vector<PtrType> m_vData;
    std::vector<unsigned> m_vDimensions;
  };

}

#endif // _STAFF_COMMON_ARRAY_H_
