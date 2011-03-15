/*
 *  Copyright 2011 Utkin Dmitry
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

#include <errno.h>
#include <rise/os/oscommon.h>
#include <rise/os/oserror.h>
#include <rise/common/ExceptionTemplate.h>
#include "Event.h"

namespace rise
{
namespace threading
{
  CEvent::CEvent()
  {
    osThreadEventCreate(&m_hEvent);
  }

  CEvent::~CEvent()
  {
    osThreadEventFree(&m_hEvent);
  }

  bool CEvent::Wait(unsigned long ulTimeout /*= RISE_WAIT_INFINITE*/)
  {
    EEventWaitResult eResult = osThreadEventWait(&m_hEvent, ulTimeout);
    RISE_ASSERTS(eResult != EEventWaitError, "Failed to wait event: " + osGetErrorStr(errno));
    return eResult == EEventWaitSignalled;
  }

  void CEvent::Signal()
  {
    osThreadEventSignal(&m_hEvent);
  }

  CEvent::operator HEvent()
  {
    return m_hEvent;
  }

} // namespace threading
} // namespace rise
