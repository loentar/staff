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
#include <staff/common/Exception.h>
#include <staff/utils/Thread.h>
#include <staff/utils/Log.h>
#include <staff/security/Sessions.h>
#include <staff/security/Time.h>
#include "ServiceInstanceManager.h"
#include "SessionManager.h"

namespace staff
{
  class SessionManager::SessionManagerImpl: public staff::Thread
  {
  public:
    typedef std::map<std::string, staff::security::Session> SessionMap;

    SessionManagerImpl():
      m_nSleepTime(1)
    {
    }

    void Start()
    {
      staff::security::Sessions& rSessions = staff::security::Sessions::Inst();
      staff::security::SessionsList lsSessions;

      rSessions.CloseExpiredSessions();
      rSessions.GetList(lsSessions);

      for (staff::security::SessionsList::const_iterator itSession = lsSessions.begin();
            itSession != lsSessions.end(); ++itSession)
      {
        m_mSessions[itSession->sSessionId] = *itSession;
        ServiceInstanceManager::Inst().CreateSession(itSession->sSessionId);
      }

      Thread::Start();
    }

    virtual void Run()
    {
      while (!IsStopping())
      {
        // ensure at least 1 sec between cleanup cycle
        Sleep(1000);

        if (m_mSessions.size() <= 1)
        {
          continue;
        }

        // get current time
        int nCurrentTime = staff::security::Time::Get();
        int nMinExpires = INT_MAX;

        // find expired sessions and calculate next expired session time
        for (SessionMap::iterator itSession = m_mSessions.begin();
              itSession != m_mSessions.end();)
        {
          if (itSession->second.nExpires <= nCurrentTime)
          {
            if (itSession->second.nId == -1)
            { // delayed session removal
              LogDebug1() << "Delayed session removal: [" << itSession->first << "]";
              // session already closed in db
            }
            else
            {
              // close expired session
              LogDebug1() << "Removing expired session: [" << itSession->first
                  << "]: " << itSession->second.nExpires << " <= " << nCurrentTime;

              staff::security::Sessions::Inst().Close(itSession->first);
            }

            //! TODO: service activity check is needed
            ServiceInstanceManager::Inst().FreeSession(itSession->first);

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
          LogDebug2() << "last session was removed";
          continue;
        }

        STAFF_DEBUG_ASSERT(nCurrentTime <= nMinExpires, "Error while calculating interval");

        m_nSleepTime = nMinExpires - nCurrentTime;
        LogDebug2() << "Sleep time: " << m_nSleepTime;
        while (--m_nSleepTime > 0)
        { // for delayed session closing
          if (IsStopping())
          {
            return;
          }
          Sleep(1000);
        }
      }

    }

    void OpenSession(const std::string& sSessionId)
    {
      staff::security::Sessions& rSessions = staff::security::Sessions::Inst();

      staff::security::Session stSession;
      rSessions.GetBySessionId(sSessionId, stSession);

      ServiceInstanceManager::Inst().CreateSession(sSessionId);
      m_mSessions[sSessionId] = stSession;
    }

    SessionMap m_mSessions;
    int m_nSleepTime;
  };

  SessionManager& SessionManager::Inst()
  {
    static SessionManager tInst;
    return tInst;
  }

  void SessionManager::Start()
  {
    m_pImpl->Start();
  }

  void SessionManager::Stop()
  {
    if (m_pImpl->IsRunning())
    {
      m_pImpl->Stop();
    }
  }

  void SessionManager::Login(const std::string& sUserName, const std::string& sPassword, std::string& sSessionId)
  {
    std::string sOldSessionId;

    if (staff::security::Sessions::Inst().GetSessionIdByUserNameAndPassword(sUserName, sPassword, sOldSessionId))
    {
      sSessionId = sOldSessionId;
      return;
    }

    staff::security::Sessions::Inst().Open(sUserName, sPassword, false, sSessionId);

    m_pImpl->OpenSession(sSessionId);
  }

  void SessionManager::Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId)
  {
    std::string sOldSessionId;
    bool bOldSessionExists = false;

    if (bCloseExisting)
    {
      bOldSessionExists = staff::security::Sessions::Inst().GetSessionIdByUserName(sUserName, sOldSessionId);
    }

    staff::security::Sessions::Inst().Open(sUserName, sPassword, bCloseExisting, sSessionId);

    if (bOldSessionExists)
    {
      LogDebug2() << "Freeing old session: " << sOldSessionId;
      ServiceInstanceManager::Inst().FreeSession(sOldSessionId);
      m_pImpl->m_mSessions.erase(sOldSessionId);
    }

    m_pImpl->OpenSession(sSessionId);
  }

  void SessionManager::Close(const std::string& sSessionId)
  {
    LogDebug2() << "Closing session [" << sSessionId << "]";

    staff::security::Sessions::Inst().Close(sSessionId);

    // don't remove all service instances bound to this session
    // only decrease expires time to next second,
    // because this function may be called from service,
    // what may cause self-destruction of service
    SessionManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    if (itSession == m_pImpl->m_mSessions.end())
    {
      LogWarning() << "Attempt to close non-existing session";
    }
    else
    {
      // deletes session after 1 sec
      itSession->second.nExpires = staff::security::Time::Get() + 1;
      // mark session as delayed for close
      itSession->second.nId = -1;
      m_pImpl->m_nSleepTime = 0;
    }
  }

  bool SessionManager::IsOpened(const std::string& sSessionId)
  {
    return m_pImpl->m_mSessions.find(sSessionId) != m_pImpl->m_mSessions.end();
  }

  bool SessionManager::Validate(const std::string& sSessionId)
  {
    return staff::security::Sessions::Inst().Validate(sSessionId);
  }

  int SessionManager::GetExpiresIn(const std::string& sSessionId)
  {
    SessionManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    STAFF_ASSERT(itSession != m_pImpl->m_mSessions.end(), "Session with id [" + sSessionId +
                 "] does not exists");

    return itSession->second.nExpires - staff::security::Time::Get();
  }

  int SessionManager::Keepalive(const std::string& sSessionId)
  {
    staff::security::Sessions& rSessions = staff::security::Sessions::Inst();
    rSessions.Keepalive(sSessionId);

    SessionManagerImpl::SessionMap::iterator itSession = m_pImpl->m_mSessions.find(sSessionId);
    STAFF_ASSERT(itSession != m_pImpl->m_mSessions.end(), "Session with id [" + sSessionId +
                 "] does not exists");

    itSession->second.nExpires = rSessions.GetExpiresById(itSession->second.nId);

    return itSession->second.nExpires - staff::security::Time::Get();
  }


  SessionManager::SessionManager()
  {
    m_pImpl = new SessionManagerImpl;
  }

  SessionManager::~SessionManager()
  {
    if (m_pImpl)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

}
