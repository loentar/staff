#ifndef _PERFORMANCECOUNTER_H_
#define _PERFORMANCECOUNTER_H_

#include <rise/os/osdllexport.h>

namespace rise
{
  //!        счетчик быстродействия
  class RISE_EXPORT CPerformanceCounter
  {
  public:
    //!        конструктор
    CPerformanceCounter();

    //!        деструктор
    ~CPerformanceCounter();

    //!        начать измерение
    void Start();

    //!        завершить измерение
    /*
       \return измеренное значение времени в микросекундах
    */
    long long Finish();

    //!        получить последнее измеренное значение
    /*
       \return последнее измеренное значение времени в микросекундах
    */
    long long GetLastMeasuredValue() const;

  private:
    long long m_llStartValue;         //!< начальное значение
    long long m_llLastMeasuredValue;  //!< последнее измеренное значение
  };

}; // namespace rise

#endif // _PERFORMANCECOUNTER_H_
