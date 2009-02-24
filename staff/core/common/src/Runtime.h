#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "staffcommonexport.h"
#include <rise/string/String.h>

// AXIS typedefs
typedef struct axutil_env axutil_env_t;

namespace staff
{
  //!  Runtime
  class STAFF_COMMON_EXPORT CRuntime
  {
  public:
    //!         получить ссылку на экземпляр обьекта
    /*! \return ссылка на экземпляр обьекта
    */
    static CRuntime& Inst();

    //!         получить окружение axis2
    /*! \return окружение axis2
    */
    axutil_env_t* GetAxis2Env();

    //!         получить домашний каталог Axis2
    /*! \return 
    */
    rise::CString GetAxis2Home() const;

    //!         получить домашний каталог staff
    /*! \return домашний каталог staff
    */
    rise::CString GetStaffHome() const;

    //!         получить домашний каталог компонентов
    /*! \return домашний каталог компонентов
    */
    rise::CString GetComponentsHome() const;

    //!         получить домашний каталог компонента
    /*! \param  sComponent - компонент
        \return домашний каталог компонента
        */
    rise::CString GetComponentHome(const rise::CString& sComponent) const;

    //!         получить переменную окружения
    /*! \param  sVariable - переменная окружения
        \return значение переменной окружения
        */
    rise::CString GetEnv(const rise::CString& sVariable) const;

  private:
    //!        конструктор по умолчанию
    CRuntime();

    //!        деструктор
    virtual ~CRuntime();

  private:
    class CRuntimeImpl;
    CRuntimeImpl* m_pImpl;
    static CRuntime* m_pInst;
  };
}

#endif // _RUNTIME_H_
