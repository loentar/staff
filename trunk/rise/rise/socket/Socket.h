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

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>
#include <rise/os/osdllexport.h>
#include <rise/os/ossocket.h>
#include <rise/common/ExceptionTemplate.h>

namespace rise
{
  //!        rise sockets
  namespace sockets
  {
    //!        Base socket class
    class RISE_EXPORT CSocket
    {
    public:
      enum ESOCKETTYPE  //! protocol
      {
        ST_STREAM = SOCK_STREAM, //!< TCP/IP
        ST_DGRAM  = SOCK_DGRAM   //!< UDP
      };

      enum ESocketOption //! socket options
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
      
      enum EShutdown //! shutdown
      {
        ES_READ = 1,   //!< shutdown for read
        ES_WRITE = 2,  //!< shutdown for write
        ES_BOTH = ES_READ | ES_WRITE  //!< shutdown for both
      };

    public:
      //!        constructor
      CSocket();

      //!        destructor
      virtual ~CSocket();

      //!        create socket
      /*! \param  ushPort - port
          \param  nType - socket type ST_STREAM or ST_DGRAM 
          */
      virtual void Create(ushort ushPort = 0, int nType = ST_STREAM);

      //!         attach to existing socket
      /*! \param  rSocket - socket
          */
      virtual void Assign(SOCKET& rSocket);
      
      //!         shutdown socket and break blocked recv/send call
      /*! \param  eShutdown - shutdown direction 
          \return true shutdown successes
          \sa EShutdown
      */
      bool Shutdown(EShutdown eShutdown = ES_WRITE);

      //!         close socket
      virtual void Close();

      //!         get socket handle
      /*! \return socket handle
      */
      SOCKET GetHandle() const;

      //!         get socket port
      /*! \return socket port
      */
      ushort GetPort() const;

      //!         get local socket IP
      /*!         Socket must be in Connected state
          \return local socket IP
      */
      ulong GetLocalIP() const;

      //!         get local socket IP string
      /*!         Socket must be in Connected state
          \return local socket IP string
      */
      CString GetLocalIPStr() const;

      //!         get local socket port
      /*!         Socket must be in Connected state
          \return local socket port
      */
      ushort GetLocalPort() const;

      //!         get remote socket IP
      /*!         Socket must be in Connected state
          \return remote socket IP
      */
      ulong GetRemoteIP() const;

      //!         get remote socket IP string
      /*!         Socket must be in Connected state
          \return remote socket IP string
      */
      CString GetRemoteIPStr() const;

      //!         get remote socket port
      /*!         Socket must be in Connected state
          \return remote socket port
      */
      ushort GetRemotePort() const;

      //!        check for init
      /*! \return true, if socket initialized
      */
      bool IsInit() const;
      
      //!        get local socket address
      /*!        Socket must be in Connected state
         \param  rstAddr - reference to address struct
      */
      void GetLocalSockAddr(sockaddr_in& rstAddr) const;

      //!        get remote socket address
      /*!        Socket must be in Connected state
         \param  rstAddr - reference to address struct
      */
      void GetRemoteSockAddr(sockaddr_in& rstAddr) const;

      //!         set non-blocking mode
      /*! \param  bNonBlock - true - set non blocking mode, false - set blocking mode
          */
      void SetNonBlockingMode(bool bNonBlock = true);

      //!         get last error
      /*! \return last error
      */
      int GetError() const;

      //!         get last error string
      /*! \return last error string
      */
      CString GetErrorStr() const;

      //!         get socket option
      /*! \param  eSockOpt - socket option
          \param  rValue - socket option value
          \return true - socket option was successfully get
      */
      template<typename TVALUE>
      bool GetSockOpt(ESocketOption eSockOpt, TVALUE& rValue)
      {
        return GetSockOpt(eSockOpt, reinterpret_cast<char*>(&rValue), sizeof(rValue));
      }

      //!         Set socket option
      /*! \param  eSockOpt - socket option
          \param  rValue - socket option value
          \return true - socket option was successfully set
      */
      template<typename TVALUE>
      bool SetSockOpt(ESocketOption eSockOpt, const TVALUE& rValue)
      {
        return SetSockOpt(eSockOpt, reinterpret_cast<const char*>(&rValue), sizeof(rValue));
      }
      
      //!             enable SIGPIPE signal
      /*  \param bUseSigPipe - true - enable SIGPIPE signal
      */
      void SetUseSigPipe(bool bUseSigPipe = true);
      
      //!         is SIGPIPE signal enabled
      /*! \return SIGPIPE signal enabled
      */
      bool IsUseSigPipe() const;

    protected:
      //!         set socket port
      /*! \param  ushPort - socket port
          */
      void SetPort(ushort ushPort);

    private:
      //!         get socket option
      /*! \param  eSockOpt - socket option
          \param  szValue - value
          \param  nParamSize - value size
          \return true if option was get
          */
      bool GetSockOpt(ESocketOption eSockOpt, char* szValue, int nParamSize);

      //!         set socket option
      /*! \param  eSockOpt - socket option
          \param  szValue - value
          \param  nParamSize - value size
          \return true if option was set
          */
      bool SetSockOpt(ESocketOption eSockOpt, const char* szValue, int nParamSize);

    private:
      SOCKET   m_tSock;       //!< socket handle
      ushort   m_ushPort;     //!< socket port
      bool     m_bUseSigPipe; //!< uses SIGPIPE
      int      m_nShutdown;   //!< shutdown state
    }; // class CSocket

  } // namespace sockets
} // namespace rise

#endif // ifndef _SOCKET_H_
