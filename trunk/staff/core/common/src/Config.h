#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "staffcommonexport.h"
#include <rise/string/String.h>

namespace rise
{
  namespace xml
  {
    class CXMLNode;
  }
}

namespace staff
{
  //! singleton - класс конфигурации
  class STAFF_COMMON_EXPORT CConfig
  {
  public:
    //!         получить ссылку на конфигурацию
    /*! \return ссылка на конфигурацию
    */
    static CConfig& Inst();

    //!         получить конфигурацию для модуля
    /*! \param  sModuleName - имя модуля
        \return конфигурация
        */
    rise::xml::CXMLNode& GetModuleConfig(const rise::CString& sModuleName);

  protected:
    //! защищенный конструктор(запрет прямого создания)
    CConfig();

    ~CConfig();

  private:
    class CConfigImpl;
    CConfigImpl* m_pImpl;
    static CConfig* m_pInst;
  };
}

#endif // _CONFIG_H_
