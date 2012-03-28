/*
 *  Copyright 2010 Utkin Dmitry
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

#ifndef PROVIDERCOMPONENT_H
#define PROVIDERCOMPONENT_H

#include <string>
#include <staff/component/Component.h>
#include "staffdascommonexport.h"

namespace staff
{
namespace das
{
  class STAFF_DAS_COMMON_EXPORT ProviderComponent: public Component
  {
  public:
    ProviderComponent(const std::string& sName);

    void AddServiceWrapper(const std::string& sServiceName, ServiceWrapper* pServiceWrapper);

    //! get component name
    /*! \return component name
    */
    virtual const std::string& GetName() const;

  private:
    std::string m_sName;
  };
}
}

#endif // PROVIDERCOMPONENT_H
