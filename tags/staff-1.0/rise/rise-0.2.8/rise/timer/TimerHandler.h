#ifndef _TIMERHANDLER_H_
#define _TIMERHANDLER_H_

#include <rise/common/types.h>
#include <list>

namespace rise
{
  //!        класс для работы с таймером
  class RISE_EXPORT CTimerHandler  
  {
  public:
    typedef word TTimerID; //! идентификатор таймера

    enum EREPEAT //! количество срабатываний таймера
    {
      EINFINITE = 0  //!< бесконечное кол-во повторений
    };

  public:
    //!          конструктор
    CTimerHandler();

    //!          деструктор
    virtual ~CTimerHandler();

    //!        добавление таймера
    /*
       CAssertException - неверные параметры вызова
       \param  PARAMETERS:     dwMSec - время милисекундах до срабатывания таймера
       \param  dwRepeat  - количество срабатываний таймера
       \return идентификатор добалвенного таймера
    */
    TTimerID AddTimer( dword dwMSec, dword dwRepeat = EINFINITE );

    //!        удаление таймера
    /*
       \param  wID - идентификатор таймера
    */
    void DelTimer( TTimerID wID );

    //!        удаление всех таймеров
    void DeleteAllTimers();

  protected:
    //!        обработчик таймера
    /* эту функцию должен перекрыть унаследованный класс */
    virtual void OnTimer( word wID ) = 0;

    friend class CTimeCatcher;
  };
} // namespace rise
  
#endif // #ifdef _TIMERHANDLER_H_
