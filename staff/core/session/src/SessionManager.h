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

#ifndef _SESSIONMANAGER_H_
#define _SESSIONMANAGER_H_

#include <string>

namespace staff
{
  //! session manager
  class CSessionManager
  {
  public:
    //!         get session manager instance
    /*! \return session manager instance
    */
    static CSessionManager& Inst();

    static void FreeInst();

    //!         login and create session
    /*! this operation must be called from guest session
        \param  sSessionId - session id
        */
    void Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId);

    //!         open extra session
    /*! \param  nExtraSessionId - extra session id
        */
    void OpenExtra(const std::string& sParentSessionId, int nExtraId, std::string& sSessionId);

    //!         close session
    /*! \param  sSessionId - session id
      */
    void Close(const std::string& sSessionId);

    //!         is session opened
    /*! \param  sSessionId - (extra)session id
        */
    bool Validate(const std::string& sSessionId);

    void Keepalive(const std::string& sSessionId);

    bool GetUserId(const std::string& sSessionId, int& nUserId);

    bool GetUserName(const std::string& sSessionId, std::string& sUserName);

  private:
    CSessionManager();
    ~CSessionManager();

  private:
    class CSessionManagerImpl;
    CSessionManagerImpl* m_pImpl;
    static CSessionManagerImpl* m_pInst;
  };
}

#endif // _SESSIONMANAGER_H_
