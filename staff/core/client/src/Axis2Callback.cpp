/*
 *  Copyright 2010 Utkin Dmitry
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

#include <axis2_callback.h>
#include "Axis2Callback.h"

namespace staff
{

  CAxis2Callback::CAxis2Callback():
    m_pCallback(NULL), m_pEnv(NULL)
  {
  }

  CAxis2Callback::~CAxis2Callback()
  {
  }

  bool CAxis2Callback::IsCompleted() const
  {
    return axis2_callback_get_complete(m_pCallback, m_pEnv) == AXIS2_TRUE;
  }

  axis2_callback_t* CAxis2Callback::Get()
  {
    return m_pCallback;
  }

  void CAxis2Callback::Set(axis2_callback_t* pCallback, axutil_env_t* pEnv)
  {
    m_pCallback = pCallback;
    m_pEnv = pEnv;
  }

}
