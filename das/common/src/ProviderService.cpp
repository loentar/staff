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

    const rise::xml::CXMLNode& rConfigElem = m_pDataSource->GetProviderConfig();
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
        ScriptExecuter tScriptExecuter(*m_pDataSource, GetProvider());
        return tScriptExecuter.Process(*itScript);
      }
    }
  }

  void ProviderService::OnDestroy()
  {
    RISE_ASSERTS(m_pDataSource, "Can't get datasource");

    const rise::xml::CXMLNode& rConfigElem = m_pDataSource->GetProviderConfig();
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
        ScriptExecuter tScriptExecuter(*m_pDataSource, GetProvider());
        return tScriptExecuter.Process(*itScript);
      }
    }
  }

  CDataObject ProviderService::Invoke(const CDataObject& rdoOperation)
  {
    RISE_ASSERTS(m_pDataSource, "Can't get datasource");

    ScriptExecuter tScriptExecuter(*m_pDataSource, GetProvider());
    return tScriptExecuter.Process(rdoOperation);
  }

  PProvider& ProviderService::GetProvider()
  {
    if (!m_tpProvider.Get())
    {
      m_tpProvider = ProviderFactory::Inst().Allocate(m_pDataSource->GetProviderName());
      m_tpProvider->Init(*m_pDataSource);
    }

    return m_tpProvider;
  }

}
}
