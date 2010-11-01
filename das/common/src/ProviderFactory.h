#ifndef PROVIDERFACTORY_H
#define PROVIDERFACTORY_H

#include <string>
#include <list>
#include "Provider.h"

namespace staff
{
namespace das
{
  class ProviderFactory
  {
  public:
    static ProviderFactory& Inst();

    void FreeInst();

    void GetProviders(StringList& rlsProviders);

    PProvider Allocate(const std::string& sProvider);

  private:
    ProviderFactory();
    ~ProviderFactory();

  private:
    static ProviderFactory* m_pInst;
    class ProviderFactoryImpl;
    ProviderFactoryImpl* m_pImpl;
  };
}
}

#endif // PROVIDERFACTORY_H
