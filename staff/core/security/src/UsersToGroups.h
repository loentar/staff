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

#ifndef _USERSTOGROUPS_H_
#define _USERSTOGROUPS_H_

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    typedef std::list<int> TIntList;

    class CUsersToGroups
    {
    public:
      static CUsersToGroups& Inst();

      static void FreeInst();

      void GetUserGroups(int nUserId, TIntList& rlsGroups);

      void GetGroupUsers(int nGroupId, TIntList& rlsUsers);

      void AddUserToGroup(int nUserId, int nGroupId);

      void RemoveUserFromGroup(int nUserId, int nGroupId);

      bool IsUserMemberOfGroup(int nUserId, int nGroupId);

    private:
      CUsersToGroups();
      ~CUsersToGroups();

    private:
      static CUsersToGroups* m_pInst;
    };
  }
}

#endif // _USERSTOGROUPS_H_
