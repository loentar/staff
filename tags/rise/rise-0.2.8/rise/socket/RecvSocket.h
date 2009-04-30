#ifndef _RECVSOCK_H_
#define _RECVSOCK_H_

#include <rise/os/osdllexport.h>
#include <rise/common/types.h>
#include <rise/common/StreamBuffer.h>
#include "Socket.h"

namespace rise
{
  namespace sockets
  {
    //!        сокет для обмена данными
    class RISE_EXPORT CRecvSocket: public CSocket
    {
    public:
      //!        default constructor
      CRecvSocket() /*throw()*/;
      
      virtual ~CRecvSocket() /*throw()*/;

      //!        ожидание данных для чтения, заданное время
      /*
         исключения:
           CLogicNoInitException - не произведена инициализация
           CFileIOException - ошибка при ожидании         
         \param  ulSeconds - время ожидания в секундах
         \param  ulUSeconds - время ожидания в микросекундах
         \param  RETURN:         true - данные появились, 
         \param  false - таймаут
      */
      bool WaitForData(ulong ulSeconds, ulong ulUSeconds = 0) const;
      
      //!        ожидание данных для чтения, неопределенное время
      /* исключния
         CLogicNoInitException - не произведена инициализация
         CFileIOException - ошибка при ожидании
      */
      void WaitForData() const;

      //!        получение размера данных которые могут быть прочитаны
      /*
         CLogicNoInitException - не произведена инициализация
         \return размер данных в байтах
      */
      ulong GetReceiveSize() const;
      
      //!        прием данных через присоединенный сокет
      /*
         CFileReadException - ошибка при чтении
         CFileCloseException - ошибка при чтении: соединение закрыто
         \param  pBuff - буфер для приема данных
         \param  ulSize - размер буфера для приема
         \param  ulReceived - указатель на количество принятых байт
         \return true - если прием данных успешно произведен, false - нет данных при неблокируемом режиме
      */
      bool Recv(byte *pBuff, ulong ulSize, ulong* pulReceived = NULL) const;
      
      //!        прием данных через присоединенный сокет
      /*
         исключения:
         CFileReadException - ошибка при чтении
         CFileCloseException - ошибка при чтении: соединение закрыто

         \param  rBuff - буфер для приема данных
         \param  bAppend - дописать принятые данные к уже имеющимся
         \param  pulReceived - количество полученых байт
         \return true - если прием данных успешно произведен, false - нет данных для чтения В БЛОКИРУЮЩЕМ И НЕБЛОКИРУЮЩЕМ РЕЖИМАХ!
      */
      bool Recv(CStreamBuffer& rBuff, bool bAppend = false, ulong* pulReceived = NULL) const;
      
      //!        получить точно указанное количество данных
      /*
         CFileReadException - ошибка при чтении
         CFileCloseException - ошибка при чтении: соединение закрыто
         \param  pBuff - буфер для приема данных
         \param  ulReceive - количество байт, которые необходимо принять
      */
      void RecvExactly(byte *pBuff, ulong ulSize) const;

      //!        получить точно указанное количество данных
      /*
         исключения:
         CFileReadException - ошибка при чтении
         CFileCloseException - ошибка при чтении: соединение закрыто

         \param  rBuff - буфер для приема данных
         \param  bAppend - дописать принятые данные к уже имеющимся
         \param  ulReceive - количество байт, которые необходимо принять
      */
      void RecvExactly(CStreamBuffer& rBuff, ulong ulReceive, bool bAppend = false) const;
      
      //!        отправка данных через присоединенный сокет
      /*
         исключений:
         CLogicNoInitException - не произведена инициализация
         CFileWriteException - ошибка при записи: сокет закрыт

         \param  pBuff - буфер для передачи данных
         \param  ulSize - количество отправляемых байт
         \param  pulSent - указатель на количество отправленных байт
         \return none
      */
      void Send(const byte *pBuff, ulong ulSize, ulong* pulSent = NULL) const;

      //!        отправка данных через присоединенный сокет
      /*
         исключения:
         CLogicNoInitException - не произведена инициализация
         CFileWriteException - ошибка при записи: сокет закрыт
         \param  pBuff - буфер для передачи данных
         \param  pulSent - указатель на количество отправленных байт
         \return none
      */
      void Send(const CStreamBuffer& rBuff, ulong* pulSent = NULL) const;

    }; // class CRecvSocket

  } // namespace sockets
} // namespace rise


#endif // ifndef _RECVSOCK_H_
