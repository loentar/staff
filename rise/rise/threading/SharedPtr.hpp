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
  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CSharedPtr
  //    PARAMETRS:      pContainer - указатель на обьект
  //    EXCEPTIONS:     std::bad_alloc
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CSharedPtr
    //    PARAMETRS:      none
    //    EXCEPTIONS:     std::bad_alloc
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CSharedPtr
  //    PARAMETRS:      none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtr<Type>::CSharedPtr(const CSharedPtr<Type>& rSharedPtr):
    m_pObject(rSharedPtr.m_pObject), m_pCount(rSharedPtr.m_pCount)
  {
    ++*m_pCount;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    DESTRUCTOR:     ~CSharedPtr
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
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

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       CSharedPtr<OtherType>
  //    DESCRIPTION:    преобразование указателя в другой тип
  //    PARAMETRS:      none
  //    RETURN:         другой тип обьекта
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
//   template<class Type>
//     template<class OtherType> 
//       CSharedPtr<Type>::operator CSharedPtr<OtherType>()
//   {
//     return CSharedPtr<OtherType>(m_pObject);
//   }

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       =
  //    DESCRIPTION:    изменение указателя на обьект
  //    PARAMETRS:      none
  //    RETURN:         указатель на обьект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtr<Type>& CSharedPtr<Type>::operator=(CSharedPtr<Type>& rOtherPtr) throw()
  {
    if (rOtherPtr.m_pObject == m_pObject)  // если один и тот же объект
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
    if (static_cast<Type*>(rOtherPtr.m_pObject) == m_pObject)  // если один и тот же объект
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

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       =
  //    DESCRIPTION:    изменение указателя на обьект
  //    PARAMETRS:      none
  //    RETURN:         указатель на обьект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    CSharedPtr<Type>& CSharedPtr<Type>::operator=(Type* pObject) throw()
    {
      if (pObject == m_pObject)  // если один и тот же объект
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
    if (static_cast<Type*>(pObject) == m_pObject)  // если один и тот же объект
      return *this;

    if ( IsUnique() && m_pObject != NULL )
      delete m_pObject;
    else
      m_pCount = new threading::CAtomicCount(pObject == NULL ? 0 : 1);

    m_pObject = static_cast<Type*>(pObject);
    return *this;
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
    Type& CSharedPtr<Type>::operator*() /*throw()*/
  {
//    ASSERTE(GetPointer(), CLogicNoInitException);
    return *GetPointer();
  }

  template<class Type>
    const Type& CSharedPtr<Type>::operator*() const /*throw()*/
  {
//    ASSERTE(GetPointer(), CLogicNoInitException);
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
    Type* CSharedPtr<Type>::operator->() throw()
  {
    return GetPointer();
  }
  
  template<class Type>
    const Type* CSharedPtr<Type>::operator->() const throw()
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
    CSharedPtr<Type>::operator Type&()
  {
    return static_cast<Type&>(*m_pObject);
  }

  template<class Type>
    CSharedPtr<Type>::operator const Type&() const
  {
    return static_cast<Type&>(*m_pObject);
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       Type*
  //    DESCRIPTION:    получение указателя на контролируемый обьект
  //    PARAMETRS:      none
  //    RETURN:         указатель на контролируемый объект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
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

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetPointer()
  //    DESCRIPTION:    получение указателя на объект
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
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
  
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetRefCount()
  //    DESCRIPTION:    получение количества ссылок
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    long CSharedPtr<Type>::GetRefCount() const throw()
  {
    return *m_pCount;
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
    bool CSharedPtr<Type>::IsUnique() const
  {
    return 1 == *m_pCount;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       IsNull()
  //    DESCRIPTION:    проверка на отсутствие инициализации
  //    PARAMETRS:      none
  //    RETURN:         true, если обьект не инициализирован
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    bool CSharedPtr<Type>::IsNull() const
  {
    return NULL == m_pCount;
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
    void CSharedPtr<Type>::Swap(CSharedPtr<Type>& rOther)
  {
    swap(m_pCount, rOther.m_pCount);
    swap(m_pObject, rOther.m_pObject);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       swap
  //    DESCRIPTION:    обмен значениями двух типов
  //    PARAMETRS:      rLeft, rRight - ссылки на обьекты типов для обмена
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    void swap(Type& rLeft, Type& rRight) throw()
  {
    Type tmp = rLeft;
    rLeft = rRight;
    rRight = tmp;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       ==
  //    DESCRIPTION:    один обьект пренадлежит двум контейнерам?
  //    PARAMETRS:      ссылки на объекты
  //    RETURN:         true, если если оба контейнера содержат один обьект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    bool operator==(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() == rRight.operator const Type&();
  }
  
  //////////////////////////////////////////////////////////////////////////////
  //    OPERATOR:       !=
  //    DESCRIPTION:    разные ли обьект пренадлежит двум контейнерам?
  //    PARAMETRS:      ссылки на объекты
  //    RETURN:         true, если если контейнеры содержат разные обьект
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  template<class Type>
    bool operator!=(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() != rRight.operator const Type&();
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
    bool operator<(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
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
    bool operator>(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw()
  {
    return rLeft.operator const Type&() > rRight.operator const Type&();
  }
  
} // namespace rise

#endif // _SHAREDPTR_HPP_
