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

#ifndef _SECURITY_H_
#define _SECURITY_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#if defined(WIN32) || defined (_WINDOWS)
  #ifdef STAFF_SECURITY_EXPORTS
    #define STAFF_SECURITY_EXPORT __declspec(dllexport)
  #else
    #define STAFF_SECURITY_EXPORT __declspec(dllimport)
  #endif

  #define __LITTLE_ENDIAN 1234
  #define __BIG_ENDIAN  4321
  #define __BYTE_ORDER __LITTLE_ENDIAN
#else
  #define STAFF_SECURITY_EXPORT 

  #include <endian.h>
  #ifndef __BYTE_ORDER
    #error "__BYTE_ORDER IS NOT DEFINED" __LINE__ " " __FILE__
  #endif
#endif

#define STAFF_SECURITY_GUEST_SESSION_ID "00000000000000000000000000000000"

  typedef unsigned char byte; //!<  data type: byte

#ifndef __cplusplus
  typedef byte bool;
#define true 1
#define false 0
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#pragma pack(1)
  //! Permission
  struct SPermission
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    byte bRead : 1;      //!<  read
    byte bWrite : 1;     //!<  write
    byte bExecute : 1;   //!<  execute
    byte bDummy : 5;
#else
    byte bDummy : 5;
    byte bExecute : 1;
    byte bWrite : 1;
    byte bRead : 1;
#endif
  };
  typedef struct SPermission TPermission;

  //! Permissions
  struct SPermissions 
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned nOthers : 3;   //!<  others
    unsigned nGroup : 3;    //!<  group
    unsigned nUser : 3;     //!<  user
    unsigned nDummy : 23;   //!<  reserved
#else
    unsigned nDummy : 23;
    unsigned nUser : 3;
    unsigned nGroup : 3;
    unsigned nOthers : 3;
#endif
  };
  typedef struct SPermissions TPermissions;

  typedef enum //!<  Object type
  {
    EObjectTypeUnknown,     //!<  unknown
    EObjectTypeComponent,   //!<  component
    EObjectTypeService,     //!<  service
    EObjectTypeOperation,   //!<  operation
    EObjectTypeLast         //!<  last
  } EObjectType;

  //! Object
  struct SObject
  {
    int nObjectId;                   //!<  object id 
    char szObjectName[128];          //!<  object name
    char szDescription[128];         //!<  description
    int nUserId;                     //!<  user id
    int nGroupId;                    //!<  group id
    int nParentObjectId;             //!<  parent object id
    TPermission stPermissionUser;    //!<  permissions for user
    TPermission stPermissionGroup;   //!<  permissions for group
    TPermission stPermissionOthers;  //!<  permissions for others
    int nType;                       //!<  object type \sa EObjectType
  };
  typedef struct SObject TObject;

#pragma pack()

typedef enum
{
  EStaffSecurityErrorOK,
  EStaffSecurityErrorAccessDenied,
  EStaffSecurityErrorAlreadyExists,
  EStaffSecurityErrorInternal
} EStaffSecurityError;

//!         initialize security module(internal function)
/*! \param  szHost - DBMS host
    \param  szPort - DBMS port
    \param  szDSN - DB name
    \param  szUser - DB user
    \param  szPassword - DB user password
    \param  nSessionExpiration - session expiration
    \return true, if init successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityInit( const char* szHost, 
                                              const char* szPort, 
                                              const char* szDSN, 
                                              const char* szUser, 
                                              const char* szPassword, 
                                              int nSessionExpiration );

//!         tests module for init
/*! \return true if module initialized
*/
STAFF_SECURITY_EXPORT bool StaffSecurityIsInit();

//!         close security module(internal function)
STAFF_SECURITY_EXPORT void StaffSecurityFree();


//!         login user and open/get session
/*! \param  szUser - user name
    \param  szPassword - password
    \param  bCloseExisting - close existing session
    \param  szSessionId - result: session id
    \param  nSessionIdSize - szSessionId size
    \return EStaffSecurityError - result
    \sa     EStaffSecurityError
    */
STAFF_SECURITY_EXPORT EStaffSecurityError StaffSecurityOpenSession( const char* szUser,
                                                                  const char* szPassword,
                                                                  bool bCloseExisting,
                                                                  char* szSessionId,
                                                                  int nSessionIdSize );

//!         open/get an extra session for user
/*! \param  szExistingSessionId - existing session id
    \param  nExtraSessionId - extra session number
    \param  szSessionId - result: extra session id
    \param  nSessionIdSize - szExtraSessionId size
    \return EStaffSecurityError - result
    \sa     EStaffSecurityError
    */
STAFF_SECURITY_EXPORT EStaffSecurityError StaffSecurityOpenExtraSession( const char* szExistingSessionId,
                                                          int nExtraSessionId,
                                                          char* szSessionId,
                                                          int nSessionIdSize );

//!         close an extra session for user
/*! \param  szExistingSessionId - parent session id
    \param  nExtraSessionId - extra session number
    \return EStaffSecurityError - result
    \sa     EStaffSecurityError
    */
STAFF_SECURITY_EXPORT EStaffSecurityError StaffSecurityCloseExtraSession( const char* szExistingSessionId,
                                                                          int nExtraSessionId);

//!         logout user and close session
/*! if closing main session, all extra sessions will be deleted
    \param  szSessionId - session id
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityCloseSession( const char* szSessionId );

//!         validate session
/*! \param  szSessionId - session id
    \return true if session exists and not expired
    */
STAFF_SECURITY_EXPORT bool StaffSecurityValidateSessionID( const char* szSessionId );

//!         keep alive session
/*! \param  szSessionId - session id
    \return true if session exists and not expired
    */
STAFF_SECURITY_EXPORT bool StaffSecurityKeepAliveSession( const char* szSessionId );

//!         get user id by session id
/*! \param  szSessionId - session id
    \param  pnUserID - result: user id
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetUserIdBySessionId( const char* szSessionId, 
                                                              int* pnUserID);

//!         get user name by session id
/*! \param  szSessionId - session id
    \param  szUserName - result: user name
    \param  nUserNameSize - szUserName size
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetUserNameBySessionId( const char* szSessionId, 
                                                                char* szUserName, 
                                                                int nUserNameSize );

//!         is user member of group
/*! \param  nUserId - user id
    \param  nGroupId - group id
    \param  pnResult - true, if user is member of group
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityIsUserMemberOf(int nUserId, int nGroupId, int* pnResult);

// ------------------- access to objects -----------------------

//!         get object by id
/*! \param  nObjectId - object id
    \param  nObjectType - object type: EObjectType
    \param  pstObject - result: resulting object
    \return true if operation successes
    \sa EObjectType
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetObjectById( int nObjectId, 
                                                       int nObjectType, 
                                                       TObject* pstObject );

//!         get object by name
/*! \param  szObjectName - object name
    \param  nObjectType - object type: EObjectType
    \param  pnParentObjectId - pointer to parent object id (may be NULL)
    \param  pstObject - result: resulting object
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetObjectByName( const char* szObjectName, 
                                                         int nObjectType, 
                                                         const int* pnParentObjectId, 
                                                         TObject* pstObject );

//!         get object list by type
/*! \param  nObjectType - object type: EObjectType
    \param  pnParentObjectId - pointer to parent object id (may be NULL)
    \param  ppstObject - result: resulting object list
    \param  pnCount - result: ppstObject object count
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetObjectListByType( int nObjectType, 
                                                             const int* pnParentObjectId, 
                                                             TObject** ppstObject, 
                                                             int* pnCount );

STAFF_SECURITY_EXPORT void StaffSecurityFreeObjectList(TObject* pstObject);

//!         get user permission to object
/*! \param  pstObject - pointer to object
    \param  nUserId - user id
    \param  pstPermission - result: permission
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetPermissionForUser( const TObject* pstObject, 
                                                              int nUserId, 
                                                              TPermission* pstPermission );

//!         get user permission to component, service and operation
/*! check access to each component service, if no rights detected, return permissions of parent object
    \param  szService - service name (including component name example: samples.calc.Calculator)
    \param  szOperation - service operation (may be NULL)
    \param  nUserId - user id
    \param  pstPermission - result: resulting permission
    \return true if operation successes
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetUserPermissionForServiceOperation( const char* szService, 
                                                                              const char* szOperation, 
                                                                              int nUserId, 
                                                                              TPermission* pstPermission );

//!         get session expiration in minutes
/*! \return session expiration in minutes
*/
STAFF_SECURITY_EXPORT int StaffSecurityGetSessionExpiration();

//!         get error description
/*! \param  nError
    \return error description
  */
STAFF_SECURITY_EXPORT const char* StaffSecurityGetErrorStr(EStaffSecurityError eError);

#ifdef __cplusplus
}
#endif

#endif // _SECURITY_H_
