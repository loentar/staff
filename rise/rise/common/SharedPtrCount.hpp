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

#ifndef _SHAREDPTRCOUNT_HPP_
#define _SHAREDPTRCOUNT_HPP_

#include "SharedPtrCount.h"

namespace rise
{
  template<class Type>
    CSharedPtrCount<Type>::CSharedPtrCount(Type* pObject):
    m_pObject(pObject)
  {
    m_pObject->Increment();
  }
  
  template<class Type>
    CSharedPtrCount<Type>::CSharedPtrCount(const CSharedPtrCount& rSharedPtrCount):
    m_pObject(rSharedPtrCount.m_pObject)
  {
    m_pObject->Increment();
  }
  
  template<class Type>
    CSharedPtrCount<Type>::~CSharedPtrCount()
  {
    if ( m_pObject->Decrement() == 0 )
      delete m_pObject;
  }

  template<class Type>
    Type& CSharedPtrCount<Type>::operator*() throw()
  {
    return *GetPointer();
  }
  
  template<class Type>
    Type* CSharedPtrCount<Type>::operator->() throw()
  {
    return GetPointer();
  }
  
  template<class Type>
    const Type* CSharedPtrCount<Type>::operator->() const throw()
  {
    return GetPointer();
  }

  template<class Type>
    CSharedPtrCount<Type>::operator Type&()
  {
    return static_cast<Type&>(*m_pObject);
  }

  template<class Type>
    CSharedPtrCount<Type>::operator const Type&() const
  {
    return static_cast<Type&>(*m_pObject);
  }
  
  
  template<class Type>
    Type* CSharedPtrCount<Type>::GetPointer() throw()
  {
    return m_pObject;
  }
  
  template<class Type>
    Type* CSharedPtrCount<Type>::GetPointer() const throw()
  {
    return m_pObject;
  }
  
  template<class Type>
    long CSharedPtrCount<Type>::GetRefCount() const throw()
  {
    return m_pObject->GetValue();
  }
  
  template<class Type>
    bool CSharedPtrCount<Type>::IsUnique() const
  {
    return 1 == m_pObject->GetValue();
  }
  
  template<class Type>
    void CSharedPtrCount<Type>::Swap(CSharedPtrCount<Type>& rOther)
  {
    Type* pTmp = m_pObject;
    m_pObject = rOther.m_pObject;
    rOther.m_pObject = pTmp;
  }

  template<class TypeLeft, class TypeRight>
    bool operator==(const CSharedPtrCount<TypeLeft>& rLeft, const CSharedPtrCount<TypeRight>& rRight) throw()
  {
    return rLeft.GetPointer() == rRight.GetPointer();
  }
  
  template<class TypeLeft, class TypeRight>
    bool operator!=(const CSharedPtrCount<TypeLeft>& rLeft, const CSharedPtrCount<TypeRight>& rRight) throw()
  {
    return rLeft.GetPointer() != rRight.GetPointer();
  }
  
  template<class Type>
    bool operator<(const CSharedPtrCount<Type>& rLeft, const CSharedPtrCount<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() < rRight.operator const Type&();
  }
  
  template<class Type>
    bool operator>(const CSharedPtrCount<Type>& rLeft, const CSharedPtrCount<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() > rRight.operator const Type&();
  }
  
} // namespace rise

#endif // _SHAREDPTRCOUNT_HPP_
