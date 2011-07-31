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


#ifndef _ABSTRACT_H_
#define _ABSTRACT_H_

#include <typeinfo>
#include <string>
#include <staff/utils/fromstring.h>
#include "staffcommonexport.h"

namespace staff
{

  //! abstract type representation
  template<typename Type>
  class STAFF_COMMON_EXPORT Abstract
  {
  public:
    Abstract():
      m_pType(NULL), m_bOwner(false)
    {
    }

    template<typename DerivedType>
    Abstract(DerivedType* pType):
      m_pType(pType), m_bOwner(true)
    {
    }

    Abstract(const Abstract<Type>& rOther):
      m_pType(rOther.m_pType), m_bOwner(rOther.m_bOwner)
    {
      rOther.m_bOwner = false;
    }

    ~Abstract()
    {
      Destroy();
    }

    inline void Destroy()
    {
      if (m_bOwner)
      {
        delete m_pType;
        m_bOwner = false;
      }
      m_pType = NULL;
    }

    inline Abstract<Type>& operator=(const Abstract<Type>& rOther)
    {
      Destroy();
      m_bOwner = rOther.m_bOwner;
      m_pType = rOther.m_pType;
      rOther.m_bOwner = false;
      return *this;
    }

    inline Abstract<Type>& operator=(Type* pType)
    {
      Destroy();
      m_bOwner = true;
      m_pType = pType;
      return *this;
    }

    inline Abstract<Type>& Set(Type* pType)
    {
      Destroy();
      m_bOwner = true;
      m_pType = pType;
      return *this;
    }

    inline Type* Get()
    {
      return m_pType;
    }

    inline Type* Get() const
    {
      return m_pType;
    }

    inline Type* Release()
    {
      Type* pType = m_pType;
      m_bOwner = false;
      m_pType = NULL;
      return pType;
    }

    inline Type& operator*()
    {
      RISE_ASSERTS(m_pType, "Abstract pointer is NULL");
      return *m_pType;
    }

    inline const Type& operator*() const
    {
      RISE_ASSERTS(m_pType, "Abstract pointer is NULL");
      return *m_pType;
    }

    inline Type* operator->()
    {
      RISE_ASSERTS(m_pType, "Abstract pointer is NULL");
      return m_pType;
    }

    inline const Type* operator->() const
    {
      RISE_ASSERTS(m_pType, "Abstract pointer is NULL");
      return m_pType;
    }

    std::string GetType() const
    {
      RISE_ASSERTS(m_pType, "Abstract pointer is NULL");
      std::string sTypeOut;
      Demangle(typeid(*m_pType).name(), sTypeOut);
      return sTypeOut;
    }

  private:
    static void Demangle(const std::string& sIn, std::string& sOut)
    {
#if defined __GNUC__
      // "N1a3bbb5ChildE"
      static const char* szNumbers = "0123456789";
      std::string::size_type nPos = 0;
      std::string::size_type nPosEnd = 0;
      std::string::size_type nNumLen = 0;
      std::string::size_type nSize = sIn.size();

      if (sIn[nPos] == 'N')
      { // namespace
        ++nPos;
      }

      while (nPos < nSize)
      {
        if (nNumLen > 0)
        {
          sOut += sIn.substr(nPos, nNumLen);
          nPos += nNumLen;
        }
        nPosEnd = sIn.find_first_not_of(szNumbers, nPos);
        if (nPosEnd == nPos) // E
        {
          RISE_ASSERTS(sIn[nPos] == 'E', "Failed to demangle string [" + sIn + "]");
          break;
        }
        if (nNumLen > 0)
        {
          sOut += '.';
        }

        if (nPosEnd != std::string::npos)
        {
          staff::FromString(sIn.substr(nPos, nPosEnd - nPos), nNumLen);
          nPos = nPosEnd;
        }
      }
#elif defined _MSC_VER
      // "struct a::bbb::Child"
      std::string::size_type nPos = sIn.find(' ', 1);
      std::string::size_type nPosEnd = 0;
      sOut.reserve(sIn.size());

      RISE_ASSERTS(nPos != std::string::npos, "Failed to demangle string [" + sIn + "]");

      nPos = sIn.find_first_not_of(' ', nPos);
      RISE_ASSERTS(nPos != std::string::npos, "Failed to demangle string [" + sIn + "]");

      while (nPos != std::string::npos)
      {
        nPosEnd = sIn.find("::", nPos);
        if (!sOut.empty())
        {
          sOut += '.';
        }

        if (nPosEnd == std::string::npos)
        {
          sOut += sIn.substr(nPos);
          break;
        }

        sOut += sIn.substr(nPos, nPosEnd - nPos);
        nPos = nPosEnd + 2;
      }
#else
#error Unsupported compiler
#endif
    }

  private:
    Type* m_pType;
    mutable bool m_bOwner;
  };

}

#endif // _ABSTRACT_H_
