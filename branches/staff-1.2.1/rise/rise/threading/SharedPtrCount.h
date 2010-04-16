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

#ifndef _SHAREDPTRCOUNT_H_
#define _SHAREDPTRCOUNT_H_

#include <rise/threading/AtomicCount.h>

namespace rise
{
  //!  smart thread-safe pointer, using ref counting objects
  template<class Type>
  class CSharedPtrCount
  {
  public:
    //!         default constructor
    CSharedPtrCount(Type* pObject);

    //!         constructor
    /*! \param  rSharedPtrCount - another shared ptr object
        */
    CSharedPtrCount(const CSharedPtrCount& rSharedPtrCount);

    //!         destructor
    ~CSharedPtrCount();

    //!         get reference to object
    /*! \return reference to object
    */
    Type& operator*() throw();

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

    //!         get pointer to object
    /*! \return pointer to object
    */
    Type* GetPointer() throw();
    
    //!         get const pointer to object
    /*! \return const pointer to object
    */
    Type* GetPointer() const throw();
    
    //!         get reference count to object
    /*! \return reference count to object
    */
    long GetRefCount() const throw();
    
    //!         is object unique? ref=1
    /*! \return true if object unique
    */
    bool IsUnique() const;
    
    //!         swap pointers
    /*! \param  rOther - other pointer
    */
    void Swap(CSharedPtrCount<Type>& rOther);

  private:
    Type* m_pObject;         //!< pointer to object
  };

  //!         is pointers are equal
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointers are equal
      */
  template<class TypeLeft, class TypeRight>
  bool operator==(const CSharedPtrCount<TypeLeft>& rLeft, const CSharedPtrCount<TypeRight>& rRight) throw();
  
  //!         is pointers are inequal
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointers are inequal
      */
  template<class TypeLeft, class TypeRight>
  bool operator!=(const CSharedPtrCount<TypeLeft>& rLeft, const CSharedPtrCount<TypeRight>& rRight) throw();

  //!         operator less
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointer1 < pointer2
      */
  template<class Type>
  bool operator<(const CSharedPtrCount<Type>& rLeft, const CSharedPtrCount<Type>& rRight) throw();
  
  //!         operator greater
  /*! \param  rLeft - pointer1
      \param  rRight - pointer2
      \return true, if pointer1 > pointer2
      */
  template<class Type>
  bool operator>(const CSharedPtrCount<Type>& rLeft, const CSharedPtrCount<Type>& rRight) throw();

} // namespace rise

#include "SharedPtrCount.hpp"

#endif // _SHAREDPTRCOUNT_H_
