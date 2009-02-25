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

      struct sockaddr_in saServer = { AF_INET, htons(GetPort()), {0}, "" };
      saServer.sin_addr.s_addr = ulAddr;

      int nRet = connect(GetHandle(), reinterpret_cast<sockaddr*>(&saServer), sizeof(saServer));
      rise::LogDebug3() << "connect: " << osGetLastSocketErrorStr();
      if(nRet == SOCKET_ERROR)
        return false;

      {
        int nNodelay = 1;
        setsockopt(GetHandle(), IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&nNodelay), sizeof(nNodelay));
        rise::LogDebug3() << "setopt(nodelay): " << osGetLastSocketErrorStr();
      }

      {
        struct linger stLinger;
        stLinger.l_onoff = 1;
        stLinger.l_linger = 5;
        SetSockOpt(ESO_LINGER, stLinger);
        rise::LogDebug3() << "setopt(linger): " << osGetLastSocketErrorStr();
      }

      return true;
    }

    bool CClientSocket::Connect(const CString& szAddr, ushort ushPort /*= 0*/)
    {
      return Connect(inet_addr(szAddr.c_str()), ushPort);
    }
  
  } // namespace sockets
} // namespace rise
