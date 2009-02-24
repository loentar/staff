#ifndef _AXIS2UTILS_H_
#define _AXIS2UTILS_H_

#include <axutil_utils_defines.h>
#include <axis2_msg_ctx.h>
#include <string>

axis2_char_t* GetServiceUriFromMsgCtx(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv);
bool ParseServiceName(const axis2_char_t* szServiceURI, const axutil_env_t* pEnv, std::string& sServiceName);
bool GetServiceName(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv, std::string& sServiceName);

#endif // _AXIS2UTILS_H_


