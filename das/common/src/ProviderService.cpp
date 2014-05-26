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

#include <staff/utils/SharedPtr.h>
#include <staff/xml/Element.h>
#include <staff/xml/Attribute.h>
#include <staff/common/DataObject.h>
#include "ProviderFactory.h"
#include "DataSourceFactory.h"
#include "DataSource.h"
#include "ProviderService.h"

namespace staff
{
namespace das
{

  ProviderService::ProviderService():
    m_pDataSource(NULL), m_tSessionStorageMutex(Mutex::Recursive)
  {
  }

  void ProviderService::OnCreate()
  {
    Var& rVar = m_mSessionStorage["id"];
    rVar.tType.eType = DataType::Generic;
    rVar.sValue = GetSessionId();

    m_pDataSource = &DataSourceFactory::Inst().GetDataSource(IService::GetServiceName());

    const ProvidersInfoList& rlsProviders = m_pDataSource->GetProviders();

    for (ProvidersInfoList::const_iterator itProvider = rlsProviders.begin();
         itProvider != rlsProviders.end(); ++itProvider)
    {
      const xml::Element& rConfigElem = itProvider->tConfig;
      const xml::Element* pOnCreate = rConfigElem.FindChildElementByName("oncreate");
      if (pOnCreate)
      {
        const xml::Element* pScript = pOnCreate->FindChildElementByName("script");
        if (!pScript)
        {
          pScript = pOnCreate->FindChildElementByName("execute");
        }

        if (pScript)
        {
          ScriptExecuter tScriptExecuter(*m_pDataSource, GetProviders());
          tScriptExecuter.SetSessionStorage(m_mSessionStorage, m_tSessionStorageMutex);
          tScriptExecuter.Process(*pScript);
        }
      }
    }
  }

  void ProviderService::OnDestroy()
  {
    STAFF_ASSERT(m_pDataSource, "Not initialized");

    const ProvidersInfoList& rlsProviders = m_pDataSource->GetProviders();

    for (ProvidersInfoList::const_iterator itProvider = rlsProviders.begin();
         itProvider != rlsProviders.end(); ++itProvider)
    {
      const xml::Element& rConfigElem = itProvider->tConfig;
      const xml::Element* pOnDestroy = rConfigElem.FindChildElementByName("ondestroy");
      if (pOnDestroy)
      {
        const xml::Element* pScript = pOnDestroy->FindChildElementByName("script");
        if (!pScript)
        {
          pScript = pOnDestroy->FindChildElementByName("execute");
        }

        if (pScript)
        {
          ScriptExecuter tScriptExecuter(*m_pDataSource, GetProviders());
          tScriptExecuter.SetSessionStorage(m_mSessionStorage, m_tSessionStorageMutex);
          tScriptExecuter.Process(*pScript);
        }
      }
    }
  }

  void ProviderService::Invoke(const DataObject& rdoOperation, DataObject& rdoResult)
  {
    STAFF_ASSERT(m_pDataSource, "Not initialized");

    ScriptExecuter tScriptExecuter(*m_pDataSource, GetProviders());
    tScriptExecuter.SetSessionStorage(m_mSessionStorage, m_tSessionStorageMutex);
    tScriptExecuter.Process(rdoOperation, rdoResult);
  }

  Providers& ProviderService::GetProviders()
  {
    if (m_stProviders.mProviders.empty())
    {
      STAFF_ASSERT(m_pDataSource, "Not initialized");

      const ProvidersInfoList& rlsProviders = m_pDataSource->GetProviders();

      if (!rlsProviders.empty())
      {
        ProviderFactory& rProviderFactory = ProviderFactory::Inst();
        for (ProvidersInfoList::const_iterator itProvider = rlsProviders.begin();
             itProvider != rlsProviders.end(); ++itProvider)
        {
          if (m_stProviders.sDefaultId.empty())
          {
            const xml::Attribute* pAttr = itProvider->tConfig.FindAttribute("default");
            if (pAttr != NULL && pAttr->GetValue() == "true")
            {
              m_stProviders.sDefaultId = itProvider->sId;
            }
          }

          PProvider tpProvider = rProviderFactory.Allocate(itProvider->sName);
          tpProvider->Init(itProvider->tConfig);
          m_stProviders.mProviders[itProvider->sId] = tpProvider;
        }

        if (m_stProviders.sDefaultId.empty())
        {
          m_stProviders.sDefaultId = rlsProviders.front().sId;
        }
      }
    }

    return m_stProviders;
  }

}
}
