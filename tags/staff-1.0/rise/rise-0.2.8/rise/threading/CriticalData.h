#ifndef _CRITICALDATA_H_
#define _CRITICALDATA_H_

#include <rise/threading/CriticalSection.h>

namespace rise
{
  namespace threading
  {
    //!        класс данных с межпотоковой синхронизацией
    template<typename Type>
    class CCriticalData  
    {
    public:
      //!        копирующий конструктор
      /*
         \param  уже существующее значение
      */
      CCriticalData(const Type &rValue);

      //!        оператор получения ссылки на обьект данных
      /*
         \return ссылка на обьект данных
      */
      operator Type&();
      
      //!        оператор для межпотоковой синхронизации
      Type& operator++();

      //!        оператор для межпотоковой синхронизации
      Type& operator++(int);

      //!        оператор для межпотоковой синхронизации
      Type& operator--();

      //!        оператор для межпотоковой синхронизации
      Type& operator--(int);

      //!        оператор для межпотоковой синхронизации
      Type& operator=(const Type& rValue);

    private:
      Type m_tValue;  //! значение
      CCriticalSection m_cs; //! критическая секция
    };
  } // namespace threading
}  // namespace rise

#include "CriticalData.hpp"

#endif // _CRITICALDATA_H_
