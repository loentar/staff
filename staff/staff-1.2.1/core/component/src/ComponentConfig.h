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

#ifndef _COMPONENTCONFIG_H_
#define _COMPONENTCONFIG_H_

#include "staffcomponentexport.h"
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
  //!  Component config
  class STAFF_COMPONENT_EXPORT CComponentConfig
  {
  public:
    //!         destructor
    ~CComponentConfig();

    //!         get component name
    /*! \return component name
    */
    const rise::CString& GetComponent() const;

    //!         get config file name
    /*! \return config file name
    */
    const rise::CString& GetConfig() const;

    //!         reload config
    /*! \param  bCreate - create new config, if no config found
        \return xml-config
        */
    rise::xml::CXMLNode& ReloadConfig(bool bCreate = false);
    
    //!         save config
    void SaveConfig();

    //!         get config
    /*! \return config
    */
    rise::xml::CXMLNode& Config();
    
    //!         get config
    /*! \return config
    */
    const rise::xml::CXMLNode& Config() const;

  protected:
    //!         protected constructor
    CComponentConfig();

    //!         internal initialization
    void Init(const rise::CString& sComponent, const rise::CString& sConfig, bool bCreate);

  private:
    CComponentConfig& operator=(const CComponentConfig&);
    CComponentConfig(const CComponentConfig&);

    class CComponentConfigImpl;
    CComponentConfigImpl* m_pImpl;
    friend class CComponentConfigManager;
  };
}


#endif // _COMPONENTCONFIG_H_
