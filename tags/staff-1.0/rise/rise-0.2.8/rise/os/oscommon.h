#ifndef _OSCOMMON_H_
#define _OSCOMMON_H_

#ifdef WIN32
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <windows.h>
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef WIN32
#define RISE_PATH_SEPARATOR "\\"
#define RISE_EXPLICIT_TEMPLATE(RISE_LOCAL_TEMPLATE) RISE_LOCAL_TEMPLATE
#else
#define RISE_PATH_SEPARATOR "/"
#define RISE_EXPLICIT_TEMPLATE(RISE_LOCAL_TEMPLATE)
#endif

#endif // _OSCOMMON_H_
