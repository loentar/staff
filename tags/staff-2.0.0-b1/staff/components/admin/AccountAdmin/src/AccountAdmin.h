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

#ifndef _ACCOUNTADMIN_H_
#define _ACCOUNTADMIN_H_

#include <string>
#include <list>
#include <staff/common/IService.h>

namespace staff
{
  namespace admin
  {
    //! list of identifiers
    typedef std::list<int> IdList;

    //! user
    struct User
    {
      int nId; //!<  user id
      std::string sName; //!<  user name
      std::string sDescription; //!< user description
    };
    
    //! list of users
    typedef std::list<User> UserList;

    //! group
    struct Group
    {
      int nId; //!<  group id
      std::string sName;  //!<  group name
      std::string sDescription; //!<  group description
    };

    //! list of groups
    typedef std::list<Group> GroupList;

    //!  Account admin
    class AccountAdmin: public staff::IService
    {
    public:
      //! destructor
      virtual ~AccountAdmin() {}



      //! get user list
      /*! \return user list
      */
      virtual UserList GetUsers() = 0;

      //! get group list
      /*! \return group list
      */
      virtual GroupList GetGroups() = 0;

      //! get user groups
      /*! \return user groups
      */
      virtual IdList GetUserGroups(int nUserId) = 0;


      
      //! adds user
      /*! \param  sUserName - user name
          \return id of created user
          */
      virtual int AddUser(const std::string& sUserName, const std::string& sDescription) = 0;
      
      //! remove user
      /*! \param  nUserId - user id
          */
      virtual void RemoveUser(int nUserId) = 0;

      //! set user password
      /*! \param  nUserId - user id
          \param  sPass - md5sum of user password
          */
      virtual void SetUserPassword(int nUserId, const std::string& sPass) = 0;



      //! add group
      /*! \param  sGroupName - group name
          \return group id
          */
      virtual int AddGroup(const std::string& sGroupName, const std::string& sDescription) = 0;
      
      //! remove group
      /*! \param  nGroupId - group id
          */
      virtual void RemoveGroup(int nGroupId) = 0;



      //! include user into group
      /*! \param  nUserId - user id
          \param  nGroupId - group id
          */
      virtual void AddUserToGroup(int nUserId, int nGroupId) = 0;

      //! include user into groups
      /*! \param  nUserId - user id
          \param  rlsGroupIds - list of groups
          */
      virtual void AddUserToGroups(int nUserId, const IdList& rlsGroupIds) = 0;

      //! exclude user from group
      /*! \param  nUserId - user id
          \param  nGroupId - group id
          */
      virtual void RemoveUserFromGroup(int nUserId, int nGroupId) = 0;

      //! exclude user from groups
      /*! \param  nUserId - user id
          \param  rlsGroupIds - list of groups
          */
      virtual void RemoveUserFromGroups(int nUserId, const IdList& rlsGroupIds) = 0;
    };
  }
}

#endif // _ACCOUNTADMIN_H_
