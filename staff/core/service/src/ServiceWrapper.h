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

#ifndef _SERVICEWRAPPER_H_
#define _SERVICEWRAPPER_H_

#include "staffserviceexport.h"
#include <rise/string/String.h>

namespace staff
{
  class COperation;

  //!  Service wrapper
  class STAFF_SERVICE_EXPORT CServiceWrapper
  {
  public:
    //!         destructor
    virtual ~CServiceWrapper();
    
    //!         invoke service operation
    /*! \param  rOperation - service operation
        */
    virtual void Invoke(COperation& rOperation) = 0;

    //!         get service session id
    /*! \return service session id
    */
    const rise::CString& GetSessionId() const;
  
    //!         set service session id
    /*! \param  sSessionId - session id
        */
    void SetSessionId(const rise::CString& sSessionId);

  private:
    rise::CString m_sSessionId; //!<  service session id
  };
}

#endif // _SERVICEWRAPPER_H_
