#include <rise/common/MutablePtr.h>
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

  CDataObject ProviderService::Invoke(const CDataObject& rdoOperation)
  {
    if (!m_tpProvider.Get())
    {
      m_pDataSource = &DataSourceFactory::Inst().GetDataSource(IService::GetServiceName());
      m_tpProvider = ProviderFactory::Inst().Allocate(m_pDataSource->GetProviderName());
      m_tpProvider->Init(*m_pDataSource);
    }

    ScriptExecuter tScriptExecuter(rdoOperation, *m_pDataSource, m_tpProvider);
    return tScriptExecuter.Process();
  }

}
}
