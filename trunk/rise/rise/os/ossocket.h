#ifndef _OSSOCKET_H_
#define _OSSOCKET_H_

#ifdef WIN32
#include <winsock2.h>
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <bits/sockaddr.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <rise/string/String.h>

#include "oscommon.h"

namespace rise
{
#ifdef WIN32
  //! длинна типа данных сокета
  typedef int socklen_t;

  //! тип: сокет
  using ::SOCKET;
#else
  //! тип: сокет
  typedef int SOCKET;
#define INVALID_SOCKET  (rise::SOCKET)(~0)
#define SOCKET_ERROR                  (-1)
#endif

  //!        создание сокета
  /*
     \param  af - спецификация адреса
     \param  type - тип сокета (SOCK_STREAM или SOCK_DGRAM)
     \param  protocol - протокол
     \return none
  */
  SOCKET osCreateSocket(int af, int type, int protocol);

  //!        закрытие сокета
  /*
     \param  s - идентификатор сокета
     \return true, если закрытие произошло успешно
  */
  bool osCloseSocket(SOCKET s);

  //!        управление режимом ввода/вывода
  /*
     \param  s - дескриптор сокета
     \param  cmd - команда
     \param  argp - указатель на параметр команды
     \return true, если команда выполнена успешно
  */
  bool osIOCtlSocket(SOCKET s, long cmd, unsigned long *argp); 

  //!        ожидание данных для чтения из сокета
  /*
     \param  sock - дескриптор сокета
     \param  nSeconds - время ожидания в секундах
     \param  nUSeconds - время ожидания в микросекундах
     \return true, если есть данные для чтения
  */
  int osSocketWaitForRecv(SOCKET sock, unsigned long nSeconds, unsigned long nUSeconds);

  //!        ожидание данных неопределенное время
  /*
     \param  sock - дескриптор сокета
     \return true, если есть данные для чтения
  */
  int osSocketWaitForRecvInfinite(SOCKET sock);

  //!        получение кода последней ошибки работы с сокетом
  /*
     \return код последней ошибки работы с сокетом
  */
  int osGetLastSocketError();

  //!        получение кода последней ошибки работы с сокетом
  /*
     \return код последней ошибки работы с сокетом
  */
  CString osGetLastSocketErrorStr();

}

#endif  // _OSSOCKET_H_
