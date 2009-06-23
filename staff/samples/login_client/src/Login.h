#ifndef _LOGIN_H_
#define _LOGIN_H_

#include <string>

namespace staff
{

  //!  Login service
  class CLogin
  {
  public:
    //!        destructor
    virtual ~CLogin() {}

    //!         login user and create session
    /*! this operation must be called from guest session
        \param  sUserName - username
        \param  sPassword - password
        \return created session id
        */
    virtual std::string Login(const std::string& sUserName, const std::string& sPassword) = 0;

    //!         logout and close session
    virtual void Logout() = 0;

    //!         keepalive session
    virtual void KeepAliveSession() = 0;

    //!         validate session
    /*! \return true - session is valid
        */
    virtual bool ValidateSession() = 0;
    
    //!         get current user name
    /*! \return current use name
        */
    virtual std::string GetUserName() = 0;

    //!         get session expiration time
    /*! \return session expiration time in minutes
        */
    virtual int GetSessionExpiration() const = 0;

  };
  
}

#endif // _LOGIN_H_
