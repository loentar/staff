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

#include <stdlib.h>
#include <string.h>
#include "StackTracer.h"
#include "Exception.h"

namespace staff
{

  Exception::Exception(const char* szFileLine, const char* szFunction, const std::string& sDescr):
    m_sDescr(szFileLine)
  {
    m_sDescr.append(szFunction).append(": ").append(sDescr);

    static short nEnableStackTracing = -1;
    if (nEnableStackTracing == -1)
    {
      char* szStackTracingEnv = NULL;
#if defined WIN32 && !defined __MINGW32__
      _dupenv_s(&szStackTracingEnv, NULL, "STAFF_EXCEPTION_STACKTRACING");
#else
      szStackTracingEnv = getenv("STAFF_EXCEPTION_STACKTRACING");
#endif
      nEnableStackTracing =
          (szStackTracingEnv && ((!strcmp(szStackTracingEnv, "1") || !strcmp(szStackTracingEnv, "TRUE"))))
          ? 1 : 0;

#ifdef WIN32
      if (szStackTracingEnv)
      {
        free(szStackTracingEnv);
      }
#endif
    }

    if (nEnableStackTracing == 1)
    {
      std::string sStack;
      StackTracer::GetStackTraceStr(sStack);
      m_sDescr.append("\nTraced stack:\n").append(sStack);
    }

  }

}

