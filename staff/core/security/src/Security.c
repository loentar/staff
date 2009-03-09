#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#else
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#endif

#include <libpq-fe.h>
#include "Security.h"

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
  g_pConn = PQsetdbLogin(szHost, szPort, "", "", szDSN, szUser, szPassword);
  STAFF_SECURITY_ASSERT(g_pConn);
  STAFF_SECURITY_ASSERT(PQstatus(g_pConn) == CONNECTION_OK);
  g_nSessionExpiration = nSessionExpiration;
  InitializeLock(&g_tLock);

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
    int anParamLengths[2] = { sizeof(nContextIdReq), strlen(STAFF_SECURITY_GUEST_SESSION_ID) };
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

  if (PQntuples(pResult) < 0)
  {
    dprintf("error executing query\n");
    PQclearLock(pResult);
    return false;
  }

  if (PQntuples(pResult) > 0)
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
  
  dprintf("\033[4mpermissions: %03o\033[0m\n", *(int*)&pstPermission);

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
