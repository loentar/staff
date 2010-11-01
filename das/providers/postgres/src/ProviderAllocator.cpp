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

#include <rise/plugin/PluginExport.h>
#include <rise/common/MutablePtr.h>
#include "Postgres.h"

namespace staff
{
namespace das
{
  class ProviderAllocatorImpl: public ProviderAllocator
  {
  public:
    virtual PProvider Allocate(const std::string& sName)
    {
      if (sName == "staff.das.Postgres")
      {
        return PProvider(new ::staff::das::Postgres);
      }
      else
      {
        RISE_THROWS(rise::CLogicNoItemException, "Can't instantiate provider: " + sName +
                    "\nAvailable providers are:\n"
                    "  staff.das.Postgres\n"
                    );
      }
    }

    virtual void GetProvidersList(StringList& rlsProvidersList)
    {
      rlsProvidersList.clear();
      rlsProvidersList.push_back("staff.das.Postgres");
    }
  };
}
}

RISE_DECLARE_PLUGIN(::staff::das::ProviderAllocatorImpl);

