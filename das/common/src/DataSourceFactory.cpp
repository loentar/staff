#include <rise/tools/FileFind.h>
#include <rise/xml/XMLDocument.h>
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
    typedef rise::CMutablePtr<ProviderComponent> PProviderComponent;
    typedef std::list<PProviderComponent> ProviderComponentList;
    typedef std::map<std::string, DataSource> DataSourcesMap;
    typedef std::map<std::string, DataSourcesMap> DataSourcesNamespacesMap;

  public:
    void Init()
    {
      const std::string& sDataSourcesDir = staff::CRuntime::Inst().GetComponentHome("staff.das") + "/datasources/";

      StringList lsDataSourcesDirs;
      rise::CFileFind::Find(sDataSourcesDir, lsDataSourcesDirs, "*", rise::CFileFind::EFA_DIR);

      for (StringList::const_iterator itDir = lsDataSourcesDirs.begin(); itDir != lsDataSourcesDirs.end(); ++itDir)
      {
        const std::string& sDir = sDataSourcesDir + *itDir;
        StringList lsDataSourcesFiles;
        rise::CFileFind::Find(sDir, lsDataSourcesFiles, "*.datasources", rise::CFileFind::EFA_FILE);
        for (StringList::const_iterator itFile = lsDataSourcesFiles.begin(); itFile != lsDataSourcesFiles.end(); ++itFile)
        {
          rise::xml::CXMLDocument tDoc;
          const std::string& sFileName = sDir + '/' + *itFile;

          tDoc.LoadFromFile(sFileName);

          for (rise::xml::CXMLNode::TXMLNodeConstIterator itDataSource = tDoc.GetRoot().NodeBegin();
              itDataSource != tDoc.GetRoot().NodeEnd(); ++itDataSource)
          {
            if (itDataSource->NodeType() == rise::xml::CXMLNode::ENTGENERIC)
            {
              DataSource tDataSource;
              tDataSource.Load(*itDataSource, sFileName);
              const std::string& sNamespace = tDataSource.GetNamespace();
              const std::string& sName = tDataSource.GetName();

              rise::LogDebug2() << "Adding datasource: " << sNamespace << "." << sName;
              m_mDataSourcesNamespaces[sNamespace][sName] = tDataSource;
            }
          }
        }
      }

      for (DataSourceFactoryImpl::DataSourcesNamespacesMap::const_iterator itDataSourceNamespace =
           m_mDataSourcesNamespaces.begin();
            itDataSourceNamespace != m_mDataSourcesNamespaces.end(); ++itDataSourceNamespace)
      {
        std::string sNamespace = itDataSourceNamespace->first;
        PProviderComponent tpComponent(new ProviderComponent(sNamespace));
        m_lsComponents.push_back(tpComponent);

        if (sNamespace.size() != 0)
        {
          sNamespace += '.';
        }

        for (DataSourceFactoryImpl::DataSourcesMap::const_iterator itDataSource =
             itDataSourceNamespace->second.begin();
          itDataSource != itDataSourceNamespace->second.end(); ++itDataSource)
        {
          const std::string& sName = sNamespace + itDataSource->first;
          PServiceWrapper tpServiceWrapper(new ProviderServiceWrapper(tpComponent, &itDataSource->second));
          tpComponent->AddServiceWrapper(sName, tpServiceWrapper);
        }

        CSharedContext::Inst().AddComponent(tpComponent);
      }

    }

  public:
    DataSourcesNamespacesMap m_mDataSourcesNamespaces;
    ProviderComponentList m_lsComponents;
  };

  DataSourceFactory::DataSourceFactory()
  {
    m_pImpl = new DataSourceFactoryImpl;
  }

  DataSourceFactory::~DataSourceFactory()
  {
    delete m_pImpl;
  }

  DataSourceFactory& DataSourceFactory::Inst()
  {
    if (!m_pInst)
    {
      m_pInst = new DataSourceFactory;
      m_pInst->m_pImpl->Init();
    }

    return *m_pInst;
  }

  void DataSourceFactory::FreeInst()
  {
    delete m_pInst;
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
    RISE_ASSERTS(itDataSourceNamespace != m_pImpl->m_mDataSourcesNamespaces.end(),
                 "Data source with namespace \"" + sDataSourceNamespace + "\" is not found");

    DataSourceFactoryImpl::DataSourcesMap::const_iterator itDataSource =
        itDataSourceNamespace->second.find(sDataSourceName);
    RISE_ASSERTS(itDataSource != itDataSourceNamespace->second.end(),
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

  DataSourceFactory* DataSourceFactory::m_pInst = NULL;

}
}
