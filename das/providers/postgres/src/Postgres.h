#ifndef _POSTGRESPROVIDER_H_
#define _POSTGRESPROVIDER_H_

#include <string>
#include <staff/das/common/Provider.h>

namespace staff
{
  namespace das
  {
    class DataSource;
    struct Type;

    //! postgres provider
    class Postgres: public IProvider
    {
    public:
      Postgres();

      virtual ~Postgres();

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
      class PostgresImpl;
      PostgresImpl* m_pImpl;
    };

  }
}

#endif // _POSTGRESPROVIDER_H_
