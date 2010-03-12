#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>
#include <rise/os/osdllexport.h>
#include <rise/os/ossocket.h>
#include <rise/common/ExceptionTemplate.h>

namespace rise
{
  namespace sockets
  {
    //!        базовый класс для работы с сокетами
    class RISE_EXPORT CSocket
    {
    public:
      enum ESOCKETTYPE  //! протокол
      {
        ST_STREAM = SOCK_STREAM, //!< TCP/IP
        ST_DGRAM  = SOCK_DGRAM   //!< UDP
      };

      enum ESocketOption //! опции сокета
      {
        ESO_BROADCAST  = SO_BROADCAST,  //!< BOOL
        ESO_DEBUG      = SO_DEBUG,      //!< BOOL
        ESO_DONTROUTE  = SO_DONTROUTE,  //!< BOOL
        ESO_KEEPALIVE  = SO_KEEPALIVE,  //!< BOOL
        ESO_LINGER     = SO_LINGER,     //!< structure LINGER
        ESO_OOBINLINE  = SO_OOBINLINE,  //!< BOOL
        ESO_RCVBUF     = SO_RCVBUF,     //!< int
        ESO_REUSEADDR  = SO_REUSEADDR,  //!< BOOL
        ESO_SNDBUF     = SO_SNDBUF,     //!< int
        ESO_RCVTIMEO   = SO_RCVTIMEO,   //!< timeval
        ESO_SNDTIMEO   = SO_SNDTIMEO,   //!< timeval
      };

    public:
      //!        конструктор
      CSocket() /*throw()*/;

      //!        деструктор
      virtual ~CSocket() /*throw()*/;

      //!        создание сокета
      /*
         исключения:
         CLogicAlreadyExistsException - инициализация уже произведена
         CFileCreateException - ошибка при создании сокета
         \param   type - тип сокета ST_STREAM или ST_DGRAM 
         \param  port - номер порта
      */
      virtual void Create(ushort ushPort = 0, int nType = ST_STREAM);

      //!        создание сокета от уже существующего
      /*
         исключения:
         CLogicAlreadyExistsException - инициализация уже произведена
         CFileIOException - ошибка при получении локального IP
         \param  rSocket - уже существующий сокет
      */
      virtual void Assign(SOCKET& rSocket);

      //!        закрытие сокета
      /*
         исключения:
         CLogicNoInitException - сокет не был создан
         CFileCloseException - ошибка при закрытии сокета
      */
      virtual void Close();

      //!        получение дескриптора сокета
      /*
         \return дескриптор сокета
      */
      SOCKET GetHandle() const;

      //!        получение порта сокета
      /*
         \return порт сокета
      */
      ushort GetPort() const;

      //!        получение локального IP-адреса связанного с сокетом
      /*
         сокет должен быть соединен!
         
         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \return локальный IP-адрес связанный с сокетом
      */
      ulong GetLocalIP() const;

      //!        получение локального IP-адреса связанного с сокетом
      /*
         сокет должен быть соединен!
         
         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \return локальный IP-адрес связанный с сокетом
      */
      CString GetLocalIPStr() const;

      //!        получение локального порта связанного с сокетом
      /*         сокет должен быть соединен!

         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \return локальный порт связанный с сокетом
      */
      ushort GetLocalPort() const;

      //!        получение удаленного IP-адреса связанного с сокетом
      /*         сокет должен быть соединен!

         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \return удаленный IP-адрес связанный с сокетом
      */
      ulong GetRemoteIP() const;

      //!        получение удаленного IP-адреса связанного с сокетом
      /*         сокет должен быть соединен!

         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \return удаленный IP-адрес связанный с сокетом
      */
      CString GetRemoteIPStr() const;

      //!        получение удаленного порта связанного с сокетом
      /*         сокет должен быть соединен!

         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \return удаленный порт связанный с сокетом
      */
      ushort GetRemotePort() const;

      //!        проверка инициализации
      /*
         \return true, если есть инициализация
      */
      bool IsInit() const;
      
      //!        получение локального адреса сокета
      /*         сокет должен быть соединен!
                 вызывает системные функции(может занять время)

         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \param  rstAddr - ссылка на значение куда будет помещен адрес 
         \return none
      */
      void GetLocalSockAddr(sockaddr_in& rstAddr) const;

      //!        получение удаленного адреса сокета
      /*         сокет должен быть соединен!
                 вызывает системные функции(может занять время)

         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \param  rstAddr - ссылка на значение куда будет помещен адрес 
      */
      void GetRemoteSockAddr(sockaddr_in& rstAddr) const;

      //!        установка/снятие неблокирующего режима
      /*
         исключения:
         CLogicNoInitException - не произведена инициализация, 
         CFileIOException - ошибка при получении локального IP

         \param  bNonBlock = true - установить неблокирующий(асинхронный) режим, bNonBlock = false - установить блокирующий(синхронный) режим
      */
      void SetNonBlockingMode(bool bNonBlock = true);

      //!        получить код ошибки
      /*
         \return системный код ошибки
      */
      int GetError();

      //!        получить строковое описание ошибки
      /*
         \return строковое описание ошибки
      */
      CString GetErrorStr();

      //!        получить информацию о сокете
      /*
         \param   eSockOpt - опция сокета
         \param   rValue - значение параметра
         \return  true - операция успешно проведена
      */
      template<typename TVALUE>
      bool GetSockOpt(ESocketOption eSockOpt, TVALUE& rValue)
      {
        return GetSockOpt(eSockOpt, reinterpret_cast<char*>(&rValue), sizeof(rValue));
      }

      //!        получить информацию о сокете
      /*
         \param   eSockOpt - опция сокета
         \param   rValue - значение параметра
         \return  true - операция успешно проведена
      */
      template<typename TVALUE>
      bool SetSockOpt(ESocketOption eSockOpt, const TVALUE& rValue)
      {
        return SetSockOpt(eSockOpt, reinterpret_cast<const char*>(&rValue), sizeof(rValue));
      }
      
      //!             установить использование SIGPIPE
      /*  \param bUseSigPipe - true - включить сигнал SIGPIPE
      */
      void SetUseSigPipe(bool bUseSigPipe = true);
      
      //!             запросить использование SIGPIPE
      bool IsUseSigPipe() const;

    protected:
      //!        установка порта сокета
      /*
         \return порт сокета
      */
      void SetPort(ushort ushPort);

    private:
      //!        получить информацию о сокете
      /*
         \param   eSockOpt - опция сокета
         \param   rValue - значение параметра
         \return  true - операция успешно проведена
      */
      bool GetSockOpt(ESocketOption eSockOpt, char* szValue, int nParamSize);

      //!        получить информацию о сокете
      /*
         \param   eSockOpt - опция сокета
         \param   rValue - значение параметра
         \return  true - операция успешно проведена
      */
      bool SetSockOpt(ESocketOption eSockOpt, const char* szValue, int nParamSize);

    private:
      SOCKET   m_tSock;       //! дескриптор сокета
      ushort   m_ushPort;     //! номер порта
      bool     m_bUseSigPipe; //! использует SIGPIPE
    }; // class CSocket

  } // namespace sockets
} // namespace rise

#endif // ifndef _SOCKET_H_
