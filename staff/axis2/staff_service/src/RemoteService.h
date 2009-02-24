#ifndef _REMOTESERVICE_H_
#define _REMOTESERVICE_H_

#include <rise/socket/RecvSocket.h>

namespace staff
{
  class COperation;
  class CDataObject;
  class CRemoteServiceWrapper;
  class CService;

   //!  класс клиентской части для stateful сервиса
  class CRemoteService: public rise::sockets::CRecvSocket
  {
  public:
    //!         конструктор
    CRemoteService();
    
    //!         деструктор
    virtual ~CRemoteService();

    void Init();

    //!         получить имя сервиса
    /*! \return имя сервиса
    */
    const rise::CString& GetName() const;

    //!         получить ID сервиса
    /*! \return ID сервиса
    */
    const rise::CString& GetID() const;

    //!         получить описание сервиса
    /*! \return описание сервиса
    */
    const rise::CString& GetDescr() const;

    //!         получить список операций сервиса
    /*! \return список операций сервиса
    */
    const CDataObject& GetOperations() const;

    //!         создать сервис с указанным идентифиактором
    /*! \param  sID - идентификатор сервиса
        */
    void CreateServiceID(const rise::CString& sID);

    //!         выполнить операцию
    /*! \param  rOperation - операция
        */
    void Invoke(COperation& rOperation);

    //!         проверка соединения
    /*! \return true - соединение в норме
        */
    bool CheckConnection();

    //!         установить ссылку на обьект для работы с удаленным сервисом
    void SetRemoteServiceWrapper(CRemoteServiceWrapper* pRemoteServiceWrapper);

    //!         получить ссылку на обьект для работы с удаленным сервисом
    /*! \return обьект для работы с удаленным сервисом
    */
    CRemoteServiceWrapper* GetRemoteServiceWrapper();
  
  private:
    class CRemoteServiceImpl;
    CRemoteServiceImpl* m_pImpl;
  };
}

#endif // _REMOTESERVICE_H_
