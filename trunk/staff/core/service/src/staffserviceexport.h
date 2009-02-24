#ifndef _STAFFSERVICEEXPORT_H_
#define _STAFFSERVICEEXPORT_H_

#if defined(WIN32) || defined (_WINDOWS)
  #pragma warning(disable: 4786)

  #ifdef STAFF_SERVICE_DLL_EXPORTS
    #define STAFF_SERVICE_EXPORT __declspec(dllexport)
  #else
    #define STAFF_SERVICE_EXPORT __declspec(dllimport)
  #endif

#else
  #define STAFF_SERVICE_EXPORT 
#endif

#endif // _STAFFSERVICEEXPORT_H_

/** \mainpage Документация на модуль сервисных функций распределенных приложений
<a href="annotated.html">Список</a> классов, реализуемых модулем.
*/
