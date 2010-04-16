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

//! object type
struct SObjectType
{
  int nId; //!<  object type id
  char szName[64];  //!<  type name
  char szDescription[128]; //!<  type description
};
typedef struct SObjectType TObjectType;

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

//////////////////////////////////////////////////////////////////////////
// objects admin

//!         get object list of id
/*! \param  ppnObjectIds - list of object ids
    \param  pnObjectsCount - returns count of ids
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetObjectIdList(int** ppnObjectIds, int* pnObjectsCount);

//!         get object list of id
/*! \param  ppnObjectIds - list of object ids
    \param  nParentId - parent object id
    \param  pnObjectsCount - returns count of ids
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetObjectIdListWithParent(int** ppnObjectIds, int nParentId, int* pnObjectsCount);

//!         get object by id
/*! \param  nObjectId - object id
    \param  pstObject - pointer to object struct
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetObjectById(int nObjectId, TObject* pstObject);

//!         get object list by id list
/*! \param  pnObjectIds - list of objects ids
    \param  nObjectIdsCount - count of object ids
    \param  ppstObjects - returning list of objects
    \param  pnObjectsCount - resulting objects count
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetObjectList(const int* pnObjectIds, int nObjectIdsCount, TObject** ppstObjects, int* pnObjectsCount);

//!         add new object
/*! TObject::nObjectId is ignored
    \param  pstObject - pointer to new object
    \param  pnObjectId - pointer to added object id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminAddObject(const TObject* pstObject, int* pnObjectId);


//!         remove object by id
/*! \param  nObjectId - object id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminRemoveObject(int nObjectId);

//!         replace object
/*! \param  pstObject - pointer to object
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminReplaceObject(const TObject* pstObject);

//!         free objects id list
/*! \param  pObjects - objects id list
    */
STAFF_SECURITY_EXPORT void StaffSecurityAdminFreeObjectIds(int* pObjects);


//!         get object type list
/*! \param  ppstObjectTypes - resulting list of objects
    \param  pnObjectTypesCount - resulting list size
    \return 
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminGetObjectTypeList(TObjectType** ppstObjectTypes, int* pnObjectTypesCount);

//!         add new type
/*! TObjectType::nId is ignored
    \param  pstObjectType - pointer to new type
    \param  pnObjectTypeId - pointer to added type id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminAddObjectType(const TObjectType* pstObjectType, int* pnObjectTypeId);


//!         remove type by id
/*! \param  nObjectTypeId - type id
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminRemoveObjectType(int nObjectTypeId);

//!         replace type
/*! \param  pstObjectType - pointer to type
    \return true, if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityAdminReplaceObjectType(const TObjectType* pstObjectType);

//!         free objects id list
/*! \param  pObjectTypes - objects id list
    */
STAFF_SECURITY_EXPORT void StaffSecurityAdminFreeObjectTypes(TObjectType* pObjectTypes);

#ifdef __cplusplus
}
#endif

#endif // _ADMIN_H_
