#ifndef _TOOLS_H_
#define _TOOLS_H_

#ifdef __cplusplus
extern "C"
{
#endif
#define STAFF_SECURITY_NOBODY_SESSION_ID "00000000000000000000000000000000"

  int staff_security_init();

  void staff_security_free();

  int staff_security_calculate_access_by_session_id(const char* szObjectPath, const char* szSessionId, int* pnAccess);

#ifdef __cplusplus
}
#endif

#endif // _TOOLS_H_
