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
