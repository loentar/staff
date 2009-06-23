#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "staffcommonexport.h"
#include <string>
#include "DataObject.h"

namespace staff
{
  class CValue;

  //!  операция(описание функции, параметров, значений параметров, возвращаемого значения и ошибок)
  class STAFF_COMMON_EXPORT COperation
  {
  public:
    //!        конструктор по умолчанию
    COperation(const std::string& sName = "", const std::string& sResponseName = "");

    //!        деструктор
    virtual ~COperation();

    //!         установить имя операции
    /*! \param  sName - имя операции
        */
    void SetName(const std::string& sName);

    //!         получить имя операции
    /*! \return имя операции
    */
    const std::string GetName() const;

    //!         установить имя ответа
    /*! \param  sName - имя операции
        */
    void SetResponseName(const std::string& sResponseName);

    //!         получить имя ответа
    /*! \return имя операции
    */
    const std::string GetResponseName() const;

    //!         set SOAP action
    /*! \param  sSoapAction - SOAP action
        */
    void SetSoapAction(const std::string& sSoapAction);

    //!         get SOAP action
    /*! \return SOAP action
        */
    const std::string& GetSoapAction() const;

    //!         добавить параметр
    /*! \param  sName - имя
        \param  tValue - значение
        */
    void AddParameter(const std::string& sName, const CValue& tValue);
    
    //!         добавить параметр как обьект данных
    /*! \param  rDataObject - обьект данных
        */
    void AddParameter(CDataObject& rDataObject);

    //!         получить обьект даных запроса
    const CDataObject& Request() const;

    //!         получить обьект даных запроса
    CDataObject& Request();

    //!         получить обьект даных результата
    const CDataObject& Result() const;

    //!         получить обьект даных результата
    CDataObject& Result();

    //!         подготовить результат к отправке
    void PrepareResult();

    //!         получить/установить результат(простой)
    /*! \return ссылка на значение результата
    */
    CValue ResultValue();

    //!         получить/установить результат(простой)
    /*! \return ссылка на значение результата
    */
    const CValue ResultValue() const;

    //!         установить результат
    /*! \param  rDataObject - обьект данных с результатом
        */
    void SetResult(CDataObject& rDataObject);

    //!         установить результат(простой)
    /*! \param  rValue - значение результата
        */
    void SetResultValue(const CValue& rValue);

    //!         проверить, является ли результат ошибкой?
    /*! \return 
        */
    bool IsFault() const;

    //!         получить полное описание ошибки
    /*! \return описание ошибки, если ошибки нет - пустая строка
        */
    std::string GetFaultString() const;

    //!         получить причину ошибки
    /*! \return причина ошибки, если ошибки нет - пустая строка
    */
    std::string GetFaultReason() const;

    //!         получить код ошибки
    /*! \return код ошибки, если ошибки нет - пустая строка
        */
    std::string GetFaultCode() const;

    //!         получить подробное описание ошибки
    /*! \return подробное описание ошибки, если ошибки нет - пустая строка
    */
    std::string GetFaultDetail() const;

    //!         получить ошибку
    /*! \return ошибка
        */
    const CDataObject GetFault() const;

    //!         установить признак результата-ошибки
    /*! \param  sReason - причина ошибки
        \param  sFaultDetail - описание ошибки
        \param  sFaultCode - код ошибки
        */
    void SetFault(const std::string& sReason, const std::string& sFaultDetail = "", const std::string& sFaultCode = "");

    //!         установить/снять признак результата-ошибки
    void ResetFault();

    //!         установить признак результата-ошибки
    /*! \param  rDataObject - обьект данных для описания ошибки
        */
    void SetUserFault(CDataObject& rDataObjectFault);

  private:
    CDataObject m_tdoRequest;
    CDataObject m_tdoResult;
    std::string m_sSoapAction;
  };
}

#endif // _OPERATION_H_
