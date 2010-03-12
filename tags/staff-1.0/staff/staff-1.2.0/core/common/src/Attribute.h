#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

#include "staffcommonexport.h"
//#include <rise/string/String.h>*/
#include <string>

typedef struct axiom_attribute axiom_attribute_t;

namespace staff
{
  class CDataObject;
  class CValue;

  //! атрибут
  class STAFF_COMMON_EXPORT CAttribute
  {
  public:
    //!         конструктор по умолчанию
    CAttribute();

    //!         инициализирующий конструктор
    CAttribute(CDataObject* pDataObject, axiom_attribute_t* pAxiomAttribute);

    //!         конструктор передачи владения
    CAttribute(CAttribute& rAttribute);

    operator axiom_attribute_t*();

    //!         инициализирующий конструктор
    /*! \param  sLocalNameInit - локальное имя атрибута
        \param  sNamespaceUriInit - URI пространства имен
        */
    CAttribute(const std::string& sLocalNameInit, const CValue& tValue);

    //!         получить локальное имя атрибута
    /*! \return локальное имя атрибута
    */
    std::string GetLocalName() const;

    //!         получить значение атрибута
    /*! \return значение атрибута
    */
    CValue GetValue() const;

    //!         получить составное имя атрибута
    /*! \return CQName атрибута
    */
    const CQName GetQName() const;

    //!         получить составное имя атрибута
    /*! \return CQName атрибута
    */
    CQName GetQName();

    //!         оператор передачи владения
    /*! \param  rstAttribute - исходный 
        \return ссылка на текущее составное имя
        */
    CAttribute& operator=(CAttribute& rAttribute);

    //!         оператор проверки на эквивалентность
    /*! \param  rstAttribute - атрибут
        \return true - текущий атрибут эквивалентен заданному
        */
    bool operator==(const CAttribute& rAttribute) const;

    //!         оператор проверки на не эквивалентность
    /*! \param  rstAttribute - атрибут
        \return true - текущий атрибут неэквивалентен заданному
        */
    bool operator!=(const CAttribute& rAttribute) const;

    CAttribute* operator->();
    const CAttribute* operator->() const;

  private:
    bool m_bOwner;
    CDataObject* m_pDataObject;
    axiom_attribute_t* m_pAxiomAttribute;
  };

} // namespace staff

#endif // _ATTRIBUTE_H_
