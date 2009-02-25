#ifndef _CRITICALSECTION_H_
#define _CRITICALSECTION_H_

#include <rise/os/osdllexport.h>
#include <rise/os/osthread.h>

namespace rise
{
  namespace threading
  {
    //!        класс критическй секции
    class RISE_EXPORT CCriticalSection  
    {
    public:
      //!        конструктор
      CCriticalSection();

      virtual ~CCriticalSection();

      //!        вход в критическую секцию
      void Enter();

      //!        попытка войти в критическую секцию без блокирования
      /*
         \return true, если попытка успешна
      */
      bool TryEnter();

      //!        выход из критической секции
      void Leave();

    private:
      HCriticalSection m_hCriticalSection; //! критическая секция
    
      // запрет копирования
      CCriticalSection(const CCriticalSection&);
      CCriticalSection operator=(const CCriticalSection&);

    };  // CCriticalSection

    //!  критическая секция области видимости
    class RISE_EXPORT CScopedCriticalSection
    {
      public:
        //!        конструктор
        /*
           \param  cs - ссылка на существующую критическую секцию
        */
        CScopedCriticalSection(CCriticalSection& cs);

        //!        деструктор
        /*         класс не предназначен для наследования */
        ~CScopedCriticalSection();

      private:
        CCriticalSection& m_cs;  //! ссылка на критическую секцию

        // запрет копирования
       CScopedCriticalSection(const CScopedCriticalSection&);
       CScopedCriticalSection& operator=(const CScopedCriticalSection&);

    };
  } // namespace threading
} // namespace rise

#endif // _CRITICALSECTION_H_
