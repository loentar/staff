/*
 *  Copyright 2012 Utkin Dmitry
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


#ifndef _STAFF_UTILS_SHAREDPTR_H_
#define _STAFF_UTILS_SHAREDPTR_H_

#include "AtomicCounter.h"
#include "Exception.h"

namespace staff
{

  //! thread-safe, ref-counting, mutable shared pointer
  template <typename Type>
  class SharedPtr
  {
  public:
    //! default constructor
    inline SharedPtr():
      m_pData(NULL),
      m_pCounter(NULL)
    {
    }

    //! initalizing constructor
    /*! \param  pData - data
      */
    inline SharedPtr(Type* pData):
      m_pData(pData),
      m_pCounter(new AtomicCounter(1))
    {
    }

    //! constructor
    /*! \param  rPtr - other shared pointer
      */
    inline SharedPtr(const SharedPtr& rPtr):
      m_pData(rPtr.m_pData),
      m_pCounter(rPtr.m_pCounter)
    {
      if (m_pCounter)
      {
        ++*m_pCounter;
      }
    }

    //! constructor
    /*! \param  rPtr - other shared pointer
      */
    template <typename OtherType>
    inline SharedPtr<Type>(const SharedPtr<OtherType>& rPtr):
      m_pData(static_cast<Type*>(rPtr.m_pData)),
      m_pCounter(rPtr.m_pCounter)
    {
      if (m_pCounter)
      {
        ++*m_pCounter;
      }
    }

    //! destructor
    inline ~SharedPtr()
    {
      Free();
    }

    //! initializer
    /*! \param  pData - data
        \return
      */
    inline SharedPtr& operator=(Type* pData)
    {
      Set(pData);
      return *this;
    }

    inline SharedPtr& operator=(const SharedPtr& rPtr)
    {
      Set(rPtr.m_pData, rPtr.m_pCounter);
      return *this;
    }


    //! set object
    /*! \param  pData - pointer to object
        \param  pCounter - references counter
        \return pointer to object
        */
    inline Type* Set(Type* pData, AtomicCounter* pCounter = NULL)
    {
      if (m_pData != pData)
      {
        Free();

        m_pData = pData;
        if (m_pData)
        {
          m_pCounter = pCounter;
          if (m_pCounter)
          {
            ++*m_pCounter;
          }
          else
          {
            m_pCounter = new AtomicCounter(1);
          }
        }
      }

      return m_pData;
    }

    //! decrease counter, detach object and counter.
    /*! if counter == 0, free object and counter
        */
    inline void Free()
    {
      if (m_pCounter)
      {
        if (!(--*m_pCounter))
        {
          delete m_pCounter;
          delete m_pData;
        }
        m_pCounter = NULL;
        m_pData = NULL;
      }
    }

    //! release one reference to object and return stored object
    /*! Use with care.
        if counter == 1, free counter.
        To make this function safe, please check SharedPtr for uniqity,
        else stored object can be destroyed by another instance of SharedPtr,
        that references to this object

        \sa IsUnique
        \return stored object
        */
    inline Type* Release()
    {
      Type* pObject = m_pData;

      if (m_pCounter)
      {
        if (!(--*m_pCounter))
        {
          delete m_pCounter;
        }
        m_pCounter = NULL;
        m_pData = NULL;
      }

      return pObject;
    }


    //! ger reference to stored object
    /*! \return referene to object
      */
    inline const Type& operator*() const
    {
      STAFF_ASSERT(m_pData, "Can't get reference to object that is NULL");
      return *m_pData;
    }

    //! ger reference to stored object
    /*! \return referene to type
      */
    inline Type& operator*()
    {
      STAFF_ASSERT(m_pData, "Can't get reference to object that is NULL");
      return *m_pData;
    }

    //! dereferencing operator
    /*! \return referene to type
      */
    inline const Type* operator->() const
    {
      STAFF_ASSERT(m_pData, "Attempt to dereferencing NULL");
      return m_pData;
    }

    //! dereferencing operator
    /*! \return referene to type
      */
    inline Type* operator->()
    {
      STAFF_ASSERT(m_pData, "Attempt to dereferencing NULL");
      return m_pData;
    }

    //! get pointer to stored object
    /*! \return pointer to stored object
      */
    inline const Type* Get() const
    {
      return m_pData;
    }

    //! get pointer to stored object
    /*! \return pointer to stored object
      */
    inline Type* Get()
    {
      return m_pData;
    }

    //! get pointer to stored object
    /*! \return pointer to stored object
      */
    inline operator const Type*() const
    {
      return m_pData;
    }

    //! get pointer to stored object
    /*! \return pointer to stored object
      */
    inline operator Type*()
    {
      return m_pData;
    }

    //! check, is stored object NULL
    /*! \return true, if stored object is NULL
      */
    inline bool operator!() const
    {
      return !m_pData;
    }

    //! is object unique
    /*! \return true if object unique (reference count == 1)
    */
    inline bool IsUnique() const
    {
      STAFF_ASSERT(m_pData, "Not initialized");
      return *m_pCounter == 1;
    }

    //! get references count to stored object
    /*! \return references count
      */
    inline long GetRefCount() const
    {
      STAFF_ASSERT(m_pData, "Not initialized");
      return *m_pCounter;
    }

  private:
    mutable Type* m_pData; //!< pointer to object
    mutable AtomicCounter* m_pCounter;
    template <typename> friend class SharedPtr;
  };
  
}

#endif // _STAFF_UTILS_SHAREDPTR_H_
