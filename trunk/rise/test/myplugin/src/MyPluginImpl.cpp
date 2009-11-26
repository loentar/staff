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
