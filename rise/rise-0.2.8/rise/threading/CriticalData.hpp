#ifndef _CRITICALDATA_HPP_
#define _CRITICALDATA_HPP_

namespace rise
{
  namespace threading
  {
      //////////////////////////////////////////////////////////////////////////////
      //    CONSTRUCTOR:    CCriticalData
      //    DESCRIPTION:    копирующий конструктор
      //    PARAMETRS:      уже существующее значение
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      template<typename Type>
      CCriticalData<Type>::CCriticalData(const Type &rValue)
      {
        CScopedCriticalSection cs(m_cs);
        m_tValue = rValue;
      }

      //////////////////////////////////////////////////////////////////////////////
      //    OPERATOR:       Type
      //    DESCRIPTION:    оператор получения ссылки на обьект данных
      //    PARAMETRS:      none
      //    RETURN:         ссылка на обьект данных
      //    EXCEPTIONS:     none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      template<typename Type>
      CCriticalData<Type>::operator Type&()
      {
        return m_tValue;
      }
      
      //////////////////////////////////////////////////////////////////////////////
      //    OPERATOR:       ...
      //    DESCRIPTION:    операторы для можпотоковой синхронизации
      //    PARAMETRS:      none
      //    RETURN:         none
      //    EXCEPTIONS:     none
      //    COMMENT:        none
      //////////////////////////////////////////////////////////////////////////////
      template<typename Type>
      Type& CCriticalData<Type>::operator++()
      {
        CScopedCriticalSection cs(m_cs);
        return ++m_tValue;
      }

      template<typename Type>
      Type& CCriticalData<Type>::operator++(int)
      {
        CScopedCriticalSection cs(m_cs);
        Type tValue = m_tValue;
        ++m_tValue;
        return tValue;
      }
      
      template<typename Type>
      Type& CCriticalData<Type>::operator--()
      {
        CScopedCriticalSection cs(m_cs);
        return --m_tValue;
      }
      
      template<typename Type>
      Type& CCriticalData<Type>::operator--(int)
      {
        CScopedCriticalSection cs(m_cs);
        Type tValue = m_tValue;
        --m_tValue;
        return tValue;
      }
      
      template<typename Type>
      Type& CCriticalData<Type>::operator=(const Type& rValue)
      {
        CScopedCriticalSection cs(m_cs);
        return m_tValue = rValue;
      }
      
  }
}

#endif // _CRITICALDATA_HPP_
