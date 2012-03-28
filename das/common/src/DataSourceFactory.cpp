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
#include <staff/utils/File.h>
#include <staff/utils/Log.h>
#include <staff/xml/Document.h>
#include <staff/xml/Element.h>
#include <staff/xml/XmlReader.h>
#include <staff/common/Runtime.h>
#include <staff/component/Component.h>
#include <staff/component/SharedContext.h>
#include "ProviderComponent.h"
#include "ProviderServiceWrapper.h"
#include "DataSource.h"
#include "DataSourceFactory.h"

namespace staff
{
namespace das
{
  class DataSourceFactory::DataSourceFactoryImpl
  {
  public:
    typedef SharedPtr<ProviderComponent> PProviderComponent;
    typedef std::list<PProviderComponent> ProviderComponentList;
    typedef std::map<std::string, DataSource> DataSourcesMap;
    typedef std::map<std::string, DataSourcesMap> DataSourcesNamespacesMap;

  public:
    void Init()
    {
      const std::string& sDataSourcesDir = staff::Runtime::Inst().GetComponentHome("staff.das") + 
        STAFF_PATH_SEPARATOR "datasources" STAFF_PATH_SEPARATOR;

      StringList lsDataSourcesDirs;
      File(sDataSourcesDir).List(lsDataSourcesDirs, "*", File::AttributeDirectory);

      for (StringList::const_iterator itDir = lsDataSourcesDirs.begin(); itDir != lsDataSourcesDirs.end(); ++itDir)
      {
        const std::string& sDir = sDataSourcesDir + *itDir;
        StringList lsDataSourcesFiles;
        File(sDir).List(lsDataSourcesFiles, "*.datasources", File::AttributeAnyFile);
        for (StringList::const_iterator itFile = lsDataSourcesFiles.begin();
             itFile != lsDataSourcesFiles.end(); ++itFile)
        {
          try
          {
            xml::Document tDoc;
            const std::string& sFileName = sDir + STAFF_PATH_SEPARATOR + *itFile;

            xml::XmlFileReader(sFileName).ReadDocument(tDoc);

            for (xml::Element* pDataSource = tDoc.GetRootElement().GetFirstChildElement();
                pDataSource; pDataSource = pDataSource->GetNextSiblingElement())
            {
              DataSource tDataSource;
              tDataSource.Load(*pDataSource, sFileName);
              const std::string& sNamespace = tDataSource.GetNamespace();
              const std::string& sName = tDataSource.GetName();

              LogDebug2() << "Adding datasource: " << sNamespace << "." << sName;
              m_mDataSourcesNamespaces[sNamespace][sName] = tDataSource;
            }
          }
          STAFF_CATCH_ALL
        }
      }

      for (DataSourceFactoryImpl::DataSourcesNamespacesMap::const_iterator itDataSourceNamespace =
           m_mDataSourcesNamespaces.begin();
            itDataSourceNamespace != m_mDataSourcesNamespaces.end(); ++itDataSourceNamespace)
      {
        std::string sNamespace = itDataSourceNamespace->first;
        PProviderComponent tpComponent(new ProviderComponent(sNamespace));
        m_lsComponents.push_back(tpComponent);

        if (!sNamespace.empty())
        {
          sNamespace += '.';
        }

        for (DataSourceFactoryImpl::DataSourcesMap::const_iterator itDataSource =
             itDataSourceNamespace->second.begin();
          itDataSource != itDataSourceNamespace->second.end(); ++itDataSource)
        {
          const std::string& sName = sNamespace + itDataSource->first;
          tpComponent->AddServiceWrapper(sName, new ProviderServiceWrapper(tpComponent, &itDataSource->second));
        }

        SharedContext::Inst().AddComponent(tpComponent);
      }

    }

  public:
    DataSourcesNamespacesMap m_mDataSourcesNamespaces;
    ProviderComponentList m_lsComponents;
  };

  DataSourceFactory::DataSourceFactory()
  {
    m_pImpl = new DataSourceFactoryImpl;
    try
    {
      m_pImpl->Init();
    }
    STAFF_CATCH_ALL
  }

  DataSourceFactory::~DataSourceFactory()
  {
    delete m_pImpl;
  }

  DataSourceFactory& DataSourceFactory::Inst()
  {
    static DataSourceFactory tInst;
    return tInst;
  }

  const DataSource& DataSourceFactory::GetDataSource(const std::string& sName) const
  {
    std::string sDataSourceNamespace;
    std::string sDataSourceName;

    std::string::size_type nPos = sName.find_last_of('.');

    if (nPos != std::string::npos)
    {
      sDataSourceNamespace = sName.substr(0, nPos);
      sDataSourceName = sName.substr(nPos + 1);
    }
    else
    {
      sDataSourceName = sName;
    }

    DataSourceFactoryImpl::DataSourcesNamespacesMap::const_iterator itDataSourceNamespace =
        m_pImpl->m_mDataSourcesNamespaces.find(sDataSourceNamespace);
    STAFF_ASSERT(itDataSourceNamespace != m_pImpl->m_mDataSourcesNamespaces.end(),
                 "Data source with namespace \"" + sDataSourceNamespace + "\" is not found");

    DataSourceFactoryImpl::DataSourcesMap::const_iterator itDataSource =
        itDataSourceNamespace->second.find(sDataSourceName);
    STAFF_ASSERT(itDataSource != itDataSourceNamespace->second.end(),
                 "Data source \"" + sDataSourceName + "\" is not found in namespace \""
                 + sDataSourceNamespace + "\"");
    return itDataSource->second;
  }

  void DataSourceFactory::GetDataSources(StringList& rlsSources) const
  {
    rlsSources.clear();
    for (DataSourceFactoryImpl::DataSourcesNamespacesMap::const_iterator itDataSourceNamespace =
         m_pImpl->m_mDataSourcesNamespaces.begin();
          itDataSourceNamespace != m_pImpl->m_mDataSourcesNamespaces.end(); ++itDataSourceNamespace)
    {
      std::string sNamespace = itDataSourceNamespace->first;
      if (sNamespace.size() != 0)
      {
        sNamespace += '.';
      }

      for (DataSourceFactoryImpl::DataSourcesMap::const_iterator itDataSource =
           itDataSourceNamespace->second.begin();
        itDataSource != itDataSourceNamespace->second.end(); ++itDataSource)
      {
        rlsSources.push_back(sNamespace + itDataSource->first);
      }
    }
  }

}
}
