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

#ifndef _STRING_HPP_
#define _STRING_HPP_

#include <rise/common/streamtypes.h>

namespace rise
{
  
  template<class TData>
  TData& FromStr(const CString& sData, TData& tResult)
  {
    CIStringStream(sData) >> tResult;
    return tResult;
  }

  template<class TData>
  CString ToStr(const TData& rtData)
  {
    COStringStream tOStringStream;
    tOStringStream << rtData;
    return tOStringStream.str().c_str();
  }

  template<class TData>
  void ToStr(const TData& rtData, CString& sResult)
  {
    COStringStream tOStringStream;
    tOStringStream << rtData;
    sResult = tOStringStream.str().c_str();
  }

} // namespace rise

#endif // _STRING_HPP_

