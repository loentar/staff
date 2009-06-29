#ifndef _ACCOUNTADMIN_H_
#define _ACCOUNTADMIN_H_

#include <string>

namespace staff
{
  namespace admin
  {
    //! list of identifiers
    typedef std::list<int> TIdList;

    //! user
    struct SUser
    {
      int nId; //!<  user id
      std::string sName; //!<  user name
      std::string sDescription; //!< user description
    };
    
    //! list of users
    typedef std::list<SUser> TUserList;

    //! group
    struct SGroup
    {
      int nId; //!<  group id
      std::string sName;  //!<  group name
      std::string sDescription; //!<  group description
    };

    //! list of groups
    typedef std::list<SGroup> TGroupList;

    //!  Account admin
    class CAccountAdmin
    {
    public:
      //!        destructor
      virtual ~CAccountAdmin() {}



      //!         get user list
      /*! \return user list
      */
      virtual TUserList GetUsers() = 0;

      //!         get group list
      /*! \return group list
      */
      virtual TGroupList GetGroups() = 0;

      //!         get user groups
      /*! \return user groups
      */
      virtual TIdList GetUserGroups(int nUserId) = 0;


      
      //!         adds user
      /*! \param  sUserName - user name
          \return id of created user
          */
      virtual int AddUser(const std::string& sUserName, const std::string& sDescription) = 0;
      
      //!         remove user
      /*! \param  nUserId - user id
          */
      virtual void RemoveUser(int nUserId) = 0;

      //!         set user password
      /*! \param  nUserId - user id
          \param  sPass - md5sum of user password
          */
      virtual void SetUserPassword(int nUserId, const std::string& sPass) = 0;



      //!         add group
      /*! \param  sGroupName - group name
          \return group id
          */
      virtual int AddGroup(const std::string& sGroupName, const std::string& sDescription) = 0;
      
      //!         remove group
      /*! \param  nGroupId - group id
          */
      virtual void RemoveGroup(int nGroupId) = 0;



      //!         include user into group
      /*! \param  nUserId - user id
          \param  nGroupId - group id
          */
      virtual void AddUserToGroup(int nUserId, int nGroupId) = 0;

      //!         include user into groups
      /*! \param  nUserId - user id
          \param  rlsGroupIds - list of groups
          */
      virtual void AddUserToGroups(int nUserId, const TIdList& rlsGroupIds) = 0;

      //!         exclude user from group
      /*! \param  nUserId - user id
          \param  nGroupId - group id
          */
      virtual void RemoveUserFromGroup(int nUserId, int nGroupId) = 0;

      //!         exclude user from groups
      /*! \param  nUserId - user id
          \param  rlsGroupIds - list of groups
          */
      virtual void RemoveUserFromGroups(int nUserId, const TIdList& rlsGroupIds) = 0;
    };
  }
}

#endif // _ACCOUNTADMIN_H_
