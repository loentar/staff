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

#include <string>
#include <staff/utils/Reflector.h>
#include "Exception.h"

namespace staff
{

  //! abstract type representation
  template<typename Type>
  class Abstract
  {
  public:
    inline Abstract():
      m_pType(NULL), m_bOwner(false)
    {
    }

    template<typename DerivedType>
    inline Abstract(DerivedType* pType):
      m_pType(pType), m_bOwner(true)
    {
    }

    inline Abstract(const Abstract<Type>& rOther):
      m_pType(rOther.m_pType), m_bOwner(rOther.m_bOwner)
    {
      rOther.m_bOwner = false;
    }

    inline ~Abstract()
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
      STAFF_ASSERT(m_pType, "Abstract pointer is NULL");
      return *m_pType;
    }

    inline const Type& operator*() const
    {
      STAFF_ASSERT(m_pType, "Abstract pointer is NULL");
      return *m_pType;
    }

    inline Type* operator->()
    {
      STAFF_ASSERT(m_pType, "Abstract pointer is NULL");
      return m_pType;
    }

    inline const Type* operator->() const
    {
      STAFF_ASSERT(m_pType, "Abstract pointer is NULL");
      return m_pType;
    }

    inline std::string GetType() const
    {
      return Reflector::GetTypeName(m_pType);
    }

  private:
    Type* m_pType;
    mutable bool m_bOwner;
  };

}

#endif // _ABSTRACT_H_
