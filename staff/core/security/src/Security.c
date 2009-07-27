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

#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#else
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <libpq-fe.h>
#include "Security.h"
#include "Admin.h"

#ifdef WIN32
  typedef CRITICAL_SECTION TLock;
  typedef LPCRITICAL_SECTION PLock;
#else
  typedef pthread_mutex_t TLock;
  typedef pthread_mutex_t* PLock;
#endif


PGconn* g_pConn = NULL;
TLock g_tLock;
int g_nSessionExpiration = 0;

#define dprintf printf("%s[%d]: ", GetBaseFile(__FILE__), __LINE__); printf

#define STAFF_SECURITY_ASSERT(expression) \
  if(!(expression)) { dprintf("ERROR ASSERTING: %s\n", #expression); return false; }

const char* GetBaseFile( const char* szFilePath )
{
  const char* szFile = strrchr(szFilePath, 
#ifdef WIN32
    '\\'
#else
    '/'
#endif
    );

  if (szFile)
  {
    return szFile + 1;
  }
  else
  {
    return szFilePath;
  }
}

void InitializeLock(PLock pLock)
{
#ifdef WIN32
  InitializeCriticalSection(pLock);
#else
  pthread_mutexattr_t tAttr;
  pthread_mutexattr_settype(&tAttr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init(pLock, &tAttr);
#endif
}

void DeleteLock(PLock pLock)
{
#ifdef WIN32
  DeleteCriticalSection(pLock);
#else
  pthread_mutex_destroy(pLock);
#endif
}

void EnterLock(PLock pLock)
{
#ifdef WIN32
  EnterCriticalSection(pLock);
#else
  pthread_mutex_lock(pLock);
#endif
}

void LeaveLock(PLock pLock)
{
#ifdef WIN32
  LeaveCriticalSection(pLock);
#else
  pthread_mutex_unlock(pLock);
#endif
}

PGresult* PQexecLock(PGconn *conn, const char *query)
{
  EnterLock(&g_tLock);
  return PQexec(conn, query);
}

PGresult* PQexecParamsLock(PGconn *conn, 
 const char *command,
 int nParams,
 const Oid *paramTypes,
 const char *const * paramValues,
 const int *paramLengths,
 const int *paramFormats,
 int resultFormat)
{
  EnterLock(&g_tLock);
  return PQexecParams(conn, command, nParams, paramTypes, paramValues, paramLengths, paramFormats, resultFormat);
}

#define PQclearLock LeaveLock(&g_tLock); PQclear

//////////////////////////////////////////////////////////////////////////////////////////////////

bool StaffSecurityInit( const char* szHost, 
                        const char* szPort,
                        const char* szDSN,
                        const char* szUser,
                        const char* szPassword,
                        int nSessionExpiration )
{
  InitializeLock(&g_tLock);
  g_pConn = PQsetdbLogin(szHost, szPort, "", "", szDSN, szUser, szPassword);
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(PQstatus(g_pConn) == CONNECTION_OK);
  g_nSessionExpiration = nSessionExpiration;

  return true;
}

void StaffSecurityFree()
{
  if (g_pConn)
  {
    PQfinish(g_pConn);
    g_pConn = NULL;
    DeleteLock(&g_tLock);
  }
}

bool CreateSession(int nContextId, char* szSessionId, int nSessionIdSize, bool bRemoveOld, int nExtraSessionId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  char* pResult = NULL;
  int nSize = 0;
  int nContextIdReq = htonl(nContextId);

  if(bRemoveOld)
  {
    if(nExtraSessionId == 0)
    {
      int anParamLengths[1] = { sizeof(nContextIdReq) };
      int anParamFormats[1] = { 1 };
      const char* aszParams[1] = { (const char*)&nContextIdReq };

      // delete old sessions
      pPGResult = PQexecParamsLock(g_pConn, 
        "delete from \"session\" where \"contextid\" = $1::int4;",
        1, NULL, aszParams, anParamLengths, anParamFormats, 0);

      PQclearLock(pPGResult);
    }
    else
    {
      int nExtraSessionIdReq = htonl(nExtraSessionId);
      int anParamLengths[2] = { sizeof(nContextIdReq), sizeof(nExtraSessionIdReq) };
      int anParamFormats[2] = { 1, 1 };
      const char* aszParams[2] = { (const char*)&nContextIdReq, (const char*)&nExtraSessionIdReq };

      // delete old session
      pPGResult = PQexecParamsLock(g_pConn, 
        "delete from \"session\" where \"contextid\" = $1::int4 and \"extraid\" = $2::int4;",
        2, NULL, aszParams, anParamLengths, anParamFormats, 0);

      PQclearLock(pPGResult);
    }
  }

  // create session id
  pPGResult = PQexecLock(g_pConn, "select md5(cast (now() as text));");
  
  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
  {
    dprintf("failed to create new session(get created session): %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);
  if(!pResult)
  {
    dprintf("error getting result\n");
    PQclearLock(pPGResult);
    return false;
  }

  nSize = min(nSessionIdSize - 1, (int)strlen(pResult));
  strncpy(szSessionId, pResult, nSize);
  szSessionId[nSize] = '\0';
  PQclearLock(pPGResult);

  dprintf("new session id: %s\n", szSessionId);

  {
    int nExtraSessionIdReq = htonl(nExtraSessionId);
    int anParamLengths[3] = { nSize, sizeof(nContextIdReq), sizeof(nExtraSessionIdReq) };
    int anParamFormats[3] = { 0, 1, 1 };
    const char* aszParams[3] = { szSessionId, (const char*)&nContextIdReq, (const char*)&nExtraSessionIdReq };

    // create new session
    pPGResult = PQexecParamsLock(g_pConn, 
      "insert into \"session\"(\"sid\",\"contextid\",\"extraid\") values($1::text,$2::int4,$3::int4);",
      3, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("failed to create new session: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

bool GetUserIdByNameAndPasswd( const char* szUser, 
                               const char* szPassword,
                               int* pnUserId )
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;

  int anParamLengths[2] = { (int)strlen(szUser), (int)strlen(szPassword) };
  int anParamFormats[2] = { 0, 0 };
  const char* aszParams[2] = { szUser, szPassword };

  pPGResult = PQexecParamsLock(g_pConn, "select \"userid\" from \"users\" "
    "where \"username\"=$1 and \"password\"=$2;",
    2, NULL, aszParams, anParamLengths, anParamFormats, 1);

  tQueryStatus = PQresultStatus(pPGResult);

  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  if(PQntuples(pPGResult) <= 0)
  {
    dprintf("can't find user/password: %s\n", szUser);
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);
  if(!pResult)
  {
    dprintf("error getting result\n");
    PQclearLock(pPGResult);
    return false;
  }

  *pnUserId = ntohl(*((unsigned*)pResult));
  PQclearLock(pPGResult);

  return true;
}

bool GetContextIdByUserId(int nUserId, int* pnContextId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nUserIdReq = htonl(nUserId);
  int anParamLengths[1] = { sizeof(nUserIdReq) };
  int anParamFormats[1] = { 1 };
  const char* aszParams[1] = { (const char*)&nUserIdReq };

  pPGResult = PQexecParamsLock(g_pConn, "select \"contextid\" from \"context\" where \"userid\"=$1;",
    1, NULL, aszParams, anParamLengths, anParamFormats, 1);

  tQueryStatus = PQresultStatus(pPGResult);

  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  if(PQntuples(pPGResult) <= 0)
  {
    dprintf("can't find context for user: %d\n", nUserId);
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);
  if(!pResult)
  {
    dprintf("error getting result\n");
    PQclearLock(pPGResult);
    return false;
  }

  *pnContextId = ntohl(*((unsigned*)pResult));
  PQclearLock(pPGResult);

  return true;
}

bool GetSessionByContextId(int nContextId, int nExtraSessionId, char* szSessionId, int nSessionIdSize)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nExtraSessionIdReq = htonl(nExtraSessionId);
  int nSize = 0;
  int nContextIdReq = htonl(nContextId);
  int anParamLengths[2] = { sizeof(nContextIdReq), sizeof(nExtraSessionIdReq) };
  int anParamFormats[2] = { 1, 1 };
  const char* aszParams[2] = { (const char*)&nContextIdReq, (const char*)&nExtraSessionIdReq };

  pPGResult = PQexecParamsLock(g_pConn, "select \"sid\" from \"session\" where \"contextid\" = $1::int4 and \"extraid\" = $2::int4;",
    2, NULL, aszParams, anParamLengths, anParamFormats, 0);

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  if(PQntuples(pPGResult) <= 0)
  {
    dprintf("can't get session for context: %d\n", nContextId);
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);
  if(!pResult)
  {
    dprintf("error getting result\n");
    PQclearLock(pPGResult);
    return false;
  }

  nSize = min(nSessionIdSize - 1, (int)strlen(pResult));
  strncpy(szSessionId, pResult, nSize);
  szSessionId[nSize] = '\0';

  PQclearLock(pPGResult);

  return true;
}

bool StaffSecurityOpenSession( const char* szUser, 
                               const char* szPassword,
                               char* szSessionId,
                               int nSessionIdSize )
{
  int nUserId = -1;
  int nContextId = -1;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szUser);
  STAFF_SECURITY_ASSERT(szPassword);

  if(!GetUserIdByNameAndPasswd(szUser, szPassword, &nUserId))
  {
    dprintf("error authenticating user: %s\n", szUser);
    return false;
  }

  if(!GetContextIdByUserId(nUserId, &nContextId))
  {
    dprintf("error getting context for user: %s\n", szUser);
    return false;
  }

  if(!GetSessionByContextId(nContextId, 0, szSessionId, nSessionIdSize))
  {
    if(!CreateSession(nContextId, szSessionId, nSessionIdSize, true, 0))
    {
      dprintf("error creating session for user: %s\n", szUser);
      return false;
    }
  }

  return true;
}


bool StaffSecurityOpenExtraSession( const char* szExistingSessionId,
                                    int nExtraSessionId,
                                    char* szSessionId,
                                    int nSessionIdSize )
{
  int nUserId = -1;
  int nContextId = -1;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szExistingSessionId);
  STAFF_SECURITY_ASSERT(nExtraSessionId > 0);
  STAFF_SECURITY_ASSERT(szSessionId);

  if(!StaffSecurityGetUserIdBySessionId(szExistingSessionId, &nUserId))
  {
    dprintf("error validating session: %s\n", szExistingSessionId);
    return false;
  }

  if(!GetContextIdByUserId(nUserId, &nContextId))
  {
    dprintf("error getting context for user: %d\n", nUserId);
    return false;
  }

  if(!GetSessionByContextId(nContextId, nExtraSessionId, szSessionId, nSessionIdSize))
  {
    if(!CreateSession(nContextId, szSessionId, nSessionIdSize, false, nExtraSessionId))
    {
      dprintf("error creating session for user: %d\n", nUserId);
      return false;
    }
  }

  return true;
}

bool StaffSecurityCloseSession( const char* szSessionId )
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nExtraId = -1;
  int nContextId = -1;

  int anParamLengths[1] = { (int)strlen(szSessionId) };
  int anParamFormats[1] = { 0 };
  const char* aszParams[1] = { szSessionId };

  STAFF_SECURITY_ASSERT(szSessionId);

  pPGResult = PQexecParamsLock(g_pConn, "select \"extraid\", \"contextid\" from \"session\" where \"sid\" = $1;",
    1, NULL, aszParams, anParamLengths, anParamFormats, 1);

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);
  if(!pResult)
  {
    dprintf("error getting result\n");
    PQclearLock(pPGResult);
    return false;
  }
  
  nExtraId = ntohl(*((unsigned*)pResult));
  
  pResult = PQgetvalue(pPGResult, 0, 1);
  if(!pResult)
  {
    dprintf("error getting result\n");
    PQclearLock(pPGResult);
    return false;
  }
  
  nContextId = ntohl(*((unsigned*)pResult));
  
  PQclearLock(pPGResult);
  
  dprintf("extra: %d, context: %d", nExtraId, nContextId);

  // delete session only
  if(nExtraId != 0)  
  {
    pPGResult = PQexecParamsLock(g_pConn, "delete from \"session\" where \"sid\" = $1;",
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }
  else // delete session and subsessions
  {
    int nContextIdReq = htonl(nContextId);
    int anParamLengths[2] = { sizeof(nContextIdReq), (int)strlen(STAFF_SECURITY_GUEST_SESSION_ID) };
    int anParamFormats[2] = { 1, 0 };
    const char* aszParams[2] = { (const char*)&nContextIdReq, STAFF_SECURITY_GUEST_SESSION_ID };

    pPGResult = PQexecParamsLock(g_pConn, "delete from \"session\" where \"contextid\" = $1::int4 and \"sid\" != $2;",
      2, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

bool StaffSecurityValidateSessionID( const char* szSessionId )
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int anParamLengths[2] = { (int)strlen(szSessionId), sizeof(g_nSessionExpiration) };
  int anParamFormats[2] = { 0, 1 };
  const char* aszParams[2] = { szSessionId, (const char*)&g_nSessionExpiration };

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szSessionId);

  pPGResult = PQexecParamsLock(g_pConn,
    "select \"sid\" from \"session\" where \"sid\" = $1 and "
        "\"time\" > (now() - (select cast (($2::int4 || ' minutes') as interval)));",
    2, NULL, aszParams, anParamLengths, anParamFormats, 0);

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
  {
    dprintf("error getting result: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);

  if(pResult == NULL)
  {
    dprintf("error getting value: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  // strcmp(pResult, szSessionId);

  PQclearLock(pPGResult);

  return true;
}

bool StaffSecurityKeepAliveSession( const char* szSessionId )
{
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szSessionId);

  {
    PGresult* pPGResult = NULL;
    int anParamLengths[2] = { (int)strlen(szSessionId), sizeof(g_nSessionExpiration) };
    int anParamFormats[2] = { 0, 1 };
    const char* aszParams[2] = { szSessionId, (const char*)&g_nSessionExpiration };

    pPGResult = PQexecParamsLock(g_pConn, 
      "update \"session\" set \"time\" = DEFAULT where \"sid\" = $1 and "
        "\"time\" > (now() - (select cast (($2::int4 || ' minutes') as interval)));",
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);
    if (PQresultStatus(pPGResult) != PGRES_COMMAND_OK)
    {
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }
  return true;
}

bool StaffSecurityGetUserIdBySessionId(const char* szSessionId, int* pnUserId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;

  const char* pResult = NULL;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szSessionId && szSessionId[0]);
  STAFF_SECURITY_ASSERT(pnUserId);

  if (g_nSessionExpiration > 0)
  {
    int anParamLengths[2] = { (int)strlen(szSessionId), sizeof(g_nSessionExpiration) };
    int anParamFormats[2] = { 0, 1 };
    const char* aszParams[2] = { szSessionId, (const char*)&g_nSessionExpiration };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"userid\" from \"context\" where \"contextid\" = "
      "( select \"contextid\" from \"session\" "
          "where \"sid\" = $1 and "
                "\"time\" > (now() - (select cast (($2::int4 || ' minutes') as interval))));",
      2, NULL, aszParams, anParamLengths, anParamFormats, 1);
  } 
  else
  {
    int anParamLengths[1] = { (int)strlen(szSessionId) };
    int anParamFormats[1] = { 0 };
    const char* aszParams[1] = { szSessionId };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"userid\" from \"context\" where \"contextid\" = "
      "( select \"contextid\" from \"session\" where \"sid\" = $1);",
      1, NULL, aszParams, anParamLengths, anParamFormats, 1);
  }

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
  {
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);

  if(pResult == NULL)
  {
    dprintf("error getting value\n");
    PQclearLock(pPGResult);
    return false;
  }

  *pnUserId = ntohl(*((unsigned*)pResult));
  PQclearLock(pPGResult);
  
  STAFF_SECURITY_ASSERT(*pnUserId >= 0);

  return true;
}

bool StaffSecurityGetUserNameBySessionId( const char* szSessionId, 
                                          char* szUserName, 
                                          int nUserNameSize )
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  int nSize = 0;
  const char* pResult = NULL;
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szSessionId);
  STAFF_SECURITY_ASSERT(szUserName);

  if (g_nSessionExpiration > 0)
  {
    int anParamLengths[2] = { (int)strlen(szSessionId), sizeof(g_nSessionExpiration) };
    int anParamFormats[2] = { 0, 1 };
    const char* aszParams[2] = { szSessionId, (const char*)&g_nSessionExpiration };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"username\" from \"users\" where \"userid\" = ("
      "select \"userid\" from \"context\" where \"contextid\" = "
        "( select \"contextid\" from \"session\" "
          "where \"sid\" = $1 and \"time\" > (now() - (select cast (($2::int4 || ' minutes') as interval)))) );",
      2, NULL, aszParams, anParamLengths, anParamFormats, 0);
  } 
  else
  {
    int anParamLengths[1] = { (int)strlen(szSessionId) };
    int anParamFormats[1] = { 0 };
    const char* aszParams[1] = { szSessionId };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"username\" from \"users\" where \"userid\" = ("
        "select \"userid\" from \"context\" where \"contextid\" = "
        "( select \"contextid\" from \"session\" where \"sid\" = $1) );",
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);
  }

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
  {
    PQclearLock(pPGResult);
    return false;
  }

  pResult = PQgetvalue(pPGResult, 0, 0);

  if(pResult == NULL)
  {
    dprintf("error getting value\n");
    PQclearLock(pPGResult);
    return false;
  }

  nSize = min(nUserNameSize - 1, (int)strlen(pResult));
  strncpy(szUserName, pResult, nSize);
  szUserName[nSize] = '\0';

  PQclearLock(pPGResult);

  return true;
}

bool IsUserBelongsToGroup(int nUserId, int nGroupId, int* pnResult)
{
  ExecStatusType tQueryStatus;
  PGresult* pResult = NULL;
  int nResultCount = -1;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nUserId >= 0);
  STAFF_SECURITY_ASSERT(nGroupId >= 0);

  {
    int nUserIdReq = htonl(nUserId); // to network byte order
    int nGroupIdReq = htonl(nGroupId); // to network byte order

    int anParamLengths[2] = { sizeof(nUserIdReq), sizeof(nGroupIdReq) };
    int anParamFormats[2] = { 1, 1 };
    const char* aszParams[2] = { (const char*)&nUserIdReq, (const char*)&nGroupIdReq };

    pResult = PQexecParamsLock(g_pConn,
      "select \"groupid\" from \"usertogroups\" where \"userid\" = $1::int4 and \"groupid\" = $2::int4;",
      2, NULL, aszParams, anParamLengths, anParamFormats, 1);
  }

  tQueryStatus = PQresultStatus(pResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: \"%s\"\n", PQerrorMessage(g_pConn));
    PQclearLock(pResult);
    return false;
  }

  nResultCount = PQntuples(pResult);
  if (nResultCount < 0)
  {
    dprintf("error executing query\n");
    PQclearLock(pResult);
    return false;
  }

  if (nResultCount == 0)
  {
#ifdef _DEBUG
    dprintf("user %d is not belongs to group %d\n", nUserId, nGroupId);
#endif // _DEBUG
    *pnResult = 0;
  } else
  {
#ifdef _DEBUG
    dprintf("user %d belongs to group %d\n", nUserId, nGroupId);
#endif // _DEBUG
    *pnResult = 1;
  }

  PQclearLock(pResult);

  return true;
}


bool ParseObjectInfoResponse(PGresult* pPGResult, TObject* pstObject, int nRow)
{
  unsigned nPermissions = 0;
  byte bPermission = 0;
  const char* pResult = NULL;
  TPermissions stPermissions;

  // objectid
  pResult = PQgetvalue(pPGResult, nRow, 0);
  if(pResult == NULL)
  {
    dprintf("can't get objectid\n");
    return false;
  }

  pstObject->nObjectId = ntohl(*((unsigned*)pResult)); // from network byte order

  // name
  pResult = PQgetvalue(pPGResult, nRow, 1);
  if(pResult == NULL)
  {
    dprintf("can't get name\n");
    return false;
  }

  memset(pstObject->szObjectName, 0, sizeof(pstObject->szObjectName));
  strncpy(pstObject->szObjectName, pResult, min(sizeof(pstObject->szObjectName) - 1, strlen(pResult)) );

  // type
  pResult = PQgetvalue(pPGResult, nRow, 2);
  if(pResult == NULL)
  {
    dprintf("can't get type\n");
    return false;
  }

  pstObject->nType = ntohl(*((unsigned*)pResult)); // from network byte order

  // description
  pResult = PQgetvalue(pPGResult, nRow, 3);
  if(pResult == NULL)
  {
    dprintf("can't get description\n");
    return false;
  }

  memset(pstObject->szDescription, 0, sizeof(pstObject->szDescription));
  strncpy(pstObject->szDescription, pResult, min(sizeof(pstObject->szDescription) - 1, strlen(pResult)) );

  // userid
  pResult = PQgetvalue(pPGResult, nRow, 4);
  if(pResult == NULL)
  {
    dprintf("can't get userid\n");
    return false;
  }

  pstObject->nUserId = ntohl(*((unsigned*)pResult)); // from network byte order


  // groupid
  pResult = PQgetvalue(pPGResult, nRow, 5);
  if(pResult == NULL)
  {
    dprintf("can't get groupid\n");
    return false;
  }

  pstObject->nGroupId = ntohl(*((unsigned*)pResult)); // from network byte order

  // parentobjectid
  pResult = PQgetvalue(pPGResult, nRow, 6);
  if(pResult == NULL)
  {
    dprintf("can't get parent object id\n");
    return false;
  }

  pstObject->nParentObjectId = ntohl(*((unsigned*)pResult)); // from network byte order

  // permission
  pResult = PQgetvalue(pPGResult, nRow, 7);
  if(pResult == NULL)
  {
    dprintf("can't get permission\n");
    return false;
  }

  nPermissions = ntohl(*((unsigned*)pResult)); // from network byte order
  stPermissions = *((TPermissions*)&nPermissions);


  bPermission = stPermissions.nUser;
  pstObject->stPermissionUser = *((TPermission*)&bPermission);
  bPermission = stPermissions.nGroup;
  pstObject->stPermissionGroup = *((TPermission*)&bPermission);
  bPermission = stPermissions.nOthers;
  pstObject->stPermissionOthers = *((TPermission*)&bPermission);

#ifdef _DEBUG
  dprintf("name: %s, type: %d, description: %s, parent: %d, user: %d, group: %d, perm: %d([%d]%d-%d-%d) sizeof=%d\n", 
    pstObject->szObjectName, pstObject->nType, pstObject->szDescription, pstObject->nParentObjectId,
    pstObject->nUserId, pstObject->nGroupId, 
    *(int*)&stPermissions, stPermissions.nDummy, stPermissions.nUser, stPermissions.nGroup, stPermissions.nOthers, sizeof(stPermissions));
#endif // _DEBUG
  
  return true;
}

bool StaffSecurityGetObjectById(int nObjectId, int nObjectType, TObject* pstObject)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObject);
  STAFF_SECURITY_ASSERT(nObjectId >= 0);
  STAFF_SECURITY_ASSERT(nObjectType >= 0);

  {
    int nObjectIdReq = htonl(nObjectId); // to network byte order
    int nObjectTypeReq = htonl(nObjectType); // to network byte order
    int anParamLengths[2] = { sizeof(nObjectIdReq), sizeof(nObjectTypeReq) };
    int anParamFormats[2] = { 1, 1 };
    const char* aszParams[2] = { (const char*)&nObjectIdReq, (const char*)&nObjectTypeReq };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"objectid\", \"name\", \"type\", \"description\", \"userid\", \"groupid\", \"parentobjectid\", "
      "\"permission\" from \"objects\" where \"objectid\" = $1::int4 and \"type\" = $2::int4;",
      2, NULL, aszParams, anParamLengths, anParamFormats, 1);
  }

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: \"%s\"\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  if (PQntuples(pPGResult) <= 0)
  {
    dprintf("object %d does not exists\n", nObjectId);
    PQclearLock(pPGResult);
    return false;
  }

  if (!ParseObjectInfoResponse(pPGResult, pstObject, 0))
  {
    dprintf("can't parse info for object %d\n", nObjectId);
    PQclearLock(pPGResult);
    return false;
  }

  PQclearLock(pPGResult);

  return true;
}

bool StaffSecurityGetObjectByName( const char* szObjectName, int nObjectType, const int *pnParentObjectId, TObject* pstObject )
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObject);
  STAFF_SECURITY_ASSERT(szObjectName);

  if (pnParentObjectId == NULL)
  {
    int nObjectTypeReq = htonl(nObjectType); // to network byte order
    int anParamLengths[2] = { (int)strlen(szObjectName), sizeof(nObjectTypeReq) };
    int anParamFormats[2] = { 0, 1 };
    const char* aszParams[2] = { szObjectName, (const char*)&nObjectTypeReq };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"objectid\", \"name\", \"type\", \"description\", \"userid\", \"groupid\", \"parentobjectid\", "
      "\"permission\" from \"objects\" where \"name\" = $1 and \"type\" = $2::int4;",
      2, NULL, aszParams, anParamLengths, anParamFormats, 1);
  } 
  else
  {
    int nObjectTypeReq = htonl(nObjectType); // to network byte order
    int nParentObjectIdReq = ntohl(*pnParentObjectId); // to network byte order
    int anParamLengths[3] = { (int)strlen(szObjectName), sizeof(nObjectTypeReq), sizeof(nParentObjectIdReq) };
    int anParamFormats[3] = { 0, 1, 1 };
    const char* aszParams[3] = { szObjectName, (const char*)&nObjectTypeReq, (const char*)&nParentObjectIdReq };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"objectid\", \"name\", \"type\", \"description\", \"userid\", \"groupid\", \"parentobjectid\", "
      "\"permission\" from \"objects\" where \"name\" = $1 and \"type\" = $2::int4 and \"parentobjectid\" = $3::int4;",
      3, NULL, aszParams, anParamLengths, anParamFormats, 1);
  }

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: \"%s\"\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  if (PQntuples(pPGResult) <= 0)
  {
    dprintf("object %s does not exists\n", szObjectName);
    PQclearLock(pPGResult);
    return false;
  }

  if (!ParseObjectInfoResponse(pPGResult, pstObject, 0))
  {
    dprintf("can't parse info for object %s\n", szObjectName);
    PQclearLock(pPGResult);
    return false;
  }

  PQclearLock(pPGResult);

  return true;
}

bool StaffSecurityGetObjectListByType( int nObjectType, 
                                       const int* pnParentObjectId, 
                                       TObject** ppstObject, 
                                       int* pnCount )
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  int i = 0;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(ppstObject);
  STAFF_SECURITY_ASSERT(pnCount);

  if (pnParentObjectId == NULL)
  {
    int nObjectTypeReq = htonl(nObjectType); // to network byte order
    int anParamLengths[1] = { sizeof(nObjectTypeReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nObjectTypeReq };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"objectid\", \"name\", \"type\", \"description\", \"userid\", \"groupid\", \"parentobjectid\", "
      "\"permission\" from \"objects\" where  \"type\" = $2::int4;",
      1, NULL, aszParams, anParamLengths, anParamFormats, 1);
  } 
  else
  {
    int nObjectTypeReq = htonl(nObjectType); // to network byte order
    int nParentObjectIdReq = ntohl(*pnParentObjectId); // to network byte order
    int anParamLengths[2] = { sizeof(nObjectTypeReq), sizeof(nParentObjectIdReq) };
    int anParamFormats[2] = { 1, 1 };
    const char* aszParams[2] = { (const char*)&nObjectTypeReq, (const char*)&nParentObjectIdReq };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"objectid\", \"name\", \"type\", \"description\", \"userid\", \"groupid\", \"parentobjectid\", "
      "\"permission\" from \"objects\" where \"type\" = $1::int4 and \"parentobjectid\" = $2::int4;",
      2, NULL, aszParams, anParamLengths, anParamFormats, 1);
  }

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: \"%s\"\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  *pnCount = PQntuples(pPGResult);
  if (*pnCount < 0)
  {
    dprintf("invalid objects count with type %d found\n", nObjectType);
    PQclearLock(pPGResult);
    return false;
  }
  
  *ppstObject = (TObject*)calloc(*pnCount, sizeof(TObject));

  for(i = 0; i < *pnCount; ++i)
  {
    if (!ParseObjectInfoResponse(pPGResult, &((*ppstObject)[i]), i))
    {
      dprintf("can't parse info for object type %d[%d]\n", nObjectType, i);
      PQclearLock(pPGResult);
      return false;
    }
  }

  PQclearLock(pPGResult);

  return true;
}

void StaffSecurityFreeObjectList(TObject* pstObject)
{
  if(pstObject)
  {
    free(pstObject);
  }
}

bool StaffSecurityGetPermissionForUser( const TObject* pstObject, int nUserId, TPermission* pstPermission )
{
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObject);
  STAFF_SECURITY_ASSERT(pstPermission);

  if (nUserId == pstObject->nUserId) 
  { //возвращаем права владельца
    *pstPermission = pstObject->stPermissionUser;
  }
  else // юзер не владелец, проверим пренадлежит ли он группе
  {
    int nUserBelong = 0;
    STAFF_SECURITY_ASSERT(IsUserBelongsToGroup(nUserId, pstObject->nGroupId, &nUserBelong));
    if(nUserBelong)
    {
      *pstPermission = pstObject->stPermissionGroup;
    }
    else
    {
      *pstPermission = pstObject->stPermissionOthers;
    }
  }
  
  dprintf("\033[4muser permissions to [%s]: %c%c%c\033[0m\n", 
    pstObject->szObjectName,
    pstPermission->bRead ? 'r' : '-',
    pstPermission->bWrite ? 'w' : '-',
    pstPermission->bExecute ? 'x' : '-');

  return true;
}

bool StaffSecurityGetUserPermissionForServiceOperation( const char* szService, const char* szOperation, int nUserId, TPermission* pstPermission )
{
  int* pnComponentId = NULL;
  TObject stObjectComponent;
  TObject stObjectService;
  TObject stObjectOperation;

  STAFF_SECURITY_ASSERT(szService);

  {
    char szComponent[128] = "";
    const char* szCompBegin = szService;
    const char* szCompEnd = strchr(szCompBegin, '.');

    while (szCompEnd)
    {
      int nLen = min((int)(szCompEnd - szCompBegin), sizeof(szComponent) - 1);
      strncpy(szComponent, szCompBegin, nLen);
      szComponent[nLen] = '\0';

      if(!StaffSecurityGetObjectByName(szComponent, EObjectTypeComponent, pnComponentId, &stObjectComponent))
      { // не найдено описание компонента очередного уровня вложенности, возьмем права текущего
        if (!pnComponentId) // если не находили компонент
        { // берем права для корневого компонента
          STAFF_SECURITY_ASSERT(StaffSecurityGetObjectByName("ROOTCOMPONENT", EObjectTypeComponent, NULL, &stObjectComponent));
        }

        STAFF_SECURITY_ASSERT(StaffSecurityGetPermissionForUser(&stObjectComponent, nUserId, pstPermission));
        return true;
      }
      pnComponentId = &stObjectComponent.nObjectId;

      szCompBegin = szCompEnd + 1;
      szCompEnd = strchr(szCompBegin, '.');
    }

    szService = szCompBegin;
  }

  // пробуем получить права на сервис
  if(StaffSecurityGetObjectByName(szService, EObjectTypeService, pnComponentId, &stObjectService))
  {
    if(szOperation && StaffSecurityGetObjectByName(szOperation, EObjectTypeOperation, 
          &stObjectService.nObjectId, &stObjectOperation))
    { // берем права на операцию
      STAFF_SECURITY_ASSERT(StaffSecurityGetPermissionForUser(&stObjectOperation, nUserId, pstPermission));
    } 
    else
    { // берем права на сервис
      STAFF_SECURITY_ASSERT(StaffSecurityGetPermissionForUser(&stObjectService, nUserId, pstPermission));
    }
  } // если не получилось, то возвращаем права на компонент
  else
  {
    if (!pnComponentId) // если не находили компонент
    { // берем права для корневого компонента
      STAFF_SECURITY_ASSERT(StaffSecurityGetObjectByName("ROOTCOMPONENT", EObjectTypeComponent, NULL, &stObjectComponent));
    }

    STAFF_SECURITY_ASSERT(StaffSecurityGetPermissionForUser(&stObjectComponent, nUserId, pstPermission));
  }

  return true;
}

bool StaffSecurityIsInit()
{
  STAFF_SECURITY_ASSERT(g_pConn);
  return true;
}

int StaffSecurityGetSessionExpiration()
{
  return g_nSessionExpiration;
}

//////////////////////////////////////////////////////////////////////////
// Admin

bool StaffSecurityAdminGetUsers(TUser** ppUsers, int* pnCount)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nIndex = 0;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(ppUsers);
  STAFF_SECURITY_ASSERT(pnCount);


  pPGResult = PQexecLock(g_pConn, "select \"userid\",\"username\",\"description\" from \"users\" order by \"userid\";");

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  *pnCount = PQntuples(pPGResult);
  if(*pnCount < 0)
  {
    dprintf("can't get users list\n");
    PQclearLock(pPGResult);
    return false;
  }

  *ppUsers = (TUser*)calloc(*pnCount, sizeof(TUser));

  for(nIndex = 0; nIndex < *pnCount; ++nIndex)
  {
    // get id
    pResult = PQgetvalue(pPGResult, nIndex, 0);
    if(!pResult)
    {
      dprintf("error getting id\n");
      *pnCount = 0;
      StaffSecurityAdminFreeUsers(*ppUsers);
      PQclearLock(pPGResult);
      return false;
    }
    (*ppUsers)[nIndex].nId = atoi(pResult);

    // get name
    pResult = PQgetvalue(pPGResult, nIndex, 1);
    if(!pResult)
    {
      dprintf("error getting name\n");
      *pnCount = 0;
      StaffSecurityAdminFreeUsers(*ppUsers);
      PQclearLock(pPGResult);
      return false;
    }
    strncpy((*ppUsers)[nIndex].szName, pResult, sizeof((*ppUsers)[nIndex].szName) - 1);

    // get description
    pResult = PQgetvalue(pPGResult, nIndex, 2);
    if(!pResult)
    {
      dprintf("error getting description\n");
      *pnCount = 0;
      StaffSecurityAdminFreeUsers(*ppUsers);
      PQclearLock(pPGResult);
      return false;
    }
    strncpy((*ppUsers)[nIndex].szDescription, pResult, sizeof((*ppUsers)[nIndex].szDescription) - 1);
  }

  PQclearLock(pPGResult);

  return true;
}


bool StaffSecurityAdminGetGroups(TGroup** ppGroups, int* pnCount)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nIndex = 0;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(ppGroups);
  STAFF_SECURITY_ASSERT(pnCount);


  pPGResult = PQexecLock(g_pConn, "select \"groupid\",\"groupname\",\"description\" from \"groups\" order by \"groupid\";");

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  *pnCount = PQntuples(pPGResult);
  if(*pnCount < 0)
  {
    dprintf("can't get groups list\n");
    PQclearLock(pPGResult);
    return false;
  }

  *ppGroups = (TGroup*)calloc(*pnCount, sizeof(TGroup));

  for(nIndex = 0; nIndex < *pnCount; ++nIndex)
  {
    // get id
    pResult = PQgetvalue(pPGResult, nIndex, 0);
    if(!pResult)
    {
      dprintf("error getting id\n");
      *pnCount = 0;
      StaffSecurityAdminFreeGroups(*ppGroups);
      PQclearLock(pPGResult);
      return false;
    }
    (*ppGroups)[nIndex].nId = atoi(pResult);

    // get name
    pResult = PQgetvalue(pPGResult, nIndex, 1);
    if(!pResult)
    {
      dprintf("error getting name\n");
      *pnCount = 0;
      StaffSecurityAdminFreeGroups(*ppGroups);
      PQclearLock(pPGResult);
      return false;
    }
    strncpy((*ppGroups)[nIndex].szName, pResult, sizeof((*ppGroups)[nIndex].szName) - 1);

    // get description
    pResult = PQgetvalue(pPGResult, nIndex, 2);
    if(!pResult)
    {
      dprintf("error getting description\n");
      *pnCount = 0;
      StaffSecurityAdminFreeGroups(*ppGroups);
      PQclearLock(pPGResult);
      return false;
    }
    strncpy((*ppGroups)[nIndex].szDescription, pResult, sizeof((*ppGroups)[nIndex].szDescription) - 1);
  }

  PQclearLock(pPGResult);

  return true;
}


bool StaffSecurityAdminGetUserGroups(int nUserId, int** ppnGroups, int* pnGroupsCount)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;

  int nIndex = 0;
  int nUserIdReq = htonl(nUserId);
  int anParamLengths[1] = { sizeof(nUserId) };
  int anParamFormats[1] = { 1 };
  const char* aszParams[1] = { (const char*)&nUserIdReq };

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nUserId >= 0);
  STAFF_SECURITY_ASSERT(ppnGroups);
  STAFF_SECURITY_ASSERT(pnGroupsCount);


  pPGResult = PQexecParamsLock(g_pConn, "select \"groupid\" from \"usertogroups\" where \"userid\" = $1::int4;",
    1, NULL, aszParams, anParamLengths, anParamFormats, 1);

  tQueryStatus = PQresultStatus(pPGResult);

  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  *pnGroupsCount = PQntuples(pPGResult);
  if(*pnGroupsCount < 0)
  {
    dprintf("can't get groups for user: %d\n", nUserId);
    PQclearLock(pPGResult);
    return false;
  }

  *ppnGroups = (int*)calloc(*pnGroupsCount, sizeof(int));

  for(nIndex = 0; nIndex < *pnGroupsCount; ++nIndex)
  {
    pResult = PQgetvalue(pPGResult, nIndex, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      return false;
    }

    (*ppnGroups)[nIndex] = ntohl(*((unsigned*)pResult));
  }
  
  PQclearLock(pPGResult);

  return true;
}


bool StaffSecurityAdminAddUser(const char* szUserName, const char* szDescription, int* pnUserId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nUserIdReq = -1;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szUserName);
  STAFF_SECURITY_ASSERT(szDescription);
  STAFF_SECURITY_ASSERT(pnUserId);

  // create user id
  {
    pPGResult = PQexecLock(g_pConn, "select nextval('users_sequence');");

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
    {
      dprintf("failed to create new user id: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    pResult = PQgetvalue(pPGResult, 0, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      return false;
    }

    *pnUserId = atoi(pResult);
    nUserIdReq = htonl(*pnUserId);

    PQclearLock(pPGResult);
  }

  // create user
  {
    int anParamLengths[3] = { sizeof(nUserIdReq), (int)strlen(szUserName), (int)strlen(szDescription) };
    int anParamFormats[3] = { 1, 0, 0 };
    const char* aszParams[3] = { (const char*)&nUserIdReq, szUserName, szDescription };
    pPGResult = PQexecParamsLock(g_pConn, 
      "insert into \"users\"(\"userid\", \"username\", \"description\") values($1::int4, $2, $3);", 
      3, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }
  
  // create user context
  {
    int anParamLengths[1] = { sizeof(nUserIdReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nUserIdReq };
    pPGResult = PQexecParamsLock(g_pConn, 
      "insert into \"context\"(\"userid\") values($1::int4);", 
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}


bool StaffSecurityAdminRemoveUser(int nUserId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  int nUserIdReq = htonl(nUserId);

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nUserId >= 0);

  // drop user 
  {
    int anParamLengths[1] = { sizeof(nUserIdReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nUserIdReq };
    pPGResult = PQexecParamsLock(g_pConn, 
      "delete from \"users\" where \"userid\" = $1::int4;", 
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}


bool StaffSecurityAdminSetUserPassword(int nUserId, const char* szPass)
{
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nUserId >= 0);
  STAFF_SECURITY_ASSERT(szPass);

  {
    int nUserIdReq = htonl(nUserId);
    ExecStatusType tQueryStatus;
    PGresult* pPGResult = NULL;
    int anParamLengths[2] = { sizeof(nUserIdReq), (int)strlen(szPass) };
    int anParamFormats[2] = { 1, 0 };
    const char* aszParams[2] = { (const char*)&nUserIdReq, szPass };
    pPGResult = PQexecParamsLock(g_pConn, 
      "update \"users\" set \"password\" = $2 where \"userid\" = $1::int4;", 
      2, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

void StaffSecurityAdminFreeUsers(TUser* pUsers)
{
  if(pUsers)
  {
    free(pUsers);
  }
}

void StaffSecurityAdminFreeGroups(TGroup* pGroups)
{
  if(pGroups)
  {
    free(pGroups);
  }
}

void StaffSecurityAdminFreeGroupIds(int* pGroups)
{
  if(pGroups)
  {
    free(pGroups);
  }
}

bool StaffSecurityAdminAddGroup(const char* szGroupName, const char* szDescription, int* pnGroupId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nGroupIdReq = -1;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(szGroupName);
  STAFF_SECURITY_ASSERT(szDescription);
  STAFF_SECURITY_ASSERT(pnGroupId);

  // create group id
  {
    pPGResult = PQexecLock(g_pConn, "select nextval('groups_sequence');");

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
    {
      dprintf("failed to create new group id: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    pResult = PQgetvalue(pPGResult, 0, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      return false;
    }

    *pnGroupId = atoi(pResult);
    nGroupIdReq = htonl(*pnGroupId);

    PQclearLock(pPGResult);
  }

  // create group
  {
    int anParamLengths[3] = { sizeof(nGroupIdReq), (int)strlen(szGroupName), (int)strlen(szDescription) };
    int anParamFormats[3] = { 1, 0, 0 };
    const char* aszParams[3] = { (const char*)&nGroupIdReq, szGroupName, szDescription };
    pPGResult = PQexecParamsLock(g_pConn, 
      "insert into \"groups\"(\"groupid\", \"groupname\", \"description\") values($1::int4, $2, $3);", 
      3, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}


bool StaffSecurityAdminRemoveGroup(int nGroupId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  int nGroupIdReq = htonl(nGroupId);

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nGroupId >= 0);

  // drop group 
  {
    int anParamLengths[1] = { sizeof(nGroupIdReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nGroupIdReq };
    pPGResult = PQexecParamsLock(g_pConn, 
      "delete from \"groups\" where \"groupid\" = $1::int4;", 
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}




bool StaffSecurityAdminAddUserToGroup(int nUserId, int nGroupId)
{
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nUserId >= 0);
  STAFF_SECURITY_ASSERT(nGroupId >= 0);

  // add user to group
  {
    ExecStatusType tQueryStatus;
    PGresult* pPGResult = NULL;
    int nUserIdReq = htonl(nUserId);
    int nGroupIdReq = htonl(nGroupId);

    int anParamLengths[2] = { sizeof(nUserIdReq), sizeof(nGroupIdReq) };
    int anParamFormats[2] = { 1, 1 };
    const char* aszParams[2] = { (const char*)&nUserIdReq, (const char*)&nGroupIdReq };
    pPGResult = PQexecParamsLock(g_pConn, 
      "insert into \"usertogroups\"(\"userid\", \"groupid\") values($1::int4, $2::int4);", 
      2, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}


bool StaffSecurityAdminAddUserToGroups(int nUserId, const int* pnGroupIds, int nGroupsCount, int* pnFailedCount)
{
  int nIndex = 0;
  STAFF_SECURITY_ASSERT(nUserId >= 0);
  STAFF_SECURITY_ASSERT(pnGroupIds);
  STAFF_SECURITY_ASSERT(nGroupsCount >= 0);

  for (nIndex = 0; nIndex < nGroupsCount; ++nIndex)
  {
    if(!StaffSecurityAdminAddUserToGroup(nUserId, pnGroupIds[nIndex]))
    {
      if(pnFailedCount)
      {
        ++(*pnFailedCount);
      }
    }
  }

  return pnFailedCount ? *pnFailedCount != nGroupsCount : true;
}


bool StaffSecurityAdminRemoveUserFromGroup(int nUserId, int nGroupId)
{
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nUserId >= 0);
  STAFF_SECURITY_ASSERT(nGroupId >= 0);

  // remove user from group
  {
    ExecStatusType tQueryStatus;
    PGresult* pPGResult = NULL;
    int nUserIdReq = htonl(nUserId);
    int nGroupIdReq = htonl(nGroupId);

    int anParamLengths[2] = { sizeof(nUserIdReq), sizeof(nGroupIdReq) };
    int anParamFormats[2] = { 1, 1 };
    const char* aszParams[2] = { (const char*)&nUserIdReq, (const char*)&nGroupIdReq };
    pPGResult = PQexecParamsLock(g_pConn, 
      "delete from \"usertogroups\" where (\"userid\" = $1::int4 and \"groupid\" = $2::int4);", 
      2, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);

    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}


bool StaffSecurityAdminRemoveUserFromGroups(int nUserId, const int* pnGroupIds, int nGroupsCount, int* pnFailedCount)
{
  int nIndex = 0;
  STAFF_SECURITY_ASSERT(nUserId >= 0);
  STAFF_SECURITY_ASSERT(pnGroupIds);
  STAFF_SECURITY_ASSERT(nGroupsCount >= 0);

  for (nIndex = 0; nIndex < nGroupsCount; ++nIndex)
  {
    if(!StaffSecurityAdminRemoveUserFromGroup(nUserId, pnGroupIds[nIndex]))
    {
      if(pnFailedCount)
      {
        ++(*pnFailedCount);
      }
    }
  }

  return pnFailedCount ? *pnFailedCount != nGroupsCount : true;
}


bool StaffSecurityAdminGetObjectIdList(int** ppnObjectIds, int* pnObjectsCount)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nIndex = 0;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(ppnObjectIds);
  STAFF_SECURITY_ASSERT(pnObjectsCount);

  pPGResult = PQexecParamsLock(g_pConn, "select \"objectid\" from \"objects\";",
    0, NULL, NULL, NULL, NULL, 1);

  tQueryStatus = PQresultStatus(pPGResult);

  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  *pnObjectsCount = PQntuples(pPGResult);
  if(*pnObjectsCount < 0)
  {
    dprintf("can't get object list\n");
    PQclearLock(pPGResult);
    return false;
  }

  *ppnObjectIds = (int*)calloc(*pnObjectsCount, sizeof(int));

  for(nIndex = 0; nIndex < *pnObjectsCount; ++nIndex)
  {
    pResult = PQgetvalue(pPGResult, nIndex, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      return false;
    }

    (*ppnObjectIds)[nIndex] = ntohl(*((unsigned*)pResult));
  }

  PQclearLock(pPGResult);

  return true;
}

bool StaffSecurityAdminGetObjectIdListWithParent(int** ppnObjectIds, int nParentId, int* pnObjectsCount)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nIndex = 0;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(ppnObjectIds);
  STAFF_SECURITY_ASSERT(pnObjectsCount);

  {
    int nParentIdReq = htonl(nParentId); // to network byte order
    int anParamLengths[1] = { sizeof(nParentIdReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nParentIdReq };

    // get children excluding self-parent
    pPGResult = PQexecParamsLock(g_pConn,
      "select \"objectid\" from \"objects\" "
        "where (\"parentobjectid\" = $1::int4 and \"objectid\" != $1::int4);",
      1, NULL, aszParams, anParamLengths, anParamFormats, 1);
  }

  tQueryStatus = PQresultStatus(pPGResult);

  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  *pnObjectsCount = PQntuples(pPGResult);
  if(*pnObjectsCount < 0)
  {
    dprintf("can't get object list\n");
    PQclearLock(pPGResult);
    return false;
  }

  *ppnObjectIds = (int*)calloc(*pnObjectsCount, sizeof(int));

  for(nIndex = 0; nIndex < *pnObjectsCount; ++nIndex)
  {
    pResult = PQgetvalue(pPGResult, nIndex, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      return false;
    }

    (*ppnObjectIds)[nIndex] = ntohl(*((unsigned*)pResult));
  }

  PQclearLock(pPGResult);

  return true;
}


bool StaffSecurityAdminGetObjectById(int nObjectId, TObject* pstObject)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObject);
  STAFF_SECURITY_ASSERT(nObjectId >= 0);

  {
    int nObjectIdReq = htonl(nObjectId); // to network byte order
    int anParamLengths[1] = { sizeof(nObjectIdReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nObjectIdReq };

    pPGResult = PQexecParamsLock(g_pConn,
      "select \"objectid\", \"name\", \"type\", \"description\", \"userid\", \"groupid\", \"parentobjectid\", "
      "\"permission\" from \"objects\" where \"objectid\" = $1::int4;",
      1, NULL, aszParams, anParamLengths, anParamFormats, 1);
  }

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: \"%s\"\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  if (PQntuples(pPGResult) <= 0)
  {
    dprintf("object %d does not exists\n", nObjectId);
    PQclearLock(pPGResult);
    return false;
  }

  if (!ParseObjectInfoResponse(pPGResult, pstObject, 0))
  {
    dprintf("can't parse info for object %d\n", nObjectId);
    PQclearLock(pPGResult);
    return false;
  }

  PQclearLock(pPGResult);

  return true;
}


bool StaffSecurityAdminGetObjectList(const int* pnObjectIds, int nObjectIdsCount, TObject** ppstObjects, int* pnObjectsCount)
{
  int nIndex = 0;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pnObjectIds);
  STAFF_SECURITY_ASSERT(nObjectIdsCount > 0);
  STAFF_SECURITY_ASSERT(ppstObjects);
  STAFF_SECURITY_ASSERT(pnObjectsCount);

  *ppstObjects = (TObject*)calloc(nObjectIdsCount, sizeof(TObject));
  if(!*ppstObjects)
  {
    dprintf("error allocating memory!!!");
    return false;
  }
  
  *pnObjectsCount = 0;

  // TODO: select * from objects where objectid in (1,2,3,4);
  for(nIndex = 0; nIndex != nObjectIdsCount; ++nIndex)
  {
    if(StaffSecurityAdminGetObjectById(pnObjectIds[nIndex], &(*ppstObjects)[nIndex]))
    {
      ++*pnObjectsCount;
    }
  }

  if (*pnObjectsCount == 0)
  {
    StaffSecurityFreeObjectList(*ppstObjects);
    return false;
  }

  return true;
}

bool StaffSecurityAdminAddObject(const TObject* pstObject, int* pnObjectId)
{
  ExecStatusType tQueryStatus;
  const char* pResult = NULL;
  PGresult* pPGResult = NULL;
  int nObjectIdReq = 0;
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObject);
  STAFF_SECURITY_ASSERT(pnObjectId);

  // create object id
  {
    pPGResult = PQexecLock(g_pConn, "select nextval('objects_sequence');");

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
    {
      dprintf("failed to create new object id: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    pResult = PQgetvalue(pPGResult, 0, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      return false;
    }

    *pnObjectId = atoi(pResult);
    nObjectIdReq = htonl(*pnObjectId);

    PQclearLock(pPGResult);
  }

  // create object
  {
    TPermissions stPermissions;
    int nUserIdReq = htonl(pstObject->nUserId);
    int nGroupIdReq = htonl(pstObject->nGroupId);
    int nParentObjectIdReq = htonl(pstObject->nParentObjectId);
    int nPermissionReq = 0;
    int nTypeReq = htonl(pstObject->nType);

    int anParamLengths[8] = 
    {
      sizeof(nObjectIdReq), 
      (int)strlen(pstObject->szObjectName),
      (int)strlen(pstObject->szDescription),
      sizeof(nUserIdReq), 
      sizeof(nGroupIdReq), 
      sizeof(nParentObjectIdReq), 
      sizeof(nPermissionReq), 
      sizeof(nTypeReq)
    };
    int anParamFormats[8] = { 1, 0, 0, 1, 1, 1, 1, 1 };
    const char* aszParams[8] = 
    {
      (const char*)&nObjectIdReq, 
      pstObject->szObjectName,
      pstObject->szDescription,
      (const char*)&nUserIdReq, 
      (const char*)&nGroupIdReq, 
      (const char*)&nParentObjectIdReq, 
      (const char*)&nPermissionReq, 
      (const char*)&nTypeReq
    };


    stPermissions.nDummy = 0;
    stPermissions.nUser = *(unsigned*)&pstObject->stPermissionUser;
    stPermissions.nGroup = *(unsigned*)&pstObject->stPermissionGroup;
    stPermissions.nOthers = *(unsigned*)&pstObject->stPermissionOthers;

    nPermissionReq = htonl(*(unsigned*)&stPermissions);



    pPGResult = PQexecParamsLock(g_pConn, 
      "insert into \"objects\"("
        "\"objectid\", \"name\", \"description\", \"userid\", \"groupid\", \"parentobjectid\", \"permission\", \"type\""
      ") values($1::int4, $2, $3, $4::int4, $5::int4, $6::int4, $7::int4, $8::int4);",
      8, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

bool StaffSecurityAdminRemoveObject(int nObjectId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  int nObjectIdReq = htonl(nObjectId);

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nObjectId >= 0);

  // drop object 
  {
    int anParamLengths[1] = { sizeof(nObjectIdReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nObjectIdReq };
    pPGResult = PQexecParamsLock(g_pConn, 
      "delete from \"objects\" where \"objectid\" = $1::int4;", 
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

bool StaffSecurityAdminReplaceObject(const TObject* pstObject)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObject);

  // create object
  {
    TPermissions stPermissions;
    int nObjectIdReq = htonl(pstObject->nObjectId);
    int nUserIdReq = htonl(pstObject->nUserId);
    int nGroupIdReq = htonl(pstObject->nGroupId);
    int nParentObjectIdReq = htonl(pstObject->nParentObjectId);
    int nPermissionReq = 0;
    int nTypeReq = htonl(pstObject->nType);

    int anParamLengths[8] = 
    {
      sizeof(nObjectIdReq), 
      (int)strlen(pstObject->szObjectName),
      (int)strlen(pstObject->szDescription),
      sizeof(nUserIdReq), 
      sizeof(nGroupIdReq), 
      sizeof(nParentObjectIdReq), 
      sizeof(nPermissionReq), 
      sizeof(nTypeReq)
    };
    int anParamFormats[8] = { 1, 0, 0, 1, 1, 1, 1, 1 };
    const char* aszParams[8] = 
    {
      (const char*)&nObjectIdReq, 
      pstObject->szObjectName,
      pstObject->szDescription,
      (const char*)&nUserIdReq, 
      (const char*)&nGroupIdReq, 
      (const char*)&nParentObjectIdReq, 
      (const char*)&nPermissionReq, 
      (const char*)&nTypeReq
    };


    stPermissions.nDummy = 0;
    stPermissions.nUser = *(unsigned*)&pstObject->stPermissionUser;
    stPermissions.nGroup = *(unsigned*)&pstObject->stPermissionGroup;
    stPermissions.nOthers = *(unsigned*)&pstObject->stPermissionOthers;

    nPermissionReq = htonl(*(unsigned*)&stPermissions);

    pPGResult = PQexecParamsLock(g_pConn, 
      "update \"objects\" set "
      "\"name\" = $2, \"type\" = $8::int4, \"description\" = $3, "
      "\"userid\" = $4::int4, \"groupid\" = $5::int4, \"parentobjectid\" = $6::int4, \"permission\" = $7::int4 "
      "where \"objectid\" = $1::int4;",
      8, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

void StaffSecurityAdminFreeObjectIds(int* pObjects)
{
  if(pObjects)
  {
    free(pObjects);
  }
}



bool StaffSecurityAdminGetObjectTypeList(TObjectType** ppstObjectTypes, int* pnObjectTypesCount)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  const char* pResult = NULL;
  int nIndex = 0;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(ppstObjectTypes);
  STAFF_SECURITY_ASSERT(pnObjectTypesCount);

  pPGResult = PQexecParamsLock(g_pConn,
    "select \"objecttypeid\", \"name\", \"description\" from \"objecttypes\";",
    0, NULL, NULL, NULL, NULL, 1);

  tQueryStatus = PQresultStatus(pPGResult);
  if (tQueryStatus != PGRES_TUPLES_OK)
  {
    dprintf("error executing query: \"%s\"\n", PQerrorMessage(g_pConn));
    PQclearLock(pPGResult);
    return false;
  }

  *pnObjectTypesCount = PQntuples(pPGResult);

  if (*pnObjectTypesCount < 0)
  {
    dprintf("failed to get types id\n");
    PQclearLock(pPGResult);
    return false;
  }

  *ppstObjectTypes = (TObjectType*)calloc(*pnObjectTypesCount, sizeof(TObjectType));

  for (nIndex = 0; nIndex < *pnObjectTypesCount; ++nIndex)
  {
    TObjectType* pObjectType = &(*ppstObjectTypes)[nIndex];

    pResult = PQgetvalue(pPGResult, nIndex, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      free(ppstObjectTypes);
      return false;
    }
    pObjectType->nId = ntohl(*(unsigned*)pResult);

    pResult = PQgetvalue(pPGResult, nIndex, 1);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      free(ppstObjectTypes);
      return false;
    }
    strncpy(pObjectType->szName, pResult, sizeof(pObjectType->szName) - 1);
    pObjectType->szName[sizeof(pObjectType->szName) - 1] = '\0';
  
    pResult = PQgetvalue(pPGResult, nIndex, 2);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      free(ppstObjectTypes);
      return false;
    }
    strncpy(pObjectType->szDescription, pResult, sizeof(pObjectType->szDescription) - 1);
    pObjectType->szName[sizeof(pObjectType->szDescription) - 1] = '\0';
  }

  PQclearLock(pPGResult);

  return true;
}

bool StaffSecurityAdminAddObjectType(const TObjectType* pstObjectType, int* pnObjectTypeId)
{
  ExecStatusType tQueryStatus;
  const char* pResult = NULL;
  PGresult* pPGResult = NULL;
  int nObjectTypeIdReq = 0;
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObjectType);
  STAFF_SECURITY_ASSERT(pnObjectTypeId);

  // create objecttype id
  {
    pPGResult = PQexecLock(g_pConn, "select nextval('objecttypes_sequence');");

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_TUPLES_OK || PQntuples(pPGResult) <= 0)
    {
      dprintf("failed to create new objecttype id: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    pResult = PQgetvalue(pPGResult, 0, 0);
    if(!pResult)
    {
      dprintf("error getting result\n");
      PQclearLock(pPGResult);
      return false;
    }

    *pnObjectTypeId = atoi(pResult);
    nObjectTypeIdReq = htonl(*pnObjectTypeId);

    PQclearLock(pPGResult);
  }

  // create objecttype
  {
    int nObjectTypeIdReq = htonl(pstObjectType->nId);

    int anParamLengths[3] = 
    {
      sizeof(nObjectTypeIdReq), 
      (int)strlen(pstObjectType->szName),
      (int)strlen(pstObjectType->szDescription)
    };
    int anParamFormats[3] = { 1, 0, 0 };
    const char* aszParams[3] = 
    {
      (const char*)&nObjectTypeIdReq, 
      pstObjectType->szName,
      pstObjectType->szDescription
    };

    pPGResult = PQexecParamsLock(g_pConn, 
      "insert into \"objecttypes\"("
      "\"objecttypeid\", \"name\", \"description\""
      ") values($1::int4, $2, $);",
      3, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}


bool StaffSecurityAdminRemoveObjectType(int nObjectTypeId)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;
  int nObjectTypeIdReq = htonl(nObjectTypeId);

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(nObjectTypeId >= 0);

  // drop objecttype 
  {
    int anParamLengths[1] = { sizeof(nObjectTypeIdReq) };
    int anParamFormats[1] = { 1 };
    const char* aszParams[1] = { (const char*)&nObjectTypeIdReq };
    pPGResult = PQexecParamsLock(g_pConn, 
      "delete from \"objecttypes\" where \"objecttypeid\" = $1::int4;", 
      1, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

bool StaffSecurityAdminReplaceObjectType(const TObjectType* pstObjectType)
{
  ExecStatusType tQueryStatus;
  PGresult* pPGResult = NULL;

  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(pstObjectType);

  // update objecttype
  {
    int nObjectTypeIdReq = htonl(pstObjectType->nId);

    int anParamLengths[3] = 
    {
      sizeof(nObjectTypeIdReq), 
      (int)strlen(pstObjectType->szName),
      (int)strlen(pstObjectType->szDescription)
    };
    int anParamFormats[3] = { 1, 0, 0 };
    const char* aszParams[3] = 
    {
      (const char*)&nObjectTypeIdReq, 
      pstObjectType->szName,
      pstObjectType->szDescription
    };

    pPGResult = PQexecParamsLock(g_pConn, 
      "update \"objecttypes\" set "
      "\"name\" = $2, \"description\" = $3::int4 "
      "where \"objecttypeid\" = $1::int4;",
      8, NULL, aszParams, anParamLengths, anParamFormats, 0);

    tQueryStatus = PQresultStatus(pPGResult);
    if (tQueryStatus != PGRES_COMMAND_OK)
    {
      dprintf("error executing query: %s\n", PQerrorMessage(g_pConn));
      PQclearLock(pPGResult);
      return false;
    }

    PQclearLock(pPGResult);
  }

  return true;
}

void StaffSecurityAdminFreeObjectTypes(TObjectType* pObjectTypes)
{
  if(pObjectTypes != NULL)
  {
    free(pObjectTypes);
  }
}
