#include <iostream>
#include "CriticalSection.h"

namespace rise
{
  namespace threading
  {
    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CCriticalSection
    //    DESCRIPTION:    default constructor
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CCriticalSection::CCriticalSection()
    {
      osInitializeCriticalSection(&m_hCriticalSection);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CCriticalSection
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CCriticalSection::~CCriticalSection()
    {
      osDeleteCriticalSection(&m_hCriticalSection);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Enter()
    //    DESCRIPTION:    вход в критическую секцию
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CCriticalSection::Enter()
    {
      osEnterCriticalSection(&m_hCriticalSection);
    }
  
    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       TryEnter()
    //    DESCRIPTION:    попытка войти в критическую секцию без блокирования
    //    PARAMETRS:      none
    //    RETURN:         true, если попытка успешна
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CCriticalSection::TryEnter()
    {
      return osTryEnterCriticalSection(&m_hCriticalSection);
    }
  
    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Leave()
    //    DESCRIPTION:    выход из критической секции
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CCriticalSection::Leave()
    {
      osLeaveCriticalSection(&m_hCriticalSection);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CScopedCriticalSection
    //    DESCRIPTION:    default constructor
    //    PARAMETRS:      cs - ссылка на существующую критическую секцию
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CScopedCriticalSection::CScopedCriticalSection(CCriticalSection& cs):
      m_cs(cs)
    {
      m_cs.Enter();
    }
  
    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CScopedCriticalSection
    //    COMMENT:        не предназначен для наследования
    //////////////////////////////////////////////////////////////////////////////
    CScopedCriticalSection::~CScopedCriticalSection()
    {
      m_cs.Leave();
    }
  
  } // namespace threading
} // namespace rise
