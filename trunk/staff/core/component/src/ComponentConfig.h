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
#include <string>

namespace staff
{
  namespace xml
  {
    class Element;
  }

  //!  Component config
  class STAFF_COMPONENT_EXPORT ComponentConfig
  {
  public:
    //! destructor
    ~ComponentConfig();

    //! get component name
    /*! \return component name
    */
    const std::string& GetComponent() const;

    //! get config file name
    /*! \return config file name
    */
    const std::string& GetConfig() const;

    //! reload config
    /*! \param  bCreate - create new config, if no config found
        \return xml-config
        */
    xml::Element& ReloadConfig(bool bCreate = false);
    
    //! save config
    void SaveConfig();

    //! get config
    /*! \return config
    */
    xml::Element& Config();
    
    //! get config
    /*! \return config
    */
    const xml::Element& Config() const;

  protected:
    //! protected constructor
    ComponentConfig();

    //! internal initialization
    void Init(const std::string& sComponent, const std::string& sConfig, bool bCreate);

  private:
    ComponentConfig& operator=(const ComponentConfig&);
    ComponentConfig(const ComponentConfig&);

    class ComponentConfigImpl;
    ComponentConfigImpl* m_pImpl;
    friend class ComponentConfigManager;
  };

}


#endif // _COMPONENTCONFIG_H_
