#ifndef _PROVIDER_H_
#define _PROVIDER_H_

#include <string>
#include <list>
#include <staff/codegen/Interface.h>

namespace rise
{
  template <typename Type> class CMutablePtr;
  namespace xml
  {
    class CXMLNode;
  }
}

namespace staff
{
  class CDataObject;

  namespace das
  {
    class DataSource;
    struct Type;

    typedef std::list<std::string> StringList; //!< string list

    //! DAS provider wrapper
    class IProvider
    {
    public:
      virtual ~IProvider() {}

      //! initialize
      /*! \param rDataSource - data source
        */
      virtual void Init(const DataSource& rDataSource) = 0;

      //! deinitialize
      virtual void Deinit() = 0;

      //! get provider name
      /*! \return provider name
        */
      virtual const std::string& GetName() const = 0;

      //! get provider description
      /*! \return provider description
        */
      virtual const std::string& GetDescr() const = 0;

      //! invoke the operation
      /*! \param  sExecute - execute string
          \return operation result
          */
      virtual void Invoke(const std::string& sExecute, const Type& rstReturnType, staff::CDataObject& rdoResult) = 0;
    };

    //! pointer to provider wrapper
    typedef rise::CMutablePtr<IProvider> PProvider;

    //! provider allocator
    class ProviderAllocator
    {
    public:
      //! virtual destructor
      virtual ~ProviderAllocator() {}

      //! create new provider instance
      /*! \return new provider instance
        */
      virtual PProvider Allocate(const std::string& sName) = 0;

      //! get providers list
      /*! \param rlsProvidersList - resulting providers list
        */
      virtual void GetProvidersList(StringList& rlsProvidersList) = 0;
    };

  }
}

#endif // _PROVIDER_H_
