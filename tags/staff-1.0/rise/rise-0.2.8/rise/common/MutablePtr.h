#ifndef _MUTABLEPTR_H_
#define _MUTABLEPTR_H_

#include <rise/common/ExceptionTemplate.h>

namespace rise
{
  //! класс изменяемого указателя
  /*! в основном предназначен для применения в контейнерах для обеспечения 
      неконстантного доступа к хранимым обьектам константного контейнера*/
  template<typename Type>
  class CMutablePtr
  {
  public:
    //!         конструктор по умолчанию
    CMutablePtr():
      m_pnCounter(NULL), m_pData(NULL) 
    {}
      
    //!         конструктор распределения владением
    /*! \param  rPtr - ссылка на другой обьект класса
        */
    CMutablePtr(const CMutablePtr& rPtr):
      m_pnCounter(NULL), m_pData(NULL) 
    {
      operator=(rPtr);
    }

    //!         инициализирующий конструктор
    /*! \param  pData - обьект
        */
    CMutablePtr(Type* pData):
      m_pnCounter(NULL), m_pData(NULL) 
    {
      operator=(pData);
    }

    //!         деструктор
    ~CMutablePtr()
    {
      Release();
    }
      
    //!         оператор распределения владением
    /*! \param  rPtr - ссылка на другой обьект класса 
        \return ссылка на текущий обьект
        */
    CMutablePtr& operator=(const CMutablePtr& rPtr)
    {
      if (m_pData != rPtr.m_pData)
        Set(rPtr.m_pData, rPtr.m_pnCounter);
      return *this;
    }
    
    //!         оператор инициализации владения
    /*! \param  pData - обьект
        \return ссылка на текущий обьект
        */
    CMutablePtr& operator=(Type* pData)
    {
      if (m_pData != pData)
        Set(pData);
      return *this;
    }

    //!         оператор проверки на эквивалентность
    /*! \param  rPtr - ссылка на другой обьект класса 
        \return true - изменяемые указатели эквивалентны
        */
    bool operator==(const CMutablePtr& rPtr) const
    {
      return rPtr.m_pData == m_pData && rPtr.m_pnCounter == m_pnCounter;
    }
    
    //!         оператор проверки на эквивалентность
    /*! \param  rPtr - ссылка на другой обьект класса 
        \return true - изменяемые указатели эквивалентны
        */
    bool operator!=(const CMutablePtr& rPtr) const
    {
      return !operator==(rPtr);
    }
    
    //!         получить указатель на хранимый обьект
    /*! \return указатель на хранимый обьект
    */
    Type* Get() const
    {
      return m_pData;
    }
    
    //!         оператор получения ссылки на хранимый обьект
    /*! \return ссылка 
    */
    Type& operator*() const
    {
      RISE_ASSERTES(m_pnCounter != NULL && m_pData != NULL, rise::CLogicNoInitException, "trying to deference NULL ptr");
      
      return *m_pData;
    }
    
    //!         оператор разыменования
    /*! \return указатель на обьект
    */
    Type* operator->() const
    {
      return m_pData;
    }
    
    //!         оператор приведения к хранимому обьекту
    /*! \return указатель на хранимый объект
    */
    operator Type*() const
    {
      return m_pData;
    }
    
    //!         установка хранимого объекта
    /*! \param  pData - указатель на хранимый обьект
        \param  pnCounter - счетчик ссылок на обьект
        \return указатель на хранимый объект
        */
    Type* Set(Type* pData, int* pnCounter = NULL)
    {
      Release();
      m_pData = pData;
      if(pnCounter != NULL)
      {
        m_pnCounter = pnCounter;
        ++*m_pnCounter;
      }
      else
      {
        if(m_pData != NULL)
          m_pnCounter = new int(1);
      }
      
      return m_pData;
    }
    
    //!         освободить хранимый объект
    /*! метод уменьшает внутрений счетчик ссылок на обьект. обьект будет удален когда счетчик будет = 0 */
    void Release()
    {
      if(m_pnCounter != NULL)
      {
        --*m_pnCounter;
        if(*m_pnCounter == 0)
        {
          delete m_pnCounter;
          m_pnCounter = NULL;
          if(m_pData != NULL)
            delete m_pData;
        }
      }
    }
    
  private:
    int* m_pnCounter; //!<  счетчик ссылок на объект
    mutable Type* m_pData; //!<  объект
  };
}

#endif // _MUTABLEPTR_H_
