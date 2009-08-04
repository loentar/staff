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
#endif
#include <rise/os/ossocket.h>
#include <rise/os/oserror.h>
#include <rise/common/Log.h>
#include <rise/string/String.h>
#include "Socket.h"


#ifdef WIN32
#define RISE_SHUT_WR SD_SEND
#else
#define RISE_SHUT_WR SHUT_WR
#endif

namespace rise
{
  namespace sockets
  {
#ifdef WIN32
    static class CWinSocketInitializer
    {
    private:
      bool m_bInit;
    public:
      CWinSocketInitializer()
      {
        WSADATA wsaData;
        int nRet = WSAStartup(MAKEWORD(2, 0), &wsaData);
        if (nRet != 0)
          LogError() << "WSAStartup FAILED: " << osGetErrorStr(osGetLastSocketError());
        m_bInit = nRet == 0;
      }

      ~CWinSocketInitializer()
      {
        if (!m_bInit)
          return;
        int nRet = WSACleanup();
        if (nRet != 0)
          LogError() << "WSACleanup FAILED: " << osGetErrorStr(osGetLastSocketError());
      }
    } cWinSocketInitializer;
#endif

    CSocket::CSocket():
      m_tSock(0), m_ushPort(0), m_bUseSigPipe(false)
    {
    }

    CSocket::~CSocket()
    {
      Close();
    }

    void CSocket::Create(ushort ushPort /*= 0*/, int nType /*= ST_STREAM*/)
    {
      RISE_ASSERTE(!IsInit(), CLogicAlreadyExistsException);

      SOCKET tSock = osCreateSocket(AF_INET, nType, nType == ST_STREAM ? IPPROTO_TCP : nType == ST_DGRAM ? IPPROTO_UDP : nType);
      RISE_ASSERTES( tSock != INVALID_SOCKET, CFileCreateException, osGetLastSocketErrorStr());

      m_tSock = tSock;
      m_ushPort = ushPort;
    }

    void CSocket::Assign(SOCKET& rSocket)
    {
      RISE_ASSERTE(!IsInit(), CLogicAlreadyExistsException);
      LogDebug2() << "using sockid: " << static_cast<unsigned>(rSocket);

      m_tSock = rSocket;
      m_ushPort = GetRemotePort();
    }

    SOCKET CSocket::GetHandle() const
    {
      return m_tSock;
    }

    ushort CSocket::GetPort() const
    {
      return m_ushPort;
    }

    ulong CSocket::GetLocalIP() const
    {
      sockaddr_in stAddr;
      GetLocalSockAddr(stAddr);
      return stAddr.sin_addr.s_addr;
    }

    CString CSocket::GetLocalIPStr() const
    {
      sockaddr_in stAddr;
      GetLocalSockAddr(stAddr);
      return inet_ntoa(stAddr.sin_addr);
    }

    ushort CSocket::GetLocalPort() const
    {
      sockaddr_in stAddr;
      GetLocalSockAddr(stAddr);
      return htons(stAddr.sin_port);
    }

    ulong CSocket::GetRemoteIP() const
    {
      sockaddr_in stAddr;
      GetRemoteSockAddr(stAddr);
      return stAddr.sin_addr.s_addr;
    }

    CString CSocket::GetRemoteIPStr() const
    {
      sockaddr_in stAddr;
      GetRemoteSockAddr(stAddr);
      return inet_ntoa(stAddr.sin_addr);
    }


    ushort CSocket::GetRemotePort() const
    {
      sockaddr_in stAddr;
      GetRemoteSockAddr(stAddr);
      return htons(stAddr.sin_port);
    }

    void CSocket::Close()
    {
      if (m_tSock <= 0)
        return;
        
#ifdef WIN32
      DWORD tTimeValue = 1;
#else
      struct timeval tTimeValue;
      tTimeValue.tv_sec = 0;
      tTimeValue.tv_usec = 1000;
#endif

      shutdown(m_tSock, RISE_SHUT_WR);
      rise::LogDebug3() << "shutdown: " << osGetLastSocketErrorStr();

      SetSockOpt(ESO_RCVTIMEO, tTimeValue);

      {                                                
        char szTmpBuf[32];
        int nRcvd = recv(m_tSock, szTmpBuf, sizeof(szTmpBuf), 0);
        rise::LogDebug3() << "recv: " << nRcvd << "bytes: " << osGetLastSocketErrorStr();
      }
    
      if(!osCloseSocket(m_tSock))
        rise::LogWarning() << "can't closesocket: " << osGetLastSocketErrorStr();
      m_tSock = 0;
    }

    bool CSocket::IsInit() const
    {
      return m_tSock != 0;
    }

    void CSocket::GetLocalSockAddr( sockaddr_in& rstAddr ) const
    {
      RISE_ASSERTE(IsInit(), CLogicNoInitException);

      socklen_t nNameLen = sizeof(rstAddr);
      int nRet = getsockname(m_tSock, reinterpret_cast<sockaddr*>(&rstAddr), &nNameLen);
      RISE_ASSERTES(nRet == 0, CFileIOException, osGetLastSocketErrorStr());
      LogDebug2() << "port=" << htons(rstAddr.sin_port) << " IP=" << inet_ntoa(rstAddr.sin_addr);
    }

    void CSocket::GetRemoteSockAddr(sockaddr_in& rstAddr) const
    {
      RISE_ASSERTE(IsInit(), CLogicNoInitException);

      socklen_t nNameLen = sizeof(rstAddr);
      int nRet = getpeername(m_tSock, reinterpret_cast<sockaddr*>(&rstAddr), &nNameLen);
      RISE_ASSERTES(nRet == 0, CFileIOException, osGetLastSocketErrorStr());
      LogDebug2() << "port=" << htons(rstAddr.sin_port) << " IP=" << inet_ntoa(rstAddr.sin_addr);
    }

    void CSocket::SetNonBlockingMode(bool bNonBlock)
    {
      ulong ulVal = static_cast<ulong>(bNonBlock);

      RISE_ASSERTES(osIOCtlSocket(m_tSock, FIONBIO, &ulVal), CFileIOException, osGetLastSocketErrorStr());
      LogDebug2() << "set non-blocking mode: " << bNonBlock << " OK";
    }

    int CSocket::GetError()
    {
      return osGetLastSocketError();
    }

    CString CSocket::GetErrorStr()
    {
      return osGetLastSocketErrorStr();
    }

    void CSocket::SetPort(ushort ushPort)
    {
      m_ushPort = ushPort;
    }

    bool CSocket::GetSockOpt(ESocketOption eSockOpt, char* szValue, int nParamSize)
    {
      bool bOK = getsockopt(GetHandle(), SOL_SOCKET, eSockOpt, szValue, reinterpret_cast<socklen_t*>(&nParamSize) ) == 0;
      if(!bOK)
        LogError() << "Error: " << osGetLastSocketErrorStr();
      else
        LogDebug3() << "GetSockOpt: " << osGetLastSocketErrorStr();
      return bOK;
    }

    bool CSocket::SetSockOpt(ESocketOption eSockOpt, const char* szValue, int nParamSize)
    {
      bool bOK = setsockopt(GetHandle(), SOL_SOCKET, eSockOpt, szValue, static_cast<socklen_t>(nParamSize)) == 0;
      if(!bOK)
        LogError() << "Error: " << osGetLastSocketErrorStr();
      else
        LogDebug3() << "SetSockOpt: " << osGetLastSocketErrorStr();
      return bOK;
    }

    void CSocket::SetUseSigPipe(bool bUseSigPipe)
    {
      m_bUseSigPipe = bUseSigPipe;
    }

    bool CSocket::IsUseSigPipe() const
    {
      return m_bUseSigPipe;
    }

  } // namespace sockets
}  // namespace rise
