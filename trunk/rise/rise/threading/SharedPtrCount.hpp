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
  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CSharedPtrCount
  //    PARAMETRS:      pContainer - указатель на обьект
  //    EXCEPTIONS:     std::bad_alloc
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtrCount<Type>::CSharedPtrCount(Type* pObject):
    m_pObject(pObject)
  {
    m_pObject->Increment();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CSharedPtrCount
  //    PARAMETRS:      none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtrCount<Type>::CSharedPtrCount(const CSharedPtrCount& rSharedPtrCount):
    m_pObject(rSharedPtrCount.m_pObject)
  {
    m_pObject->Increment();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    DESTRUCTOR:     ~CSharedPtrCount
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtrCount<Type>::~CSharedPtrCount()
  {
    if ( m_pObject->Decrement() == 0 )
      delete m_pObject;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       *
  //    DESCRIPTION:    получение указателя на обьект
  //    PARAMETRS:      none
  //    RETURN:         указатель на обьект
  //    EXCEPTIONS:     CLogicNoInitException - обьект не проинициализирован
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    Type& CSharedPtrCount<Type>::operator*() throw()
  {
    return *GetPointer();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       ->
  //    DESCRIPTION:    оператор разыменовывания
  //    PARAMETRS:      none
  //    RETURN:         указатель на обьект
  //    EXCEPTIONS:     CLogicNoInitException - обьект не проинициализирован
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
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

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       Type&
  //    DESCRIPTION:    получение ссылки на контролируемый обьект
  //    PARAMETRS:      none
  //    RETURN:         ссылка на контролируемый объект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtrCount<Type>::operator Type&()
  {
    return static_cast<Type&>(*m_pObject);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       Type&
  //    DESCRIPTION:    получение константной ссылки на контролируемый обьект
  //    PARAMETRS:      none
  //    RETURN:         константная ссылка на контролируемый объект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtrCount<Type>::operator const Type&() const
  {
    return static_cast<Type&>(*m_pObject);
  }
  
  
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetPointer()
  //    DESCRIPTION:    получение указателя на объект
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    Type* CSharedPtrCount<Type>::GetPointer() throw()
  {
    return m_pObject;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetPointer()
  //    DESCRIPTION:    получение константного указателя на объект
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    Type* CSharedPtrCount<Type>::GetPointer() const throw()
  {
    return m_pObject;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetRefCount()
  //    DESCRIPTION:    получение количества ссылок
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    long CSharedPtrCount<Type>::GetRefCount() const throw()
  {
    return m_pObject->GetValue();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       IsUnique()
  //    DESCRIPTION:    уникален ли объект
  //    PARAMETRS:      none
  //    RETURN:         true, если объект уникален
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    bool CSharedPtrCount<Type>::IsUnique() const
  {
    return 1 == m_pObject->GetValue();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Swap(...)
  //    DESCRIPTION:    обмен указателей на объект
  //    PARAMETRS:      rOther - ссылка на другой объект
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    void CSharedPtrCount<Type>::Swap(CSharedPtrCount<Type>& rOther)
  {
    Type* pTmp = m_pObject;
    m_pObject = rOther.m_pObject;
    rOther.m_pObject = pTmp;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       ==
  //    DESCRIPTION:    один обьект пренадлежит двум контейнерам?
  //    PARAMETRS:      ссылки на объекты
  //    RETURN:         true, если если оба контейнера содержат один обьект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class TypeLeft, class TypeRight>
    bool operator==(const CSharedPtrCount<TypeLeft>& rLeft, const CSharedPtrCount<TypeRight>& rRight) throw()
  {
    return rLeft.GetPointer() == rRight.GetPointer();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       !=
  //    DESCRIPTION:    разные ли обьект пренадлежит двум контейнерам?
  //    PARAMETRS:      ссылки на объекты
  //    RETURN:         true, если если контейнеры содержат разные обьект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class TypeLeft, class TypeRight>
    bool operator!=(const CSharedPtrCount<TypeLeft>& rLeft, const CSharedPtrCount<TypeRight>& rRight) throw()
  {
    return rLeft.GetPointer() != rRight.GetPointer();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       <
  //    DESCRIPTION:    оператор меньше
  //    PARAMETRS:      ссылки на объекты
  //    RETURN:         true, если если левый операнд меньше правого
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    bool operator<(const CSharedPtrCount<Type>& rLeft, const CSharedPtrCount<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() < rRight.operator const Type&();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       >
  //    DESCRIPTION:    оператор больше
  //    PARAMETRS:      ссылки на объекты
  //    RETURN:         true, если если левый операнд больше правого
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    bool operator>(const CSharedPtrCount<Type>& rLeft, const CSharedPtrCount<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() > rRight.operator const Type&();
  }
  
} // namespace rise

#endif // _SHAREDPTRCOUNT_HPP_
