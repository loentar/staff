#ifndef _STAFFCOMPONENTEXPORT_H_
#define _STAFFCOMPONENTEXPORT_H_

#if defined(WIN32) || defined (_WINDOWS)
  #pragma warning(disable: 4786)

  #ifdef STAFF_DLL_EXPORTS
    #define STAFF_COMPONENT_EXPORT __declspec(dllexport)
  #else
    #define STAFF_COMPONENT_EXPORT __declspec(dllimport)
  #endif

  #ifndef STAFF_DEPRECATED
    #define STAFF_DEPRECATED(Replacement) _CRT_INSECURE_DEPRECATE(Replacement)
  #endif

#else
  #define STAFF_COMPONENT_EXPORT 
  
  #ifndef STAFF_DEPRECATED
    #define STAFF_DEPRECATED(Replacement)
  #endif
#endif

#endif // _STAFFCOMPONENTEXPORT_H_


/** \mainpage Документация на модуль компонентных функций распределенных приложений
<a href="annotated.html">Список</a> классов, реализуемых модулем.
*/
