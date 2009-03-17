#ifndef _ADMIN_H_
#define _ADMIN_H_

#include "Security.h"

#ifdef __cplusplus
extern "C" 
{
#endif

//! user
struct SUser
{
  int nId; //!<  user id
  char szName[64]; //!<  user name
  char szDescription[128]; //!< user description
};
typedef struct SUser TUser;

//! group
struct SGroup
{
  int nId; //!<  group id
  char szName[64];  //!<  group name
  char szDescription[128]; //!<  group description
};
typedef struct SGroup TGroup;


//!         get user list
/*! to free list use \sa StaffSecurityAdminFreeUsers
    \param  ppUsers - resulting list of users
    \param  pnCount - user count
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetUsers(TUser** ppUsers, int* pnCount);

//!         get group list
/*! to free list use \sa StaffSecurityAdminFreeGroups
    \param  ppGroups - resulting list of groups
    \param  pnCount - groups count
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetGroups(TGroup** ppGroups, int* pnCount);

//!         get user groups
/*! to free list use \sa StaffSecurityAdminFreeGroups
    \param  nUserId - user id
    \param  ppnGroups - resulting user groups
    \param  pnGroupsCount - groups count
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetUserGroups(int nUserId, int** ppnGroups, int* pnGroupsCount);

//!         adds user
/*! \param  szUserName - user name
    \param  szDescription - user description
    \param  pnUserId - resulting user id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminAddUser(const char* szUserName, const char* szDescription, int* pnUserId);

//!         remove user
/*! \param  nUserId - user id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminRemoveUser(int nUserId);

//!         set user password
/*! \param  nUserId - user id
    \param  szPass - new user password(md5)
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminSetUserPassword(int nUserId, const char* szPass);

//!         free user list
/*! \param  pUsers - user list
    */
STAFF_SECURITY_EXPORT void StaffSecurityAdminFreeUsers(TUser* pUsers);

//!         free groups list
/*! \param  pGroups - groups list
    */
STAFF_SECURITY_EXPORT void StaffSecurityAdminFreeGroups(TGroup* pGroups);

//!         free groups id list
/*! \param  pGroups - groups id list
    */
STAFF_SECURITY_EXPORT void StaffSecurityAdminFreeGroupIds(int* pGroups);


//!         add group
/*! \param  szGroupName - group name
    \param  szDescription - group description
    \param  pnGroupId - resulting group id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminAddGroup(const char* szGroupName, const char* szDescription, int* pnGroupId);

//!         remove group
/*! \param  nGroupId - group id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminRemoveGroup(int nGroupId);



//!         include user into group
/*! \param  nUserId - user id
    \param  nGroupId - group id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminAddUserToGroup(int nUserId, int nGroupId);

//!         include user into groups
/*! \param  nUserId - user id
    \param  pnGroupIds - list of groups
    \param  nGroupsCount - groups count
    \param  pnFailedCount - resulting count of fails(can be NULL)
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminAddUserToGroups(int nUserId, const int* pnGroupIds, int nGroupsCount, int* pnFailedCount);

//!         exclude user from group
/*! \param  nUserId - user id
    \param  nGroupId - group id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminRemoveUserFromGroup(int nUserId, int nGroupId);

//!         exclude user from groups
/*! \param  nUserId - user id
    \param  pnGroupIds - list of groups
    \param  nGroupsCount - count of groups
    \param  pnFailedCount - resulting count of fails(can be NULL)
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminRemoveUserFromGroups(int nUserId, const int* pnGroupIds, int nGroupsCount, int* pnFailedCount);

#ifdef __cplusplus
}
#endif

#endif // _ADMIN_H_