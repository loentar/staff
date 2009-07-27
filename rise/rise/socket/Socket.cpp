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

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CSocket
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CSocket::CSocket() /*throw()*/:
      m_tSock(0), m_ushPort(0), m_bUseSigPipe(false)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CSocket
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CSocket::~CSocket() /*throw()*/
    {
      Close();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Create(...)
    //    DESCRIPTION:    создание сокета
    //    PARAMETRS:      (in) port - номер порта
    //                         type - тип сокета SOCK_STREAM или SOCK_DGRAM 
    //    RETURN:         none
    //    EXCEPTIONS:     
    //       CLogicAlreadyExistsException - инициализация уже произведена
    //       CFileCreateException - ошибка при создании сокета
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CSocket::Create(ushort ushPort /*= 0*/, int nType /*= ST_STREAM*/)
    {
      RISE_ASSERTE(!IsInit(), CLogicAlreadyExistsException);

      SOCKET tSock = osCreateSocket(AF_INET, nType, nType == ST_STREAM ? IPPROTO_TCP : nType == ST_DGRAM ? IPPROTO_UDP : nType);
      RISE_ASSERTES( tSock != INVALID_SOCKET, CFileCreateException, osGetLastSocketErrorStr());

      m_tSock = tSock;
      m_ushPort = ushPort;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Assign
    //    DESCRIPTION:    создание сокета от уже существующего
    //    PARAMETRS:      rSocket - уже существующий сокет
    //    RETURN:         none
    //    EXCEPTIONS:     
    //       CLogicAlreadyExistsException - инициализация уже произведена
    //       CLogicNoItemException - ошибка при получении имени сокета
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CSocket::Assign(SOCKET& rSocket)
    {
      RISE_ASSERTE(!IsInit(), CLogicAlreadyExistsException);
      LogDebug2() << "using sockid: " << static_cast<unsigned>(rSocket);

      m_tSock = rSocket;
      m_ushPort = GetRemotePort();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetHandle
    //    DESCRIPTION:    получение дескриптора сокета
    //    PARAMETRS:      none
    //    RETURN:         дескриптор сокетая
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    SOCKET CSocket::GetHandle() const
    {
      return m_tSock;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetPort
    //    DESCRIPTION:    получение порта сокета
    //    PARAMETRS:      none
    //    RETURN:         порт сокета
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    ushort CSocket::GetPort() const
    {
      return m_ushPort;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetLocalIP(Str)
    //    DESCRIPTION:    получение локального IP-адреса связанного с сокетом
    //    PARAMETRS:      none
    //    RETURN:         локальный IP-адрес связанный с сокетом
    //    EXCEPTIONS:     CLogicNoInitException - не произведена инициализация, 
    //                    CFileIOException - ошибка при получении локального IP
    //    COMMENT:        сокет должен быть соединен!
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetLocalPort
    //    DESCRIPTION:    получение локального порта связанного с сокетом
    //    PARAMETRS:      none
    //    RETURN:         локальный порт связанный с сокетом
    //    EXCEPTIONS:     CLogicNoInitException - не произведена инициализация, 
    //                    CFileIOException - ошибка при получении локального IP
    //    COMMENT:        сокет должен быть соединен!
    //////////////////////////////////////////////////////////////////////////////
    ushort CSocket::GetLocalPort() const
    {
      sockaddr_in stAddr;
      GetLocalSockAddr(stAddr);
      return htons(stAddr.sin_port);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetRemoteIP(Str)
    //    DESCRIPTION:    получение удаленного IP-адреса связанного с сокетом
    //    PARAMETRS:      none
    //    RETURN:         удаленный IP-адрес связанный с сокетом
    //    EXCEPTIONS:     CLogicNoInitException - не произведена инициализация, 
    //                    CFileIOException - ошибка при получении удаленного IP
    //    COMMENT:        сокет должен быть соединен!
    //////////////////////////////////////////////////////////////////////////////
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


    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetRemotePort
    //    DESCRIPTION:    получение удаленного порта связанного с сокетом
    //    PARAMETRS:      none
    //    RETURN:         удаленный порт связанный с сокетом
    //    EXCEPTIONS:     CLogicNoInitException - не произведена инициализация, 
    //                    CFileIOException - ошибка при получении удаленного IP
    //    COMMENT:        сокет должен быть соединен!
    //////////////////////////////////////////////////////////////////////////////
    ushort CSocket::GetRemotePort() const
    {
      sockaddr_in stAddr;
      GetRemoteSockAddr(stAddr);
      return htons(stAddr.sin_port);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Close()
    //    DESCRIPTION:    закрытие сокета
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     
    //        CLogicNoInitException - сокет не был создан
    //        CFileCloseException - ошибка при закрытии сокета
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       IsInit
    //    DESCRIPTION:    проверка инициализации
    //    PARAMETRS:      none
    //    RETURN:         true, если есть инициализация
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CSocket::IsInit() const
    {
      return m_tSock != 0;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetLocalSockAddr
    //    DESCRIPTION:    получение локального адреса сокета
    //    PARAMETRS:      rstAddr - ссылка на значение куда будет помещен адрес 
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicNoInitException - не произведена инициализация, 
    //                    CFileIOException - ошибка при получении адреса
    //    COMMENT:        вызывает системные функции;
    //                    сокет должен быть соединен!
    //////////////////////////////////////////////////////////////////////////////
    void CSocket::GetLocalSockAddr( sockaddr_in& rstAddr ) const
    {
      RISE_ASSERTE(IsInit(), CLogicNoInitException);

      socklen_t nNameLen = sizeof(rstAddr);
      int nRet = getsockname(m_tSock, reinterpret_cast<sockaddr*>(&rstAddr), &nNameLen);
      RISE_ASSERTES(nRet == 0, CFileIOException, osGetLastSocketErrorStr());
      LogDebug2() << "port=" << htons(rstAddr.sin_port) << " IP=" << inet_ntoa(rstAddr.sin_addr);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetRemoteSockAddr
    //    DESCRIPTION:    получение удаленного адреса сокета
    //    PARAMETRS:      rstAddr - ссылка на значение куда будет помещен адрес 
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicNoInitException - не произведена инициализация, 
    //                    CFileIOException - ошибка при получении адреса
    //    COMMENT:        вызывает системные функции;
    //                    сокет должен быть соединен!
    //////////////////////////////////////////////////////////////////////////////
    void CSocket::GetRemoteSockAddr(sockaddr_in& rstAddr) const
    {
      RISE_ASSERTE(IsInit(), CLogicNoInitException);

      socklen_t nNameLen = sizeof(rstAddr);
      int nRet = getpeername(m_tSock, reinterpret_cast<sockaddr*>(&rstAddr), &nNameLen);
      RISE_ASSERTES(nRet == 0, CFileIOException, osGetLastSocketErrorStr());
      LogDebug2() << "port=" << htons(rstAddr.sin_port) << " IP=" << inet_ntoa(rstAddr.sin_addr);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       SetNonBlockingMode
    //    DESCRIPTION:    установка/снятие неблокирующего режима
    //    PARAMETRS:      true - установить неблокирующий(асинхронный) режим
    //                    false - установить блокирующий(синхронный) режим
    //    RETURN:         none
    //    EXCEPTIONS:     
    //       CFileIOException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       SetPort
    //    DESCRIPTION:    установка порта сокета
    //    PARAMETRS:      none
    //    RETURN:         порт сокета
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CSocket::SetPort(ushort ushPort)
    {
      m_ushPort = ushPort;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetSockOpt
    //    DESCRIPTION:    получить информацию о сокете
    //    PARAMETRS:      (in) eSockOpt - опция сокета
    //                    (out) rValue - значение параметра
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CSocket::GetSockOpt(ESocketOption eSockOpt, char* szValue, int nParamSize)
    {
      bool bOK = getsockopt(GetHandle(), SOL_SOCKET, eSockOpt, szValue, reinterpret_cast<socklen_t*>(&nParamSize) ) == 0;
      if(!bOK)
        LogError() << "Error: " << osGetLastSocketErrorStr();
      else
        LogDebug3() << "GetSockOpt: " << osGetLastSocketErrorStr();
      return bOK;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       SetSockOpt
    //    DESCRIPTION:    получить информацию о сокете
    //    PARAMETRS:      (in) eSockOpt - опция сокета
    //                    (out) rValue - значение параметра
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
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
