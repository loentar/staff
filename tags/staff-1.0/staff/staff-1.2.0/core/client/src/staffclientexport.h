#ifndef _STAFFCLIENTEXPORT_H_
#define _STAFFCLIENTEXPORT_H_

#if defined(WIN32) || defined (_WINDOWS)
  #pragma warning(disable: 4786)

  #ifdef STAFF_CLIENT_DLL_EXPORTS
    #define STAFF_CLIENT_EXPORT __declspec(dllexport)
  #else
    #define STAFF_CLIENT_EXPORT __declspec(dllimport)
  #endif

#else
  #define STAFF_CLIENT_EXPORT 
#endif

#endif // _STAFFCLIENTEXPORT_H_

/** \mainpage Документация на модуль клиентских функций распределенных приложений
<a href="annotated.html">Список</a> классов, реализуемых модулем.
*/
