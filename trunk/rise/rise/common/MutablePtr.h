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

#ifndef _MUTABLEPTR_H_
#define _MUTABLEPTR_H_

#include <rise/common/ExceptionTemplate.h>

namespace rise
{
  //! mutable pointer class
  template<typename Type>
  class CMutablePtr
  {
  public:
    //!         constructor
    CMutablePtr():
      m_pnCounter(NULL), m_pData(NULL) 
    {}
      
    //!         constructor with counting refs
    /*! \param  rPtr - reference to source object
        */
    CMutablePtr(const CMutablePtr& rPtr):
      m_pnCounter(NULL), m_pData(NULL) 
    {
      operator=(rPtr);
    }

    //!         initializing constructor
    /*! \param  pData - object
        */
    CMutablePtr(Type* pData):
      m_pnCounter(NULL), m_pData(NULL) 
    {
      operator=(pData);
    }

    //!         destructor
    ~CMutablePtr()
    {
      Release();
    }
      
    //!         operator with counting refs
    /*! \param  rPtr - ref to source object
        \return ref to current object
        */
    CMutablePtr& operator=(const CMutablePtr& rPtr)
    {
      if (m_pData != rPtr.m_pData)
        Set(rPtr.m_pData, rPtr.m_pnCounter);
      return *this;
    }
    
    //!         initialization
    /*! \param  pData - pointer to object
        \return reference to current object
        */
    CMutablePtr& operator=(Type* pData)
    {
      if (m_pData != pData)
        Set(pData);
      return *this;
    }

    //!         tests a target pointer for equality with a specified pointer
    /*! \param  rPtr - reference to another object
        \return true - pointers are equals
        */
    bool operator==(const CMutablePtr& rPtr) const
    {
      return rPtr.m_pData == m_pData && rPtr.m_pnCounter == m_pnCounter;
    }
    
    //!         tests a target pointer for inequality with a specified pointer
    /*! \param  rPtr - reference to another object
        \return true - pointers are not equals
        */
    bool operator!=(const CMutablePtr& rPtr) const
    {
      return !operator==(rPtr);
    }
    
    //!         get pointer to storing object
    /*! \return pointer to storing object
    */
    Type* Get() const
    {
      return m_pData;
    }
    
    //!         get reference to storing object
    /*! \return reference to storing object
    */
    Type& operator*() const
    {
      RISE_ASSERTES(m_pnCounter != NULL && m_pData != NULL, rise::CLogicNoInitException, "trying to deference NULL ptr");
      
      return *m_pData;
    }
    
    //!         operator->
    /*! \return pointer to storing object
    */
    Type* operator->() const
    {
      return m_pData;
    }
    
    //!         cast operator
    /*! \return pointer to storing object
    */
    operator Type*() const
    {
      return m_pData;
    }
    
    //!         set storing object
    /*! \param  pData - pointer to object
        \param  pnCounter - object references counter
        \return pointer to object
        */
    Type* Set(Type* pData, int* pnCounter = NULL)
    {
      Release();
      m_pData = pData;
      if(pnCounter != NULL)
      {
        m_pnCounter = pnCounter;
        ++*m_pnCounter;
      }
      else
      {
        if(m_pData != NULL)
          m_pnCounter = new int(1);
      }
      
      return m_pData;
    }
    
    //!         free storing object
    /*! decrease internal counter to object. 
       object will be deleted when ref count = 0 */
    void Release()
    {
      if(m_pnCounter != NULL)
      {
        --*m_pnCounter;
        if(*m_pnCounter == 0)
        {
          delete m_pnCounter;
          m_pnCounter = NULL;
          if(m_pData != NULL)
            delete m_pData;
        }
      }
    }
    
  private:
    int* m_pnCounter;      //!<  reference counter
    mutable Type* m_pData; //!<  pointer to object
  };
}

#endif // _MUTABLEPTR_H_
