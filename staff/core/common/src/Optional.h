/*
 *  Copyright 2011 Utkin Dmitry
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


#ifndef _STAFF_OPTIONAL_H_
#define _STAFF_OPTIONAL_H_

#include <rise/common/MutablePtr.h>

namespace staff
{

  template <typename Type>
  class Optional
  {
  public:
    Optional()
    {
    }

    Optional(const Type& rData):
      m_tpData(new Type(rData))
    {
    }

    Optional(const Optional& rOpt):
      m_tpData(const_cast< Optional<Type>& >(rOpt).m_tpData)
    {
    }

    inline Optional<Type>& operator=(const Type& rData)
    {
      Get() = rData;
      return *this;
    }

    inline Optional<Type>& operator=(const Optional<Type>& rOpt)
    {
      m_tpData = rOpt.m_tpData;
      return *this;
    }

    inline bool operator==(const Type& rOpt) const
    {
      if (!m_tpData.Get())
      {
        return false;
      }

      return *m_tpData.Get() == rOpt;
    }

    inline operator Type&()
    {
      return Get();
    }

    inline operator const Type&() const
    {
      return Get();
    }

    inline Type* operator->()
    {
      return &Get();
    }

    inline const Type* operator->() const
    {
      return &Get();
    }

    inline Type& operator*()
    {
      return Get();
    }

    inline const Type& operator*() const
    {
      return Get();
    }

    inline void Reset()
    {
      m_tpData.Set(NULL);
    }

    inline bool IsNull() const
    {
      return !m_tpData.Get();
    }

    inline bool IsSet() const
    {
      return !!m_tpData.Get();
    }

    inline Type& Get() const
    {
      Type* pData = m_tpData.Get();
      if (!pData)
      {
        pData = new Type;
        m_tpData.Set(pData);
      }

      return *pData;
    }

  private:
    mutable rise::CMutablePtr<Type> m_tpData;
  };

} // namespace staff

#endif // _STAFF_OPTIONAL_H_
