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
    //!           клиентский сокет
    class RISE_EXPORT CClientSocket: public CRecvSocket  
    {
    public:
      //! конструктор
      CClientSocket();
      
      //! деструктор
      virtual ~CClientSocket();
      
      //!         соединение с удаленным сокетом
      /*! 
          CLogicParamException - при инициализации и при вызове этой функции не был указан корректный порт
          \param  ulAddr - IP-адрес сокета
          \param  ushPort - порт
          \return true - соединение прошло успешно
          */
      bool Connect(ulong ulAddr, ushort ushPort = 0);

      //!         соединение с удаленным сокетом
      /*! 
          CLogicParamException - при инициализации и при вызове этой функции не был указан корректный порт
          \param  szAddr - IP-адрес сокета в формате "0.0.0.0"
          \param  ushPort - порт
          \return true - соединение прошло успешно
          */
      bool Connect(const CString& szAddr, ushort ushPort = 0);
    };
  }  // namespace sockets
} // namespace rise

#endif // ifndef _CLIENTSOCKET_H_
