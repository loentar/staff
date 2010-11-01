#ifndef PROVIDERSERVICEWRAPPER_H
#define PROVIDERSERVICEWRAPPER_H

#include <staff/component/ServiceWrapper.h>

namespace staff
{
namespace das
{
  class DataSource;

  class ProviderServiceWrapper: public CServiceWrapper
  {
  public:
    ProviderServiceWrapper(CComponent* pComponent, const DataSource* pDataSource);

    //!         get service name
    /*! \return service name
    */
    virtual const std::string& GetName() const;

    //!         get service description
    /*! \return service description
    */
    virtual const std::string& GetDescr() const;

    //!         get service operations
    /*! \return service operations
    */
    virtual CDataObject GetOperations() const;

    //!         invoke service operation
    /*! \param  rOperation - operation
        \param  sSessionId - service session id
        \param  sInstanceId - service instance id
        */
    virtual void Invoke(COperation& rOperation, const std::string& sSessionId, const std::string& sInstanceId);

    //!         get service's component
    /*! \return service's component
    */
    virtual const CComponent* GetComponent() const;

    //!         get service's component
    /*! \return service's component
    */
    virtual CComponent* GetComponent();

    //!         get pointer to service implementation
    /*! \param  sSessionId - service session id
        \param  sInstanceId - service instance id
        \return pointer to service implementation or NULL, if service non-local
        */
    virtual IService* GetImpl(const std::string& sSessionId, const std::string& sInstanceId);

    //!         create new service impl
    /*! \return resulting service impl
      */
    virtual IService* NewImpl();

  private:
    //!         get service description
    /*! \return service description
    */
    virtual CDataObject GetServiceDescription() const;

  private:
    CComponent* m_pComponent;
    const DataSource* m_pDataSource;
    std::string m_sName;
  };
}
}

#endif // PROVIDERSERVICEWRAPPER_H
