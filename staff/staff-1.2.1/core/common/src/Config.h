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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "staffcommonexport.h"
#include <rise/string/String.h>

namespace rise
{
  namespace xml
  {
    class CXMLNode;
  }
}

namespace staff
{
  //! Config class
  class STAFF_COMMON_EXPORT CConfig
  {
  public:
    //!         get config instance
    /*! \return config instance
    */
    static CConfig& Inst();

    //!         get module config
    /*! \param  sModuleName - module name
        \return config
        */
    rise::xml::CXMLNode& GetModuleConfig(const rise::CString& sModuleName);

  protected:
    CConfig();
    ~CConfig();

  private:
    class CConfigImpl;
    CConfigImpl* m_pImpl;
    static CConfig* m_pInst;
  };
}

#endif // _CONFIG_H_
