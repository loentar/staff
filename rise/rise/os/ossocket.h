/*
 *  Copyright 2009 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

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
#ifdef OS_Linux
#include <bits/sockaddr.h>
#endif
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
  //! socklen_t for win32
  typedef int socklen_t;

  //! using global socker
  using ::SOCKET;
#else
  //! type: socket
  typedef int SOCKET;
#define INVALID_SOCKET  (rise::SOCKET)(~0)
#define SOCKET_ERROR                  (-1)
#endif

  //!        create socket
  /*! \param  af - address format
      \param  type - socket type (SOCK_STREAM or SOCK_DGRAM)
      \param  protocol - protocol
      \return socket handle
  */
  SOCKET osCreateSocket(int af, int type, int protocol);

  //!        close socket
  /*! \param  s - socket 
      \return true, if socket was successfully closed
  */
  bool osCloseSocket(SOCKET s);

  //!        set io mode
  /*! \param  s - socket handle
      \param  cmd - command
      \param  argp - command param
      \return true, if io mode was set
  */
  bool osIOCtlSocket(SOCKET s, long cmd, unsigned long *argp); 

  //!         wait for data
  /*! \param  sock - socket handle
      \param  nSeconds - seconds
      \param  nUSeconds - microseconds
      \return true, some data is available for reading
  */
  int osSocketWaitForRecv(SOCKET sock, unsigned long nSeconds, unsigned long nUSeconds);

  //!         wait for data undefined time
  /*! \param  sock - socket handle
      \return true, some data is available for reading
  */
  int osSocketWaitForRecvInfinite(SOCKET sock);

  //!        get last socket error
  /*
     \return last socket error
  */
  int osGetLastSocketError();

  //!        get last socket error description
  /*
     \return last socket error description
  */
  CString osGetLastSocketErrorStr();

}

#endif  // _OSSOCKET_H_
