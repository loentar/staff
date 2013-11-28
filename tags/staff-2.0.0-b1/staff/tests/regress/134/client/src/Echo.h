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

#ifndef _ECHO_H_
#define _ECHO_H_

#include <string>
#include <staff/common/IService.h>
#include <staff/client/ICallback.h>

  //! echo service
  // *serviceUri: http://localhost:9090/axis2/services/echo
  // *targetNamespace: http://ws.apache.org/axis2/services/echo
  // *targetNamespacePrefix: ns1
  class Echo: public staff::IService
  {
  public:
    //! echo string
    /*! \param text - input string
        \return resulting string
        */
    // *requestElement: echoString
    // *responseElement: echoString
    // *resultElement: text
    virtual void EchoString(const std::string& sEcho, staff::ICallback< const std::string& >& rCallback) = 0;
  };

#endif // _ECHO_H_

