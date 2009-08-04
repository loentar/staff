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

#ifndef _CRITICALDATA_HPP_
#define _CRITICALDATA_HPP_

namespace rise
{
  namespace threading
  {
      template<typename Type>
      CCriticalData<Type>::CCriticalData(const Type &rValue)
      {
        CScopedCriticalSection cs(m_cs);
        m_tValue = rValue;
      }

      template<typename Type>
      CCriticalData<Type>::operator Type&()
      {
        return m_tValue;
      }
      
      template<typename Type>
      Type& CCriticalData<Type>::operator++()
      {
        CScopedCriticalSection cs(m_cs);
        return ++m_tValue;
      }

      template<typename Type>
      Type& CCriticalData<Type>::operator++(int)
      {
        CScopedCriticalSection cs(m_cs);
        Type tValue = m_tValue;
        ++m_tValue;
        return tValue;
      }
      
      template<typename Type>
      Type& CCriticalData<Type>::operator--()
      {
        CScopedCriticalSection cs(m_cs);
        return --m_tValue;
      }
      
      template<typename Type>
      Type& CCriticalData<Type>::operator--(int)
      {
        CScopedCriticalSection cs(m_cs);
        Type tValue = m_tValue;
        --m_tValue;
        return tValue;
      }
      
      template<typename Type>
      Type& CCriticalData<Type>::operator=(const Type& rValue)
      {
        CScopedCriticalSection cs(m_cs);
        return m_tValue = rValue;
      }
      
  }
}

#endif // _CRITICALDATA_HPP_
