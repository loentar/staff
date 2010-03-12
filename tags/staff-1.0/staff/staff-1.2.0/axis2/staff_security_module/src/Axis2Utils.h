#ifndef _AXIS2UTILS_H_
#define _AXIS2UTILS_H_

#include <axis2_module_desc.h>
#include <axis2_msg_ctx.h>

#define AXIS2_UTILS_CHECK(expression) \
  if(!(expression)) { printf("error: %s[%d]: %s\n", GetBaseFile(__FILE__), __LINE__, #expression); return AXIS2_FAILURE; }

axis2_status_t GetServiceNameAndOperation(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv, const axis2_char_t** pszServiceName, const axis2_char_t** pszServiceOperation);
axis2_status_t GetSessionId(axis2_msg_ctx_t* pMsgCtx, const axutil_env_t* pEnv, const axis2_char_t** pszSessionId);
const axis2_char_t* GetParamValue(const axis2_char_t* szParam, axis2_module_desc_t* pModuleDesc, const axutil_env_t* pEnv);
const char* GetBaseFile(const char* szFilePath);

#endif // _AXIS2UTILS_H_


