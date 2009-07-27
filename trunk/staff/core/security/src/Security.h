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

  typedef unsigned char byte; //!<  тип данных байт

#ifndef __cplusplus
  typedef byte bool;
#define true 1
#define false 0
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#pragma pack(1)
  //! структура "права" 
  struct SPermission
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    byte bRead : 1;      //!<  право на чтение
    byte bWrite : 1;     //!<  право на запись
    byte bExecute : 1;   //!<  право на выполнение
    byte bDummy : 5;
#else
    byte bDummy : 5;
    byte bExecute : 1;   //!<  право на выполнение
    byte bWrite : 1;     //!<  право на запись
    byte bRead : 1;      //!<  право на чтение
#endif
  };
  typedef struct SPermission TPermission;

  //! структура "разрешения"
  struct SPermissions 
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned nOthers : 3;   //!<  права остальным
    unsigned nGroup : 3;    //!<  права группе
    unsigned nUser : 3;     //!<  права пользователю
    unsigned nDummy : 23;   //!<  зарезервировано
#else
    unsigned nDummy : 23;
    unsigned nUser : 3;
    unsigned nGroup : 3;
    unsigned nOthers : 3;
#endif
  };
  typedef struct SPermissions TPermissions;

  enum EObjectType //!<  тип объекта
  {
    EObjectTypeUnknown,     //!<  неизвестный
    EObjectTypeComponent,   //!<  компонент
    EObjectTypeService,     //!<  сервис
    EObjectTypeOperation,   //!<  операция
    EObjectTypeLast         //!<  последний используемый
  };

  //! структура "объект"
  struct SObject
  {
    int nObjectId;                   //!<  идентификатор объекта
    char szObjectName[128];          //!<  имя объекта
    char szDescription[128];         //!<  описание
    int nUserId;                     //!<  идентификатор пользователя-владельца
    int nGroupId;                    //!<  идентификатор группы-владельца
    int nParentObjectId;             //!<  идентификатор родительского объекта
    TPermission stPermissionUser;    //!<  права доступа для пользователя
    TPermission stPermissionGroup;   //!<  права доступа для группы
    TPermission stPermissionOthers;  //!<  права доступа для остальных
    int nType;                       //!<  тип объекта \sa EObjectType
  };
  typedef struct SObject TObject;

#pragma pack()

//!         инициализация модуля безопасности
/*! \param  szHost - имя машины СУБД
    \param  szPort - порт СУБД
    \param  szDSN - имя БД
    \param  szUser - пользователь БД
    \param  szPassword - пароль пользователя
    \param  nSessionExpiration - истечение сессии в минутах
    \return статус инициализации, если успешно, true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityInit( const char* szHost, 
                                              const char* szPort, 
                                              const char* szDSN, 
                                              const char* szUser, 
                                              const char* szPassword, 
                                              int nSessionExpiration );

//!         проверка модуля безопасности на инициализацию
/*! \return статус инициализации, если успешно, true
*/
STAFF_SECURITY_EXPORT bool StaffSecurityIsInit();

//!         завершение работы с модулем безопасности
STAFF_SECURITY_EXPORT void StaffSecurityFree();

//!         открытие сессии(регистрация пользователя в системе)
/*! \param  szUser - имя пользователя
    \param  szPassword - пароль
    \param  szSessionId - результат - идентификатор созданной сессии
    \param  nSessionIdSize - размер буфера szSessionId
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityOpenSession( const char* szUser, 
                                                     const char* szPassword,
                                                     char* szSessionId,
                                                     int nSessionIdSize );

//!         открытие дополнительной сессии для пользователя
/*! \param  szExistingSessionId - идентификатор существующей сессии
    \param  nExtraSessionId - идентификатор(номер дополнительной сессии)
    \param  szSessionId - результат - идентификатор созданной сессии
    \param  nSessionIdSize - размер буфера szExtraSessionId
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityOpenExtraSession( const char* szExistingSessionId,
                                                          int nExtraSessionId,
                                                          char* szSessionId,
                                                          int nSessionIdSize );

//!         закрытие сессии(разрегистрация пользователя в системе)
/*! при закрытии главной сессии будут закрыты все дополнительные сессии
    \param  szSessionId - идентификатор сессии
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityCloseSession( const char* szSessionId );

//!         проверка сессии на существование
/*! \param  szSessionId - идентификатор сессии
    \return если операция выполнена успешно(сессия валидна): true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityValidateSessionID( const char* szSessionId );

//!         поддержание сессии
/*! \param  szSessionId - идентификатор сессии
    \return если операция выполнена успешно(сессия валидна): true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityKeepAliveSession( const char* szSessionId );

//!         получить идентификатор пользователя по идентификатору сессии
/*! \param  szSessionId - идентификатор сессии
    \param  pnUserID - результирующий идентификатор пользователя
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetUserIdBySessionId( const char* szSessionId, 
                                                              int* pnUserID);

//!         получить имя пользователя по идентификатору сессии
/*! \param  szSessionId - идентификатор сессии
    \param  szUserName - указатель на буфер куда будет записано имя пользователя
    \param  nUserNameSize - размер буфера
    \return 
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetUserNameBySessionId( const char* szSessionId, 
                                                                char* szUserName, 
                                                                int nUserNameSize );

// ------------------- права -----------------------

//!         получить объект по идентификатору
/*! \param  nObjectId - идентификатор
    \param  nObjectType - тип объекта EObjectType
    \param  pstObject - указатель на результирующий объект
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetObjectById( int nObjectId, 
                                                       int nObjectType, 
                                                       TObject* pstObject );

//!         получить объект по имени
/*! \param  szObjectName - имя объекта
    \param  nObjectType - тип объекта EObjectType
    \param  pnParentObjectId - указатель на идентификатор родительского объекта(может быть =NULL)
    \param  pstObject - указатель на результирующий объект
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetObjectByName( const char* szObjectName, 
                                                         int nObjectType, 
                                                         const int* pnParentObjectId, 
                                                         TObject* pstObject );

//!         получить список объектов по типу
/*! \param  nObjectType - тип объекта EObjectType
    \param  pnParentObjectId - указатель на идентификатор родительского объекта(может быть =NULL)
    \param  ppstObject - указатель на массив результирующих объектов
    \param  pnCount - указатель на количество прочитанных объектов
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetObjectListByType( int nObjectType, 
                                                             const int* pnParentObjectId, 
                                                             TObject** ppstObject, 
                                                             int* pnCount );

STAFF_SECURITY_EXPORT void StaffSecurityFreeObjectList(TObject* pstObject);

//!         получить права доступа пользователя к объекту
/*! \param  pstObject - указатель на объект
    \param  nUserId - идентификатор пользователя
    \param  pstPermission - право доступа
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetPermissionForUser( const TObject* pstObject, 
                                                              int nUserId, 
                                                              TPermission* pstPermission );

//!         получить права доступа пользователя к компоненту, сервису и операции
/*! проверяется доступ к компонентам сервиса, если права не определены, возвращаются права для родительского объекта
    \param  szService - имя сервиса(может содержать имя компонента. пример: component.service)
    \param  szOperation - операция(может == 0)
    \param  nUserId - идентификатор пользователя
    \param  pstPermission - результирующие права
    \return если операция выполнена успешно: true
    */
STAFF_SECURITY_EXPORT bool StaffSecurityGetUserPermissionForServiceOperation( const char* szService, 
                                                                              const char* szOperation, 
                                                                              int nUserId, 
                                                                              TPermission* pstPermission );

//!         получить время истечения сессии в минутах
/*! \return время истечения сессии в минутах
*/
STAFF_SECURITY_EXPORT int StaffSecurityGetSessionExpiration();

#ifdef __cplusplus
}
#endif

#endif // _SECURITY_H_
