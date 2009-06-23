#ifndef _AXIS2CLIENT_H_
#define _AXIS2CLIENT_H_

#include "staffclientexport.h"
#include <string>

namespace staff
{
  class COperation;

  //!  Axis2Client
  class STAFF_CLIENT_EXPORT CAxis2Client
  {
  public:
    //!         constructor
    CAxis2Client();

    //!         destructor
    ~CAxis2Client();

    //!         client init
    /*! \param  sServiceUri - service URI
        \param  sSessionId - sessionId
        */
    void Init(const std::string& sServiceUri, const std::string& sSessionId);

    //!         deinit
    void Deinit();

    //!         get service URI
    /*! \return service URI
        */
    const std::string& GetServiceURI() const;

    //!         invoke operation
    /*! \param  rOperation - operation
        */
    void Invoke(COperation& rOperation);    

    //!         check for init
    /*! \return true if Init() was called
        \sa Init
        */
    bool IsInit() const;
    
  private:
    CAxis2Client(const CAxis2Client&);
    CAxis2Client& operator=(const CAxis2Client&);
    class CAxis2ClientImpl;
    CAxis2ClientImpl* m_pImpl;
  };
}

#endif // _AXIS2CLIENT_H_
