#ifndef PROVIDERSERVICE_H
#define PROVIDERSERVICE_H

#include <staff/common/IService.h>
#include "Provider.h"

namespace staff
{
  class CDataObject;

namespace das
{
  class DataSource;
  struct Operation;

  class ProviderService: public IService
  {
  public:
    ProviderService();
    CDataObject Invoke(const CDataObject& rdoOperation);

  private:
    PProvider m_tpProvider;
    const DataSource* m_pDataSource;
  };
}
}

#endif // PROVIDERSERVICE_H
