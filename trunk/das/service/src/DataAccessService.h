#ifndef _DATAACCESSSERVICE_H_
#define _DATAACCESSSERVICE_H_

#include <staff/common/IService.h>
#include <string>
#include <list>

namespace staff
{
  class CDataObject;

  namespace das
  {
    typedef std::list<std::string> StringList; //!< string list

    // *loadAtStartup: true
    //! Data access service
    class DataAccessService: public staff::IService
    {
    public:
      virtual ~DataAccessService() {}

      //! get providers
      /*! \return provider list
        */
      virtual StringList GetProviders() = 0;

      //! get data sources
      /*! \return data sources list
        */
      virtual StringList GetDataSources() = 0;

      //! use data source
      /*! \param sDataSource - data source name
        */
      virtual void SetDataSource(const std::string& sDataSource) = 0;

      //! free data source
      virtual void FreeDataSource() = 0;

      //! get interface definition
      /*! \return interface
        */
      virtual CDataObject GetInterface() const = 0;

      //! invoke the operation
      /*! \param  rdoOperation - operation name and params
          \return operation result
          */
      virtual CDataObject Invoke(const CDataObject& rdoOperation) = 0;

    };
  }
}

#endif // _DATAACCESSSERVICE_H_
