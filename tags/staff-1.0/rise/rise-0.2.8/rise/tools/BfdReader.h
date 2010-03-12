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
