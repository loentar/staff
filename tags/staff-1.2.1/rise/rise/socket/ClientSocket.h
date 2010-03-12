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

#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#include <rise/os/osdllexport.h>
#include "RecvSocket.h"

namespace rise
{
  namespace sockets
  {
    //!           client socket
    class RISE_EXPORT CClientSocket: public CRecvSocket  
    {
    public:
      //!         constructor
      CClientSocket();
      
      //!         destructor
      virtual ~CClientSocket();
      
      //!         connect with remote server socket
      /*! \param  ulAddr - server socket address
          \param  ushPort - port
          \return true - connection established
          */
      bool Connect(ulong ulAddr, ushort ushPort = 0);

      //!         connect with remote server socket
      /*! \param  szAddr - IP-address with format "0.0.0.0"
          \param  ushPort - port
          \return true - connection established
          */
      bool Connect(const CString& szAddr, ushort ushPort = 0);
    };
  }  // namespace sockets
} // namespace rise

#endif // ifndef _CLIENTSOCKET_H_
