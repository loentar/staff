/*
 *  Copyright 2009 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

#include <iostream>
#include "CriticalSection.h"

namespace rise
{
  namespace threading
  {
    CCriticalSection::CCriticalSection()
    {
      osInitializeCriticalSection(&m_hCriticalSection);
    }

    CCriticalSection::~CCriticalSection()
    {
      osDeleteCriticalSection(&m_hCriticalSection);
    }

    void CCriticalSection::Enter()
    {
      osEnterCriticalSection(&m_hCriticalSection);
    }
  
    bool CCriticalSection::TryEnter()
    {
      return osTryEnterCriticalSection(&m_hCriticalSection);
    }
  
    void CCriticalSection::Leave()
    {
      osLeaveCriticalSection(&m_hCriticalSection);
    }

    CScopedCriticalSection::CScopedCriticalSection(CCriticalSection& cs):
      m_cs(cs)
    {
      m_cs.Enter();
    }
  
    CScopedCriticalSection::~CScopedCriticalSection()
    {
      m_cs.Leave();
    }
  
  } // namespace threading
} // namespace rise
