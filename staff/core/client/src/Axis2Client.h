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

#ifndef _AXIS2CLIENT_H_
#define _AXIS2CLIENT_H_

#include "staffclientexport.h"
#include <string>

namespace staff
{
  class COperation;

  //!  Axis2Client
  class STAFF_CLIENT_EXPORT CAxis2Client
  {
  public:
    //!         constructor
    CAxis2Client();

    //!         destructor
    ~CAxis2Client();

    //!         client init
    /*! \param  sServiceUri - service URI
        \param  sSessionId - sessionId
        */
    void Init(const std::string& sServiceUri, const std::string& sSessionId);
    
    //!         deinit
    void Deinit();

    //!         set target namespace
    /*! \param sTarget namespace - target namespace
        */
    void SetTargetNamespace(const std::string& sTargetNamespace);

    //!         get service URI
    /*! \return service URI
        */
    const std::string& GetServiceURI() const;

    //!         invoke operation
    /*! \param  rOperation - operation
        */
    void Invoke(COperation& rOperation);    

    //!         check for init
    /*! \return true if Init() was called
        \sa Init
        */
    bool IsInit() const;
    
  private:
    CAxis2Client(const CAxis2Client&);
    CAxis2Client& operator=(const CAxis2Client&);
    class CAxis2ClientImpl;
    CAxis2ClientImpl* m_pImpl;
  };
}

#endif // _AXIS2CLIENT_H_
