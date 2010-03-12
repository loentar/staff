#ifndef _AXIS2CLIENT_H_
#define _AXIS2CLIENT_H_

#include "staffclientexport.h"
#include <rise/string/String.h>

namespace staff
{
  class COperation;

  //!  Axis2Client
  class STAFF_CLIENT_EXPORT CAxis2Client
  {
  public:
    //!         конструктор
    CAxis2Client();

    //!         деструктор
    ~CAxis2Client();

    //!         инициализация клиента
    /*! \param  sServiceName - имя сервиса
        \param  sHostName - имя хоста или IP-адрес
        \param  sHostPort - порт хоста
        */
    void Init(const rise::CString& sServiceName, const rise::CString& sHostName = "localhost", const rise::CString& sHostPort = "9090" );

    //!         установить используемый SessionId сервиса
    /*! \param  sSessionId - SessionId сервиса
        */
    void SetSessionId(const rise::CString& sSessionId);

    //!         деинициализация
    void Deinit();

    //!         получить URI сервиса
    /*! \return URI сервиса
        */
    const rise::CString& GetServiceURI() const;

    //!         выполнить операцию
    /*! \param  rOperation - операция
        */
    void Invoke(COperation& rOperation);    

    //!         проверка на инициализацию
    bool IsInit() const;
    
  private:
    CAxis2Client& operator=(const CAxis2Client&);
    class CAxis2ClientImpl;
    CAxis2ClientImpl* m_pImpl;
  };
}

#endif // _AXIS2CLIENT_H_
