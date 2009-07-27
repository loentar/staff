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

#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include <rise/os/osdllexport.h>
#include "Socket.h"
#include "RecvSocket.h"

namespace rise
{
  namespace sockets
  {
    //!        серверный сокет
    class RISE_EXPORT CServerSocket: public CSocket  
    {
    public:
      //!        конструктор
      CServerSocket() /*throw()*/;
      
      //!        деструктор
      ~CServerSocket() /*throw()*/;
      
      //!        создание сокета
      /*
         исключения:
           CLogicAlreadyExistsException - инициализация уже произведена
           CFileCreateException - ошибка при создании сокета
           CFileOpenException - ошибка при переходе в режим прослушивания
         \param  ushPort - номер порта
         \param  nType - тип сокета ST_STREAM или ST_DGRAM 
         \param  nBacklog - максимальная длина очереди входящих подключений
         \return none
      */
      void Create(ushort ushPort, ulong unAddress = INADDR_ANY, int nType = CSocket::ST_STREAM, int nBacklog = 5);

      //!        принятие входящего подключения
      /*
         \param  sRecv - принятый сокет
         \return true, если входящее подключение принято
      */
      bool Accept(CRecvSocket& rRecv);

    };

  } // namespace sockets
} // namespace rise

#endif // ifndef _SERVERSOCKET_H_
