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

#include <rise/os/ossocket.h>
#include <rise/common/Log.h>
#include <rise/string/String.h>
#include "ServerSocket.h"

namespace rise
{
  namespace sockets
  {

    CServerSocket::CServerSocket()
    {
    }

    CServerSocket::~CServerSocket()
    {
    }

    void CServerSocket::Create( ushort ushPort, ulong unAddress /*= INADDR_ANY*/, int nType /*= CSocket::ST_STREAM*/, int nBacklog /*= 5*/ )
    {
      struct sockaddr_in saddr;

      CSocket::Create(ushPort, nType);

      memset(&saddr, 0, sizeof(saddr));
      saddr.sin_family = AF_INET;
      saddr.sin_port = htons(ushPort);
      saddr.sin_addr.s_addr = unAddress;

      try
      {
        int nValue = 1;
#ifdef WIN32
        SetSockOpt((ESocketOption)SO_EXCLUSIVEADDRUSE, nValue);
#else
        SetSockOpt(ESO_REUSEADDR, nValue);
#endif
      
        int nRes = bind(GetHandle(), reinterpret_cast<struct sockaddr *>(&saddr), sizeof(saddr));
        RISE_ASSERTES( nRes != SOCKET_ERROR, CFileCreateException, osGetLastSocketErrorStr());
        LogDebug2() << "bind ok";
        nRes = listen(GetHandle(), nBacklog);
        RISE_ASSERTES( nRes != SOCKET_ERROR, CFileOpenException, osGetLastSocketErrorStr());
        LogDebug2() << "listen ok";
      } catch(...)
      {
        Close();
        throw;
      }

      LogDebug2() << "listening on port: " << ushPort << "; sockid:" << GetHandle();
    }


    bool CServerSocket::Accept(CRecvSocket& rRecv)
    {
      RISE_ASSERTE(IsInit(), CLogicNoInitException);
      RISE_ASSERTE(!rRecv.IsInit(), CLogicAlreadyExistsException);

      SOCKET sock = accept(GetHandle(), NULL, NULL);
      if (sock == SOCKET_ERROR)
      {
        RISE_ASSERTES(osGetLastSocketError() == EWOULDBLOCK, CFileReadException, osGetLastSocketErrorStr());
        return false;
      }

      LogDebug2() << "Accept OK!";

      rRecv.Assign(sock);
      
      return true;
    }

  } // namespace sockets
} // namespace rise
