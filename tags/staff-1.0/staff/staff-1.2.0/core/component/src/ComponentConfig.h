#ifndef _COMPONENTCONFIG_H_
#define _COMPONENTCONFIG_H_

#include "staffcomponentexport.h"
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
  //!  конфигурация компонента
  class STAFF_COMPONENT_EXPORT CComponentConfig
  {
  public:
    //!         деструктор
    ~CComponentConfig();

    //!         получить имя компонента
    /*! \return имя компонента
    */
    const rise::CString& GetComponent() const;

    //!         получть имя конфигурационного файла
    /*! \return имя конфигурационного файла
    */
    const rise::CString& GetConfig() const;

    //!         перезагрузить конфигурацию
    /*! \param  bCreate - создать новую конфигурацию при ошибке загрузки
        \return конфигурация
        */
    rise::xml::CXMLNode& ReloadConfig(bool bCreate = false);
    
    //!         сохранить конфигурацию
    void SaveConfig();

    //!         получить конфигурацию
    /*! \return конфигурация
    */
    rise::xml::CXMLNode& Config();
    
    //!         получить конфигурацию
    /*! \return конфигурация
    */
    const rise::xml::CXMLNode& Config() const;

  protected:
    //!         защищенный конструктор
    CComponentConfig();

    //!         инициализация
    /*! внутренняя функция
        */
    void Init(const rise::CString& sComponent, const rise::CString& sConfig, bool bCreate);

  private:
    //!         запрет копирования
    CComponentConfig& operator=(const CComponentConfig&);

    //!         запрет копирования
    CComponentConfig(const CComponentConfig&);

    class CComponentConfigImpl;
    CComponentConfigImpl* m_pImpl;
    friend class CComponentConfigManager;
  };
}


#endif // _COMPONENTCONFIG_H_
