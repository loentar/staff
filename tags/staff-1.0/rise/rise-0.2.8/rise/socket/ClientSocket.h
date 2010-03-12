#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#include <rise/os/osdllexport.h>
#include "RecvSocket.h"

namespace rise
{
  namespace sockets
  {
    //!           клиентский сокет
    class RISE_EXPORT CClientSocket: public CRecvSocket  
    {
    public:
      //! конструктор
      CClientSocket();
      
      //! деструктор
      virtual ~CClientSocket();
      
      //!         соединение с удаленным сокетом
      /*! 
          CLogicParamException - при инициализации и при вызове этой функции не был указан корректный порт
          \param  ulAddr - IP-адрес сокета
          \param  ushPort - порт
          \return true - соединение прошло успешно
          */
      bool Connect(ulong ulAddr, ushort ushPort = 0);

      //!         соединение с удаленным сокетом
      /*! 
          CLogicParamException - при инициализации и при вызове этой функции не был указан корректный порт
          \param  szAddr - IP-адрес сокета в формате "0.0.0.0"
          \param  ushPort - порт
          \return true - соединение прошло успешно
          */
      bool Connect(const CString& szAddr, ushort ushPort = 0);
    };
  }  // namespace sockets
} // namespace rise

#endif // ifndef _CLIENTSOCKET_H_
