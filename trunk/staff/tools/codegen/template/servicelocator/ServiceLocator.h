// This file generated by staff_codegen
// For more information please visit: http://code.google.com/p/staff/
// DO NOT EDIT

#ifndef _SERVICELOCATOR_H_
#define _SERVICELOCATOR_H_

#include <string>

namespace staff
{
  //!  Service locator
  class CServiceLocator
  {
  public:
    //!         get instance
    /*! \return instance
    */
    static CServiceLocator& Inst();
    
    //!         get pointer to service wrapper
    /*! \param  sServiceName - service name
        \param  sID - service session id
        */
    void* LocateService(const std::string& sServiceName, const std::string& sID);

  protected:
    CServiceLocator();
    ~CServiceLocator();
  
  private:
    class CServiceLocatorImpl;
    CServiceLocatorImpl* m_pImpl;
    static CServiceLocator* m_pInst;
  };
}

#endif // _SERVICELOCATOR_H_