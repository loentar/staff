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

#include <rise/os/oscommon.h>
#include <rise/common/Log.h>
#include <rise/os/ossocket.h>
#ifndef WIN32
#include <netinet/tcp.h>
#endif
#include <rise/os/oserror.h>
#include "Socket.h"
#include "ClientSocket.h"

namespace rise
{
  namespace sockets
  {
    CClientSocket::CClientSocket()
    {
    }

    CClientSocket::~CClientSocket()
    {
    }

    bool CClientSocket::Connect( ulong ulAddr, ushort ushPort /*= 0*/ )
    {
      if (ushPort != 0) 
        SetPort(ushPort);

      RISE_ASSERTP(GetPort());

      struct sockaddr_in saServer =
#if defined OS_Linux
        { AF_INET, htons(GetPort()), {0}, "" };
#else
        { AF_INET, htons(GetPort()), 0, {0} };
#endif
      saServer.sin_addr.s_addr = ulAddr;

      int nRet = connect(GetHandle(), reinterpret_cast<sockaddr*>(&saServer), sizeof(saServer));
      if(nRet == SOCKET_ERROR)
      {
        rise::LogError() << "connect: " << osGetLastSocketErrorStr();
        return false;
      }

      SetNeedShutdown();

      {
        int nNodelay = 1;
        if (setsockopt(GetHandle(), IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&nNodelay), sizeof(nNodelay)) != 0)
        {
          rise::LogError() << "setopt(nodelay): " << osGetLastSocketErrorStr();
        }
      }

      {
        struct linger stLinger;
        stLinger.l_onoff = 1;
        stLinger.l_linger = 5;
        if (!SetSockOpt(ESO_LINGER, stLinger))
        {
          rise::LogError() << "setopt(linger): " << osGetLastSocketErrorStr();
        }
      }

      return true;
    }

    bool CClientSocket::Connect(const CString& szAddr, ushort ushPort /*= 0*/)
    {
      return Connect(inet_addr(szAddr.c_str()), ushPort);
    }
  
  } // namespace sockets
} // namespace rise
