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

#ifndef _SHAREDPTR_H_
#define _SHAREDPTR_H_

#include <rise/threading/AtomicCount.h>

namespace rise
{
  //!  smart thread-safe pointer
  template<class Type>
  class CSharedPtr
  {
  public:
    //!         default constructor
    CSharedPtr();

    //!         initializing constructor
    /*! \param  pObject - object
        */
    CSharedPtr(Type* pObject);

    //!         constructor
    /*! \param  rSharedPtr - another shared ptr object
        */
    CSharedPtr(const CSharedPtr<Type>& rSharedPtr);

    //!         destructor
    ~CSharedPtr();

    //!         replace pointer
    /*! \return *this
    */
    CSharedPtr<Type>& operator=(CSharedPtr<Type>& rOtherPtr) throw();

    //!         replace and cast pointer
    /*! \return *this
    */
    template<class OtherType>
    CSharedPtr<Type>& operator=(CSharedPtr<OtherType>& rOtherPtr) throw();

    //!         replace object
    /*! \return *this
    */
    CSharedPtr<Type>& operator=(Type* pObject) throw();

    //!         replace and cast object
    /*! \return *this
    */
    template<class OtherType>
    CSharedPtr<Type>& operator=(OtherType* pObject) throw();

    //!         get reference to object
    /*! \return reference to object
    */
    Type& operator*();

    //!         get const reference to object
    /*! \return const reference to object
    */
    const Type& operator*() const;
    
    //!         operator ->
    /*! \return pointer to object
    */
    Type* operator->() throw();

    //!         operator ->
    /*! \return const pointer to object
    */
    const Type* operator->() const throw();

    //!         type cast operator
    operator Type&();

    //!         type cast operator
    operator const Type&() const;

    //!         get pointer to object operator
    /*! \return pointer to object operator
    */
    operator Type*();

    //!         get const pointer to object operator
    /*! \return const pointer to object operator
    */
    operator const Type*() const;

    //!         get pointer to object
    /*! \return pointer to object
    */
    Type* GetPointer() throw();

    //!         get const pointer to object
    /*! \return const pointer to object
    */
    const Type* GetPointer() const throw();
    
    //!         get reference count to object
    /*! \return reference count to object
    */
    long GetRefCount() const throw();
    
    //!         is object unique? ref=1
    /*! \return true if object unique
    */
    bool IsUnique() const;
    
    //!         is pointer not initialized
    /*! \return pointer is not initialized
    */
    bool IsNull() const;
    
    //!         swap pointers
    /*! \param  rOther - other pointer
        */
    void Swap(CSharedPtr<Type>& rOther);

  private:
    Type* m_pObject;         //!< pointer to object
    threading::CAtomicCount* m_pCount;  //!< pointer to ref count
    
  };

  //!        swap two types
  /*! \param  rLeft - type1
      \param  rRight - type2
      */
  template<class Type>
  void swap(Type& rLeft, Type& rRight) throw();

  //!         is pointers are equal
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointers are equal
      */
  template<class Type>
  bool operator==(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();
  
  //!         is pointers are inequal
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointers are not equal
      */
  template<class Type>
  bool operator!=(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();

  //!         operator less
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointer1 < pointer2
      */
  template<class Type>
  bool operator<(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();
  
  //!         operator greater
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointer1 > pointer2
      */
  template<class Type>
  bool operator>(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();
  
} // namespace rise

#include "SharedPtr.hpp"

#endif // _SHAREDPTR_H_
