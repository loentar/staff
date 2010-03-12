#ifndef _SERVICEDISPATCHERCLIENT_H_
#define _SERVICEDISPATCHERCLIENT_H_

#include "staffserviceexport.h"

namespace staff
{
  class COperation;
  class CServiceWrapper;
  
  //!  часть sdk для сервиса staff. принимает запросы от диспетчера staff и передает их сервису staff
  class STAFF_SERVICE_EXPORT CServiceDispatcherClient
  {
  public:
    //!         получить ссылку на реализацию
    static CServiceDispatcherClient& ServiceDispatcherClient();

    //!         запустить клиент
    /*! \param  bSingleThread - не запускать отдельный поток для клиента, обрабатывать запросы в текущем потоке
        */
    void Start(bool bSingleThread = true);

    //!         остановить клиент
    void Stop();

    //!         установить обертку
    /*! \param  rServiceWrapper - обертка сервиса
        */
    void SetWrapper(CServiceWrapper& rServiceWrapper);

  protected:
    CServiceDispatcherClient();
  private:
    CServiceDispatcherClient& operator=(const CServiceDispatcherClient&);
    class CServiceDispatcherClientImpl;
    static CServiceDispatcherClientImpl* m_pImpl;
  };
}

#endif // _SERVICEDISPATCHERCLIENT_H_
