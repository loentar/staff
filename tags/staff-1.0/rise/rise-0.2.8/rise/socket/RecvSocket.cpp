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

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CRecvSocket
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CRecvSocket::CRecvSocket() /*throw()*/
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CRecvSocket
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CRecvSocket::~CRecvSocket() /*throw()*/
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       WaitForData(...)
    //    DESCRIPTION:    получение размера данных которые могут быть прочитаны
    //    PARAMETRS:      nSeconds - время ожидания в секундах
    //                    nUSeconds - время ожидания в микросекундах
    //    RETURN:         размер данных в байтах
    //    EXCEPTIONS:     
    //       CLogicNoInitException - не произведена инициализация
    //       CFileIOException - ошибка при ожидании
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CRecvSocket::WaitForData(ulong ulSeconds, ulong ulUSeconds) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "не произведена инициализация");

      int nRet = osSocketWaitForRecv(GetHandle(), ulSeconds, ulUSeconds);
      RISE_ASSERTES(nRet >= 0, CFileIOException, osGetLastSocketErrorStr());

      return (nRet > 0);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       WaitForData(...)
    //    DESCRIPTION:    ожидание данных для чтения, неопределенное время
    //    PARAMETRS:      none
    //    RETURN:         true - данные появились,
    //                    false - ожидание прервано
    //    EXCEPTIONS:     
    //       CLogicNoInitException - не произведена инициализация
    //       CFileIOException - ошибка при ожидании
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CRecvSocket::WaitForData() const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "не произведена инициализация");

      int nRet = osSocketWaitForRecvInfinite(GetHandle());
      RISE_ASSERTES(nRet >= 0, CFileIOException, osGetLastSocketErrorStr() );
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetReceiveSize(...)
    //    DESCRIPTION:    получение размера данных которые могут быть прочитаны
    //    PARAMETRS:      none
    //    RETURN:         размер данных в байтах
    //    EXCEPTIONS:     
    //       CLogicNoInitException - не произведена инициализация
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    ulong CRecvSocket::GetReceiveSize() const
    {
      unsigned long ulSize = 0;
      RISE_ASSERTES(IsInit(), CLogicNoInitException, 
        "ошибка при получении размера данных: не произведена инициализация");
      bool bRet = osIOCtlSocket(GetHandle(), FIONREAD, &ulSize);
      RISE_ASSERTES(bRet, CFileReadException, "ошибка при получении размера данных: " + osGetLastSocketErrorStr());
      return ulSize;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Recv(...)
    //    DESCRIPTION:    прием данных
    //    PARAMETRS:      (out) pBuff - буфер для приема данных
    //                    (in) ulSize - размер буфера для приема
    //                    (out) ulReceived - указатель на количество принятых байт
    //    RETURN:         true, если прием данных успешно произведен
    //                    false, нет данных при неблокируемом режиме
    //    EXCEPTIONS:     
    //       CLogicNoInitException - не произведена инициализация
    //       CFileReadException - ошибка при чтении
    //       CFileCloseException - ошибка при чтении: соединение закрыто
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CRecvSocket::Recv(byte *pBuff, ulong ulSize, ulong* pulReceived/* = NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "ошибка при чтении из сокета: не произведена инициализация");

      int nRecv = recv(GetHandle(), reinterpret_cast<char*>(pBuff), static_cast<int>(ulSize), IsUseSigPipe() ? 0 : MSG_NOSIGNAL);
      if (pulReceived != NULL)
        *pulReceived = static_cast<ulong>(nRecv);
      
      LogDebug3() << "received " << nRecv << "bytes";
      RISE_ASSERTES(nRecv != 0, CFileCloseException, "ошибка при чтении из сокета: соединение закрыто: " + osGetLastSocketErrorStr());

      if ( nRecv == SOCKET_ERROR )
      {
        RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, 
          "ошибка при чтении из сокета при неблокирующем режиме: " + osGetLastSocketErrorStr());
        return false;
      }

      return true;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Recv(...)
    //    DESCRIPTION:    прием данных через присоединенный сокет
    //    PARAMETRS:      rBuff - буфер для приема данных
    //                    bAppend - дописать принятые данные к уже имеющимся
    //                    pulReceived - количество полученых байт
    //    RETURN:         true, если прием данных успешно произведен
    //                    false, нет данных для чтения В БЛОКИРУЮЩЕМ И НЕБЛОКИРУЮЩЕМ РЕЖИМАХ!
    //    EXCEPTIONS:     
    //       CFileReadException - ошибка при чтении
    //       CFileCloseException - ошибка при чтении: соединение закрыто
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CRecvSocket::Recv(CStreamBuffer& rBuff, bool bAppend /*= false*/, ulong* pulReceived/* = NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "ошибка при чтении из сокета: не произведена инициализация");

      ulong ulSize = GetReceiveSize();
      if (ulSize == 0)  /// нет данных для чтения
        return false;

      PBuffer pBuff = NULL;
      if (bAppend)
        pBuff = rBuff.Grow(ulSize);
      else
      {
        rBuff.Reset();
        rBuff.Resize(ulSize);
        pBuff = rBuff.GetBuffer();
      }

      RISE_ASSERT(pBuff);
      int nRecv = recv(GetHandle(), reinterpret_cast<char*>(pBuff), static_cast<int>(ulSize), IsUseSigPipe() ? 0 : MSG_NOSIGNAL);
      if (pulReceived != NULL)
        *pulReceived = static_cast<ulong>(nRecv);

      LogDebug3() << "received " << nRecv << "bytes";

      if ( nRecv == SOCKET_ERROR )
      {
        RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, 
                        "ошибка при чтении из сокета при неблокирующем режиме: " + osGetLastSocketErrorStr());
        return false;
      }

      if (bAppend)
        rBuff.Resize(rBuff.GetSize() - ulSize + nRecv);
      else
        rBuff.Resize(nRecv);

      RISE_ASSERTES(nRecv != 0, CFileCloseException, "ошибка при чтении из сокета: соединение закрыто: " + osGetLastSocketErrorStr());

      return true;
    }

    //!        получить точно указанное количество данных
    /*
       CFileReadException - ошибка при чтении
       CFileCloseException - ошибка при чтении: соединение закрыто
       \param  pBuff - буфер для приема данных
       \param  ulReceive - количество байт, которые необходимо принять
    */
    void CRecvSocket::RecvExactly(byte *pBuff, ulong ulReceive) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "ошибка при чтении из сокета: не произведена инициализация");

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
            "ошибка при чтении из сокета при неблокирующем режиме: " + osGetLastSocketErrorStr());
          return;
        }

        RISE_ASSERTES(nRecv != 0, CFileCloseException, "ошибка при чтении из сокета: соединение закрыто: " + osGetLastSocketErrorStr());

        nNeedRecv -= nRecv;
        pBuff += nRecv;
      }
      nRecv = static_cast<int>(ulReceive) - nNeedRecv;

      
      LogDebug3() << "received " << nRecv << "bytes";
      RISE_ASSERTES(nRecv != 0, CFileCloseException, "ошибка при чтении из сокета: соединение закрыто: " + osGetLastSocketErrorStr());

      if ( nRecv == SOCKET_ERROR )
        RISE_ASSERTES(osGetLastSocketError() != EWOULDBLOCK, CFileReadException, 
          "ошибка при чтении из сокета при неблокирующем режиме: " + osGetLastSocketErrorStr());
    }

    //!        прием данных через присоединенный сокет
    /*
       исключения:
       CFileReadException - ошибка при чтении
       CFileCloseException - ошибка при чтении: соединение закрыто

       \param  rBuff - буфер для приема данных
       \param  bAppend - дописать принятые данные к уже имеющимся
       \param  ulReceive - количество байт, которые необходимо принять
    */
    void CRecvSocket::RecvExactly(CStreamBuffer& rBuff, ulong ulReceive, bool bAppend /*= false*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "ошибка при чтении из сокета: не произведена инициализация");

      int nRecv = 0;
      PBuffer pBuff = NULL;
      if (bAppend)
        pBuff = rBuff.Grow(ulReceive);
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
            "ошибка при чтении из сокета при неблокирующем режиме: " + osGetLastSocketErrorStr());
          return;
        }

        RISE_ASSERTES(nRecv != 0, CFileCloseException, "ошибка при чтении из сокета: соединение закрыто: " + osGetLastSocketErrorStr());

        nNeedRecv -= nRecv;
        pBuff += nRecv;
      }
      nRecv = static_cast<int>(ulReceive) - nNeedRecv;

      RISE_ASSERTES(static_cast<ulong>(nRecv) == ulReceive, CFileCloseException, "запрошенное количество данных не может быть получено: " + osGetLastSocketErrorStr());

      return;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Send(...)
    //    DESCRIPTION:    отправка данных через присоединенный сокет
    //    PARAMETRS:      pBuff - буфер для передачи данных
    //                    ulSize - количество отправляемых байт
    //                    pulSent - указатель на количество отправленных байт
    //    RETURN:         none
    //    EXCEPTIONS:     
    //       CLogicNoInitException - не произведена инициализация
    //       CFileWriteException - ошибка при записи: сокет закрыт
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CRecvSocket::Send(const byte *pBuff, ulong ulSize, ulong* pulSent /*= NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "ошибка при записи в сокет: не произведена инициализация");

      int nSent = send(GetHandle(), (char *)pBuff, (int)ulSize, IsUseSigPipe() ? 0 : MSG_NOSIGNAL);
      if (pulSent != NULL)
        *pulSent = static_cast<ulong>(nSent);

      LogDebug3() << "sent " << nSent << "bytes";

      RISE_ASSERTES( nSent != -1, CFileWriteException, 
        "ошибка при записи в сокет:" + osGetErrorStr(osGetLastSocketError()) );
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Send(...)
    //    DESCRIPTION:    отправка данных через присоединенный сокет
    //    PARAMETRS:      rBuff - буфер для передачи данных
    //                    pulSent - указатель на количество отправленных байт
    //    RETURN:         none
    //    EXCEPTIONS:     
    //       CLogicNoInitException - не произведена инициализация
    //       CFileWriteException - ошибка при записи: сокет закрыт
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CRecvSocket::Send(const CStreamBuffer& rBuff, ulong* pulSent /*= NULL*/) const
    {
      RISE_ASSERTES(IsInit(), CLogicNoInitException, "ошибка при записи в сокет: не произведена инициализация");
      RISE_ASSERTES((rBuff.GetData() != NULL) && (rBuff.GetSize() != 0), CLogicParamException, "буфер пуст");

      int nSent = send(GetHandle(), reinterpret_cast<const char*>(rBuff.GetData()), static_cast<int>(rBuff.GetSize()), IsUseSigPipe() ? 0 : MSG_NOSIGNAL);
      if (pulSent != NULL)
        *pulSent = static_cast<ulong>(nSent);

      LogDebug3() << "sent " << nSent << "bytes";

      RISE_ASSERTES( nSent != -1, CFileWriteException, "ошибка при записи в сокет:" + osGetErrorStr(osGetLastSocketError()) );
    }


  } // namespace sockets
} // namespace rise
