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

#ifdef __linux__
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include "oserror.h"
#include "ossocket.h"

namespace rise
{

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osCreateSocket
  //    DESCRIPTION:    создание сокета
  //    PARAMETRS:      af - спецификация адреса
  //                    type - тип сокета (SOCK_STREAM или SOCK_DGRAM)
  //                    protocol - протокол
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  SOCKET osCreateSocket(int af, int type, int protocol)
  {
    return socket(af, type, protocol);
  }


  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osCloseSocket
  //    DESCRIPTION:    закрытие сокета
  //    PARAMETRS:      s - идентификатор сокета
  //    RETURN:         true, если закрытие произошло успешно
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  bool osCloseSocket(SOCKET s)
  {
#ifdef WIN32
    return closesocket(s) == 0;
#else
    return close(s) == 0;
#endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osIOCtlSocket
  //    DESCRIPTION:    управление режимом ввода/вывода
  //    PARAMETRS:      s - дескриптор сокета
  //                    cmd - команда
  //                    argp - указатель на параметр команды
  //    RETURN:         true, если команда выполнена успешно
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  bool osIOCtlSocket(SOCKET s, long cmd, unsigned long *argp)
  {
#ifdef WIN32
    return ioctlsocket(s, cmd, argp) == 0;
#else
    return ioctl(s, cmd, (TChar *)argp) == 0;
#endif
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osGetLastSocketError
  //    DESCRIPTION:    получение кода последней ошибки работы с сокетом
  //    PARAMETRS:      none
  //    RETURN:         код последней ошибки работы с сокетом
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int osGetLastSocketError()
  {
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
  }


  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osSocketWaitForRecv
  //    DESCRIPTION:    ожидание данных для чтения из сокета
  //    PARAMETRS:      sock - дескриптор сокета
  //                    nSeconds - время ожидания в секундах
  //                    nUSeconds - время ожидания в микросекундах
  //    RETURN:         true, если есть данные для чтения
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int osSocketWaitForRecv(SOCKET sock, unsigned long nSeconds, unsigned long nUSeconds)
  {
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
#ifdef WIN32
#pragma warning (push)
#pragma warning ( disable : 4127 )
#endif
    FD_SET(sock, &rfds);
#ifdef WIN32
#pragma warning (pop)
#endif

    tv.tv_sec = nSeconds;
    tv.tv_usec = nUSeconds;

    return select(int(sock + 1), &rfds, NULL, NULL, &tv);
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       osSocketWaitForRecvInfinite
  //    DESCRIPTION:    ожидание данных неопределенное время
  //    PARAMETRS:      sock - дескриптор сокета
  //    RETURN:         true, если есть данные для чтения
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  int osSocketWaitForRecvInfinite(SOCKET sock)
  {
    fd_set rfds;

    FD_ZERO(&rfds);
#ifdef WIN32
#pragma warning (push)
#pragma warning ( disable : 4127 )
#endif
    FD_SET(sock, &rfds);
#ifdef WIN32
#pragma warning (pop)
#endif

    return select(int(sock + 1), &rfds, NULL, NULL, NULL);
  }

  CString osGetLastSocketErrorStr()
  {
    int nErr =
#ifdef WIN32
      WSAGetLastError();
#else
      errno;
#endif
    return osGetErrorStr(nErr) + CString("(") + ToStr(nErr) + ")";
  }
}
