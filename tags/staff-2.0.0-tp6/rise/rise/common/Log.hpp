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

#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <rise/string/Encoding.h>

namespace rise
{
  template<typename TDATA>
  const CLogStream& CLogStream::operator<<( TDATA tData ) const
  {
    if (m_pLogStream != NULL)
    {
      if(m_nSrcRecode != m_nDstRecode)
      {
        COStringStream tOStringStream;
        tOStringStream << tData << '\0' << '\0';
        OutRecoded(tOStringStream.str());
      }
      else
      {
        *m_pLogStream << tData;
      }
    }
    
    return *this;
  }
}

#endif // _LOG_HPP_
