#ifndef _ATOMICCOUNT_H_
#define _ATOMICCOUNT_H_

#include <rise/os/osdllexport.h>
#include <rise/threading/CriticalSection.h>

namespace rise
{
  namespace threading
  {

    //!  атомарный счетчик(потоко-безопасный)
    class RISE_EXPORT CAtomicCount  
    {
    public:
      //!        конструктор
      CAtomicCount() throw();

      //!        конструктор
      CAtomicCount(long lValue) throw();

      //!        атомарный инкремент
      /*
         \return новое значение
      */
      long Increment() throw();

      //!        атомарный декремент
      /*
         \return новое значение
      */
      long Decrement() throw();

      //!        получение значения счетчика
      /*
         \return текущее значение счетчика
      */
      long GetValue() const throw();

      //!        атомарный преинкремент
      /*
         \return новое значение
      */
      long operator++() throw();

      //!        атомарный предекремент
      /*
         \return новое значение
      */
      long operator--() throw();

      //!        атомарный постинкремент
      /*
         \return новое значение
      */
      long operator++(int) throw();
    
      //!        атомарный постдекремент
      /*
         \return новое значение
      */
      long operator--(int) throw();
    
      //!        получение значения счетчика
      /*
         \return текущее значение счетчика
      */
      operator long() const throw();

    private:
      CAtomicCount(CAtomicCount&);
      CAtomicCount operator=(const CAtomicCount&);
    
      volatile long m_lValue;  //! текущее значение
#ifndef WIN32
      CCriticalSection m_csLock;
#endif
    };
  }
}
  
#endif // _ATOMICCOUNT_H_
