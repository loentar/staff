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

#ifndef PROVIDERSERVICE_H
#define PROVIDERSERVICE_H

#include <staff/common/IService.h>
#include <staff/utils/Mutex.h>
#include "staffdascommonexport.h"
#include "Provider.h"
#include "ScriptExecuter.h"

namespace staff
{
  class DataObject;

namespace das
{
  class DataSource;
  struct Operation;

  class STAFF_DAS_COMMON_EXPORT ProviderService: public IService
  {
  public:
    ProviderService();
    virtual void OnCreate();
    virtual void OnDestroy();
    virtual void Invoke(const DataObject& rdoOperation, DataObject& rdoResult);

  private:
    Providers& GetProviders();

  private:
    Providers m_stProviders;
    const DataSource* m_pDataSource;
    VarMap m_mSessionStorage;
    Mutex m_tSessionStorageMutex;
  };
}
}

#endif // PROVIDERSERVICE_H
