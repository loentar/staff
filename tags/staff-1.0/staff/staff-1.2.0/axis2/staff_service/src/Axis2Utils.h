#ifndef _AXIS2UTILS_H_
#define _AXIS2UTILS_H_

#include <string>

namespace staff
{
  class CService;
}

bool Axis2UtilsCreateVirtualService(const std::string& sServiceName, 
                                    const staff::CService* pService, 
                                    void* pSvcClass, 
                                    const struct axutil_env* pEnv, 
                                    struct axis2_conf* pConf);

bool Axis2UtilsRemoveVirtualService(const std::string& sServiceName, 
                                    const struct axutil_env* pEnv, 
                                    struct axis2_conf* pConf);

#endif // _AXIS2UTILS_H_

