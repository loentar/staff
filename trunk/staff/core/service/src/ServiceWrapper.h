#ifndef _SERVICEWRAPPER_H_
#define _SERVICEWRAPPER_H_

#include "staffserviceexport.h"
#include <rise/string/String.h>

namespace staff
{
  class COperation;

  //!  класс обработчика запросов
  /*   от этого класса наследуется упаковщик для обработки запросов(сервисная часть)
  */
  class STAFF_SERVICE_EXPORT CServiceWrapper
  {
  public:
    //!         виртуальный деструктор
    virtual ~CServiceWrapper();
    
    //!         выполнить запрос
    /*!         эту функцию необходимо перекрыть в дочернем классе
        \param  rOperation - операция
        */
    virtual void Invoke(COperation& rOperation) = 0;

    //!         получить SessionId сервиса
    /*! \return SessionId сервиса
    */
    const rise::CString& GetSessionId() const;
  
    //!         установить SessionId сервиса
    /*! \param  sSessionId - SessionId сервиса
        */
    void SetSessionId(const rise::CString& sSessionId);

  private:
    rise::CString m_sSessionId;
  };
}

#endif // _SERVICEWRAPPER_H_
