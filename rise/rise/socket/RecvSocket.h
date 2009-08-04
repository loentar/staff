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
    //!        server data socket
    class RISE_EXPORT CRecvSocket: public CSocket
    {
    public:
      //!        default constructor
      CRecvSocket();
      
      virtual ~CRecvSocket();

      //!         wait for data
      /*! \param  ulSeconds - seconds
          \param  ulUSeconds - ultra seconds
          \return true - data is available, false - timeout
      */
      bool WaitForData(ulong ulSeconds, ulong ulUSeconds = 0) const;
      
      //!         wait for data
      void WaitForData() const;

      //!         get available data size
      /*! \return available data size
      */
      ulong GetReceiveSize() const;
      
      //!        receive data
      /*! \param  pBuff - buffer
          \param  ulSize - buffer size
          \param  pulReceived - pointer to received data count
          \return true - data was received, false - no data receiving(non-blocking mode)
      */
      bool Recv(byte *pBuff, ulong ulSize, ulong* pulReceived = NULL) const;
      
      //!        receive data
      /*! \param  rBuff - stream buffer
          \param  bAppend - append data
          \param  pulReceived - pointer to received data count
          \return true - data was received, false - no data receiving(non-blocking mode)
          */
      bool Recv(CStreamBuffer& rBuff, bool bAppend = false, ulong* pulReceived = NULL) const;
      
      //!        receive exactly N bytes
      /*! \param  pBuff - buffer
          \param  ulSize - bytes to recv
      */
      void RecvExactly(byte *pBuff, ulong ulSize) const;

      //!        receive exactly N bytes
      /*! \param  rBuff - data buffer
          \param  ulReceive - bytes to recv
          \param  bAppend - append data
          */
      void RecvExactly(CStreamBuffer& rBuff, ulong ulReceive, bool bAppend = false) const;
      
      //!        send data
      /*! \param  pBuff - data buffer 
          \param  ulSize - data length
          \param  pulSent - sent bytes count
      */
      void Send(const byte *pBuff, ulong ulSize, ulong* pulSent = NULL) const;

      //!        send data
      /*! \param  rBuff - data buffer
          \param  pulSent - sent bytes count
          */
      void Send(const CStreamBuffer& rBuff, ulong* pulSent = NULL) const;

    }; // class CRecvSocket

  } // namespace sockets
} // namespace rise


#endif // ifndef _RECVSOCK_H_
