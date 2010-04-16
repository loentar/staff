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
#include <rise/os/oserror.h>
#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#else
#define MSG_NOSIGNAL 0
#endif

#include <rise/os/ossocket.h>
#include <rise/common/Log.h>
#include <rise/string/String.h>
#include <rise/common/types.h>
#include "RecvSocket.h"

namespace rise
{
  namespace sockets
  {

    CRecvSocket::CRecvSocket()
    {
    }

    CRecvSocket::~CRecvSocket()
    {
    }

    bool CRecvSocket::WaitForData(ulong ulSeconds, ulong ulUSeconds) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Not initialized");

      int nRet = osSocketWaitForRecv(GetHandle(), ulSeconds, ulUSeconds);
      RISE_ASSERTES(nRet >= 0, CFileIOException, osGetLastSocketErrorStr());

      return (nRet > 0);
    }

    void CRecvSocket::WaitForData() const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Not initialized");

      int nRet = osSocketWaitForRecvInfinite(GetHandle());
      RISE_ASSERTES(nRet >= 0, CFileIOException, osGetLastSocketErrorStr() );
    }

    ulong CRecvSocket::GetReceiveSize() const
    {
      unsigned long ulSize = 0;
      RISE_ASSERTES(IsInit(), CLogicNoInitException, 
        "Can\'t get data size: Not initialized");
      bool bRet = osIOCtlSocket(GetHandle(), FIONREAD, &ulSize);
      RISE_ASSERTES(bRet, CFileReadException, "Can\'t get data size: " + osGetLastSocketErrorStr());
      return ulSize;
    }

    bool CRecvSocket::Recv(byte *pBuff, ulong ulSize, ulong* pulReceived/* = NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Can't read data: Not initialized");

      int nRecv = recv(GetHandle(), reinterpret_cast<char*>(pBuff), static_cast<int>(ulSize), IsUseSigPipe() ? 0 : MSG_NOSIGNAL);
      if (pulReceived != NULL)
        *pulReceived = static_cast<ulong>(nRecv);
      
      LogDebug3() << "received " << nRecv << "bytes";
      RISE_ASSERTES(nRecv != 0, CFileCloseException, "Can't read: connection closed: " + osGetLastSocketErrorStr());

      if ( nRecv == SOCKET_ERROR )
      {
        RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, 
          "Can't read from socket: " + osGetLastSocketErrorStr());
        return false;
      }

      return true;
    }

    bool CRecvSocket::Recv(CStreamBuffer& rBuff, bool bAppend /*= false*/, ulong* pulReceived/* = NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Can't read data: Not initialized");

      char tBuf[32];
      int nRecv = recv(GetHandle(), tBuf, sizeof(tBuf), MSG_PEEK | (IsUseSigPipe() ? 0 : MSG_NOSIGNAL));

        
      if (nRecv == SOCKET_ERROR)
      { 
        int nErr = GetError();
        if (nErr == EWOULDBLOCK)
        {
          return false;
        }

        RISE_ASSERTES(nErr != EMSGSIZE, CFileReadException, "Can't read from socket: " + osGetLastSocketErrorStr());
      }
      else
      if (nRecv == 0)
      {
        // connection closed
        if (pulReceived != NULL)
        {
          *pulReceived = 0;
        }

        LogDebug3() << "Connection closed.";
        return true;
      }
      

      ulong ulSize = GetReceiveSize();
      RISE_ASSERTES(ulSize != 0, CFileReadException, "Error while getting receive size: " + osGetLastSocketErrorStr());

      PBuffer pBuff = NULL;
      if (bAppend)
      {
        pBuff = rBuff.Grow(ulSize);
      }
      else
      {
        rBuff.Reset();
        rBuff.Resize(ulSize);
        pBuff = rBuff.GetBuffer();
      }

      RISE_ASSERT(pBuff);
      nRecv = recv(GetHandle(), reinterpret_cast<char*>(pBuff), static_cast<int>(ulSize), IsUseSigPipe() ? 0 : MSG_NOSIGNAL);

      if (pulReceived != NULL)
      {
        *pulReceived = static_cast<ulong>(nRecv);
      }

      LogDebug3() << "received " << nRecv << "bytes";

      if (nRecv == SOCKET_ERROR)
      {
        RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, "Can't read from socket: " + osGetLastSocketErrorStr());
        return false;
      }
      else
      if (nRecv == 0)
      {
        LogDebug3() << "Connection closed.";
        return true;
      }

      if (bAppend)
      {
        rBuff.Resize(rBuff.GetSize() - ulSize + nRecv);
      }
      else
      {
        rBuff.Resize(nRecv);
      }

      return true;
    }

    void CRecvSocket::RecvExactly(byte *pBuff, ulong ulReceive) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Can't read from socket: Not initialized");

      RISE_ASSERT(pBuff);

      int nRecv = 0;
      int nNeedRecv = static_cast<int>(ulReceive);
      while(nNeedRecv > 0)
      {
        WaitForData();
        nRecv = recv(GetHandle(), reinterpret_cast<char*>(pBuff), nNeedRecv, (IsUseSigPipe() ? 0 : MSG_NOSIGNAL));

        LogDebug3() << "received " << nRecv << "bytes";

        if ( nRecv == SOCKET_ERROR )
        {
          RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, 
            "Can\'t read from socket: " + osGetLastSocketErrorStr());
          return;
        }

        RISE_ASSERTES(nRecv != 0, CFileCloseException, "Can't read from socket: Connection closed: " + osGetLastSocketErrorStr());

        nNeedRecv -= nRecv;
        pBuff += nRecv;
      }
      nRecv = static_cast<int>(ulReceive) - nNeedRecv;

      
      LogDebug3() << "received " << nRecv << "bytes";
      RISE_ASSERTES(nRecv != 0, CFileCloseException, "Can't read from socket: Connection closed: " + osGetLastSocketErrorStr());

      if ( nRecv == SOCKET_ERROR )
        RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, 
          "Can\'t read from socket: " + osGetLastSocketErrorStr());
    }

    void CRecvSocket::RecvExactly(CStreamBuffer& rBuff, ulong ulReceive, bool bAppend /*= false*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Can't read from socket: Not initialized");

      int nRecv = 0;
      PBuffer pBuff = NULL;
      if (bAppend)
      {
        pBuff = rBuff.Grow(ulReceive);
      }
      else
      {
        rBuff.Reset();
        rBuff.Resize(ulReceive);
        pBuff = rBuff.GetBuffer();
      }

      RISE_ASSERT(pBuff);
      int nNeedRecv = static_cast<int>(ulReceive);
      while(nNeedRecv > 0)
      {
        WaitForData();
        nRecv = recv(GetHandle(), reinterpret_cast<char*>(pBuff), nNeedRecv, (IsUseSigPipe() ? 0 : MSG_NOSIGNAL));

        LogDebug3() << "received " << nRecv << "bytes";

        if ( nRecv == SOCKET_ERROR )
        {
          RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, 
            "Can\'t read from socket: " + osGetLastSocketErrorStr());
          return;
        }

        RISE_ASSERTES(nRecv != 0, CFileCloseException, "Can\'t read from socket: connection closed: " + osGetLastSocketErrorStr());

        nNeedRecv -= nRecv;
        pBuff += nRecv;
      }
      nRecv = static_cast<int>(ulReceive) - nNeedRecv;

      RISE_ASSERTES(static_cast<ulong>(nRecv) == ulReceive, CFileCloseException, "Can't get requested data size: " + osGetLastSocketErrorStr());

      return;
    }

    void CRecvSocket::Send(const byte *pBuff, ulong ulSize, ulong* pulSent /*= NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Can\'t send data: Not initialized");

      int nSent = send(GetHandle(), (char *)pBuff, (int)ulSize, IsUseSigPipe() ? 0 : MSG_NOSIGNAL);
      if (pulSent != NULL)
        *pulSent = static_cast<ulong>(nSent);

      LogDebug3() << "sent " << nSent << "bytes";

      RISE_ASSERTES( nSent != -1, CFileWriteException, 
        "Can\'t send:" + osGetErrorStr(osGetLastSocketError()) );
    }

    void CRecvSocket::Send(const CStreamBuffer& rBuff, ulong* pulSent /*= NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "Can\'t send data: Not initialized");
      RISE_ASSERTES((rBuff.GetData() != NULL) && (rBuff.GetSize() != 0), CLogicParamException, "Buffer is empty");

      int nSent = send(GetHandle(), reinterpret_cast<const char*>(rBuff.GetData()), static_cast<int>(rBuff.GetSize()), IsUseSigPipe() ? 0 : MSG_NOSIGNAL);
      if (pulSent != NULL)
        *pulSent = static_cast<ulong>(nSent);

      LogDebug3() << "sent " << nSent << "bytes";

      RISE_ASSERTES( nSent != -1, CFileWriteException, "Can\'t send:" + osGetErrorStr(osGetLastSocketError()) );
    }

  } // namespace sockets
} // namespace rise
