/*
 *  Copyright 2010 Utkin Dmitry
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

#include <map>
#include <rise/timer/TimerHandler.h>
#include <staff/security/Sessions.h>
#include "SessionManager.h"

namespace staff
{
  class CSessionManager::CSessionManagerImpl: public rise::CTimerHandler
  {
  public:
    typedef std::map<std::string, staff::security::SSession> TSessionMap;

    TSessionMap mSessions;
  };

  CSessionManager& CSessionManager::Inst()
  {
    if (!m_pInst)
    {
      m_pInst = new CSessionManager;
    }

    return *m_pInst;
  }

  void CSessionManager::FreeInst()
  {
    if (m_pInst)
    {
      delete m_pInst;
      m_pInst = NULL;
    }
  }

  void CSessionManager::Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId)
  {

  }

  void CSessionManager::OpenExtra(const std::string& sParentSessionId, int nExtraId, std::string& sSessionId)
  {

  }

  void CSessionManager::Close(const std::string& sSessionId)
  {

  }

  bool CSessionManager::Validate(const std::string& sSessionId)
  {

  }

  void CSessionManager::Keepalive(const std::string& sSessionId)
  {

  }

  bool CSessionManager::GetUserId(const std::string& sSessionId, int& nUserId)
  {

  }

  bool CSessionManager::GetUserName(const std::string& sSessionId, std::string& sUserName)
  {

  }

  CSessionManager::CSessionManager()
  {
    m_pImpl = new CSessionManagerImpl;

    try
    {
      staff::security::CSessions& rSessions = staff::security::CSessions::Inst();
      staff::security::TSessionsList lsSessions;

      rSessions.Cleanup();
      rSessions.GetList(lsSessions);

      for (staff::security::TSessionsList::const_iterator itSession = lsSessions.begin();
            itSession != lsSessions.end(); ++itSession)
      {
        m_pImpl->mSessions[itSession->sSessionId] = *itSession;
      }
    }
    RISE_CATCH_ALL;
  }

  CSessionManager::~CSessionManager()
  {
    if (m_pImpl)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }


  CSessionManager* CSessionManager::m_pInst = NULL;
}
