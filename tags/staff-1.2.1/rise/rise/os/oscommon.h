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

/*! \mainpage Rise
Rise is an tool library for C++, that wraps system-specific functions into classes.
<br>Supported OSes are:
  - Linux (x86/x86_64/mips);
  - Windows (x86/x86_64).
<p>
Rise implements the following tools and classes:
  - Logs <a href="classrise_1_1CLog.html">More...</a>;
  - Signals <a href="classrise_1_1CSignalHandler.html">More...</a>;
  - Timers <a href="classrise_1_1CTimerHandler.html">More...</a>;
  - Exceptions <a href="classrise_1_1CException.html">More...</a>;
  - Console management (coloring, cursor position management etc) <a href="console_8h.html">More...</a>;
  - Byte order conversion <a href="classrise_1_1CByteOrder.html">More...</a>;
  - Performance counter (simple profiling class) <a href="classrise_1_1CPerformanceCounter.html">More...</a>;
  - Dynamic libraries <a href="classrise_1_1plugin_1_1CDynamicLibrary.html">More...</a>, including simple plugin system <a href="classrise_1_1plugin_1_1CPluginManager.html">More...</a>;
  - Multi threading: <a href="classrise_1_1threading_1_1CThread.html">Thread</a>, <a href="classrise_1_1threading_1_1CCriticalSection.html">Critical section</a>, <a href="classrise_1_1threading_1_1CScopedCriticalSection.html">Scoped critical section</a>, <a href="classrise_1_1threading_1_1CAtomicCount.html">Atomic counter</a>, <a href="classrise_1_1CSharedPtr.html">Shared Ptr</a>, etc. ;
  - Processes <a href="classrise_1_1process_1_1CProcess.html">More...</a>;
  - String tools <a href="String_8h.html">More...</a>;
  - Encoding <a href="classrise_1_1CEncoding.html">More...</a>;
  - Net tools: Sockets <a href="classrise_1_1sockets_1_1CSocket.html">More...</a>;
  - File finder <a href="classrise_1_1CFileFind.html">More...</a>;
  - Stack tracer <a href="classrise_1_1tools_1_1CStackTracer.html">More...</a>;
  - Simple xml DOM for access to xml config <a href="classrise_1_1xml_1_1CXMLDocument.html">More...</a>.
<p>
See also <a href="annotated.html">annotated class list</a>, implemented by library.
*/

#endif // _OSCOMMON_H_
