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

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include "FileUnpacker.h"

namespace webapp
{
  namespace admin
  {
    CFileUnpacker::CFileUnpacker():
      m_nStatus(EUnknown)
    {
    }

    void CFileUnpacker::Start(const std::string& sUnpackCmd)
    {
      m_sUnpackCmd = sUnpackCmd;
      rise::threading::CThread::Start();
    }

    int CFileUnpacker::GetStatus()
    {
      return m_nStatus;
    }

    void CFileUnpacker::Run(void*)
    {
      int nStatus = -2;
      rise::LogDebug() << "Unpacking: [" << m_sUnpackCmd << "]";
      RISE_ASSERTES(m_tUnpackProc.Exec(m_sUnpackCmd), rise::CLogicNoItemException, "Cannot start unpack process: \"" + m_sUnpackCmd + "\"");

      m_nStatus = ERunning;

      while (m_nStatus == ERunning)
      {
        nStatus = m_tUnpackProc.GetExitStatus(true);

        if (nStatus == -2)
        { // some error
          m_nStatus = EGenericError;
        }
        else
        if (nStatus != -1)
        { // finished
          #ifndef WIN32
            m_nStatus = WIFEXITED(nStatus) ? WEXITSTATUS(nStatus) : EGenericError;
          #else
            m_nStatus = nStatus;
          #endif
        }
      }
    }
  }
}

