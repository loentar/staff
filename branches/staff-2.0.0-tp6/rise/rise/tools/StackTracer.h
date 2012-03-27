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

#ifndef _STACKTRACER_H_
#define _STACKTRACER_H_

#include <rise/os/osdllexport.h>
#include <rise/string/String.h>
#include <rise/tools/AddrInfo.h>
#include <list>

namespace rise
{
  //!        rise misc tools
  namespace tools
  {
    //!  stack tracing result
    typedef std::list<SAddrInfo> TAddrInfoList;

    //!  Stack tracer
    class RISE_EXPORT CStackTracer
    {
    public:
      //!         trace stack
      /*! \param  sTraceResult - string trace stack result
          \param  nSkip - skip nSkip top stack levels
          */
      static void StackTraceStr(CString& sTraceResult, int nSkip = 0);

      //!         trace stack
      /*! \param  rTraceResult - trace stack result
          \param  nSkip - skip nSkip top stack levels
          */
      static void StackTrace(TAddrInfoList& rTraceResult, int nSkip = 0);
    };
  }
}

#endif // _STACKTRACER_H_
