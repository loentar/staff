#include <rise/os/osthread.h>
#include "PerformanceCounter.h"

namespace rise 
{

  //////////////////////////////////////////////////////////////////////////////
  //    CONSTRUCTOR:    CPerformanceCounter
  //    DESCRIPTION:    default constructor
  //    PARAMETRS:      none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CPerformanceCounter::CPerformanceCounter():
    m_llStartValue(0ll), m_llLastMeasuredValue(0ll)
  {

  }

  //////////////////////////////////////////////////////////////////////////////
  //    DESTRUCTOR:     ~CPerformanceCounter
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  CPerformanceCounter::~CPerformanceCounter()
  {

  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Start
  //    DESCRIPTION:    начать измерение
  //    PARAMETRS:      none
  //    RETURN:         none
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  void CPerformanceCounter::Start()
  {
    m_llStartValue = osQueryPerfomance();
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       Finish
  //    DESCRIPTION:    завершить измерение
  //    PARAMETRS:      none
  //    RETURN:         измеренное значение времени в микросекундах
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  long long CPerformanceCounter::Finish()
  {
    m_llLastMeasuredValue = osQueryPerfomance() - m_llStartValue;
    m_llStartValue = 0ll;
    return m_llLastMeasuredValue;
  }

  //////////////////////////////////////////////////////////////////////////////
  //    FUNCTION:       GetLastMeasuredValue
  //    DESCRIPTION:    получить последнее измеренное значение
  //    PARAMETRS:      none
  //    RETURN:         последнее измеренное значение времени в микросекундах
  //    EXCEPTIONS:     none
  //    COMMENT:        none
  //////////////////////////////////////////////////////////////////////////////
  long long CPerformanceCounter::GetLastMeasuredValue() const
  {
    return m_llLastMeasuredValue;
  }

} // namespace rise
