#ifndef _DATASOURCEFACTORY_H_
#define _DATASOURCEFACTORY_H_

#include <string>
#include <list>

namespace staff
{
namespace das
{
  class DataSource;

  typedef std::list<std::string> StringList;

  class DataSourceFactory
  {
  public:
    static DataSourceFactory& Inst();
    static void FreeInst();

    const DataSource& GetDataSource(const std::string& sName) const;
    void GetDataSources(StringList& rlsSources) const;

  private:
    DataSourceFactory();
    ~DataSourceFactory();

  private:
    static DataSourceFactory* m_pInst;
    class DataSourceFactoryImpl;
    DataSourceFactoryImpl* m_pImpl;
  };
}
}

#endif // _DATASOURCEFACTORY_H_
