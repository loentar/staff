#ifndef _LOGIN_H_
#define _LOGIN_H_

#include <string>

namespace staff
{
  //!  сервис аутентификации
  class CLogin
  {
  public:
    //!        деструктор
    virtual ~CLogin() {}

    //!         аутентификация и вход пользователя в систему
    /*! аутентификация пользователя производится из гостевой сессии
        \param  sUserName - имя пользователя
        \param  sPassword - пароль
        \return идентификатор сессии
        */
    virtual std::string Login(const std::string& sUserName, const std::string& sPassword) = 0;
    
    //!         открыть дополнительную сессию
    /*! функция должна вызываться не из гостевой сессии
        \return идентификатор дополнительной сессии
        */
    virtual std::string OpenExtraSession() = 0;

    //!         выход пользователя из системы
    virtual void Logout() = 0;

    //!         продлить сессию
    virtual void KeepAliveSession() = 0;

    //!         проверить идентификатор сессии на валидность
    /*! \return true - сессия верна
        */
    virtual bool ValidateSession() = 0;
    
    //!         получить имя пользователя
    /*! \return текущий имя пользователя
        */
    virtual std::string GetUserName() = 0;

    //!         получить время истечения сессии по умолчанию
    /*! \return время истечения сессии в минутах
        */
    virtual int GetSessionExpiration() const = 0;

  };
  
}

#endif // _LOGIN_H_
