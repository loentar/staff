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

#include <rise/threading/AtomicCount.h>
#include "SharedPtr.h"

namespace rise
{
  template<class Type>
  CSharedPtr<Type>::CSharedPtr() throw():
    m_pObject(NULL),
    m_pCounter(NULL)
  {
  }

  template<class Type>
  CSharedPtr<Type>::CSharedPtr(Type* pObject) throw():
    m_pObject(pObject),
    m_pCounter(new threading::CAtomicCount(1))
  {
  }

  template<class Type> template<class OtherType>
  CSharedPtr<Type>::CSharedPtr(OtherType* pObjectOther) throw():
    m_pObject(static_cast<Type*>(pObjectOther)),
    m_pCounter(new threading::CAtomicCount(1))
  {
  }

  template<class Type>
  CSharedPtr<Type>::CSharedPtr(const CSharedPtr<Type>& rPtr) throw():
    m_pObject(rPtr.m_pObject),
    m_pCounter(rPtr.m_pCounter)
  {
    if (m_pCounter)
    {
      ++*m_pCounter;
    }
  }

  template<class Type> template<class OtherType>
  CSharedPtr<Type>::CSharedPtr(const CSharedPtr<OtherType>& rPtrOther) throw():
    m_pObject(static_cast<Type*>(rPtrOther.m_pObject)),
    m_pCounter(rPtrOther.m_pCounter)
  {
    if (m_pCounter)
    {
      ++*m_pCounter;
    }
  }

  template<class Type>
  CSharedPtr<Type>::~CSharedPtr() throw()
  {
    Free();
  }

  template<class Type>
  void CSharedPtr<Type>::Set(Type* pObject) throw()
  {
    if (m_pObject != pObject)
    {
      Free();
      if (pObject)
      {
        m_pObject = pObject;
        m_pCounter = new threading::CAtomicCount(1);
      }
    }
  }

  template<class Type>
  template<class OtherType>
  void CSharedPtr<Type>::Set(OtherType* pObjectOther) throw()
  {
    Set(static_cast<Type*>(pObjectOther));
  }

  template<class Type>
  void CSharedPtr<Type>::Set(CSharedPtr<Type>& rPtr) throw()
  {
    if (m_pObject != rPtr.m_pObject)
    {
      Free();
      this->m_pObject = rPtr.m_pObject;
      this->m_pCounter = rPtr.m_pCounter;
      if (this->m_pCounter)
      {
        ++*this->m_pCounter;
      }
    }
  }

  template<class Type>
  template<class OtherType>
  void CSharedPtr<Type>::Set(CSharedPtr<OtherType>& rPtrOther) throw()
  {
    if (m_pObject != rPtrOther.m_pObject)
    {
      Free();
      this->m_pObject = static_cast<Type*>(rPtrOther.m_pObject);
      this->m_pCounter = rPtrOther.m_pCounter;
      if (this->m_pCounter)
      {
        ++*this->m_pCounter;
      }
    }
  }


  template<class Type>
  void CSharedPtr<Type>::Free() throw()
  {
    if (m_pCounter != NULL && (--*m_pCounter) == 0)
    {
      delete m_pCounter;
      m_pCounter = NULL;
      if (m_pObject)
      {
        delete m_pObject;
        m_pObject = NULL;
      }
    }
  }

  template<class Type>
  Type* CSharedPtr<Type>::Release() throw()
  {
    Type* pObject = m_pObject;

    if (m_pCounter != NULL && (--*m_pCounter) == 0)
    {
      delete m_pCounter;
      m_pCounter = NULL;
      m_pObject = NULL;
    }

    return pObject;
  }

  template<class Type>
  CSharedPtr<Type>& CSharedPtr<Type>::operator=(CSharedPtr<Type>& rPtr) throw()
  {
    Set(rPtr);
    return *this;
  }

  template<class Type> template<class OtherType>
  CSharedPtr<Type>& CSharedPtr<Type>::operator=(CSharedPtr<OtherType>& rPtrOther) throw()
  {
    Set(rPtrOther);
    return *this;
  }

  template<class Type>
  CSharedPtr<Type>& CSharedPtr<Type>::operator=(Type* pObject) throw()
  {
    Set(pObject);
    return *this;
  }

  template<class Type>
  template<class OtherType>
  CSharedPtr<Type>& CSharedPtr<Type>::operator=(OtherType* pObjectOther) throw()
  {
    Set(pObjectOther);
    return *this;
  }

  template<class Type>
  Type& CSharedPtr<Type>::operator*()
  {
    return *m_pObject;
  }

  template<class Type>
  const Type& CSharedPtr<Type>::operator*() const
  {
    return *m_pObject;
  }
  
  template<class Type>
  Type* CSharedPtr<Type>::operator->() throw()
  {
    return m_pObject;
  }
  
  template<class Type>
  const Type* CSharedPtr<Type>::operator->() const throw()
  {
    return m_pObject;
  }

  template<class Type>
  Type* CSharedPtr<Type>::Get() throw()
  {
    return m_pObject;
  }

  template<class Type>
  const Type* CSharedPtr<Type>::Get() const throw()
  {
    return m_pObject;
  }
  
  template<class Type>
  long CSharedPtr<Type>::GetRefCount() const throw()
  {
    return *m_pCounter;
  }
  
  template<class Type>
  bool CSharedPtr<Type>::IsUnique() const
  {
    return *m_pCounter == 1;
  }
  
  template<class Type>
  bool CSharedPtr<Type>::IsNull() const
  {
    return m_pCounter == NULL;
  }

  template<class Type>
  bool CSharedPtr<Type>::operator!() const
  {
    return m_pObject == NULL;
  }

  template<class Type>
  void CSharedPtr<Type>::Swap(CSharedPtr<Type>& rPtr)
  {
    Type* pObject = m_pObject;
    threading::CAtomicCount* pCounter = m_pCounter;

    m_pObject = rPtr.m_pObject;
    m_pCounter = rPtr.m_pCounter;

    rPtr.m_pObject = pObject;
    rPtr.m_pCounter = pCounter;
  }
  
} // namespace rise

#endif // _SHAREDPTR_HPP_
