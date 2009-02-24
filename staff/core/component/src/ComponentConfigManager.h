#ifndef _CCOMPONENTCONFIGMANAGER_H_
#define _CCOMPONENTCONFIGMANAGER_H_

#include "staffcomponentexport.h"
#include <rise/string/String.h>

namespace staff
{
  class CComponentConfig;

  //! хранитель конфигураций компонентов
  class STAFF_COMPONENT_EXPORT CComponentConfigManager
  {
  public:
    //!         получить экземпляр
    /*! \return экземпляр
    */
    static CComponentConfigManager& Inst();

    //!         получить(и загрузить) конфигурацию компонента
    /*! \param  sComponent - имя компонента
        \param  sConfig - имя конфигурационного файла
        \param  bCreate - создать новую конфиграцию при ошибке загрузки
        \return конфигурация компонента
        */
    CComponentConfig& GetComponentConfig(const rise::CString& sComponent, const rise::CString& sConfig = "config.xml", 
      bool bCreate = false);
  
  protected:
    CComponentConfigManager();
    
    ~CComponentConfigManager();

  private:
    static CComponentConfigManager* m_pInst;
    class CComponentConfigManagerImpl;
    CComponentConfigManagerImpl* m_pImpl;
  };
}

#endif // _CCOMPONENTCONFIGMANAGER_H_
