#ifndef PROVIDERCOMPONENT_H
#define PROVIDERCOMPONENT_H

#include <string>
#include <staff/component/Component.h>

namespace staff
{
namespace das
{
  class ProviderComponent: public CComponent
  {
  public:
    ProviderComponent(const std::string& sName);

    void AddServiceWrapper(const std::string& sServiceName, PServiceWrapper& rpServiceWrapper);

    //!         get component name
    /*! \return component name
    */
    virtual const std::string& GetName() const;

    //!         get service with given name
    /*! \param  sService - service name
        \return pointer to service or NULL, if no service found
        */
    virtual const CServiceWrapper* GetService(const std::string& sService) const;

    //!         get service with given name
    /*! \param  sService - service name
        \return pointer to service or NULL, if no service found
        */
    virtual CServiceWrapper* GetService(const std::string& sService);

    //!         get component's services map
    /*! \return component's services map
    */
    virtual const TServiceWrapperMap& GetServices() const;

  private:
    staff::TServiceWrapperMap m_mServices;
    std::string m_sName;
  };
}
}

#endif // PROVIDERCOMPONENT_H
