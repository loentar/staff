#ifndef _MYSQLPROVIDER_H_
#define _MYSQLPROVIDER_H_

#include <string>
#include <staff/das/common/Provider.h>

namespace staff
{
  namespace das
  {
    class DataSource;
    struct Type;

    //! MySql provider
    class MySql: public IProvider
    {
    public:
      MySql();

      virtual ~MySql();

      //! initialize
      /*! \param rDataSource - data source
        */
      virtual void Init(const DataSource& rDataSource);

      //! deinitialize
      virtual void Deinit();

      //! get provider name
      /*! \return provider name
        */
      virtual const std::string& GetName() const;

      //! get provider description
      /*! \return provider description
        */
      virtual const std::string& GetDescr() const;

      //! invoke the operation
      /*! \param  sExecute - execute string
          \param  rdoResult - operation result
          */
      virtual void Invoke(const std::string& sExecute, const Type& rstReturnType, staff::CDataObject& rdoResult);

    private:
      class MySqlImpl;
      MySqlImpl* m_pImpl;
    };

  }
}

#endif // _MYSQLPROVIDER_H_
