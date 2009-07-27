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

#ifndef _BFDREADER_H_
#define _BFDREADER_H_

#include <rise/os/osdllexport.h>
#include <rise/string/String.h>
#include <rise/tools/AddrInfo.h>

namespace rise
{
  namespace tools
  {
    //!  класс для дампа бинарных(исполняемых) файлов (для linux)
    class RISE_EXPORT CBfdReader
    {
    public:
      //!         конструктор
      CBfdReader();

      //!         деструктор
      ~CBfdReader();
      
      //!         открыть бинарный файл для дампа
      /*! \param  sFileName - имя файла
          \return true - файл успешно открыт
          */
      bool Open(const CString& sFileName);

      //!         закрыть файл
      void Close();
      
      //!         открыт ли файл
      /*! \return true - файл открыт
      */
      bool IsOpen();

      //!         получить имя файла
      /*! \return имя открытого файла
          */
      const rise::CString& GetFileName();

      //!         найти информацию об адресе
      /*! \param  pAddr - указатель на адрес
          \param  rAddrInfo - информация об адресе
          \return true - информация успешно получена
          */
      bool LookupAddrPtr( const void* pAddr, SAddrInfo& rAddrInfo );

      //!         найти информацию об адресе
      /*! \param  pAddr - адрес в шестнадцатеричной форме
          \param  rAddrInfo - информация об адресе
          \return true - информация успешно получена
          */
      bool LookupAddr( const CString& sAddrHex, SAddrInfo& rAddrInfo );

    private:
      CBfdReader& operator=(const CBfdReader&);
      CBfdReader(const CBfdReader&);
      class CBfdReaderImpl;
      CBfdReaderImpl* m_pImpl;
    };
  }
}

#endif // _BFDREADER_H_
