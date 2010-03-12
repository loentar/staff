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
