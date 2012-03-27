#ifndef _DATAACCESSSERVICE_H_
#define _DATAACCESSSERVICE_H_

#include <staff/common/IService.h>
#include <string>
#include <list>

namespace staff
{
  class DataObject;

  namespace das
  {
    typedef std::list<std::string> StringList; //!< string list

    //! Data service
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

      //! get interdace definition
      /*! \return interface
        */
      virtual DataObject GetInterface() const = 0;

      //! invoke the operation
      /*! \param  rdoOperation - operation name and params
          \return operation result
          */
      virtual DataObject Invoke(const DataObject& rdoOperation) = 0;

    };
  }
}

#endif // _DATAACCESSSERVICE_H_
