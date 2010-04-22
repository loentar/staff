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

#include "staffsecurityexport.h"
#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    typedef std::list<int> TIntList; //!< int list

    //! users to groups relation
    class STAFF_SECURITY_EXPORT CUsersToGroups
    {
    public:
      //! get users to groups instance
      /*! \param users to groups instance
        */
      static CUsersToGroups& Inst();

      //! free users to groups instance
      static void FreeInst();

      //! get user groups
      /*! \param nUserId - user id
          \param rlsGroups - resulting user groups
          */
      void GetUserGroups(int nUserId, TIntList& rlsGroups);

      //! get users in group
      /*! \param nGroupId - group id
          \param rlsUsers - resulting users
          */
      void GetGroupUsers(int nGroupId, TIntList& rlsUsers);

      //! add user to group
      /*! \param nUserId - user id
          \param nGroupId - group id
          */
      void AddUserToGroup(int nUserId, int nGroupId);

      //! remove user from group
      /*! \param nUserId - user id
          \param nGroupId - group id
          */
      void RemoveUserFromGroup(int nUserId, int nGroupId);

      //! get is user member of group or not
      /*! \param nUserId - user id
          \param nGroupId - group id
          \return true if user is member of group
          */
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
