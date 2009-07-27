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

#ifndef _ADDRINFOREADER_H_
#define _ADDRINFOREADER_H_

#include <rise/os/osdllexport.h>
#include <rise/tools/AddrInfo.h>

namespace rise
{
  namespace tools
  {
    //!  класс получения информации об адресе
    class RISE_EXPORT CAddrInfoReader
    {
    public:
      //!         получить информацию об адресе
      /*! \param  pAddr - адрес
          \param  rAddrInfo - результирующая информация
          \return bool - информация успешно получена
          */
      static bool LookupAddr(const void* pAddr, SAddrInfo& rAddrInfo);

    private:
      class CAddrInfoReaderImpl;
      static CAddrInfoReaderImpl& Impl();
      static CAddrInfoReaderImpl* m_pImpl;
    };
  }
}

#endif // _ADDRINFOREADER_H_
