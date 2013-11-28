/*
 *  Copyright 2012 Utkin Dmitry
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

#ifndef _STAFF_UTILS_PLUGINEXPORT_H_
#define _STAFF_UTILS_PLUGINEXPORT_H_

#define STAFF_PLUGIN_EXPORTED_SYMBOL tStaffPluginModule
#define STAFF_PLUGIN_EXPORTED_SYMBOL_STR "tStaffPluginModule"

#ifdef WIN32
#define STAFF_DECLARE_PLUGIN(ClassName) \
  ClassName STAFF_PLUGIN_EXPORTED_SYMBOL; \
  extern "C" STAFF_DLL_EXPORT void* StaffGetPluginAddress() \
  { return &STAFF_PLUGIN_EXPORTED_SYMBOL; }
#else
#define STAFF_DECLARE_PLUGIN(ClassName) \
  ClassName STAFF_PLUGIN_EXPORTED_SYMBOL;
#endif

#endif // _STAFF_UTILS_PLUGINEXPORT_H_
