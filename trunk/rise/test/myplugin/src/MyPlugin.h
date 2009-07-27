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

#ifndef _MyPlugin_h_
#define _MyPlugin_h_

#include <rise/string/String.h>

namespace myplugin  // namespace for plug-in
{
  //! interface class for plugin
  class CMyPlugin
  {
  public:
    //!  virtual destructor
    virtual ~CMyPlugin() {};

    //!   returns sum of "a" and "b" for test
    /*!   EXCEPTIONS:     CLogicFormatException, CLogicSizeException
          \param a - operand a
          \param b - operand b
          \return sum of "a" and "b"
    */
    virtual int Sum(int a, int b) = 0;

    //!  another example: put text value to log
    /*!  \param nType - type of message(log level)
         \param sLogString - text value
    */
    virtual void TestPutToLog(int nType, const rise::CString& sLogString) = 0;
  };
} // namespace myplugin

#endif // _MyPlugin_h_
