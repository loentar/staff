#include <iostream>
#include <rise/common/Log.h>
#include <rise/plugin/PluginExport.h>
#include "MyPluginImpl.h"

// declaration for main class of plug-in
// must be on global namespace
RISE_DECLARE_PLUGIN(myplugin::CMyPluginImpl); 

namespace myplugin
{
  int CMyPluginImpl::Sum( int a, int b )
  {
    return a + b;
  }

  void CMyPluginImpl::TestPutToLog( int nType, const rise::CString& sLogString )
  {
    rise::PutToLog(nType) << "string: " << sLogString;
  }

} // namespace myplugin
