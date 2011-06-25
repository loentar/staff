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

#ifndef _WSDL_H_
#define _WSDL_H_

#include <string>
#include <staff/common/IService.h>

namespace staff
{
  class Operation;

namespace wsdl
{
  //! REST service to get the WSDL
  class Wsdl: public staff::IService
  {
  public:
    //! get wsdl
    /*! returns modified wsdl file content via operation
        \param service - service name
        \param rOperation - operation info
        */
    // *restMethod: GET
    // *restLocation: get/{sComponent}/{sFileName}
    virtual void Get(const std::string& sComponent, const std::string& sFileName, Operation& rOperation) = 0;
  };
}
}


#endif // _WSDL_H_

