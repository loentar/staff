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

#ifndef _XMLVALUE_HPP_
#define _XMLVALUE_HPP_

#include <rise/string/String.h>

namespace rise
{
  namespace xml
  {
    template<typename TTYPE>
    CXMLValue::CXMLValue(const TTYPE rValue)
    {
      operator=(rValue);
    }

    template<typename TTYPE>
    CXMLValue& CXMLValue::operator=(const TTYPE rValue)
    {
      ToStr(rValue, m_sValue);
      return *this;
    }


    template<typename TTYPE>
    CXMLValue::operator const TTYPE() const
    {
      TTYPE tValue = static_cast<TTYPE>(0);
      return FromStr(m_sValue, tValue);
    }

    template<typename TTYPE>
    CXMLValue::operator TTYPE()
    {
      TTYPE tValue = static_cast<TTYPE>(0);
      return FromStr(m_sValue, tValue);
    }

    template<typename TTYPE>
    bool CXMLValue::operator==(TTYPE rValue) const
    {
      return rValue == static_cast<TTYPE>(*this);
    }

    template<typename TTYPE>
    bool CXMLValue::operator!=(const TTYPE rValue) const
    {
      return !(operator==(rValue));
    }

  } // namespace xml
} // namespace rise

#endif // _XMLVALUE_H_
