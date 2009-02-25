#ifndef _PLUGINEXPORT_H_
#define _PLUGINEXPORT_H_

#include <rise/os/osdl.h>

#define RISE_PLUGIN_EXPORTED_SYMBOL tPluginModule
#define RISE_PLUGIN_EXPORTED_SYMBOL_STR "tPluginModule"

#ifdef WIN32
#define RISE_DECLARE_PLUGIN(ClassName) \
  ClassName RISE_PLUGIN_EXPORTED_SYMBOL; \
  extern "C" RISE_DLL_EXPORT rise::PLibSymbol RiseGetPluginAddress() \
  { return (rise::PLibSymbol)&RISE_PLUGIN_EXPORTED_SYMBOL; }
#else
#define RISE_DECLARE_PLUGIN(ClassName) \
  ClassName RISE_PLUGIN_EXPORTED_SYMBOL;
#endif

#endif // _PLUGINEXPORT_H_
