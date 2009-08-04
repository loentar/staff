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

#ifndef _SHAREDPTR_HPP_
#define _SHAREDPTR_HPP_

#include "SharedPtr.h"

namespace rise
{
  template<class Type>
    CSharedPtr<Type>::CSharedPtr(Type* pObject):
    m_pObject(pObject)
  {
    try
    {
      m_pCount = new threading::CAtomicCount(pObject == NULL ? 0 : 1);
    } catch (...) 
    {
      delete m_pCount;
      throw;
    }
  }

  template<class Type>
  CSharedPtr<Type>::CSharedPtr():
    m_pObject(NULL)
  {
    try
    {
      m_pCount = new threading::CAtomicCount;
    } catch (...) 
    {
      delete m_pCount;
      throw;
    }
  }

  template<class Type>
    CSharedPtr<Type>::CSharedPtr(const CSharedPtr<Type>& rSharedPtr):
    m_pObject(rSharedPtr.m_pObject), m_pCount(rSharedPtr.m_pCount)
  {
    ++*m_pCount;
  }
  
  template<class Type>
    CSharedPtr<Type>::~CSharedPtr() /*throw()*/
  {
    if ( --*m_pCount <= 0 )
    {
      try {
        delete m_pCount;
      } catch (...) {}
      
      if (m_pObject)
        try 
        {
          delete m_pObject;
        } catch (...) {}
    }  
  }

  template<class Type>
    CSharedPtr<Type>& CSharedPtr<Type>::operator=(CSharedPtr<Type>& rOtherPtr) throw()
  {
    if (rOtherPtr.m_pObject == m_pObject)
      return *this;

    if ( --*m_pCount <= 0 )
    {
      delete m_pCount;
      if (m_pObject)
        delete m_pObject;
    }

    m_pObject = rOtherPtr.m_pObject;
    m_pCount = rOtherPtr.m_pCount;

    ++*m_pCount;
    return *this;
  }

  template<class Type> template<class OtherType>
    CSharedPtr<Type>& CSharedPtr<Type>::operator=(CSharedPtr<OtherType>& rOtherPtr) throw()
  {
    if (static_cast<Type*>(rOtherPtr.m_pObject) == m_pObject)
      return *this;

    if ( --*m_pCount <= 0 )
    {
      delete m_pCount;
      if (m_pObject)
        delete m_pObject;
    }

    m_pObject = static_cast<Type*>(rOtherPtr.m_pObject);
    m_pCount = rOtherPtr.m_pCount;

    ++*m_pCount;
    return *this;
  }

  template<class Type>
    CSharedPtr<Type>& CSharedPtr<Type>::operator=(Type* pObject) throw()
    {
      if (pObject == m_pObject)
        return *this;

      if ( IsUnique() && m_pObject != NULL )
        delete m_pObject;
      else
        m_pCount = new threading::CAtomicCount(pObject == NULL ? 0 : 1);

      m_pObject = pObject;
      return *this;
  }

  template<class Type>
    template<class OtherType>
    CSharedPtr<Type>& CSharedPtr<Type>::operator=(OtherType* pObject) throw()
  {
    if (static_cast<Type*>(pObject) == m_pObject)
      return *this;

    if ( IsUnique() && m_pObject != NULL )
      delete m_pObject;
    else
      m_pCount = new threading::CAtomicCount(pObject == NULL ? 0 : 1);

    m_pObject = static_cast<Type*>(pObject);
    return *this;
  }

  template<class Type>
    Type& CSharedPtr<Type>::operator*()
  {
    return *GetPointer();
  }

  template<class Type>
    const Type& CSharedPtr<Type>::operator*() const
  {
    return *GetPointer();
  }
  
  template<class Type>
    Type* CSharedPtr<Type>::operator->() throw()
  {
    return GetPointer();
  }
  
  template<class Type>
    const Type* CSharedPtr<Type>::operator->() const throw()
  {
    return GetPointer();
  }

  template<class Type>
    CSharedPtr<Type>::operator Type&()
  {
    return static_cast<Type&>(*m_pObject);
  }

  template<class Type>
    CSharedPtr<Type>::operator const Type&() const
  {
    return static_cast<Type&>(*m_pObject);
  }
  
  template<class Type>
    CSharedPtr<Type>::operator Type*()
  {
    return m_pObject;
  }
  
  template<class Type>
    CSharedPtr<Type>::operator const Type*() const
  {
    return m_pObject;
  }

  template<class Type>
    Type* CSharedPtr<Type>::GetPointer() throw()
  {
    return m_pObject;
  }

  template<class Type>
    const Type* CSharedPtr<Type>::GetPointer() const throw()
  {
    return m_pObject;
  }
  
  template<class Type>
    long CSharedPtr<Type>::GetRefCount() const throw()
  {
    return *m_pCount;
  }
  
  template<class Type>
    bool CSharedPtr<Type>::IsUnique() const
  {
    return 1 == *m_pCount;
  }
  
  template<class Type>
    bool CSharedPtr<Type>::IsNull() const
  {
    return NULL == m_pCount;
  }

  template<class Type>
    void CSharedPtr<Type>::Swap(CSharedPtr<Type>& rOther)
  {
    swap(m_pCount, rOther.m_pCount);
    swap(m_pObject, rOther.m_pObject);
  }

  template<class Type>
    void swap(Type& rLeft, Type& rRight) throw()
  {
    Type tmp = rLeft;
    rLeft = rRight;
    rRight = tmp;
  }

  template<class Type>
    bool operator==(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() == rRight.operator const Type&();
  }
  
  template<class Type>
    bool operator!=(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() != rRight.operator const Type&();
  }
  
  template<class Type>
    bool operator<(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() < rRight.operator const Type&();
  }
  
  template<class Type>
    bool operator>(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() > rRight.operator const Type&();
  }
  
} // namespace rise

#endif // _SHAREDPTR_HPP_
