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

#include <limits.h>
#include <map>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/threading/Thread.h>
#include <staff/security/Sessions.h>
#include <staff/security/Time.h>
#include "ServiceInstanceManager.h"
#include "SessionManager.h"

namespace staff
{
  class CSessionManager::CSessionManagerImpl: public rise::threading::CThread
  {
  public:
    typedef std::map<std::string, staff::security::SSession> TSessionMap;

    virtual void Run(void*)
    {
      {
        staff::security::CSessions& rSessions = staff::security::CSessions::Inst();
        staff::security::TSessionsList lsSessions;

        rSessions.CloseExpiredSessions();
        rSessions.GetList(lsSessions);

        for (staff::security::TSessionsList::const_iterator itSession = lsSessions.begin();
              itSession != lsSessions.end(); ++itSession)
        {
          m_mSessions[itSession->sSessionId] = *itSession;
          CServiceInstanceManager::Inst().CreateSession(itSession->sSessionId);
        }
      }

      while (!IsStopping())
      {
        if (m_mSessions.size() <= 1)
        {
          Sleep(1000);
          continue;
        }

        // get current time
        int nCurrentTime = staff::security::CTime::Get();
        int nMinExpires = INT_MAX;

        // find expired sessions and calculate next expired session time
        for (TSessionMap::iterator itSession = m_mSessions.begin();
              itSession != m_mSessions.end();)
        {
          if (itSession->second.nExpires <= nCurrentTime)
          { // close expired session
            rise::LogDebug1() << "Removing expired session: [" << itSession->first
                << "]: " << itSession->second.nExpires << " <= " << nCurrentTime;
            CloseSession(itSession->first);
            m_mSessions.erase(itSession++); // safe remove
            continue;
          }
          else
          if (itSession->second.nExpires < nMinExpires)
          {
            nMinExpires = itSession->second.nExpires;
          }
          ++itSession;
        }

        if (nMinExpires == INT_MAX)
        { // last session was removed
          rise::LogDebug2() << "last session was removed";
          continue;
        }

#ifdef DEBUG
        RISE_ASSERT(nCurrentTime <= nMinExpires);
#endif

        rise::LogDebug2() << "Sleep time: " << (nMinExpires - nCurrentTime);
        Sleep(1000 * (nMinExpires - nCurrentTime));
      }

    }

    void OpenSession(const std::string& sSessionId)
    {
      staff::security::CSessions& rSessions = staff::security::CSessions::Inst();

      staff::security::SSession stSession;
      rSessions.GetBySessionId(sSessionId, stSession);

      CServiceInstanceManager::Inst().CreateSession(sSessionId);
      m_mSessions[sSessionId] = stSession;
    }

    void CloseSession(const std::string& sSessionId)
    {
      CServiceInstanceManager::Inst().FreeSession(sSessionId);
      staff::security::CSessions::Inst().Close(sSessionId);
    }

    TSessionMap m_mSessions;
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

  void CSessionManager::Start()
  {
    m_pImpl->Start();
  }

  void CSessionManager::Stop()
  {
    m_pImpl->Stop();
  }

  void CSessionManager::Login(const std::string& sUserName, const std::string& sPassword, std::string& sSessionId)
  {
    std::string sOldSessionId;

    if (staff::security::CSessions::Inst().GetSessionIdByUserNameAndPassword(sUserName, sPassword, sOldSessionId))
    {
      sSessionId = sOldSessionId;
      return;
    }

    staff::security::CSessions::Inst().Open(sUserName, sPassword, false, sSessionId);

    m_pImpl->OpenSession(sSessionId);
  }

  void CSessionManager::Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId)
  {
    std::string sOldSessionId;
    bool bOldSessionExists = false;

    if (bCloseExisting)
    {
      bOldSessionExists = staff::security::CSessions::Inst().GetSessionIdByUserName(sUserName, sOldSessionId);
    }

    staff::security::CSessions::Inst().Open(sUserName, sPassword, bCloseExisting, sSessionId);

    if (bOldSessionExists)
    {
      rise::LogDebug2() << "Freeing old session: " << sOldSessionId;
      CServiceInstanceManager::Inst().FreeSession(sOldSessionId);
      m_pImpl->m_mSessions.erase(sOldSessionId);
    }

    m_pImpl->OpenSession(sSessionId);
  }

  void CSessionManager::Close(const std::string& sSessionId)
  {
    m_pImpl->CloseSession(sSessionId);
    m_pImpl->m_mSessions.erase(sSessionId);
  }

  bool CSessionManager::IsOpened(const std::string& sSessionId)
  {
    return m_pImpl->m_mSessions.find(sSessionId) != m_pImpl->m_mSessions.end();
  }

  bool CSessionManager::Validate(const std::string& sSessionId)
  {
    return staff::security::CSessions::Inst().Validate(sSessionId);
  }

  void CSessionManager::Keepalive(const std::string& sSessionId)
  {
    staff::security::CSessions& rSessions = staff::security::CSessions::Inst();
    rSessions.Keepalive(sSessionId);

    CSessionManagerImpl::TSessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    RISE_ASSERT(itSession != m_pImpl->m_mSessions.end());

    itSession->second.nExpires = rSessions.GetExpiresById(itSession->second.nId);
  }

  CSessionManager::CSessionManager()
  {
    m_pImpl = new CSessionManagerImpl;
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
