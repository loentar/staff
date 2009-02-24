#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <rise/string/String.h>
#include "staffcomponentexport.h"

namespace staff
{
  class CDataObject;
  class COperation;
  class CComponent;

  //!  сервис диспетчера
  class STAFF_COMPONENT_EXPORT CService
  {
  public:
    //!        деструктор
    virtual ~CService();

    //!         получить имя сервиса
    /*! \return имя сервиса
    */
    virtual const rise::CString& GetName() const = 0;

    //!         получить описание сервиса
    /*! \return описание сервиса
    */
    virtual const rise::CString& GetDescr() const = 0;
    
    //!         получить описание операций
    /*! \return описание операций
    */
    virtual const CDataObject& GetOperations() const = 0;
    
    //!         вызвать операцию сервиса
    /*! \param  rOperation - операция
        \param  sID - идентификатор сервиса
        */
    virtual void Invoke(COperation& rOperation, const rise::CString& sID) = 0;

    //!         получить связанный с сервисом компонент
    /*! \return связанный с сервисом компонент
    */
    virtual const CComponent* GetComponent() const = 0;

    //!         получить связанный с сервисом компонент
    /*! \return связанный с сервисом компонент
    */
    virtual CComponent* GetComponent() = 0;

    //!         получить указатель на реализацию сервиса(только для локальных сервисов)
    /*! \param  sID - идентификатор сервиса в группе
        \return указатель на реализацию сервиса
        */
    virtual void* GetImpl(const rise::CString& sID) = 0;
  };
}

#endif // _SERVICE_H_
