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

#include <rise/common/MutablePtr.h>
#include <rise/xml/XMLNode.h>
#include <staff/common/DataObject.h>
#include "ProviderFactory.h"
#include "DataSourceFactory.h"
#include "DataSource.h"
#include "ScriptExecuter.h"
#include "ProviderService.h"

namespace staff
{
namespace das
{

  ProviderService::ProviderService():
    m_pDataSource(NULL)
  {
  }

  void ProviderService::OnCreate()
  {
    m_pDataSource = &DataSourceFactory::Inst().GetDataSource(IService::GetServiceName());

    const ProvidersInfoList& rlsProviders = m_pDataSource->GetProviders();

    for (ProvidersInfoList::const_iterator itProvider = rlsProviders.begin();
         itProvider != rlsProviders.end(); ++itProvider)
    {
      const rise::xml::CXMLNode& rConfigElem = itProvider->tConfig;
      rise::xml::CXMLNode::TXMLNodeConstIterator itOnCreate = rConfigElem.FindSubnode("oncreate");
      if (itOnCreate != rConfigElem.NodeEnd())
      {
        rise::xml::CXMLNode::TXMLNodeConstIterator itScript = itOnCreate->FindSubnode("script");
        if (itScript == itOnCreate->NodeEnd())
        {
          itScript = itOnCreate->FindSubnode("execute");
        }

        if (itScript != itOnCreate->NodeEnd())
        {
          ScriptExecuter tScriptExecuter(*m_pDataSource, GetProviders());
          return tScriptExecuter.Process(*itScript);
        }
      }
    }
  }

  void ProviderService::OnDestroy()
  {
    RISE_ASSERTS(m_pDataSource, "Not initialized");

    const ProvidersInfoList& rlsProviders = m_pDataSource->GetProviders();

    for (ProvidersInfoList::const_iterator itProvider = rlsProviders.begin();
         itProvider != rlsProviders.end(); ++itProvider)
    {
      const rise::xml::CXMLNode& rConfigElem = itProvider->tConfig;
      rise::xml::CXMLNode::TXMLNodeConstIterator itOnDestroy = rConfigElem.FindSubnode("ondestroy");
      if (itOnDestroy != rConfigElem.NodeEnd())
      {
        rise::xml::CXMLNode::TXMLNodeConstIterator itScript = itOnDestroy->FindSubnode("script");
        if (itScript == itOnDestroy->NodeEnd())
        {
          itScript = itOnDestroy->FindSubnode("execute");
        }

        if (itScript != itOnDestroy->NodeEnd())
        {
          ScriptExecuter tScriptExecuter(*m_pDataSource, GetProviders());
          return tScriptExecuter.Process(*itScript);
        }
      }
    }
  }

  DataObject ProviderService::Invoke(const DataObject& rdoOperation)
  {
    RISE_ASSERTS(m_pDataSource, "Not initialized");

    ScriptExecuter tScriptExecuter(*m_pDataSource, GetProviders());
    return tScriptExecuter.Process(rdoOperation);
  }

  Providers& ProviderService::GetProviders()
  {
    if (m_stProviders.mProviders.empty())
    {
      RISE_ASSERTS(m_pDataSource, "Not initialized");

      const ProvidersInfoList& rlsProviders = m_pDataSource->GetProviders();

      if (!rlsProviders.empty())
      {
        ProviderFactory& rProviderFactory = ProviderFactory::Inst();
        m_stProviders.sDefaultId = rlsProviders.front().sId;

        for (ProvidersInfoList::const_iterator itProvider = rlsProviders.begin();
             itProvider != rlsProviders.end(); ++itProvider)
        {
          PProvider tpProvider = rProviderFactory.Allocate(itProvider->sName);
          tpProvider->Init(itProvider->tConfig);
          m_stProviders.mProviders[itProvider->sId] = tpProvider;
        }
      }
    }

    return m_stProviders;
  }

}
}
