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

namespace rise
{
namespace threading
{
  class CAtomicCount;
}

  //!  thread-safe smart pointer with implicit cast support
  template<class Type>
  class CSharedPtr
  {
  public:
    //!         default constructor
    CSharedPtr() throw();

    //!         initializing constructor
    /*! \param  pObject - object
        */
    CSharedPtr(Type* pObject) throw();

    //!         initializing constructor
    /*! \param  pObject - object
        */
    template<class OtherType>
    CSharedPtr(OtherType* pObjectOther) throw();

    //!         constructor
    /*! \param  rSharedPtr - another shared ptr object
        */
    CSharedPtr(const CSharedPtr<Type>& rPtr) throw();

    //!         constructor
    /*! \param  rSharedPtr - another shared ptr object
        */
    template<class OtherType>
    CSharedPtr(const CSharedPtr<OtherType>& rPtrOther) throw();

    //!         destructor
    ~CSharedPtr() throw();

    //!         set pointer, initialize ref count
    /*! \param  rPtrOther - pointer
    */
    void Set(Type* pObject) throw();

    //!         cast and set pointer
    /*! \param  rPtrOther - pointer with other(castable) type
    */
    template<class OtherType>
    void Set(OtherType* pObjectOther) throw();

    //!         set pointer, increase ref count
    /*! \param  rPtr - smart pointer
    */
    void Set(CSharedPtr<Type>& rPtr) throw();

    //!         cast and set pointer, increase ref count
    /*! \param  rPtrOther - smart pointer with other(castable) type
    */
    template<class OtherType>
    void Set(CSharedPtr<OtherType>& rPtrOther) throw();

    //!         decrease counter, detach object and counter.
    /*! if counter == 0, free object and counter
        */
    void Free() throw();

    //!         release one reference to object and return stored object
    /*! Use with care.
        if counter == 0, free counter.
        To made this function safe, please check SharedPtr for uniqity,
        else object can be destroyed by another instance of SharedPtr,
        that references to this object

        \sa IsUnique
        \return stored object
        */
    Type* Release() throw();

    //!         replace pointer
    /*! \return *this
    */
    CSharedPtr<Type>& operator=(CSharedPtr<Type>& rPtrOther) throw();

    //!         replace and cast pointer
    /*! \return *this
    */
    template<class OtherType>
    CSharedPtr<Type>& operator=(CSharedPtr<OtherType>& rPtrOther) throw();

    //!         replace object
    /*! \return *this
    */
    CSharedPtr<Type>& operator=(Type* pObject) throw();

    //!         replace and cast object
    /*! \return *this
    */
    template<class OtherType>
    CSharedPtr<Type>& operator=(OtherType* pObjectOther) throw();

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

    //!         get pointer to object
    /*! \return pointer to object
    */
    Type* Get() throw();

    //!         get const pointer to object
    /*! \return const pointer to object
    */
    const Type* Get() const throw();

    //!         get reference count to object
    /*! \return reference count to object
    */
    long GetRefCount() const throw();

    //!         is object unique
    /*! \return true if object unique (reference count == 1)
    */
    bool IsUnique() const;

    //!         is pointer not initialized
    /*! \return true if pointer is not initialized
    */
    bool IsNull() const;

    //!         is pointer not initialized
    /*! \return true if pointer is not initialized
    */
    bool operator!() const;

    //!         swap pointers
    /*! \param  rOther - other pointer
        */
    void Swap(CSharedPtr<Type>& rPtr);

  private:
    Type* m_pObject;                      //!< pointer to object
    threading::CAtomicCount* m_pCounter;  //!< pointer to ref counter

    template<class> friend class CSharedPtr;
  };

} // namespace rise

#include "SharedPtr.hpp"

#endif // _SHAREDPTR_H_
