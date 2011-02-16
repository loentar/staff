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
#include <fcntl.h>
#endif
#include "oserror.h"
#include "ossocket.h"

namespace rise
{
  SOCKET osCreateSocket(int af, int type, int protocol)
  {
    SOCKET tSock = socket(af, type, protocol);
#ifdef __linux__
    fcntl(tSock, F_SETFD, FD_CLOEXEC);
#endif
    return tSock;
  }

  bool osCloseSocket(SOCKET s)
  {
#ifdef WIN32
    return closesocket(s) == 0;
#else
    return close(s) == 0;
#endif
  }

  bool osIOCtlSocket(SOCKET s, long cmd, unsigned long *argp)
  {
#ifdef WIN32
    return ioctlsocket(s, cmd, argp) == 0;
#else
    return ioctl(s, cmd, (TChar *)argp) == 0;
#endif
  }

  int osGetLastSocketError()
  {
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
  }

  int osSocketWaitForRecv(SOCKET sock, unsigned long nSeconds, unsigned long nUSeconds)
  {
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
#if defined WIN32 && !defined __MINGW32__
#pragma warning (push)
#pragma warning ( disable : 4127 )
#endif
    FD_SET(sock, &rfds);
#if defined WIN32 && !defined __MINGW32__
#pragma warning (pop)
#endif

    tv.tv_sec = nSeconds;
    tv.tv_usec = nUSeconds;

    return select(int(sock + 1), &rfds, NULL, NULL, &tv);
  }

  int osSocketWaitForRecvInfinite(SOCKET sock)
  {
    fd_set rfds;

    FD_ZERO(&rfds);
#if defined WIN32 && !defined __MINGW32__
#pragma warning (push)
#pragma warning ( disable : 4127 )
#endif
    FD_SET(sock, &rfds);
#if defined WIN32 && !defined __MINGW32__
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
