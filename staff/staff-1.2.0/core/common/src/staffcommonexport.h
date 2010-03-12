#ifndef _STAFFCOMMONEXPORT_H_
#define _STAFFCOMMONEXPORT_H_

#if defined(WIN32) || defined (_WINDOWS)
  #pragma warning(disable: 4786)

  #ifdef STAFF_DLL_EXPORTS
    #define STAFF_COMMON_EXPORT __declspec(dllexport)
  #else
    #define STAFF_COMMON_EXPORT __declspec(dllimport)
  #endif

  #define STAFF_DEPRECATED(Replacement) _CRT_INSECURE_DEPRECATE(Replacement)

#else
  #define STAFF_COMMON_EXPORT 
  #define STAFF_DEPRECATED(Replacement)
#endif

#endif // _STAFFCOMMONEXPORT_H_


/** \mainpage Документация на модуль общих функций распределенных приложений
<a href="annotated.html">Список</a> классов, реализуемых модулем.
*/
