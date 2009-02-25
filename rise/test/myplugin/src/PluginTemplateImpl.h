#ifndef _PluginTemplateImpl_h_
#define _MyPluginImpl_h_

#include <rise/string/String.h>
#include <rise/os/osdllexport.h>
#include <rise/common/types.h>
#include "MyPlugin.h"

namespace myplugin
{
  //!  one of implementation for interface class "CMyPlugin"
  class CMyPluginImpl: public CMyPlugin
  {
  public:
    int Sum(int a, int b);
    void TestPutToLog(int nType, const rise::CString& sLogString);
  };
} // namespace myplugin

#endif // _MyPluginImpl_h_
